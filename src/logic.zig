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
    //Prevent overflow by promoting all quantities to i64
    const y = @intCast(i64, year);
    const c = @intCast(i64, cal.*.leap_cycle.year_count);
    const l = @intCast(i64, cal.*.leap_cycle.leap_year_count);
    const A = @intCast(i64, cal.*.leap_cycle.offset_years);

    const adjusted: i64 = (l * y) - A;
    const m_simple = @mod(adjusted, c);
    const res_simple = m_simple < l;
    if (cal.*.leap_cycle.LEAP_GREGORIAN_SKIP) {
        return res_simple and (@mod(adjusted, 100) != 0 or @mod(adjusted, 400) == 0);
    } else {
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
        var dd_doy = ic.day_of_year;
        if (isLeap(prev_day.year, cal)) {
            dd_doy = ic.day_of_leap_year;
        }

        const dd: DoyDate = .{
            .year = prev_day.year,
            .doy = dd_doy + 1,
        };
        const norm_dd = normDoy(dd, cal);
        const next_day = try doyToMonthDay(norm_dd, cal);
        const cmp = compare(prev_day, next_day, null) catch unreachable;
        if (cmp == 0) {
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

fn diffModifier(bigDiff: i64, smallDiff: i64) i64 {
    if (bigDiff > 0 and smallDiff < 0) {
        return -1;
    }
    if (bigDiff < 0 and smallDiff > 0) {
        return 1;
    }
    return 0;
}

//MJD conversions
fn mjdToDoy(mjd: i32, cal: *const base.Cal) base.Err!DoyDate {
    const lc = cal.*.leap_cycle;

    var day_shifted: i32 = 0;
    if (@subWithOverflow(i32, mjd, cal.*.epoch_mjd, &day_shifted)) {
        return base.Err.Overflow;
    }

    var day_total: i32 = 0;
    if (@subWithOverflow(i32, day_shifted, lc.offset_days, &day_total)) {
        return base.Err.Overflow;
    }

    var f_400_quot: i32 = 0;
    var f_400_rem: i32 = 0;
    var f_100_quot: i32 = 0;
    var f_100_rem: i32 = day_total;
    if (lc.LEAP_GREGORIAN_SKIP) {
        const common_400: i32 = @intCast(i32, yearLen(false, cal)) * 400;
        const leap_400: i32 = (100 - 3) * @intCast(i32, lc.leap_days);
        const total_400 = common_400 + leap_400;
        f_400_quot = @divFloor(day_total, total_400);
        f_400_rem = @mod(day_total, total_400);

        const common_100: i32 = @intCast(i32, yearLen(false, cal)) * 100;
        const leap_100: i32 = ((100 / lc.year_count) - 1) * @intCast(i32, lc.leap_days);
        const total_100 = common_100 + leap_100;
        f_100_quot = @divFloor(f_400_rem, total_100);
        f_100_rem = @mod(f_400_rem, total_100);
    }

    const common_cycle: i32 = @intCast(i32, yearLen(false, cal)) * lc.year_count;
    const leap_cycle = lc.leap_year_count * lc.leap_days;
    const total_cycle = common_cycle + leap_cycle;
    const f_cycle_quot = @divFloor(f_100_rem, total_cycle);
    const f_cycle_rem = @mod(f_100_rem, total_cycle);
    if (lc.leap_year_count > 1) {
        unreachable;
    } else {
        const f_common_quot = @divFloor(f_cycle_rem, yearLen(false, cal));
        const f_common_rem = @mod(f_cycle_rem, yearLen(false, cal));

        var res: DoyDate = .{ .year = 0, .doy = 0 };
        res.year = 400 * f_400_quot + 100 * f_100_quot + lc.year_count * f_cycle_quot + f_common_quot;
        res.year += lc.offset_years;

        if (f_100_quot == lc.year_count or f_common_quot == lc.year_count) {
            res.doy = yearLen(true, cal);
        } else {
            res.year += 1;
            res.doy = @intCast(u16, f_common_rem + 1);
        }
        return res;
    }
}

fn doyToMjd(doy: DoyDate, cal: *const base.Cal) base.Err!i32 {
    const lc = cal.*.leap_cycle;
    //Let's make some assumptions about good calendars.
    if (lc.year_count <= lc.leap_year_count) {
        return base.Err.BadCalendar;
    }
    if (lc.common_days <= lc.leap_days) {
        return base.Err.BadCalendar;
    }

    var off_year: i32 = 0;
    if (@subWithOverflow(i32, doy.year, 1 + lc.offset_years, &off_year)) {
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
    if (lc.LEAP_GREGORIAN_SKIP) {
        const f_400_quot = @divFloor(off_year, 400);
        const f_100_quot = @divFloor(off_year, 100);
        leap_days += ((f_400_quot - f_100_quot) * @intCast(i32, lc.leap_days));
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
    var res: i32 = 0;
    if (@addWithOverflow(i32, year_start_mjd, doy.doy, &res)) {
        return base.Err.Overflow;
    }
    return res;
}

//Validation
fn valid_year(d: base.Date, cal: *const base.Cal) bool {
    return (cal.*.CAL_YEAR_ZERO or d.year != 0);
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
    return (!cal.*.CAL_YEAR_ZERO) and (y < 1);
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
fn getDayOfWeek(d: base.Date, cal: *const base.Cal) base.Err!base.Weekday {
    if (cal.*.CAL_PERENNIAL) {
        if (seekIc(d, cal)) |ic| {
            return base.Weekday.MON13_NO_WEEKDAY;
        }

        const f_week_rem = @mod(d.day, cal.*.week_length);
        const shifted_f_week_rem = f_week_rem + @enumToInt(cal.*.start_weekday) - 1;
        const res = clockModulo(@intCast(i32, shifted_f_week_rem), cal.*.week_length);
        return @intToEnum(base.Weekday, @intCast(u8, res));
    } else {
        const d_doy = try monthDayToDoy(d, cal);
        const d_mjd = try doyToMjd(d_doy, cal);
        const f_week_rem = @mod(d_mjd, cal.*.week_length);
        const shifted_f_week_rem = f_week_rem + @enumToInt(base.Weekday.MON13_WEDNESDAY);
        const res = clockModulo(@intCast(i32, shifted_f_week_rem), cal.*.week_length);
        return @intToEnum(base.Weekday, @intCast(u8, res));
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

//Public functions
pub fn valid(
    d: base.Date,
    cal: *const base.Cal,
) bool {
    if (valid_year(d, cal)) {
        return valid_assume_yz(noYzToYz(d, cal), cal);
    } else {
        return false;
    }
}

pub fn importMjd(
    cal: *const base.Cal,
    input_mjd: *const i64,
) base.Err!base.Date {
    if (input_mjd.* > maxInt(i32) or input_mjd.* < minInt(i32)) {
        return base.Err.Overflow;
    }
    const mjd_32 = @intCast(i32, input_mjd.*);
    const doy = try mjdToDoy(mjd_32, cal);
    const res_yz = try doyToMonthDay(doy, cal);
    return yzToNoYz(res_yz, cal);
}

pub fn importUnix(
    cal: *const base.Cal,
    input_unix: *const i64,
) base.Err!base.Date {
    const res_yz = try unixToDate(input_unix.*, cal);
    return yzToNoYz(res_yz, cal);
}

pub fn importRd(
    cal: *const base.Cal,
    input_rd: *const i64,
) base.Err!base.Date {
    const res_yz = try rdToDate(input_rd.*, cal);
    return yzToNoYz(res_yz, cal);
}

pub fn importC99Tm(
    cal: *const base.Cal,
    input: *const c_void,
) base.Err!base.Date {
    const input_c99_tm = @ptrCast(*const C99Tm, @alignCast(@alignOf(C99Tm), input));
    return try C99TmToDate(input_c99_tm);
}

pub fn convert(
    d: base.Date,
    src: *const base.Cal,
    dest: *const base.Cal,
) base.Err!base.Date {
    const src_yz = try noYzToValidYz(d, src);
    if (src == dest) {
        return yzToNoYz(src_yz, src);
    }

    const src_doy = try monthDayToDoy(src_yz, src);
    const mjd = try doyToMjd(src_doy, src);
    const dest_doy = try mjdToDoy(mjd, dest);
    const dest_yz = try doyToMonthDay(dest_doy, dest);
    return yzToNoYz(dest_yz, dest);
}

pub fn addDays(
    d: base.Date,
    cal: *const base.Cal,
    offset: i32,
) base.Err!base.Date {
    const d_yz = try noYzToValidYz(d, cal);
    if (offset == 0) {
        return d;
    }

    const d_doy = try monthDayToDoy(d_yz, cal);
    const d_mjd = try doyToMjd(d_doy, cal);
    var res_mjd: i32 = 0;
    if (@addWithOverflow(i32, d_mjd, offset, &res_mjd)) {
        return base.Err.Overflow;
    }
    const res_doy = try mjdToDoy(res_mjd, cal);
    const res_yz = try doyToMonthDay(res_doy, cal);

    return yzToNoYz(res_yz, cal);
}

pub fn addMonths(
    d: base.Date,
    cal: *const base.Cal,
    offset: i32,
) base.Err!base.Date {
    const d_yz = try noYzToValidYz(d, cal);
    if (offset == 0) {
        return d;
    }

    const skipped_d = try skipIntercalary(d_yz, cal);
    const rolled_d = try rollMonth(skipped_d, offset, cal);
    const res_yz = try normDay(rolled_d, cal);

    return yzToNoYz(res_yz, cal);
}

pub fn addYears(
    d: base.Date,
    cal: *const base.Cal,
    offset: i32,
) base.Err!base.Date {
    const d_yz = try noYzToValidYz(d, cal);
    if (offset == 0) {
        return d;
    }

    var y: i32 = 0;
    if (@addWithOverflow(i32, d_yz.year, offset, &y)) {
        return base.Err.Overflow;
    }
    const res = base.Date{ .year = y, .month = d_yz.month, .day = d_yz.day };

    if (valid_assume_yz(res, cal)) {
        return yzToNoYz(res, cal);
    } else {
        const d_doy = try monthDayToDoy(d_yz, cal);
        const sum_doy = DoyDate{ .year = y, .doy = d_doy.doy };
        const res_doy = normDoy(sum_doy, cal);
        const res_yz = try doyToMonthDay(res_doy, cal);
        return yzToNoYz(res_yz, cal);
    }
}

pub fn diffDays(
    d0: base.Date,
    d1: base.Date,
    cal: *const base.Cal,
) base.Err!i64 {
    const d0_norm = try noYzToValidYz(d0, cal);
    const d0_doy = try monthDayToDoy(d0_norm, cal);
    const mjd0 = try doyToMjd(d0_doy, cal);

    const d1_norm = try noYzToValidYz(d1, cal);
    const d1_doy = try monthDayToDoy(d1_norm, cal);
    const mjd1 = try doyToMjd(d1_doy, cal);
    return mjd0 - mjd1;
}

pub fn diffMonths(
    d0: base.Date,
    d1: base.Date,
    cal: *const base.Cal,
) base.Err!i64 {
    const d0_norm = try noYzToValidYz(d0, cal);
    const d1_norm = try noYzToValidYz(d1, cal);
    const d0_skip = try skipIntercalary(d0_norm, cal);
    const d1_skip = try skipIntercalary(d1_norm, cal);

    if (cal.common_month_max != cal.leap_month_max) {
        unreachable;
    }

    //Prevent overflow by promoting all quantities to i64
    const y_diff = @intCast(i64, d0_skip.year) - @intCast(i64, d1_skip.year);
    const m_diff = @intCast(i64, d0_skip.month) - @intCast(i64, d1_skip.month);
    const d_diff = @intCast(i64, d0_skip.day) - @intCast(i64, d1_skip.day);
    const month_max = @intCast(i64, cal.common_month_max);
    const ym_diff = (y_diff * month_max) + m_diff;
    const modifier = diffModifier(ym_diff, d_diff);
    return ym_diff + modifier;
}

pub fn diffYears(
    d0: base.Date,
    d1: base.Date,
    cal: *const base.Cal,
) base.Err!i64 {
    const d0_norm = try noYzToValidYz(d0, cal);
    const d1_norm = try noYzToValidYz(d1, cal);

    const segments = cal.*.leap_lookup_list; //Avoid shifted doy on leap years
    const doy0 = try monthDayToDoyFromSegments(d0_norm, cal, segments);
    const doy1 = try monthDayToDoyFromSegments(d1_norm, cal, segments);

    //Prevent overflow by promoting all quantities to i64
    const y_diff = @intCast(i64, doy0.year) - @intCast(i64, doy1.year);
    const d_diff = @intCast(i64, doy0.doy) - @intCast(i64, doy1.doy);
    const modifier = diffModifier(y_diff, d_diff);
    return y_diff + modifier;
}

pub fn compare(
    d0: base.Date,
    d1: base.Date,
    raw_cal: ?*const base.Cal,
) base.Err!c_int {
    var d0_norm = d0;
    var d1_norm = d1;
    if (raw_cal) |cal| {
        d0_norm = try noYzToValidYz(d0, cal);
        d1_norm = try noYzToValidYz(d1, cal);
    }

    if (d0_norm.year != d1_norm.year) {
        return d0_norm.year - d1_norm.year;
    }

    if (raw_cal) |cal| {
        if (d0_norm.month == 0 or d1_norm.month == 0) {
            const doy0 = try monthDayToDoy(d0_norm, cal);
            const doy1 = try monthDayToDoy(d1_norm, cal);
            return @intCast(c_int, doy0.doy) - @intCast(c_int, doy1.doy);
        }
    }

    if (d0_norm.month != d1_norm.month) {
        return @intCast(c_int, d0_norm.month) - @intCast(c_int, d1_norm.month);
    }

    return @intCast(c_int, d0_norm.day) - @intCast(c_int, d1_norm.day);
}

pub fn extractDayOfYear(
    d: base.Date,
    cal: *const base.Cal,
) base.Err!i64 {
    const d_norm = try noYzToValidYz(d, cal);
    const d_doy = try monthDayToDoy(d_norm, cal);
    return d_doy.doy;
}

pub fn extractDayOfWeek(
    d: base.Date,
    cal: *const base.Cal,
) base.Err!i64 {
    const d_norm = try noYzToValidYz(d, cal);
    const weekday = try getDayOfWeek(d_norm, cal);
    return @enumToInt(weekday);
}

pub fn extractIsLeapYear(
    d: base.Date,
    cal: *const base.Cal,
) base.Err!i64 {
    const d_norm = try noYzToValidYz(d, cal);
    return @boolToInt(isLeap(d_norm.year, cal));
}

pub fn extractMjd(
    d: base.Date,
    cal: *const base.Cal,
) base.Err!i64 {
    const d_norm = try noYzToValidYz(d, cal);
    const d_doy = try monthDayToDoy(d_norm, cal);
    return try doyToMjd(d_doy, cal);
}

pub fn extractUnix(
    d: base.Date,
    cal: *const base.Cal,
) base.Err!i64 {
    const d_norm = try noYzToValidYz(d, cal);
    return try dateToUnix(d_norm, cal);
}

pub fn extractRd(
    d: base.Date,
    cal: *const base.Cal,
) base.Err!i64 {
    const d_norm = try noYzToValidYz(d, cal);
    return try dateToRd(d_norm, cal);
}
