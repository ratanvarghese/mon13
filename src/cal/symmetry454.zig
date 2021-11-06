const base = @import("../base.zig");
const gen = @import("../gen.zig");
const cal_gr = @import("gregorian.zig");

const COMMON = [_:null]?base.Segment{
    .{ .offset = 28 * 0 + 35 * 0, .month = 1, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 1 + 35 * 0, .month = 2, .day_start = 1, .day_end = 35 },
    .{ .offset = 28 * 1 + 35 * 1, .month = 3, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 2 + 35 * 1, .month = 4, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 3 + 35 * 1, .month = 5, .day_start = 1, .day_end = 35 },
    .{ .offset = 28 * 3 + 35 * 2, .month = 6, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 4 + 35 * 2, .month = 7, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 5 + 35 * 2, .month = 8, .day_start = 1, .day_end = 35 },
    .{ .offset = 28 * 5 + 35 * 3, .month = 9, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 6 + 35 * 3, .month = 10, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 7 + 35 * 3, .month = 11, .day_start = 1, .day_end = 35 },
    .{ .offset = 28 * 7 + 35 * 4, .month = 12, .day_start = 1, .day_end = 28 },
};

const LEAP = [_:null]?base.Segment{
    .{ .offset = 28 * 0 + 35 * 0, .month = 1, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 1 + 35 * 0, .month = 2, .day_start = 1, .day_end = 35 },
    .{ .offset = 28 * 1 + 35 * 1, .month = 3, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 2 + 35 * 1, .month = 4, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 3 + 35 * 1, .month = 5, .day_start = 1, .day_end = 35 },
    .{ .offset = 28 * 3 + 35 * 2, .month = 6, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 4 + 35 * 2, .month = 7, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 5 + 35 * 2, .month = 8, .day_start = 1, .day_end = 35 },
    .{ .offset = 28 * 5 + 35 * 3, .month = 9, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 6 + 35 * 3, .month = 10, .day_start = 1, .day_end = 28 },
    .{ .offset = 28 * 7 + 35 * 3, .month = 11, .day_start = 1, .day_end = 35 },
    .{ .offset = 28 * 7 + 35 * 4, .month = 12, .day_start = 1, .day_end = 35 },
};

var common_var: [COMMON.len:null]?base.Segment = COMMON;
var leap_var: [LEAP.len:null]?base.Segment = LEAP;

pub const symmetry454 = base.Cal{
    .intercalary_list = null,
    .common_lookup_list = @as([*:null]?base.Segment, &common_var),
    .leap_lookup_list = @as([*:null]?base.Segment, &leap_var),
    .leap_cycle = .{
        .year_count = 293,
        .leap_year_count = 52,
        .offset_years = -146,
        .common_days = gen.dayCount(COMMON[0..COMMON.len]),
        .leap_days = gen.leapDayCount(COMMON[0..COMMON.len], LEAP[0..LEAP.len]),
        .offset_days = -53326,
        .skip100 = false,
        .skip4000 = false,
        .symmetric = true,
    },
    .week = .{
        .start = @enumToInt(base.Weekday7.Monday),
        .length = cal_gr.gregorian.week.length,
        .continuous = true,
    },
    .epoch_mjd = cal_gr.gregorian.epoch_mjd,
    .common_month_max = gen.monthMax(COMMON[0..COMMON.len]),
    .leap_month_max = gen.monthMax(LEAP[0..LEAP.len]),
    .year0 = true,
};
