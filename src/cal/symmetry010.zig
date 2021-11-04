const base = @import("../base.zig");
const gen = @import("../gen.zig");
const cal_454 = @import("symmetry454.zig");

const COMMON = [_:null]?base.Segment{
    .{ .offset = 30 * 0 + 31 * 0, .month = 1, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 1 + 31 * 0, .month = 2, .day_start = 1, .day_end = 31 },
    .{ .offset = 30 * 1 + 31 * 1, .month = 3, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 2 + 31 * 1, .month = 4, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 3 + 31 * 1, .month = 5, .day_start = 1, .day_end = 31 },
    .{ .offset = 30 * 3 + 31 * 2, .month = 6, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 4 + 31 * 2, .month = 7, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 5 + 31 * 2, .month = 8, .day_start = 1, .day_end = 31 },
    .{ .offset = 30 * 5 + 31 * 3, .month = 9, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 6 + 31 * 3, .month = 10, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 7 + 31 * 3, .month = 11, .day_start = 1, .day_end = 31 },
    .{ .offset = 30 * 7 + 31 * 4, .month = 12, .day_start = 1, .day_end = 30 },
};

const LEAP = [_:null]?base.Segment{
    .{ .offset = 30 * 0 + 31 * 0, .month = 1, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 1 + 31 * 0, .month = 2, .day_start = 1, .day_end = 31 },
    .{ .offset = 30 * 1 + 31 * 1, .month = 3, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 2 + 31 * 1, .month = 4, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 3 + 31 * 1, .month = 5, .day_start = 1, .day_end = 31 },
    .{ .offset = 30 * 3 + 31 * 2, .month = 6, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 4 + 31 * 2, .month = 7, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 5 + 31 * 2, .month = 8, .day_start = 1, .day_end = 31 },
    .{ .offset = 30 * 5 + 31 * 3, .month = 9, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 6 + 31 * 3, .month = 10, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 7 + 31 * 3, .month = 11, .day_start = 1, .day_end = 31 },
    .{ .offset = 30 * 7 + 31 * 4, .month = 12, .day_start = 1, .day_end = 37 },
};

var common_var: [COMMON.len:null]?base.Segment = COMMON;
var leap_var: [LEAP.len:null]?base.Segment = LEAP;

pub const symmetry010 = base.Cal{
    .intercalary_list = cal_454.symmetry454.intercalary_list,
    .common_lookup_list = @as([*:null]?base.Segment, &common_var),
    .leap_lookup_list = @as([*:null]?base.Segment, &leap_var),
    .leap_cycle = cal_454.symmetry454.leap_cycle,
    .week = cal_454.symmetry454.week,
    .epoch_mjd = cal_454.symmetry454.epoch_mjd,
    .common_month_max = gen.monthMax(COMMON[0..COMMON.len]),
    .leap_month_max = gen.monthMax(LEAP[0..LEAP.len]),
    .year0 = cal_454.symmetry454.year0,
};
