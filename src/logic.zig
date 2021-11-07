const std = @import("std");
const minInt = std.math.minInt;
const maxInt = std.math.maxInt;
const base = @import("base.zig");

const DoyDate = struct {
    year: i32,
    doy: u16,
};

const IcRes = struct {
    ic: base.Intercalary,
    ici: u8,
};

const UNIX_EPOCH_IN_MJD: i32 = 40587;
const RD_EPOCH_IN_MJD: i32 = -678576;

const C99Tm = extern struct {
    tm_sec: c_int,
    tm_min: c_int,
    tm_hour: c_int,
    tm_mday: c_int,
    tm_mon: c_int,
    tm_year: c_int,
    tm_wday: c_int,
    tm_yday: c_int,
    tm_isdst: c_int,
};

fn clockModulo(a: i32, b: u31) u31 {
    return @intCast(u31, @mod((a - 1), b)) + 1;
}

fn isLeap(year: i32, cal: *const base.Cal) bool {
    const lc = cal.*.leap_cycle;
    //Prevent overflow by promoting all quantities to i64
    const y = @intCast(i64, year);
    const c = @intCast(i64, lc.year_count);
    const l = @intCast(i64, lc.leap_year_count);
    const A = @intCast(i64, lc.offset_years);

    const adjusted: i64 = (l * y) - A;
    const m_simple = @mod(adjusted, c);
    const res_simple = m_simple < l;
    if (lc.skip100) {
        const res100 = (@mod(adjusted, 100) != 0 or @mod(adjusted, 400) == 0);
        if (lc.skip4000) {
            const res4000 = (@mod(adjusted, 4000) != 0);
            return res_simple and res100 and res4000;
        } else {
            return res_simple and res100;
        }
    } else {
        if (lc.skip4000) {
            unreachable;
        }
        return res_simple;
    }
}

fn segmentLen(s: base.Segment) u8 {
    return (s.day_end - s.day_start) + 1;
}

pub fn yearLen(leap: bool, cal: *const base.Cal) u16 {
    const lc = cal.*.leap_cycle;
    if (leap) {
        return lc.common_days + lc.leap_days;
    } else {
        return lc.common_days;
    }
}

fn getSegments(year: i32, cal: *const base.Cal) [*:null]const ?base.Segment {
    if (isLeap(year, cal)) {
        return cal.*.leap_lookup_list;
    } else {
        return cal.*.common_lookup_list;
    }
}

fn monthDayToDoyFromSegments(
    d: base.Date,
    cal: *const base.Cal,
    segments: [*:null]const ?base.Segment,
) base.Err!DoyDate {
    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {
        if (d.month == s.month and d.day >= s.day_start and d.day <= s.day_end) {
            const day_of_month = d.day - s.day_start;
            return DoyDate{ .year = d.year, .doy = s.offset + day_of_month + 1 };
        }
    }

    return base.Err.DateNotFound;
}

fn monthDayToDoy(d: base.Date, cal: *const base.Cal) base.Err!DoyDate {
    return monthDayToDoyFromSegments(d, cal, getSegments(d.year, cal));
}

fn doyToMonthDay(d: DoyDate, cal: *const base.Cal) base.Err!base.Date {
    const segments = getSegments(d.year, cal);

    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {
        const s_end = s.offset + segmentLen(s);
        if (d.doy > s.offset and d.doy <= s_end) {
            const day = @intCast(u8, d.doy - s.offset + s.day_start - 1);
            const res: base.Date = .{
                .year = d.year,
                .month = s.month,
                .day = day,
            };
            return res;
        }
    }
    return base.Err.DoyNotFound;
}

pub fn seekIcRes(d: base.Date, cal: *const base.Cal) ?IcRes {
    if (cal.*.intercalary_list) |ic_list| {
        var ici: u8 = 0;
        while (ic_list[ici]) |res| : (ici += 1) {
            if (res.month == d.month and res.day == d.day) {
                return IcRes{ .ic = res, .ici = ici };
            }
        }
    }
    return null;
}

fn seekIc(d: base.Date, cal: *const base.Cal) ?base.Intercalary {
    if (cal.*.intercalary_list) |ic_list| {
        var ici: u8 = 0;
        while (ic_list[ici]) |res| : (ici += 1) {
            if (res.month == d.month and res.day == d.day) {
                return res;
            }
        }
    }
    return null;
}

fn rollMonth(d: base.Date, offset: i32, cal: *const base.Cal) base.Err!base.Date {
    if (cal.common_month_max != cal.leap_month_max) {
        unreachable;
    }
    const month_max = cal.common_month_max;
    var month_sum: i32 = 0;
    if (@addWithOverflow(i32, d.month, offset, &month_sum)) {
        return base.Err.Overflow;
    }

    const year_shift = @divFloor((month_sum - 1), month_max); //-1 in case month_sum == month_max
    const res: base.Date = .{
        .year = d.year +% year_shift,
        .month = @intCast(u8, clockModulo(month_sum, month_max)),
        .day = d.day,
    };
    return res;
}

fn normDoy(d: DoyDate, cal: *const base.Cal) DoyDate {
    var doy_done: u16 = 0;
    var year: i32 = d.year;
    while (true) {
        const leap = isLeap(year, cal);
        const doy_sum = doy_done +% yearLen(leap, cal);
        if (doy_sum >= d.doy) {
            //doy_done never exceeds d.doy.
            //That makes it slightly easier to determine the result.
            //Use >= for 1-based doy, > for 0-based doy.
            break;
        } else {
            var year_sum: i32 = 0;
            if (@addWithOverflow(i32, year, 1, &year_sum)) {
                break;
            }
            year = year_sum;
            doy_done = doy_sum;
        }
    }
    return .{ .year = year, .doy = d.doy - doy_done };
}

fn skipIntercalary(d: base.Date, cal: *const base.Cal) base.Err!base.Date {
    var prev_day = d;
    while (seekIc(prev_day, cal)) |ic| {
        const prev_leap = isLeap(prev_day.year, cal);
        var dd_doy = ic.day_of_year;
        if (prev_leap) {
            dd_doy = ic.day_of_leap_year;
        }

        var offset: u16 = 1;
        if (yearLen(prev_leap, cal) < dd_doy) {
            offset = 0;
        }

        const dd: DoyDate = .{
            .year = prev_day.year,
            .doy = dd_doy + offset,
        };
        const norm_dd = normDoy(dd, cal);
        const next_day = try doyToMonthDay(norm_dd, cal);
        if (prev_day.year == next_day.year and prev_day.month == next_day.month and prev_day.day == next_day.day) {
            return base.Err.BadCalendar;
        }
        prev_day = next_day;
    }
    return prev_day;
}

fn normDay(d: base.Date, cal: *const base.Cal) base.Err!base.Date {
    const segments = getSegments(d.year, cal);

    var matching_si: u8 = 0;
    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {
        if (s.month == d.month) {
            if (d.day >= s.day_start and d.day <= s.day_end) {
                return d;
            }
            matching_si = si;
        }
    }

    //d.day is too big
    if (segments[matching_si]) |matching_s| {
        //const excess_days = d.day -% matching_s.day_start;
        const dd: DoyDate = .{
            .year = d.year,
            .doy = matching_s.offset +% d.day,
        };

        const norm_dd = normDoy(dd, cal);
        const res = try doyToMonthDay(norm_dd, cal);
        return res;
    } else {
        return base.Err.BadCalendar;
    }
}

fn diffModifier(bigDiff: i32, smallDiff: i32) i32 {
    if (bigDiff > 0 and smallDiff < 0) {
        return -1;
    }
    if (bigDiff < 0 and smallDiff > 0) {
        return 1;
    }
    return 0;
}

fn daysInCycle(cal: *const base.Cal) i32 {
    const lc = cal.*.leap_cycle;
    const common_cycle: i32 = @intCast(i32, yearLen(false, cal)) * lc.year_count;
    const leap_cycle = lc.leap_year_count * lc.leap_days;
    return common_cycle + leap_cycle;
}

//MJD conversions
fn mjdToDoySymmetric(mjd: i32, cal: *const base.Cal) base.Err!DoyDate {
    //Based on http://individual.utoronto.ca/kalendis/Symmetry454-Arithmetic.pdf
    //FixedToSymYear
    //Most differences are caused by trying to convert to integer math.
    const lc = cal.*.leap_cycle;
    const day_shifted: i64 = @intCast(i64, mjd) - @intCast(i64, cal.*.epoch_mjd);
    const num: i64 = @intCast(i64, lc.year_count) * day_shifted;
    const total_cycle = daysInCycle(cal);

    var estimate_y: i64 = @divFloor(num, total_cycle);
    if (@mod(num, total_cycle) != 0) {
        estimate_y += 1;
    }

    if (estimate_y > std.math.maxInt(i32) or estimate_y < std.math.minInt(i32)) {
        return base.Err.Overflow;
    }
    const estimate_start_doy = DoyDate{ .year = @intCast(i32, estimate_y), .doy = 1 };
    const estimate_start_mjd = try doyToMjd(estimate_start_doy, cal);

    var sym_year = estimate_start_doy.year;
    var start_mjd = estimate_start_mjd;
    if (estimate_start_mjd < mjd) {
        if ((mjd - estimate_start_mjd) >= yearLen(false, cal)) {
            const next_year = estimate_start_doy.year + 1;
            const next_start_doy = DoyDate{ .year = next_year, .doy = 1 };
            const next_start_mjd = try doyToMjd(next_start_doy, cal);
            if (next_start_mjd <= mjd) {
                sym_year = next_year;
                start_mjd = next_start_mjd;
            }
        }
    } else if (estimate_start_mjd > mjd) {
        const prev_year = estimate_start_doy.year - 1;
        const prev_start_doy = DoyDate{ .year = prev_year, .doy = 1 };
        const prev_start_mjd = try doyToMjd(prev_start_doy, cal);
        sym_year = prev_year;
        start_mjd = prev_start_mjd;
    }

    const final_doy = mjd - start_mjd + 1;
    if (final_doy < 1) {
        return base.Err.BadCalendar;
    }
    if (final_doy > yearLen(isLeap(sym_year, cal), cal)) {
        return base.Err.BadCalendar;
    }

    return DoyDate{ .year = sym_year, .doy = @intCast(u16, final_doy) };
}

fn mjdToDoyCommon(mjd: i32, cal: *const base.Cal) base.Err!DoyDate {
    //Based on "Calendrical Calculations: The Ultimate Edition"
    //Chapters 1 and 2
    //Most differences are caused by generalizing across calendars.
    const lc = cal.*.leap_cycle;

    var day_shifted: i32 = 0;
    if (@subWithOverflow(i32, mjd, cal.*.epoch_mjd, &day_shifted)) {
        return base.Err.Overflow;
    }

    var day_total: i32 = 0;
    if (@subWithOverflow(i32, day_shifted, lc.offset_days, &day_total)) {
        return base.Err.Overflow;
    }

    var f_4000_quot: i32 = 0;
    var f_4000_rem: i32 = 0;
    var f_400_quot: i32 = 0;
    var f_400_rem: i32 = 0;
    var f_100_quot: i32 = 0;
    var f_100_rem: i32 = 0;
    if (lc.skip4000) {
        const common_4000: i32 = @intCast(i32, yearLen(false, cal)) * 4000;
        const leap_4000: i32 = (1000 - 31) * @intCast(i32, lc.leap_days);
        const total_4000 = common_4000 + leap_4000;
        f_4000_quot = @divFloor(day_total, total_4000);
        f_4000_rem = @mod(day_total, total_4000);
    } else {
        f_4000_rem = day_total;
    }

    if (lc.skip100) {
        const common_400: i32 = @intCast(i32, yearLen(false, cal)) * 400;
        const leap_400: i32 = (100 - 3) * @intCast(i32, lc.leap_days);
        const total_400 = common_400 + leap_400;
        f_400_quot = @divFloor(f_4000_rem, total_400);
        f_400_rem = @mod(f_4000_rem, total_400);

        const common_100: i32 = @intCast(i32, yearLen(false, cal)) * 100;
        const leap_100: i32 = ((100 / lc.year_count) - 1) * @intCast(i32, lc.leap_days);
        const total_100 = common_100 + leap_100;
        f_100_quot = @divFloor(f_400_rem, total_100);
        f_100_rem = @mod(f_400_rem, total_100);
    } else {
        f_100_rem = day_total;
    }

    const total_cycle = daysInCycle(cal);
    const f_cycle_quot = @divFloor(f_100_rem, total_cycle);
    const f_cycle_rem = @mod(f_100_rem, total_cycle);
    const f_common_quot = @divFloor(f_cycle_rem, yearLen(false, cal));
    const f_common_rem = @mod(f_cycle_rem, yearLen(false, cal));

    var res: DoyDate = .{ .year = 0, .doy = 0 };
    res.year = 4000 * f_4000_quot + 400 * f_400_quot + 100 * f_100_quot + lc.year_count * f_cycle_quot + f_common_quot;
    res.year += lc.offset_years;

    if (f_100_quot == lc.year_count or f_common_quot == lc.year_count) {
        res.doy = yearLen(true, cal);
    } else {
        res.year += 1;
        res.doy = @intCast(u16, f_common_rem + 1);
    }
    return res;
}

fn mjdToDoy(mjd: i32, cal: *const base.Cal) base.Err!DoyDate {
    const lc = cal.*.leap_cycle;
    if (lc.leap_year_count > 1) {
        if (lc.symmetric) {
            return mjdToDoySymmetric(mjd, cal);
        } else {
            return base.Err.BadCalendar;
        }
    } else {
        return mjdToDoyCommon(mjd, cal);
    }
}

fn yearStartMjdCommon(year: i32, cal: *const base.Cal) base.Err!i32 {
    const lc = cal.*.leap_cycle;
    //Let's make some assumptions about good calendars.
    if (lc.year_count <= lc.leap_year_count) {
        return base.Err.BadCalendar;
    }
    if (lc.common_days <= lc.leap_days) {
        return base.Err.BadCalendar;
    }

    var off_year: i32 = 0;
    if (@subWithOverflow(i32, year, 1 + lc.offset_years, &off_year)) {
        return base.Err.Overflow;
    }
    var common_days: i32 = 0;
    if (@mulWithOverflow(i32, off_year, lc.common_days, &common_days)) {
        return base.Err.Overflow;
    }
    const f_leap_quot = @divFloor(off_year, @intCast(i32, lc.year_count));

    //At this point, we know off_year * lc.common_days hasn't overflowed.
    //And we also have our assumptions about good calendars.
    //Since leap years are less common than common years,
    //and leap days are less common than common days,
    //we can assume calculating the leap days shouldn't overflow.

    var leap_days: i32 = @intCast(i32, lc.leap_year_count) * @intCast(i32, f_leap_quot) * @intCast(i32, lc.leap_days);
    if (lc.skip4000) {
        const f_4000_quot = @divFloor(off_year, 4000);
        leap_days -= (f_4000_quot * @intCast(i32, lc.leap_days));
    }
    if (lc.skip100) {
        const f_400_quot = @divFloor(off_year, 400);
        leap_days += (f_400_quot * @intCast(i32, lc.leap_days));
        const f_100_quot = @divFloor(off_year, 100);
        leap_days -= (f_100_quot * @intCast(i32, lc.leap_days));
    }

    var off_days: i32 = 0;
    if (@addWithOverflow(i32, common_days, leap_days, &off_days)) {
        return base.Err.Overflow;
    }
    var total_days: i32 = 0;
    if (@addWithOverflow(i32, off_days, lc.offset_days, &total_days)) {
        return base.Err.Overflow;
    }
    var year_start_mjd: i32 = 0;
    if (@addWithOverflow(i32, total_days, cal.*.epoch_mjd - 1, &year_start_mjd)) {
        return base.Err.Overflow;
    }
    return year_start_mjd;
}

fn yearStartMjdSymmetric(year: i32, cal: *const base.Cal) base.Err!i32 {
    const lc = cal.*.leap_cycle;
    const E = year - 1;
    var common_days: i32 = 0;
    if (@mulWithOverflow(i32, E, lc.common_days, &common_days)) {
        return base.Err.Overflow;
    }

    const leap_num = @intCast(i32, lc.leap_year_count) * E - @intCast(i32, lc.offset_years);
    const leap_quot = @divFloor(leap_num, lc.year_count);
    const leap_days = lc.leap_days * leap_quot;

    var total_days: i32 = 0;
    if (@addWithOverflow(i32, common_days, leap_days, &total_days)) {
        return base.Err.Overflow;
    }

    var year_start_mjd: i32 = 0;
    if (@addWithOverflow(i32, total_days, cal.*.epoch_mjd - 1, &year_start_mjd)) {
        return base.Err.Overflow;
    }
    return year_start_mjd;
}

fn doyToMjd(doy: DoyDate, cal: *const base.Cal) base.Err!i32 {
    var year_start_mjd: i32 = 0;
    const lc = cal.*.leap_cycle;
    if (lc.leap_year_count > 1) {
        if (lc.symmetric) {
            year_start_mjd = try yearStartMjdSymmetric(doy.year, cal);
        } else {
            return base.Err.BadCalendar;
        }
    } else {
        year_start_mjd = try yearStartMjdCommon(doy.year, cal);
    }

    var res: i32 = 0;
    if (@addWithOverflow(i32, year_start_mjd, doy.doy, &res)) {
        return base.Err.Overflow;
    }
    return res;
}

//Validation
fn valid_year(d: base.Date, cal: *const base.Cal) bool {
    return (cal.*.year0 or d.year != 0);
}

fn valid_assume_yz(d_yz: base.Date, cal: *const base.Cal) bool {
    const segments = getSegments(d_yz.year, cal);

    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {
        if (d_yz.month == s.month and d_yz.day >= s.day_start and d_yz.day <= s.day_end) {
            return true;
        }
    }
    return false;
}

//Year Zero adjustment
fn yzNeedsAdjustment(y: i32, cal: *const base.Cal) bool {
    return (!cal.*.year0) and (y < 1);
}

fn yzToNoYz(d: base.Date, cal: *const base.Cal) base.Date {
    const y = if (yzNeedsAdjustment(d.year, cal)) (d.year -% 1) else d.year;
    return .{ .year = y, .month = d.month, .day = d.day };
}

fn noYzToYz(d: base.Date, cal: *const base.Cal) base.Date {
    const y = if (yzNeedsAdjustment(d.year, cal)) (d.year +% 1) else d.year;
    return .{ .year = y, .month = d.month, .day = d.day };
}

fn noYzToValidYz(d: base.Date, cal: *const base.Cal) base.Err!base.Date {
    if (valid_year(d, cal)) {
        const d_yz = noYzToYz(d, cal);
        if (valid_assume_yz(d_yz, cal)) {
            return d_yz;
        }
    }
    return base.Err.InvalidDate;
}

//Day of Week
fn getDayOfWeek(d: base.Date, cal: *const base.Cal) base.Err!u8 {
    const w = cal.*.week;
    if (w.continuous) {
        const d_doy = try monthDayToDoy(d, cal);
        const d_mjd = try doyToMjd(d_doy, cal);
        const f_week_rem = @mod(d_mjd, w.length);
        const shifted_f_week_rem = f_week_rem + @enumToInt(base.Weekday7.Wednesday);
        const res = clockModulo(@intCast(i32, shifted_f_week_rem), w.length);
        return @intCast(u8, res);
    } else {
        if (seekIc(d, cal)) |ic| {
            return @enumToInt(base.Weekday7.NoWeekday);
        }

        const f_week_rem = @mod(d.day, w.length);
        const shifted_f_week_rem = f_week_rem + w.start - 1;
        const res = clockModulo(@intCast(i32, shifted_f_week_rem), w.length);
        return @intCast(u4, res);
    }
}

//Unix Time
fn dateToUnix(d: base.Date, cal: *const base.Cal) base.Err!i64 {
    const d_doy = try monthDayToDoy(d, cal);
    const d_mjd = try doyToMjd(d_doy, cal);
    var unix_days: i64 = 0;
    if (@subWithOverflow(i64, @intCast(i64, d_mjd), @intCast(i64, UNIX_EPOCH_IN_MJD), &unix_days)) {
        return base.Err.Overflow;
    }
    return 24 * 60 * 60 * unix_days;
}

fn unixToDate(u: i64, cal: *const base.Cal) base.Err!base.Date {
    const unix_days = @divFloor(u, 24 * 60 * 60);
    if (unix_days > maxInt(i32) or unix_days < minInt(i32)) {
        return base.Err.Overflow;
    }
    const unix_days_32 = @intCast(i32, unix_days);
    var res_mjd: i32 = 0;
    if (@addWithOverflow(i32, unix_days_32, UNIX_EPOCH_IN_MJD, &res_mjd)) {
        return base.Err.Overflow;
    }
    const res_doy = try mjdToDoy(res_mjd, cal);
    const res = try doyToMonthDay(res_doy, cal);
    return res;
}

//Rata Die
fn dateToRd(d: base.Date, cal: *const base.Cal) base.Err!i64 {
    const d_doy = try monthDayToDoy(d, cal);
    const d_mjd = try doyToMjd(d_doy, cal);
    var rata_die: i64 = 0;
    if (@subWithOverflow(i64, @intCast(i64, d_mjd), @intCast(i64, RD_EPOCH_IN_MJD), &rata_die)) {
        return base.Err.Overflow;
    }
    return rata_die;
}

fn rdToDate(rd: i64, cal: *const base.Cal) base.Err!base.Date {
    if (rd > maxInt(i32) or rd < minInt(i32)) {
        return base.Err.Overflow;
    }
    const rd_32 = @intCast(i32, rd);
    var res_mjd: i32 = 0;
    if (@addWithOverflow(i32, rd_32, RD_EPOCH_IN_MJD, &res_mjd)) {
        return base.Err.Overflow;
    }
    const res_doy = try mjdToDoy(res_mjd, cal);
    const res = try doyToMonthDay(res_doy, cal);
    return res;
}

//C99 struct tm
fn C99TmToDate(tm: *const C99Tm) base.Err!base.Date {
    if (tm.*.tm_mday < 1 or tm.*.tm_mday > maxInt(u8)) {
        return base.Err.Overflow;
    }
    if (tm.*.tm_mon < 0 or tm.*.tm_mon > maxInt(u8)) {
        return base.Err.Overflow;
    }
    if (tm.*.tm_year < minInt(i32) or tm.*.tm_year > maxInt(i32)) {
        return base.Err.Overflow;
    }

    return base.Date{
        .day = @intCast(u8, tm.*.tm_mday),
        .month = @intCast(u8, tm.*.tm_mon + 1),
        .year = @intCast(i32, tm.*.tm_year + 1900),
    };
}

//New Public functions
pub fn validYmd(cal: *const base.Cal, year: i32, month: u8, day: u8) bool {
    const d = base.Date{ .year = year, .month = month, .day = day };
    if (valid_year(d, cal)) {
        return valid_assume_yz(noYzToYz(d, cal), cal);
    } else {
        return false;
    }
}

pub fn mjdFromYmd(cal: *const base.Cal, year: i32, month: u8, day: u8) base.Err!i32 {
    const d = base.Date{ .year = year, .month = month, .day = day };
    const d_norm = try noYzToValidYz(d, cal);
    const d_doy = try monthDayToDoy(d_norm, cal);
    return try doyToMjd(d_doy, cal);
}

pub fn mjdFromC99Tm(cal: *const base.Cal, tm: *const c_void) base.Err!i32 {
    const input_c99_tm = @ptrCast(*const C99Tm, @alignCast(@alignOf(C99Tm), tm));
    const d = try C99TmToDate(input_c99_tm);
    const d_yz = try noYzToValidYz(d, cal);
    const d_doy = try monthDayToDoy(d_yz, cal);
    const d_mjd = try doyToMjd(d_doy, cal);
    return d_mjd;
}

pub fn mjdFromUnix(unix: i64) base.Err!i32 {
    const unix_days = @divFloor(unix, 24 * 60 * 60);
    if (unix_days > maxInt(i32) or unix_days < minInt(i32)) {
        return base.Err.Overflow;
    }
    const unix_days_32 = @intCast(i32, unix_days);
    var res_mjd: i32 = 0;
    if (@addWithOverflow(i32, unix_days_32, UNIX_EPOCH_IN_MJD, &res_mjd)) {
        return base.Err.Overflow;
    }
    return res_mjd;
}

pub fn mjdFromRd(rd: i32) base.Err!i32 {
    var res_mjd: i32 = 0;
    if (@addWithOverflow(i32, rd, RD_EPOCH_IN_MJD, &res_mjd)) {
        return base.Err.Overflow;
    }
    return res_mjd;
}

pub fn mjdToYmd(
    mjd: i32,
    cal: *const base.Cal,
    raw_year: ?*i32,
    raw_month: ?*u8,
    raw_day: ?*u8,
) base.Err!void {
    const doy = try mjdToDoy(mjd, cal);
    const res_yz = try doyToMonthDay(doy, cal);
    const d = yzToNoYz(res_yz, cal);
    if (raw_year) |year| {
        year.* = d.year;
    }
    if (raw_month) |month| {
        month.* = d.month;
    }
    if (raw_day) |day| {
        day.* = d.day;
    }
}

pub fn mjdToC99Tm(mjd: i32, cal: *const base.Cal, tm: *c_void) base.Err!void {
    const doy = try mjdToDoy(mjd, cal);
    const d_yz = try doyToMonthDay(doy, cal);
    const d = yzToNoYz(d_yz, cal);
    const weekday = try getDayOfWeek(d, cal);

    var output_c99_tm = @ptrCast(*C99Tm, @alignCast(@alignOf(C99Tm), tm));
    output_c99_tm.*.tm_sec = 0;
    output_c99_tm.*.tm_min = 0;
    output_c99_tm.*.tm_hour = 0;
    output_c99_tm.*.tm_mday = @intCast(c_int, d.day);
    output_c99_tm.*.tm_mon = @intCast(c_int, d.month - 1);
    output_c99_tm.*.tm_year = @intCast(c_int, d.year - 1900);
    output_c99_tm.*.tm_wday = @intCast(c_int, @mod(weekday, 7));
    output_c99_tm.*.tm_yday = doy.doy - 1;
    output_c99_tm.*.tm_isdst = 0;
}

pub fn mjdToUnix(mjd: i32) base.Err!i64 {
    const mjd_64 = @intCast(i64, mjd);
    const epoch = @intCast(i64, UNIX_EPOCH_IN_MJD);
    var unix_days: i64 = 0;
    if (@subWithOverflow(i64, mjd_64, epoch, &unix_days)) {
        return base.Err.Overflow;
    }
    return 24 * 60 * 60 * unix_days;
}

pub fn mjdToRd(mjd: i32) base.Err!i32 {
    var rata_die: i32 = 0;
    if (@subWithOverflow(i32, mjd, RD_EPOCH_IN_MJD, &rata_die)) {
        return base.Err.Overflow;
    }
    return rata_die;
}

pub fn mjdToIsLeapYear(mjd: i32, cal: *const base.Cal) base.Err!bool {
    const doy = try mjdToDoy(mjd, cal);
    const d_yz = try doyToMonthDay(doy, cal);
    return isLeap(d_yz.year, cal);
}

pub fn mjdToDayOfWeek(mjd: i32, cal: *const base.Cal) base.Err!u8 {
    const doy = try mjdToDoy(mjd, cal);
    const d_yz = try doyToMonthDay(doy, cal);
    const weekday = try getDayOfWeek(d_yz, cal);
    return weekday;
}

pub fn mjdToDayOfYear(mjd: i32, cal: *const base.Cal) base.Err!u16 {
    const doy = try mjdToDoy(mjd, cal);
    return doy.doy;
}

pub fn addMonths(mjd: i32, cal: *const base.Cal, offset: i32) base.Err!i32 {
    if (offset == 0) {
        return mjd;
    }
    const doy = try mjdToDoy(mjd, cal);
    const d_yz = try doyToMonthDay(doy, cal);
    const skipped_d = try skipIntercalary(d_yz, cal);
    const rolled_d = try rollMonth(skipped_d, offset, cal);
    const res_yz = try normDay(rolled_d, cal);
    const res_doy = try monthDayToDoy(res_yz, cal);
    return try doyToMjd(res_doy, cal);
}

pub fn addYears(mjd: i32, cal: *const base.Cal, offset: i32) base.Err!i32 {
    if (offset == 0) {
        return mjd;
    }

    const doy = try mjdToDoy(mjd, cal);
    const d_yz = try doyToMonthDay(doy, cal);
    var y: i32 = 0;
    if (@addWithOverflow(i32, d_yz.year, offset, &y)) {
        return base.Err.Overflow;
    }
    const rolled_d = base.Date{ .year = y, .month = d_yz.month, .day = d_yz.day };
    if (valid_assume_yz(rolled_d, cal)) {
        const res_doy = try monthDayToDoy(rolled_d, cal);
        return try doyToMjd(res_doy, cal);
    } else {
        const skipped_d = try skipIntercalary(rolled_d, cal);
        const norm_day = try normDay(skipped_d, cal);
        const res_doy = try monthDayToDoy(norm_day, cal);
        return try doyToMjd(res_doy, cal);
    }
}

pub fn diffMonths(mjd0: i32, mjd1: i32, cal: *const base.Cal) base.Err!i32 {
    const doy0 = try mjdToDoy(mjd0, cal);
    const doy1 = try mjdToDoy(mjd1, cal);
    const d0_norm = try doyToMonthDay(doy0, cal);
    const d1_norm = try doyToMonthDay(doy1, cal);
    const d0_skip = try skipIntercalary(d0_norm, cal);
    const d1_skip = try skipIntercalary(d1_norm, cal);

    if (cal.common_month_max != cal.leap_month_max) {
        unreachable;
    }

    var y_diff: i32 = 0;
    if (@subWithOverflow(i32, d0_skip.year, d1_skip.year, &y_diff)) {
        return base.Err.Overflow;
    }

    const m_diff = @intCast(i32, d0_skip.month) - @intCast(i32, d1_skip.month);
    const d_diff = @intCast(i32, d0_skip.day) - @intCast(i32, d1_skip.day);
    const month_max = @intCast(i32, cal.common_month_max);

    var y_diff_to_m: i32 = 0;
    if (@mulWithOverflow(i32, y_diff, month_max, &y_diff_to_m)) {
        return base.Err.Overflow;
    }
    var ym_diff: i32 = 0;
    if (@addWithOverflow(i32, y_diff_to_m, m_diff, &ym_diff)) {
        return base.Err.Overflow;
    }

    const modifier = diffModifier(ym_diff, d_diff);

    var res = ym_diff;
    if (@addWithOverflow(i32, ym_diff, modifier, &res)) {
        return base.Err.Overflow;
    }
    return res;
}

pub fn diffYears(mjd0: i32, mjd1: i32, cal: *const base.Cal) base.Err!i32 {
    const doy0 = try mjdToDoy(mjd0, cal);
    const doy1 = try mjdToDoy(mjd1, cal);

    var y_diff: i32 = 0;
    if (@subWithOverflow(i32, doy0.year, doy1.year, &y_diff)) {
        return base.Err.Overflow;
    }

    const d0 = try doyToMonthDay(doy0, cal);
    const d1 = try doyToMonthDay(doy1, cal);
    const adjusted0 = try monthDayToDoyFromSegments(d0, cal, cal.*.leap_lookup_list);
    const adjusted1 = try monthDayToDoyFromSegments(d1, cal, cal.*.leap_lookup_list);

    const d_diff = @intCast(i32, adjusted0.doy) - @intCast(i32, adjusted1.doy);
    const modifier = diffModifier(y_diff, d_diff);

    var res: i32 = y_diff;
    if (@addWithOverflow(i32, y_diff, modifier, &res)) {
        return base.Err.Overflow;
    }
    return res;
}
