const base = @import("../base.zig");
const gen = @import("../gen.zig");

const COMMON = [_:null]?base.Segment{
    .{ .offset = 30 * 0, .month = 1, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 1, .month = 2, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 2, .month = 3, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 3, .month = 4, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 4, .month = 5, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 5, .month = 6, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 6, .month = 7, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 7, .month = 8, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 8, .month = 9, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 9, .month = 10, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 10, .month = 11, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 11, .month = 12, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 12, .month = 0, .day_start = 1, .day_end = 5 },
};

const IC = [_:null]?base.Intercalary{
    gen.initIc(.{ .month = 0, .day = 1 }, COMMON[0..COMMON.len], COMMON[0..COMMON.len]),
    gen.initIc(.{ .month = 0, .day = 2 }, COMMON[0..COMMON.len], COMMON[0..COMMON.len]),
    gen.initIc(.{ .month = 0, .day = 3 }, COMMON[0..COMMON.len], COMMON[0..COMMON.len]),
    gen.initIc(.{ .month = 0, .day = 4 }, COMMON[0..COMMON.len], COMMON[0..COMMON.len]),
    gen.initIc(.{ .month = 0, .day = 5 }, COMMON[0..COMMON.len], COMMON[0..COMMON.len]),
};

var ic_var: [IC.len:null]?base.Intercalary = IC;
var common_var: [COMMON.len:null]?base.Segment = COMMON;

pub const ancient_egyptian = base.Cal{
    .intercalary_list = @as([*:null]?base.Intercalary, &ic_var),
    .common_lookup_list = @as([*:null]?base.Segment, &common_var),
    .leap_lookup_list = @as([*:null]?base.Segment, &common_var),
    .leap_cycle = .{
        .year_count = 1,
        .leap_year_count = 0,
        .offset_years = 0,
        .common_days = gen.dayCount(COMMON[0..COMMON.len]),
        .leap_days = 0,
        .offset_days = 0,
        .skip100 = false,
        .skip4000 = false,
        .symmetric = false,
    },
    .week = .{
        .start = @enumToInt(base.Weekday10.Primidi),
        .length = gen.lastOfEnum(base.Weekday10),
        .continuous = false,
    },
    .epoch_mjd = -951363, //Day before 1 Thoth, 1 Nabonassar
    .common_month_max = gen.monthMax(COMMON[0..COMMON.len]),
    .leap_month_max = gen.monthMax(COMMON[0..COMMON.len]),
    .year0 = true,
};
