const std = @import("std");
const minInt = std.math.minInt;
const maxInt = std.math.maxInt;
const add = std.math.add;
const sub = std.math.sub;
const mul = std.math.mul;
const base = @import("base.zig");
const gen = @import("gen.zig");

const UNIX_EPOCH_IN_MJD: i32 = 40587;
const RD_EPOCH_IN_MJD: i32 = -678576;

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

fn getSegments(year: i32, cal: *const base.Cal) [*:null]const ?base.Segment {
    if (isLeap(year, cal)) {
        return cal.*.leap_lookup_list;
    } else {
        return cal.*.common_lookup_list;
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
    const common_cycle: i32 = @intCast(i32, gen.yearLen(false, cal)) * lc.year_count;
    const leap_cycle = lc.leap_year_count * lc.leap_days;
    return common_cycle + leap_cycle;
}

const YearCycle = struct {
    count: i32 = 0,
    rem: i32 = 0,
    len: i32 = 0,

    fn cycle(days: i32, years: u16, leaps: u16, cal: *const base.Cal) YearCycle {
        const common: i32 = @intCast(i32, gen.yearLen(false, cal)) * years;
        const leap: i32 = @intCast(i32, cal.*.leap_cycle.leap_days) * leaps;
        const total = common + leap;
        return YearCycle{
            .count = @divFloor(days, total),
            .rem = @mod(days, total),
            .len = years,
        };
    }

    fn leapCycle(days: i32, cal: *const base.Cal) YearCycle {
        const lc = cal.*.leap_cycle;
        return YearCycle.cycle(days, lc.year_count, lc.leap_year_count, cal);
    }

    fn prod(self: YearCycle) i32 {
        return self.len * self.count;
    }
};

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

const Date = struct {
    year: i32,
    month: u8,
    day: u8,

    fn toDoyFromSegments(
        self: Date,
        segments: [*:null]const ?base.Segment,
    ) base.Err!DoyDate {
        var si: u8 = 0;
        while (segments[si]) |s| : (si += 1) {
            if (gen.getDayOfYearFromSegment(self.month, self.day, s)) |res_doy| {
                return DoyDate{ .year = self.year, .doy = res_doy };
            }
        }

        return base.Err.DateNotFound;
    }

    fn toDoy(self: Date, cal: *const base.Cal) base.Err!DoyDate {
        return self.toDoyFromSegments(getSegments(self.year, cal));
    }

    fn skipIntercalary(self: Date, cal: *const base.Cal) base.Err!Date {
        var prev_day = self;
        while (gen.seekIc(prev_day.month, prev_day.day, cal)) |ic| {
            const prev_leap = isLeap(prev_day.year, cal);
            var dd_doy = ic.day_of_year;
            if (prev_leap) {
                dd_doy = ic.day_of_leap_year;
            }

            var offset: u16 = 1;
            if (gen.yearLen(prev_leap, cal) < dd_doy) {
                offset = 0;
            }

            const dd: DoyDate = .{
                .year = prev_day.year,
                .doy = dd_doy + offset,
            };
            const norm_dd = try dd.norm(cal);
            const next_day = try norm_dd.toMonthDay(cal);
            if (prev_day.year == next_day.year and prev_day.month == next_day.month and prev_day.day == next_day.day) {
                return base.Err.BadCalendar;
            }
            prev_day = next_day;
        }
        return prev_day;
    }

    fn norm(self: Date, cal: *const base.Cal) base.Err!Date {
        const segments = getSegments(self.year, cal);

        var matching_si: u8 = 0;
        var si: u8 = 0;
        while (segments[si]) |s| : (si += 1) {
            if (s.month == self.month) {
                if (self.day >= s.day_start and self.day <= s.day_end) {
                    return self;
                }
                matching_si = si;
            }
        }

        //d.day is too big
        if (segments[matching_si]) |matching_s| {
            //const excess_days = d.day -% matching_s.day_start;
            const dd: DoyDate = .{
                .year = self.year,
                .doy = matching_s.offset +% self.day,
            };

            const norm_dd = try dd.norm(cal);
            const res = try norm_dd.toMonthDay(cal);
            return res;
        } else {
            return base.Err.BadCalendar;
        }
    }

    //Validation
    fn valid_year(self: Date, cal: *const base.Cal) bool {
        return (cal.*.year0 or self.year != 0);
    }

    fn valid_assume_yz(self: Date, cal: *const base.Cal) bool {
        const segments = getSegments(self.year, cal);

        var si: u8 = 0;
        while (segments[si]) |s| : (si += 1) {
            if (self.month == s.month and self.day >= s.day_start and self.day <= s.day_end) {
                return true;
            }
        }
        return false;
    }

    //Year Zero adjustment
    fn yzNeedsAdjustment(self: Date, cal: *const base.Cal) bool {
        return (!cal.*.year0) and (self.year < 1);
    }

    fn yzToNoYz(self: Date, cal: *const base.Cal) Date {
        const y = if (self.yzNeedsAdjustment(cal)) (self.year -% 1) else self.year;
        return .{ .year = y, .month = self.month, .day = self.day };
    }

    fn noYzToYz(self: Date, cal: *const base.Cal) Date {
        const y = if (self.yzNeedsAdjustment(cal)) (self.year +% 1) else self.year;
        return .{ .year = y, .month = self.month, .day = self.day };
    }

    fn noYzToValidYz(self: Date, cal: *const base.Cal) base.Err!Date {
        if (self.valid_year(cal)) {
            const d_yz = self.noYzToYz(cal);
            if (d_yz.valid_assume_yz(cal)) {
                return d_yz;
            }
        }
        return base.Err.InvalidDate;
    }
};

const DoyDate = struct {
    year: i32,
    doy: u16,

    fn toMonthDay(self: DoyDate, cal: *const base.Cal) base.Err!Date {
        const segments = getSegments(self.year, cal);

        var si: u8 = 0;
        while (segments[si]) |s| : (si += 1) {
            const s_len = (s.day_end - s.day_start) + 1;
            const s_end = s.offset + s_len;
            if (self.doy > s.offset and self.doy <= s_end) {
                const day = @intCast(u8, self.doy - s.offset + s.day_start - 1);
                const res: Date = .{
                    .year = self.year,
                    .month = s.month,
                    .day = day,
                };
                return res;
            }
        }
        return base.Err.DoyNotFound;
    }

    fn norm(self: DoyDate, cal: *const base.Cal) base.Err!DoyDate {
        var doy_done: u16 = 0;
        var year: i32 = self.year;
        while (true) {
            const leap = isLeap(year, cal);
            const doy_sum = try add(u16, doy_done, gen.yearLen(leap, cal));
            if (doy_sum >= self.doy) {
                //doy_done never exceeds d.doy.
                //That makes it slightly easier to determine the result.
                //Use >= for 1-based doy, > for 0-based doy.
                break;
            } else {
                year = try add(i32, year, 1);
                doy_done = doy_sum;
            }
        }
        return DoyDate{ .year = year, .doy = self.doy - doy_done };
    }

    fn toYearStartMjdCommon(self: DoyDate, cal: *const base.Cal) base.Err!i32 {
        const year = self.year;
        const lc = cal.*.leap_cycle;
        //Let's make some assumptions about good calendars.
        if (lc.year_count <= lc.leap_year_count) {
            return base.Err.BadCalendar;
        }
        if (lc.common_days <= lc.leap_days) {
            return base.Err.BadCalendar;
        }

        const off_year = try sub(i32, year, 1 + lc.offset_years);
        const common_days = try mul(i32, off_year, lc.common_days);
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

        const off_days = try add(i32, common_days, leap_days);
        const total_days = try add(i32, off_days, lc.offset_days);
        return add(i32, total_days, cal.*.epoch_mjd - 1);
    }

    fn toYearStartMjdSymmetric(self: DoyDate, cal: *const base.Cal) base.Err!i32 {
        const lc = cal.*.leap_cycle;
        const E = self.year - 1;
        const common_days = try mul(i32, E, lc.common_days);

        const leap_num = @intCast(i32, lc.leap_year_count) * E - @intCast(i32, lc.offset_years);
        const leap_quot = @divFloor(leap_num, lc.year_count);
        const leap_days = lc.leap_days * leap_quot;
        const total_days = try add(i32, common_days, leap_days);

        return add(i32, total_days, cal.*.epoch_mjd - 1);
    }

    fn toMjd(self: DoyDate, cal: *const base.Cal) base.Err!i32 {
        var year_start_mjd: i32 = 0;
        const lc = cal.*.leap_cycle;
        if (lc.leap_year_count > 1) {
            if (lc.symmetric) {
                year_start_mjd = try self.toYearStartMjdSymmetric(cal);
            } else {
                return base.Err.BadCalendar;
            }
        } else {
            year_start_mjd = try self.toYearStartMjdCommon(cal);
        }

        return add(i32, year_start_mjd, self.doy);
    }

    fn fromMjdSymmetric(mjd: i32, cal: *const base.Cal) base.Err!DoyDate {
        //Based on http://individual.utoronto.ca/kalendis/Symmetry454-Arithmetic.pdf
        //FixedToSymYear
        //Most differences are caused by trying to convert to integer math.
        const lc = cal.*.leap_cycle;
        const day_shifted: i64 = @intCast(i64, mjd) - @intCast(i64, cal.*.epoch_mjd);
        const num: i64 = @intCast(i64, lc.year_count) * day_shifted;
        const total_cycle = daysInCycle(cal);

        var y64: i64 = @divFloor(num, total_cycle);
        if (@mod(num, total_cycle) != 0) {
            y64 += 1;
        }

        if (y64 > std.math.maxInt(i32) or y64 < std.math.minInt(i32)) {
            return base.Err.Overflow;
        }
        const estimate = DoyDate{ .year = @intCast(i32, y64), .doy = 1 };
        const estimate_start_mjd = (try estimate.toYearStartMjdSymmetric(cal)) + 1;

        var sym = estimate;
        var start_mjd = estimate_start_mjd;
        if (estimate_start_mjd < mjd) {
            if ((mjd - estimate_start_mjd) >= gen.yearLen(false, cal)) {
                const next_year = DoyDate{ .year = estimate.year + 1, .doy = 1 };
                const next_start_mjd = (try next_year.toYearStartMjdSymmetric(cal)) + 1;
                if (next_start_mjd <= mjd) {
                    sym = next_year;
                    start_mjd = next_start_mjd;
                }
            }
        } else if (estimate_start_mjd > mjd) {
            const prev_year = DoyDate{ .year = estimate.year - 1, .doy = 1 };
            const prev_start_mjd = (try prev_year.toYearStartMjdSymmetric(cal)) + 1;
            sym = prev_year;
            start_mjd = prev_start_mjd;
        }

        const final_doy = mjd - start_mjd + 1;
        if (final_doy < 1) {
            return base.Err.BadCalendar;
        }
        if (final_doy > gen.yearLen(isLeap(sym.year, cal), cal)) {
            return base.Err.BadCalendar;
        }

        return DoyDate{ .year = sym.year, .doy = @intCast(u16, final_doy) };
    }

    fn fromMjdCommon(mjd: i32, cal: *const base.Cal) base.Err!DoyDate {
        //Based on "Calendrical Calculations: The Ultimate Edition"
        //Chapters 1 and 2
        //Most differences are caused by generalizing across calendars.
        const lc = cal.*.leap_cycle;

        const day_shifted = try sub(i32, mjd, cal.*.epoch_mjd);
        const day_total = try sub(i32, day_shifted, lc.offset_days);

        var f_4000 = YearCycle{};
        var f_400 = YearCycle{};
        var f_100 = YearCycle{};
        if (lc.skip4000) {
            f_4000 = YearCycle.cycle(day_total, 4000, 1000 - 31, cal);
        } else {
            f_4000.rem = day_total;
        }
        if (lc.skip100) {
            f_400 = YearCycle.cycle(f_4000.rem, 400, 100 - 3, cal);
            f_100 = YearCycle.cycle(f_400.rem, 100, ((100 / lc.year_count) - 1), cal);
        } else {
            f_100.rem = day_total;
        }
        const f_cycle = YearCycle.leapCycle(f_100.rem, cal);
        const f_common = YearCycle.cycle(f_cycle.rem, 1, 0, cal);

        var res: DoyDate = .{ .year = 0, .doy = 0 };
        res.year = f_4000.prod() + f_400.prod() + f_100.prod() + f_cycle.prod() + f_common.prod();
        res.year += lc.offset_years;

        if (f_100.count == lc.year_count or f_common.count == lc.year_count) {
            res.doy = gen.yearLen(true, cal);
        } else {
            res.year += 1;
            res.doy = @intCast(u16, f_common.rem + 1);
        }
        return res;
    }

    fn fromMjd(mjd: i32, cal: *const base.Cal) base.Err!DoyDate {
        const lc = cal.*.leap_cycle;
        if (lc.leap_year_count > 1) {
            if (lc.symmetric) {
                return DoyDate.fromMjdSymmetric(mjd, cal);
            } else {
                return base.Err.BadCalendar;
            }
        } else {
            return DoyDate.fromMjdCommon(mjd, cal);
        }
    }
};

//Public functions
pub fn validYmd(cal: *const base.Cal, year: i32, month: u8, day: u8) bool {
    const d = Date{ .year = year, .month = month, .day = day };
    if (d.valid_year(cal)) {
        return d.noYzToYz(cal).valid_assume_yz(cal);
    } else {
        return false;
    }
}

pub fn mjdFromYmd(cal: *const base.Cal, year: i32, month: u8, day: u8) base.Err!i32 {
    const d = Date{ .year = year, .month = month, .day = day };
    const d_norm = try d.noYzToValidYz(cal);
    const d_doy = try d_norm.toDoy(cal);
    return try d_doy.toMjd(cal);
}

pub fn mjdFromDayOfYear(cal: *const base.Cal, year: i32, doy: u16) base.Err!i32 {
    const d = Date{ .year = year, .month = 0, .day = 0 };
    const d_norm = try d.noYzToValidYz(cal);
    const d_doy = DoyDate{ .year = d_norm.year, .doy = doy };
    return try d_doy.toMjd(cal);
}

pub fn mjdFromC99Tm(cal: *const base.Cal, raw_tm: *const anyopaque) base.Err!i32 {
    const tm = @ptrCast(*const C99Tm, @alignCast(@alignOf(C99Tm), raw_tm));

    if (tm.*.tm_mday < 1 or tm.*.tm_mday > maxInt(u8)) {
        return base.Err.Overflow;
    }
    if (tm.*.tm_mon < 0 or tm.*.tm_mon > maxInt(u8)) {
        return base.Err.Overflow;
    }
    if (tm.*.tm_year < minInt(i32) or tm.*.tm_year > maxInt(i32)) {
        return base.Err.Overflow;
    }

    const day = @intCast(u8, tm.*.tm_mday);
    const month = @intCast(u8, tm.*.tm_mon + 1);
    const year = @intCast(i32, tm.*.tm_year + 1900);
    return try mjdFromYmd(cal, year, month, day);
}

pub fn mjdFromUnix(unix: i64) base.Err!i32 {
    const unix_days = @divFloor(unix, 24 * 60 * 60);
    if (unix_days > maxInt(i32) or unix_days < minInt(i32)) {
        return base.Err.Overflow;
    }
    const unix_days_32 = @intCast(i32, unix_days);
    return add(i32, unix_days_32, UNIX_EPOCH_IN_MJD);
}

pub fn mjdFromRd(rd: i32) base.Err!i32 {
    return add(i32, rd, RD_EPOCH_IN_MJD);
}

pub fn mjdToYmd(
    mjd: i32,
    cal: *const base.Cal,
    raw_year: ?*i32,
    raw_month: ?*u8,
    raw_day: ?*u8,
) base.Err!void {
    const doy = try DoyDate.fromMjd(mjd, cal);
    const res_yz = try doy.toMonthDay(cal);
    const d = res_yz.yzToNoYz(cal);
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

pub fn mjdToC99Tm(mjd: i32, cal: *const base.Cal, tm: *anyopaque) base.Err!void {
    const doy = try DoyDate.fromMjd(mjd, cal);
    const d_yz = try doy.toMonthDay(cal);
    const d = d_yz.yzToNoYz(cal);
    const weekday = try mjdToDayOfWeek(mjd, cal);

    var output_c99_tm = @ptrCast(*C99Tm, @alignCast(@alignOf(C99Tm), tm));
    output_c99_tm.*.tm_sec = 0;
    output_c99_tm.*.tm_min = 0;
    output_c99_tm.*.tm_hour = 0;
    output_c99_tm.*.tm_mday = @intCast(c_int, d.day);
    output_c99_tm.*.tm_mon = @intCast(c_int, d.month - 1);
    output_c99_tm.*.tm_year = @intCast(c_int, d.year - 1900);
    output_c99_tm.*.tm_wday = @intCast(c_int, @mod(weekday, cal.*.week.length));
    output_c99_tm.*.tm_yday = doy.doy - 1;
    output_c99_tm.*.tm_isdst = 0;
}

pub fn mjdToUnix(mjd: i32) base.Err!i64 {
    const unix_days = try sub(i64, mjd, UNIX_EPOCH_IN_MJD);
    return 24 * 60 * 60 * unix_days;
}

pub fn mjdToRd(mjd: i32) base.Err!i32 {
    return sub(i32, mjd, RD_EPOCH_IN_MJD);
}

pub fn mjdToIsLeapYear(mjd: i32, cal: *const base.Cal) base.Err!bool {
    const doy = try DoyDate.fromMjd(mjd, cal);
    const d_yz = try doy.toMonthDay(cal);
    return isLeap(d_yz.year, cal);
}

pub fn mjdToDayOfWeek(mjd: i32, cal: *const base.Cal) base.Err!u8 {
    const w = cal.*.week;
    if (w.continuous) {
        const f_week_rem = @mod(mjd, w.length);
        const shifted_f_week_rem = f_week_rem + @enumToInt(base.Weekday7.Wednesday);
        const res = clockModulo(@intCast(i32, shifted_f_week_rem), w.length);
        return @intCast(u8, res);
    } else {
        const doy = try DoyDate.fromMjd(mjd, cal);
        const d = try doy.toMonthDay(cal);

        if (gen.seekIc(d.month, d.day, cal)) |_| {
            return @enumToInt(base.Weekday7.NoWeekday);
        }

        const f_week_rem = @mod(d.day, w.length);
        const shifted_f_week_rem = f_week_rem + w.start - 1;
        const res = clockModulo(@intCast(i32, shifted_f_week_rem), w.length);
        return @intCast(u4, res);
    }
}

pub fn mjdToDayOfYear(mjd: i32, cal: *const base.Cal) base.Err!u16 {
    const doy = try DoyDate.fromMjd(mjd, cal);
    return doy.doy;
}

pub fn addMonths(mjd: i32, cal: *const base.Cal, offset: i32) base.Err!i32 {
    if (offset == 0) {
        return mjd;
    }
    const doy = try DoyDate.fromMjd(mjd, cal);
    const d_yz = try doy.toMonthDay(cal);
    const skipped_d = try d_yz.skipIntercalary(cal);

    if (cal.common_month_max != cal.leap_month_max) {
        return base.Err.BadCalendar;
    }
    const month_max = cal.common_month_max;
    const month_sum = try add(i32, skipped_d.month, offset);

    const year_shift = @divFloor((month_sum - 1), month_max); //-1 in case month_sum == month_max
    const rolled_d: Date = .{
        .year = try add(i32, skipped_d.year, year_shift),
        .month = @intCast(u8, clockModulo(month_sum, month_max)),
        .day = skipped_d.day,
    };

    const res_yz = try rolled_d.norm(cal);
    const res_doy = try res_yz.toDoy(cal);
    return try res_doy.toMjd(cal);
}

pub fn addYears(mjd: i32, cal: *const base.Cal, offset: i32) base.Err!i32 {
    if (offset == 0) {
        return mjd;
    }

    const doy = try DoyDate.fromMjd(mjd, cal);
    const d_yz = try doy.toMonthDay(cal);
    var y = try add(i32, d_yz.year, offset);
    const rolled_d = Date{ .year = y, .month = d_yz.month, .day = d_yz.day };
    if (rolled_d.valid_assume_yz(cal)) {
        const res_doy = try rolled_d.toDoy(cal);
        return try res_doy.toMjd(cal);
    } else {
        const skipped_d = try rolled_d.skipIntercalary(cal);
        const norm_day = try skipped_d.norm(cal);
        const res_doy = try norm_day.toDoy(cal);
        return try res_doy.toMjd(cal);
    }
}

pub fn diffMonths(mjd0: i32, mjd1: i32, cal: *const base.Cal) base.Err!i32 {
    const doy0 = try DoyDate.fromMjd(mjd0, cal);
    const doy1 = try DoyDate.fromMjd(mjd1, cal);
    const d0_norm = try doy0.toMonthDay(cal);
    const d1_norm = try doy1.toMonthDay(cal);
    const d0_skip = try d0_norm.skipIntercalary(cal);
    const d1_skip = try d1_norm.skipIntercalary(cal);

    if (cal.common_month_max != cal.leap_month_max) {
        return base.Err.BadCalendar;
    }

    const y_diff = try sub(i32, d0_skip.year, d1_skip.year);
    const m_diff = @intCast(i32, d0_skip.month) - @intCast(i32, d1_skip.month);
    const d_diff = @intCast(i32, d0_skip.day) - @intCast(i32, d1_skip.day);
    const month_max = @intCast(i32, cal.common_month_max);

    const y_diff_to_m = try mul(i32, y_diff, month_max);
    const ym_diff = try add(i32, y_diff_to_m, m_diff);
    const modifier = diffModifier(ym_diff, d_diff);
    const res = try add(i32, ym_diff, modifier);
    return res;
}

pub fn diffYears(mjd0: i32, mjd1: i32, cal: *const base.Cal) base.Err!i32 {
    const doy0 = try DoyDate.fromMjd(mjd0, cal);
    const doy1 = try DoyDate.fromMjd(mjd1, cal);

    const y_diff = try sub(i32, doy0.year, doy1.year);
    const d0 = try doy0.toMonthDay(cal);
    const d1 = try doy1.toMonthDay(cal);
    const segments = cal.*.leap_lookup_list;
    const adjusted0 = try d0.toDoyFromSegments(segments);
    const adjusted1 = try d1.toDoyFromSegments(segments);

    const d_diff = @intCast(i32, adjusted0.doy) - @intCast(i32, adjusted1.doy);
    const modifier = diffModifier(y_diff, d_diff);
    const res = try add(i32, y_diff, modifier);
    return res;
}
