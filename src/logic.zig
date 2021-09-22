const std = @import("std");
const minInt = std.math.minInt;
const maxInt = std.math.maxInt;
const base = @import("base.zig");

const doy_date = struct {
    year: i32,
    doy: u16,
};

const Err = error{
    Overflow,
    BadCalendar,
    DateNotFound,
    DoyNotFound,
};

const UNIX_EPOCH_IN_MJD: i32 = 40587;
const RD_EPOCH_IN_MJD: i32 = -678576;

fn clock_modulo(a: u32, b: u32) u32 {
    if (a == 0) {
        return b;
    } else {
        return ((a - 1) % b) + 1;
    }
}

fn is_leap(year: i32, cal: *const base.mon13_cal) Err!bool {
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

fn last_day_of_segment(year: i32, s: base.segment) base.mon13_date {
    return .{ .year = year, .month = s.month, .day = s.day_end };
}

fn segment_len(s: base.segment) u8 {
    return (s.day_end - s.day_start) + 1;
}

fn year_len(leap: bool, cal: *const base.mon13_cal) u16 {
    const lc = cal.*.leap_cycle;
    if (leap) {
        return lc.common_days + lc.leap_days;
    } else {
        return lc.common_days;
    }
}

fn get_segments(leap: bool, cal: *const base.mon13_cal) [*:null]const ?base.segment {
    if (leap) {
        return cal.*.leap_lookup_list;
    } else {
        return cal.*.common_lookup_list;
    }
}

fn last_day_of_year(year: i32, cal: *const base.mon13_cal) base.mon13_date {
    const leap = is_leap(year, cal) catch false;
    const segments = get_segments(leap, cal);
    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {}
    if (segments[si - 1]) |s| {
        return last_day_of_segment(year, s);
    } else {
        unreachable; //would require a bad cal
    }
}

fn month_day_to_doy(d: base.mon13_date, cal: *const base.mon13_cal) Err!doy_date {

    //Assuming d is normalized.
    const leap = try is_leap(d.year, cal);
    const segments = get_segments(leap, cal);

    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {
        if (d.month == s.month and d.day >= s.day_start and d.day <= s.day_end) {
            const day_of_month = d.day - s.day_start;
            return doy_date{ .year = d.year, .doy = s.offset + day_of_month + 1 };
        }
    }

    return Err.DateNotFound;
}

fn doy_to_month_day(d: doy_date, cal: *const base.mon13_cal) Err!base.mon13_date {
    const leap = try is_leap(d.year, cal);
    const segments = get_segments(leap, cal);

    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {
        const s_end = s.offset + segment_len(s);
        if (d.doy > s.offset and d.doy <= s_end) {
            const day = @intCast(u8, d.doy - s.offset + s.day_start - 1);
            const res: base.mon13_date = .{
                .year = d.year,
                .month = s.month,
                .day = day,
            };
            return res;
        }
    }
    return Err.DoyNotFound;
}

fn seek_ic(d: base.mon13_date, cal: *const base.mon13_cal) ?base.intercalary {
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

//MJD conversions
fn mjd_to_doy(mjd: i32, cal: *const base.mon13_cal) Err!doy_date {
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
        const common_400: i32 = @intCast(i32, year_len(false, cal)) * 400;
        const leap_400: i32 = (100 - 3) * @intCast(i32, lc.leap_days);
        const total_400 = common_400 + leap_400;
        f_400_quot = @divFloor(day_total, total_400);
        f_400_rem = @mod(day_total, total_400);

        const common_100: i32 = @intCast(i32, year_len(false, cal)) * 100;
        const leap_100: i32 = ((100 / lc.year_count) - 1) * @intCast(i32, lc.leap_days);
        const total_100 = common_100 + leap_100;
        f_100_quot = @divFloor(f_400_rem, total_100);
        f_100_rem = @mod(f_400_rem, total_100);
    }

    const common_cycle: i32 = @intCast(i32, year_len(false, cal)) * lc.year_count;
    const leap_cycle = lc.leap_year_count * lc.leap_days;
    const total_cycle = common_cycle + leap_cycle;
    const f_cycle_quot = @divFloor(f_100_rem, total_cycle);
    const f_cycle_rem = @mod(f_100_rem, total_cycle);
    const f_common_quot = @divFloor(f_cycle_rem, year_len(false, cal));
    const f_common_rem = @mod(f_cycle_rem, year_len(false, cal));

    var res: doy_date = .{ .year = 0, .doy = 0 };
    res.year = 400 * f_400_quot + 100 * f_100_quot + lc.year_count * f_cycle_quot + f_common_quot;
    res.year += lc.offset_years;

    if (f_100_quot == lc.year_count or f_common_quot == lc.year_count) {
        res.doy = year_len(true, cal);
    } else {
        res.year += 1;
        res.doy = @intCast(u16, f_common_rem + 1);
    }
    return res;
}

fn doy_to_mjd(doy: doy_date, cal: *const base.mon13_cal) Err!i32 {
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
//they must try their best to make a valid mon13_date for any input.
fn norm_month(d: base.mon13_date, cal: *const base.mon13_cal) base.mon13_date {

    //Assuming d.year normalized
    const leap = is_leap(d.year, cal) catch false;
    const segments = get_segments(leap, cal);

    var month_max: u8 = 1;
    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {
        if (s.month == d.month) {
            return d;
        }
        if (s.month > month_max) {
            month_max = s.month;
        }
    }

    const year_shift = d.month / month_max;
    return .{
        .year = d.year +% year_shift,
        .month = @intCast(u8, clock_modulo(d.month, month_max)),
        .day = d.day,
    };
}

fn norm_day_of_year(d: doy_date, cal: *const base.mon13_cal) doy_date {
    var doy_done: u16 = 0;
    var year: i32 = d.year;
    while (true) {
        const leap = is_leap(year, cal) catch false;
        const doy_sum = doy_done +% year_len(leap, cal);
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

fn norm_day(d: base.mon13_date, cal: *const base.mon13_cal) base.mon13_date {

    //Assuming d.year and d.month normalized
    const leap = is_leap(d.year, cal) catch false;
    const segments = get_segments(leap, cal);

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

    if (d.day == 0) {
        if (matching_si == 0) {
            return last_day_of_year(d.year -% 1, cal);
        } else if (segments[matching_si - 1]) |s| {
            return last_day_of_segment(d.year, s);
        }
    }

    //At this point, assuming a valid cal, d.day is too big.
    if (segments[matching_si]) |matching_s| {
        const excess_days = d.day -% matching_s.day_start;
        const dd: doy_date = .{
            .year = d.year,
            .doy = matching_s.offset +% excess_days,
        };

        const norm_dd = norm_day_of_year(dd, cal);
        const res = doy_to_month_day(norm_dd, cal) catch last_day_of_year(d.year, cal);
        return res;
    } else {
        unreachable; //Would require a bad calendar.
    }
}

fn normalize(d: base.mon13_date, cal: *const base.mon13_cal) base.mon13_date {
    const d_norm_month = norm_month(d, cal);
    const d_norm_day = norm_day(d_norm_month, cal);
    return d_norm_day;
}

//Year Zero adjustment
fn yz_needs_adjustment(y: i32, cal: *const base.mon13_cal) bool {
    return (!cal.*.CAL_YEAR_ZERO) and (y < 1);
}

fn yz_to_no_yz(d: base.mon13_date, cal: *const base.mon13_cal) base.mon13_date {
    const y = if (yz_needs_adjustment(d.year, cal)) (d.year -% 1) else d.year;
    return .{ .year = y, .month = d.month, .day = d.day };
}

fn no_yz_to_yz(d: base.mon13_date, cal: *const base.mon13_cal) base.mon13_date {
    const y = if (yz_needs_adjustment(d.year, cal)) (d.year +% 1) else d.year;
    return .{ .year = y, .month = d.month, .day = d.day };
}

//Adding
fn add_days(d: base.mon13_date, offset: i32, cal: *const base.mon13_cal) Err!base.mon13_date {
    const d_doy = try month_day_to_doy(d, cal);
    const d_mjd = try doy_to_mjd(d_doy, cal);
    var res_mjd: i32 = 0;
    if (@addWithOverflow(i32, d_mjd, offset, &res_mjd)) {
        return Err.Overflow;
    }
    const res_doy = try mjd_to_doy(res_mjd, cal);
    const res = try doy_to_month_day(res_doy, cal);
    return res;
}

fn add_years(d: base.mon13_date, offset: i32) Err!base.mon13_date {
    var y: i32 = 0;
    if (@addWithOverflow(i32, d.year, offset, &y)) {
        return Err.Overflow;
    }
    //What about Feb 29, Aldrin Day??
    return base.mon13_date{ .year = y, .month = d.month, .day = d.day };
}

fn add_months(d: base.mon13_date, offset: i32, cal: *const base.mon13_cal) Err!base.mon13_date {
    const must_change_month = (d.month == 0);
    var max_month: u8 = 0;
    var matching_si: u8 = 0;
    var pre_matching_si: u8 = 0;
    var post_matching_si: u8 = 0;
    const leap = try is_leap(d.year, cal);

    const segments = get_segments(leap, cal);
    var si: u8 = 0;
    while (segments[si]) |s| : (si += 1) {
        if (max_month < s.month) {
            max_month = s.month;
        }
        if (d.month == s.month) {
            if (d.day >= s.day_start and d.day <= s.day_end) {
                matching_si = si;
            }
        } else if (must_change_month) {
            if (matching_si == 0) {
                pre_matching_si = si;
            } else if (post_matching_si == 0) {
                post_matching_si = si;
            }
        }
    }

    //Assuming max_month > 0... based on assumption of good calendar
    const f_year_quot = @divFloor(offset, max_month);
    var sum_year = try add_years(d, f_year_quot);
    const month_diff = @mod(offset, max_month);
    if (month_diff == 0) {
        return sum_year;
    }
    if (must_change_month) {
        if (pre_matching_si == 0) {
            if (segments[post_matching_si]) |s| {
                sum_year.month = s.month;
                sum_year.day = s.day_start;
            }
        } else if (segments[pre_matching_si]) |s| {
            sum_year.month = s.month;
            sum_year.day = s.day_end;
        }
    }

    var y: i32 = 0;
    const sum_month = sum_year.month + month_diff;
    if (sum_month > max_month) {
        y = (sum_year.year + 1);
    } else {
        y = sum_year.year;
    }
    var res: base.mon13_date = .{
        .year = y,
        .month = @intCast(u8, clock_modulo(@intCast(u32, sum_month), max_month)),
        .day = sum_year.day,
    };

    const res_leap = try is_leap(sum_year.year, cal);
    const res_segments = get_segments(res_leap, cal);
    var res_si: u8 = 0;
    while (segments[res_si]) |s| : (res_si += 1) {
        if (res.month == s.month) {
            if (sum_year.day >= s.day_start and sum_year.day <= s.day_end) {
                res.day = sum_year.day;
            } else if (sum_year.day > s.day_end) {
                res.day = s.day_end;
            }
        }
    }
    return res;
}

//Day of Week
fn get_day_of_week(d: base.mon13_date, cal: *const base.mon13_cal) Err!base.mon13_weekday {
    if (cal.*.CAL_PERENNIAL) {
        if (seek_ic(d, cal)) |ic| {
            return base.mon13_weekday.MON13_NO_WEEKDAY;
        }

        const f_week_rem = @mod(d.day, cal.*.week_length);
        const shifted_f_week_rem = f_week_rem + @enumToInt(cal.*.start_weekday) - 1;
        const res = clock_modulo(@intCast(u32, shifted_f_week_rem), cal.*.week_length);
        return @intToEnum(base.mon13_weekday, @intCast(u8, res));
    } else {
        const d_doy = try month_day_to_doy(d, cal);
        const d_mjd = try doy_to_mjd(d_doy, cal);
        const f_week_rem = @mod(d_mjd, cal.*.week_length);
        const shifted_f_week_rem = f_week_rem + @enumToInt(base.mon13_weekday.MON13_WEDNESDAY);
        const res = clock_modulo(@intCast(u32, shifted_f_week_rem), cal.*.week_length);
        return @intToEnum(base.mon13_weekday, @intCast(u8, res));
    }
}

//Unix Time
fn date_to_unix(d: base.mon13_date, cal: *const base.mon13_cal) Err!i64 {
    const d_doy = try month_day_to_doy(d, cal);
    const d_mjd = try doy_to_mjd(d_doy, cal);
    var unix_days: i32 = 0;
    if (@subWithOverflow(i32, d_mjd, UNIX_EPOCH_IN_MJD, &unix_days)) {
        return Err.Overflow;
    }
    return 24 * 60 * 60 * @intCast(i64, unix_days);
}

fn unix_to_date(u: i64, cal: *const base.mon13_cal) Err!base.mon13_date {
    const unix_days = @divTrunc(u, 24 * 60 * 60);
    if (unix_days > maxInt(i32) or unix_days < minInt(i32)) {
        return Err.Overflow;
    }
    const unix_days_32 = @intCast(i32, unix_days);
    var res_mjd: i32 = 0;
    if (@addWithOverflow(i32, unix_days_32, UNIX_EPOCH_IN_MJD, &res_mjd)) {
        return Err.Overflow;
    }
    const res_doy = try mjd_to_doy(res_mjd, cal);
    const res = try doy_to_month_day(res_doy, cal);
    return res;
}

//Rata Die
fn date_to_rd(d: base.mon13_date, cal: *const base.mon13_cal) Err!i64 {
    const d_doy = try month_day_to_doy(d, cal);
    const d_mjd = try doy_to_mjd(d_doy, cal);
    var rata_die: i32 = 0;
    if (@subWithOverflow(i32, d_mjd, RD_EPOCH_IN_MJD, &rata_die)) {
        return Err.Overflow;
    }
    return rata_die;
}

fn rd_to_date(rd: i64, cal: *const base.mon13_cal) Err!base.mon13_date {
    if (rd > maxInt(i32) or rd < minInt(i32)) {
        return Err.Overflow;
    }
    const rd_32 = @intCast(i32, rd);
    var res_mjd: i32 = 0;
    if (@addWithOverflow(i32, rd_32, RD_EPOCH_IN_MJD, &res_mjd)) {
        return Err.Overflow;
    }
    const res_doy = try mjd_to_doy(res_mjd, cal);
    const res = try doy_to_month_day(res_doy, cal);
    return res;
}

//Public functions
pub export fn mon13_import(raw_cal: ?*const base.mon13_cal, raw_input: ?*const c_void, mode: base.mon13_import_mode, result: *base.mon13_date) c_int {
    var bad_res: base.mon13_date = .{ .year = 0, .month = 0, .day = 0 };
    result.* = bad_res;
    if (raw_cal) |cal| {
        switch (mode) {
            base.mon13_import_mode.MON13_IMPORT_MJD => {
                if (@ptrCast(?*const i64, @alignCast(@alignOf(i64), raw_input))) |input_mjd| {
                    if (input_mjd.* > maxInt(i32) or input_mjd.* < minInt(i32)) {
                        return 1;
                    }
                    const mjd_32 = @intCast(i32, input_mjd.*);
                    const doy = mjd_to_doy(mjd_32, cal) catch return 1;
                    var res_yz = doy_to_month_day(doy, cal) catch return 2;
                    result.* = yz_to_no_yz(res_yz, cal);
                    return 0;
                }
            },
            base.mon13_import_mode.MON13_IMPORT_UNIX => {
                if (@ptrCast(?*const i64, @alignCast(@alignOf(i64), raw_input))) |input_unix| {
                    var res_yz = unix_to_date(input_unix.*, cal) catch return 3;
                    result.* = yz_to_no_yz(res_yz, cal);
                    return 0;
                }
            },
            base.mon13_import_mode.MON13_IMPORT_RD => {
                if (@ptrCast(?*const i64, @alignCast(@alignOf(i64), raw_input))) |input_rd| {
                    var res_yz = rd_to_date(input_rd.*, cal) catch return 4;
                    result.* = yz_to_no_yz(res_yz, cal);
                    return 0;
                }
            },
        }
    }
    return 1;
}

pub export fn mon13_convert(d: *base.mon13_date, raw_src: ?*const base.mon13_cal, raw_dest: ?*const base.mon13_cal, result: *base.mon13_date) c_int {
    result.* = .{ .year = d.*.year, .month = d.*.month, .day = d.*.day };
    const src = raw_src orelse return 1;
    const dest = raw_dest orelse return 2;

    const src_yz = no_yz_to_yz(d.*, src);
    const src_norm = normalize(src_yz, src);
    if (src == dest) {
        result.* = .{ .year = src_norm.year, .month = src_norm.month, .day = src_norm.day };
        return 0;
    }

    const src_doy = month_day_to_doy(src_norm, src) catch return 3;
    const mjd = doy_to_mjd(src_doy, src) catch return 4;
    const dest_doy = mjd_to_doy(mjd, dest) catch return 5;
    const dest_yz = doy_to_month_day(dest_doy, dest) catch return 6;
    result.* = yz_to_no_yz(dest_yz, dest);
    return 0;
}
pub export fn mon13_add(d: *base.mon13_date, raw_cal: ?*const base.mon13_cal, offset: i32, mode: base.mon13_add_mode, result: *base.mon13_date) c_int {
    const bad_res: base.mon13_date = .{ .year = 0, .month = 0, .day = 0 };
    result.* = bad_res;
    const cal = raw_cal orelse return 1;

    const d_yz = no_yz_to_yz(d.*, cal);
    const d_norm = normalize(d_yz, cal);
    var res_yz: base.mon13_date = bad_res;
    switch (mode) {
        base.mon13_add_mode.MON13_ADD_NONE => {
            res_yz = d_norm;
        },
        base.mon13_add_mode.MON13_ADD_DAYS => {
            res_yz = add_days(d_norm, offset, cal) catch return 2;
        },
        base.mon13_add_mode.MON13_ADD_MONTHS => {
            res_yz = add_months(d_norm, offset, cal) catch return 3;
        },
        base.mon13_add_mode.MON13_ADD_YEARS => {
            res_yz = add_years(d_norm, offset) catch return 4;
        },
    }
    const res = yz_to_no_yz(res_yz, cal);
    result.* = res;
    return 0;
}
pub export fn mon13_compare(
    d0: *const base.mon13_date,
    d1: *const base.mon13_date,
    raw_cal: ?*const base.mon13_cal,
) c_int {
    var d0_norm = d0.*;
    var d1_norm = d1.*;
    if (raw_cal) |cal| {
        d0_norm = normalize(no_yz_to_yz(d0.*, cal), cal);
        d1_norm = normalize(no_yz_to_yz(d1.*, cal), cal);
    }

    if (d0_norm.year != d1_norm.year) {
        return d0_norm.year - d1_norm.year;
    }

    if (raw_cal) |cal| {
        if (d0_norm.month == 0 or d1_norm.month == 0) {
            const doy0 = month_day_to_doy(d0_norm, cal) catch return 0;
            const doy1 = month_day_to_doy(d1_norm, cal) catch return 0;
            return doy0.doy - doy1.doy;
        }
    }

    if (d0_norm.month != d1_norm.month) {
        return d0_norm.month - d1_norm.month;
    }

    return d0_norm.day - d1_norm.day;
}
pub export fn mon13_extract(
    d: *const base.mon13_date,
    raw_cal: ?*const base.mon13_cal,
    mode: base.mon13_extract_mode,
) i64 {
    const cal = raw_cal orelse return 0;
    const d_norm = normalize(no_yz_to_yz(d.*, cal), cal);
    switch (mode) {
        base.mon13_extract_mode.MON13_EXTRACT_DAY_OF_YEAR => {
            const d_doy = month_day_to_doy(d_norm, cal) catch return 0;
            return d_doy.doy;
        },
        base.mon13_extract_mode.MON13_EXTRACT_DAY_OF_WEEK => {
            return @enumToInt(get_day_of_week(d_norm, cal) catch base.mon13_weekday.MON13_NO_WEEKDAY);
        },
        base.mon13_extract_mode.MON13_EXTRACT_IS_LEAP_YEAR => {
            return @boolToInt(is_leap(d_norm.year, cal) catch false);
        },
        base.mon13_extract_mode.MON13_EXTRACT_MJD => {
            const d_doy = month_day_to_doy(d_norm, cal) catch return 1;
            const d_mjd = doy_to_mjd(d_doy, cal) catch return 1;
            return d_mjd;
        },
        base.mon13_extract_mode.MON13_EXTRACT_UNIX => {
            return date_to_unix(d_norm, cal) catch return 0;
        },
        base.mon13_extract_mode.MON13_EXTRACT_RD => {
            return date_to_rd(d_norm, cal) catch return 0;
        },
    }
    return 0;
}