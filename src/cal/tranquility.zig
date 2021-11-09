const base = @import("../base.zig");
const gen = @import("../gen.zig");
const cal_gr = @import("gregorian.zig");

const COMMON = [_:null]?base.Segment{
    .{ .offset = 28 * 0, .month = 1, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 1, .month = 2, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 2, .month = 3, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 3, .month = 4, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 4, .month = 5, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 5, .month = 6, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 6, .month = 7, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 7, .month = 8, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 8, .month = 9, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 9, .month = 10, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 10, .month = 11, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 11, .month = 12, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 12, .month = 13, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 13, .month = 0, .day_start = 1, .day_end = 1 },
};

const LEAP = [_:null]?base.Segment{
    .{ .offset = 28 * 0, .month = 1, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 1, .month = 2, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 2, .month = 3, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 3, .month = 4, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 4, .month = 5, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 5, .month = 6, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 6, .month = 7, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 7, .month = 8, .day_start = 1, .day_end = 27 },
    .{ .offset = 28 * 8 - 1, .month = 0, .day_start = 2, .day_end = 2 },
    .{ .offset = 28 * 8 + 0, .month = 8, .day_start = 28, .day_end = 28 },
    .{ .offset = 28 * 8 + 1, .month = 9, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 9 + 1, .month = 10, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 10 + 1, .month = 11, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 11 + 1, .month = 12, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 12 + 1, .month = 13, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 13 + 1, .month = 0, .day_start = 1, .day_end = 1 },
};

const IC = [_:null]?base.Intercalary{
    .{
        .month = 0,
        .day = 1,
        .day_of_year = gen.getDayOfYear(0, 1, COMMON[0..COMMON.len]),
        .day_of_leap_year = gen.getDayOfYear(0, 1, LEAP[0..LEAP.len]),
        .era_start_alt_name = true,
    },
    .{
        .month = 0,
        .day = 2,
        .day_of_year = gen.getDayOfYear(0, 2, LEAP[0..LEAP.len]),
        .day_of_leap_year = gen.getDayOfYear(0, 2, LEAP[0..LEAP.len]),
        .era_start_alt_name = false,
    },
};

var ic_var: [IC.len:null]?base.Intercalary = IC;
var common_var: [COMMON.len:null]?base.Segment = COMMON;
var leap_var: [LEAP.len:null]?base.Segment = LEAP;

pub const tranquility = base.Cal{
    .intercalary_list = @as([*:null]?base.Intercalary, &ic_var),
    .common_lookup_list = @as([*:null]?base.Segment, &common_var),
    .leap_lookup_list = @as([*:null]?base.Segment, &leap_var),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 31,
        .common_days = gen.dayCount(COMMON[0..COMMON.len]),
        .leap_days = gen.leapDayCount(COMMON[0..COMMON.len], LEAP[0..LEAP.len]),
        .offset_days = (31 * 365) + 8,
        .skip100 = true,
        .skip4000 = false,
        .symmetric = false,
    },
    .week = .{
        .start = @enumToInt(base.Weekday7.Friday),
        .length = gen.lastOfEnum(base.Weekday7),
        .continuous = false,
    },
    .epoch_mjd = gen.mjdFromVcalc(40423.5), //1 day after Moon Landing Day
    .common_month_max = gen.monthMax(COMMON[0..COMMON.len]),
    .leap_month_max = gen.monthMax(LEAP[0..LEAP.len]),
    .year0 = false,
};

pub const tranquility_year_zero = base.Cal{
    .intercalary_list = tranquility.intercalary_list,
    .common_lookup_list = tranquility.common_lookup_list,
    .leap_lookup_list = tranquility.leap_lookup_list,
    .leap_cycle = tranquility.leap_cycle,
    .week = tranquility.week,
    .epoch_mjd = tranquility.epoch_mjd,
    .common_month_max = tranquility.common_month_max,
    .leap_month_max = tranquility.leap_month_max,
    .year0 = true,
};
