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
    .{ .offset = 28 * 7, .month = 8, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 8, .month = 9, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 9, .month = 10, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 10, .month = 11, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 11, .month = 12, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 12, .month = 13, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 13, .month = 0, .day_start = 1, .day_end = 2 },
};

const IC = [_:null]?base.Intercalary{
    .{
        .month = 0,
        .day = 1,
        .day_of_year = gen.getDayOfYear(0, 1, COMMON[0..COMMON.len]),
        .day_of_leap_year = gen.getDayOfYear(0, 1, LEAP[0..LEAP.len]),
        .era_start_alt_name = false,
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

pub const positivist = base.Cal{
    .intercalary_list = @as([*:null]?base.Intercalary, &ic_var),
    .common_lookup_list = @as([*:null]?base.Segment, &common_var),
    .leap_lookup_list = @as([*:null]?base.Segment, &leap_var),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 212,
        .common_days = gen.dayCount(COMMON[0..COMMON.len]),
        .leap_days = gen.leapDayCount(COMMON[0..COMMON.len], LEAP[0..LEAP.len]),
        .offset_days = (212 * 365) + 53 - 2,
        .skip100 = true,
        .skip4000 = false,
        .symmetric = false,
    },
    .week = .{
        .start = @enumToInt(base.Weekday7.Monday),
        .length = gen.lastOfEnum(base.Weekday7),
        .continuous = false,
    },
    .epoch_mjd = gen.mjdFromVcalc(-25520.5), //1 Jan, 1789 CE
    .common_month_max = gen.monthMax(COMMON[0..COMMON.len]),
    .leap_month_max = gen.monthMax(LEAP[0..LEAP.len]),
    .year0 = true,
};
