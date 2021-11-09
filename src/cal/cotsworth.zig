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
    .{ .offset = 28 * 12, .month = 13, .day_start = 1, .day_end = 29 },
};

const LEAP = [_:null]?base.Segment{
    .{ .offset = 28 * 0, .month = 1, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 1, .month = 2, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 2, .month = 3, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 3, .month = 4, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 4, .month = 5, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 5, .month = 6, .day_start = 1, .day_end = 29 },
    .{ .offset = 28 * 6 + 1, .month = 7, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 7 + 1, .month = 8, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 8 + 1, .month = 9, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 9 + 1, .month = 10, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 10 + 1, .month = 11, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 11 + 1, .month = 12, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 12 + 1, .month = 13, .day_start = 1, .day_end = 29 },
};

const IC = [_:null]?base.Intercalary{
    .{
        .month = 13,
        .day = 29,
        .day_of_year = gen.getDayOfYear(13, 29, COMMON[0..COMMON.len]),
        .day_of_leap_year = gen.getDayOfYear(13, 29, LEAP[0..LEAP.len]),
        .era_start_alt_name = false,
    },
    .{
        .month = 6,
        .day = 29,
        .day_of_year = gen.getDayOfYear(6, 29, LEAP[0..LEAP.len]),
        .day_of_leap_year = gen.getDayOfYear(6, 29, LEAP[0..LEAP.len]),
        .era_start_alt_name = false,
    },
};

var ic_var: [IC.len:null]?base.Intercalary = IC;
var common_var: [COMMON.len:null]?base.Segment = COMMON;
var leap_var: [LEAP.len:null]?base.Segment = LEAP;

pub const cotsworth = base.Cal{
    .intercalary_list = @as([*:null]?base.Intercalary, &ic_var),
    .common_lookup_list = @as([*:null]?base.Segment, &common_var),
    .leap_lookup_list = @as([*:null]?base.Segment, &leap_var),
    .leap_cycle = cal_gr.gregorian.leap_cycle,
    .week = .{
        .start = @enumToInt(base.Weekday7.Sunday),
        .length = gen.lastOfEnum(base.Weekday7),
        .continuous = false,
    },
    .epoch_mjd = cal_gr.gregorian.epoch_mjd,
    .common_month_max = gen.monthMax(COMMON[0..COMMON.len]),
    .leap_month_max = gen.monthMax(LEAP[0..LEAP.len]),
    .year0 = true,
};
