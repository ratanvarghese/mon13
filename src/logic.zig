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

pub const Err = error{
    Overflow,
    BadCalendar,
    DateNotFound,
    DoyNotFound,
    BadMode,
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

fn isLeap(year: i32, cal: *const base.Cal) Err!bool {
    const y = year;
    const c = @intCast(i32, cal.*.leap_cycle.year_count);
    const l = @intCast(i32, cal.*.leap_cycle.leap_year_count);
    const A = @intCast(i32, cal.*.leap_cycle.offset_years);

    var ly: i32 = 0;
    if (@mulWithOverflow(i32, l, y, &ly)) {
        return Err.Overflow;
    }

    var adjusted: i32 = 0;
    if (@subWithOverflow(i32, ly, A, &adjusted)) {
        return Err.Overflow;
    }

    const m_simple = @mod(adjusted, c);
    const res_simple = m_simple < l;
    if (cal.*.leap_cycle.LEAP_GREGORIAN_SKIP) {
        return res_simple and (@mod(adjusted, 100) != 0 or @mod(adjusted, 400) == 0);
    } else {
        return res_simple;
    }
}

fn lastDayOfSegment(year: i32, s: base.Segment) base.Date {
    return .{ .year = year, .month = s.month, .day = s.day_end };
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

fn getSegments(leap: bool, cal: *const base.Cal) [*:null]const ?base.Segment {
    if (leap) {
        return cal.*.leap_lookup_list;
    } else {
        return cal.*.common_lookup_list;
    }
}

fn lastDayOfYear(year: i32, cal: *const base.Cal) base.Date {
    const leap = isLeap(year, cal) catch false;
    const segments = getSegments(leap, cal);
    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {}
    if (segments[si - 1]) |s| {
        return lastDayOfSegment(year, s);
    } else {
        unreachable; //would require a bad cal
    }
}

fn monthDayToDoy(d: base.Date, cal: *const base.Cal) Err!DoyDate {

    //Assuming d is normalized.
    const leap = try isLeap(d.year, cal);
    const segments = getSegments(leap, cal);

    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {
        if (d.month == s.month and d.day >= s.day_start and d.day <= s.day_end) {
            const day_of_month = d.day - s.day_start;
            return DoyDate{ .year = d.year, .doy = s.offset + day_of_month + 1 };
        }
    }

    return Err.DateNotFound;
}

fn doyToMonthDay(d: DoyDate, cal: *const base.Cal) Err!base.Date {
    const leap = try isLeap(d.year, cal);
    const segments = getSegments(leap, cal);

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
    return Err.DoyNotFound;
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

fn rollMonth(d: base.Date, offset: i32, cal: *const base.Cal) Err!base.Date {
    const leap = isLeap(d.year, cal) catch false;
    const segments = getSegments(leap, cal);
    var month_max: u8 = 1;
    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {
        if (s.month > month_max) {
            month_max = s.month;
        }
    }

    var month_sum: i32 = 0;
    if (@addWithOverflow(i32, d.month, offset, &month_sum)) {
        return Err.Overflow;
    }

    const year_shift = @divFloor((month_sum - 1), month_max); //-1 in case month_sum == month_max
    const res: base.Date = .{
        .year = d.year +% year_shift,
        .month = @intCast(u8, clockModulo(month_sum, month_max)),
        .day = d.day,
    };
    return res;
}

//MJD conversions
fn mjdToDoy(mjd: i32, cal: *const base.Cal) Err!DoyDate {
    const lc = cal.*.leap_cycle;

    var day_shifted: i32 = 0;
    if (@subWithOverflow(i32, mjd, cal.*.epoch_mjd, &day_shifted)) {
        return Err.Overflow;
    }

    var day_total: i32 = 0;
    if (@subWithOverflow(i32, day_shifted, lc.offset_days, &day_total)) {
        return Err.Overflow;
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

fn doyToMjd(doy: DoyDate, cal: *const base.Cal) Err!i32 {
    const lc = cal.*.leap_cycle;
    //Let's make some assumptions about good calendars.
    if (lc.year_count <= lc.leap_year_count) {
        return Err.BadCalendar;
    }
    if (lc.common_days <= lc.leap_days) {
        return Err.BadCalendar;
    }

    var off_year: i32 = 0;
    if (@subWithOverflow(i32, doy.year, 1 + lc.offset_years, &off_year)) {
        return Err.Overflow;
    }
    var common_days: i32 = 0;
    if (@mulWithOverflow(i32, off_year, lc.common_days, &common_days)) {
        return Err.Overflow;
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
        return Err.Overflow;
    }
    var total_days: i32 = 0;
    if (@addWithOverflow(i32, off_days, lc.offset_days, &total_days)) {
        return Err.Overflow;
    }
    var year_start_mjd: i32 = 0;
    if (@addWithOverflow(i32, total_days, cal.*.epoch_mjd - 1, &year_start_mjd)) {
        return Err.Overflow;
    }
    var res: i32 = 0;
    if (@addWithOverflow(i32, year_start_mjd, doy.doy, &res)) {
        return Err.Overflow;
    }
    return res;
}

//Normalization
//Normalization functions cannot return errors:
//they must try their best to make a valid date for any input.
fn normMonth(d: base.Date, cal: *const base.Cal) base.Date {
    //Assuming d.year normalized
    if (d.month == 0) {
        if (seekIc(d, cal)) |ic| {
            return d;
        }
    }
    const res = rollMonth(d, 0, cal) catch unreachable;
    return res;
}

fn normDoy(d: DoyDate, cal: *const base.Cal) DoyDate {
    var doy_done: u16 = 0;
    var year: i32 = d.year;
    while (true) {
        const leap = isLeap(year, cal) catch false;
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

fn icToCommon(year: i32, doy_offset: u16, ic: base.Intercalary, cal: *const base.Cal) base.Date {
    var dd_doy = ic.day_of_year;
    const leap = isLeap(year, cal) catch false;
    if (leap) {
        dd_doy = ic.day_of_leap_year;
    }

    const dd: DoyDate = .{
        .year = year,
        .doy = dd_doy +% doy_offset,
    };
    const norm_dd = normDoy(dd, cal);
    const res = doyToMonthDay(norm_dd, cal) catch lastDayOfYear(norm_dd.year, cal);
    return res;
}

fn normDay(d: base.Date, cal: *const base.Cal) base.Date {

    //Assuming d.year and d.month normalized
    const leap = isLeap(d.year, cal) catch false;
    const segments = getSegments(leap, cal);

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

    if (seekIc(d, cal)) |ic| {
        return icToCommon(d.year, 0, ic, cal);
    }

    if (d.day == 0) {
        if (matching_si == 0) {
            return lastDayOfYear(d.year -% 1, cal);
        } else if (segments[matching_si - 1]) |s| {
            return lastDayOfSegment(d.year, s);
        }
    }

    //At this point, assuming a valid cal, d.day is too big.
    if (segments[matching_si]) |matching_s| {
        //const excess_days = d.day -% matching_s.day_start;
        const dd: DoyDate = .{
            .year = d.year,
            .doy = matching_s.offset +% d.day,
        };

        const norm_dd = normDoy(dd, cal);
        const res = doyToMonthDay(norm_dd, cal) catch lastDayOfYear(d.year, cal);
        return res;
    } else {
        unreachable; //Would require a bad calendar.
    }
}

fn normalize(d: base.Date, cal: *const base.Cal) base.Date {
    const d_normMonth = normMonth(d, cal);
    const d_normDay = normDay(d_normMonth, cal);
    return d_normDay;
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

//Adding
fn addDays(d: base.Date, offset: i32, cal: *const base.Cal) Err!base.Date {
    const d_doy = try monthDayToDoy(d, cal);
    const d_mjd = try doyToMjd(d_doy, cal);
    var res_mjd: i32 = 0;
    if (@addWithOverflow(i32, d_mjd, offset, &res_mjd)) {
        return Err.Overflow;
    }
    const res_doy = try mjdToDoy(res_mjd, cal);
    const res = try doyToMonthDay(res_doy, cal);
    return res;
}

fn addYears(d: base.Date, offset: i32, cal: *const base.Cal) Err!base.Date {
    var y: i32 = 0;
    if (@addWithOverflow(i32, d.year, offset, &y)) {
        return Err.Overflow;
    }
    const res = base.Date{ .year = y, .month = d.month, .day = d.day };
    return normalize(res, cal);
}

fn addMonths(d: base.Date, offset: i32, cal: *const base.Cal) Err!base.Date {
    if (seekIc(d, cal)) |ic| {
        const next_day = icToCommon(d.year, 1, ic, cal);
        return addMonths(next_day, offset, cal);
    }
    const rolled_d = try rollMonth(d, offset, cal);
    return normDay(rolled_d, cal);
}

//Day of Week
fn getDayOfWeek(d: base.Date, cal: *const base.Cal) Err!base.Weekday {
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
fn dateToUnix(d: base.Date, cal: *const base.Cal) Err!i64 {
    const d_doy = try monthDayToDoy(d, cal);
    const d_mjd = try doyToMjd(d_doy, cal);
    var unix_days: i64 = 0;
    if (@subWithOverflow(i64, @intCast(i64, d_mjd), @intCast(i64, UNIX_EPOCH_IN_MJD), &unix_days)) {
        return Err.Overflow;
    }
    return 24 * 60 * 60 * unix_days;
}

fn unixToDate(u: i64, cal: *const base.Cal) Err!base.Date {
    const unix_days = @divFloor(u, 24 * 60 * 60);
    if (unix_days > maxInt(i32) or unix_days < minInt(i32)) {
        return Err.Overflow;
    }
    const unix_days_32 = @intCast(i32, unix_days);
    var res_mjd: i32 = 0;
    if (@addWithOverflow(i32, unix_days_32, UNIX_EPOCH_IN_MJD, &res_mjd)) {
        return Err.Overflow;
    }
    const res_doy = try mjdToDoy(res_mjd, cal);
    const res = try doyToMonthDay(res_doy, cal);
    return res;
}

//Rata Die
fn dateToRd(d: base.Date, cal: *const base.Cal) Err!i64 {
    const d_doy = try monthDayToDoy(d, cal);
    const d_mjd = try doyToMjd(d_doy, cal);
    var rata_die: i64 = 0;
    if (@subWithOverflow(i64, @intCast(i64, d_mjd), @intCast(i64, RD_EPOCH_IN_MJD), &rata_die)) {
        return Err.Overflow;
    }
    return rata_die;
}

fn rdToDate(rd: i64, cal: *const base.Cal) Err!base.Date {
    if (rd > maxInt(i32) or rd < minInt(i32)) {
        return Err.Overflow;
    }
    const rd_32 = @intCast(i32, rd);
    var res_mjd: i32 = 0;
    if (@addWithOverflow(i32, rd_32, RD_EPOCH_IN_MJD, &res_mjd)) {
        return Err.Overflow;
    }
    const res_doy = try mjdToDoy(res_mjd, cal);
    const res = try doyToMonthDay(res_doy, cal);
    return res;
}

//C99 struct tm
fn C99TmToDate(tm: *const C99Tm) Err!base.Date {
    if (tm.*.tm_mday < 1 or tm.*.tm_mday > maxInt(u8)) {
        return Err.Overflow;
    }
    if (tm.*.tm_mon < 0 or tm.*.tm_mon > maxInt(u8)) {
        return Err.Overflow;
    }
    if (tm.*.tm_year < minInt(i32) or tm.*.tm_year > maxInt(i32)) {
        return Err.Overflow;
    }

    return base.Date{
        .day = @intCast(u8, tm.*.tm_mday),
        .month = @intCast(u8, tm.*.tm_mon + 1),
        .year = @intCast(i32, tm.*.tm_year + 1900),
    };
}

//Public functions
pub fn import(
    cal: *const base.Cal,
    input: *const c_void,
    mode: base.ImportMode,
) Err!base.Date {
    switch (mode) {
        base.ImportMode.MJD => {
            const input_mjd = @ptrCast(*const i64, @alignCast(@alignOf(i64), input));
            if (input_mjd.* > maxInt(i32) or input_mjd.* < minInt(i32)) {
                return Err.Overflow;
            }
            const mjd_32 = @intCast(i32, input_mjd.*);
            const doy = try mjdToDoy(mjd_32, cal);
            const res_yz = try doyToMonthDay(doy, cal);
            return yzToNoYz(res_yz, cal);
        },
        base.ImportMode.UNIX => {
            const input_unix = @ptrCast(*const i64, @alignCast(@alignOf(i64), input));
            const res_yz = try unixToDate(input_unix.*, cal);
            return yzToNoYz(res_yz, cal);
        },
        base.ImportMode.RD => {
            const input_rd = @ptrCast(*const i64, @alignCast(@alignOf(i64), input));
            const res_yz = try rdToDate(input_rd.*, cal);
            return yzToNoYz(res_yz, cal);
        },
        base.ImportMode.C99_TM => {
            const input_c99_tm = @ptrCast(*const C99Tm, @alignCast(@alignOf(C99Tm), input));
            return try C99TmToDate(input_c99_tm);
        },
    }
    return Err.BadMode;
}

pub fn convert(
    raw_d: ?*const base.Date,
    raw_src: ?*const base.Cal,
    raw_dest: ?*const base.Cal,
    raw_result: ?*base.Date,
) c_int {
    const d = raw_d orelse return 8;
    var result = raw_result orelse return 7;
    result.* = .{ .year = d.*.year, .month = d.*.month, .day = d.*.day };
    const src = raw_src orelse return 1;
    const dest = raw_dest orelse return 2;

    const src_yz = noYzToYz(d.*, src);
    const src_norm = normalize(src_yz, src);
    if (src == dest) {
        result.* = yzToNoYz(src_norm, src);
        return 0;
    }

    const src_doy = monthDayToDoy(src_norm, src) catch return 3;
    const mjd = doyToMjd(src_doy, src) catch return 4;
    const dest_doy = mjdToDoy(mjd, dest) catch return 5;
    const dest_yz = doyToMonthDay(dest_doy, dest) catch return 6;
    result.* = yzToNoYz(dest_yz, dest);
    return 0;
}
pub fn add(
    raw_d: ?*const base.Date,
    raw_cal: ?*const base.Cal,
    offset: i32,
    mode: base.AddMode,
    raw_result: ?*base.Date,
) c_int {
    var result = raw_result orelse return 5;
    const bad_res: base.Date = .{ .year = 0, .month = 0, .day = 0 };
    result.* = bad_res;
    const cal = raw_cal orelse return 1;
    const d = raw_d orelse return 1;

    const d_yz = noYzToYz(d.*, cal);
    const d_norm = normalize(d_yz, cal);
    var res_yz = d_norm;
    if (offset != 0) { //Adding 0 shouldn't cause errors for valid cal.
        switch (mode) {
            base.AddMode.NONE => {
                res_yz = d_norm;
            },
            base.AddMode.DAYS => {
                res_yz = addDays(d_norm, offset, cal) catch return 2;
            },
            base.AddMode.MONTHS => {
                res_yz = addMonths(d_norm, offset, cal) catch return 3;
            },
            base.AddMode.YEARS => {
                res_yz = addYears(d_norm, offset, cal) catch return 4;
            },
        }
    }
    const res = yzToNoYz(res_yz, cal);
    result.* = res;
    return 0;
}
pub fn compare(
    raw_d0: ?*const base.Date,
    raw_d1: ?*const base.Date,
    raw_cal: ?*const base.Cal,
) c_int {
    const d0 = raw_d0 orelse return 0;
    const d1 = raw_d1 orelse return 0;

    var d0_norm = d0.*;
    var d1_norm = d1.*;
    if (raw_cal) |cal| {
        d0_norm = normalize(noYzToYz(d0.*, cal), cal);
        d1_norm = normalize(noYzToYz(d1.*, cal), cal);
    }

    if (d0_norm.year != d1_norm.year) {
        return d0_norm.year - d1_norm.year;
    }

    if (raw_cal) |cal| {
        if (d0_norm.month == 0 or d1_norm.month == 0) {
            const doy0 = monthDayToDoy(d0_norm, cal) catch return 0;
            const doy1 = monthDayToDoy(d1_norm, cal) catch return 0;
            return @intCast(c_int, doy0.doy) - @intCast(c_int, doy1.doy);
        }
    }

    if (d0_norm.month != d1_norm.month) {
        return @intCast(c_int, d0_norm.month) - @intCast(c_int, d1_norm.month);
    }

    return @intCast(c_int, d0_norm.day) - @intCast(c_int, d1_norm.day);
}
pub fn extract(
    raw_d: ?*const base.Date,
    raw_cal: ?*const base.Cal,
    mode: base.ExtractMode,
) i64 {
    const cal = raw_cal orelse return 0;
    const d = raw_d orelse return 0;
    const d_norm = normalize(noYzToYz(d.*, cal), cal);
    switch (mode) {
        base.ExtractMode.DAY_OF_YEAR => {
            const d_doy = monthDayToDoy(d_norm, cal) catch return 0;
            return d_doy.doy;
        },
        base.ExtractMode.DAY_OF_WEEK => {
            return @enumToInt(getDayOfWeek(d_norm, cal) catch base.Weekday.MON13_NO_WEEKDAY);
        },
        base.ExtractMode.IS_LEAP_YEAR => {
            return @boolToInt(isLeap(d_norm.year, cal) catch false);
        },
        base.ExtractMode.MJD => {
            const d_doy = monthDayToDoy(d_norm, cal) catch return 1;
            const d_mjd = doyToMjd(d_doy, cal) catch return 1;
            return d_mjd;
        },
        base.ExtractMode.UNIX => {
            return dateToUnix(d_norm, cal) catch return 0;
        },
        base.ExtractMode.RD => {
            return dateToRd(d_norm, cal) catch return 0;
        },
    }
    return 0;
}
