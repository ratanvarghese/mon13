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

const LEAP = [_:null]?base.Segment{
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
    .{ .offset = 30 * 12, .month = 0, .day_start = 1, .day_end = 6 },
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
        .day_of_year = gen.getDayOfYear(0, 2, COMMON[0..COMMON.len]),
        .day_of_leap_year = gen.getDayOfYear(0, 2, LEAP[0..LEAP.len]),
        .era_start_alt_name = false,
    },
    .{
        .month = 0,
        .day = 3,
        .day_of_year = gen.getDayOfYear(0, 3, COMMON[0..COMMON.len]),
        .day_of_leap_year = gen.getDayOfYear(0, 3, LEAP[0..LEAP.len]),
        .era_start_alt_name = false,
    },
    .{
        .month = 0,
        .day = 4,
        .day_of_year = gen.getDayOfYear(0, 4, COMMON[0..COMMON.len]),
        .day_of_leap_year = gen.getDayOfYear(0, 4, LEAP[0..LEAP.len]),
        .era_start_alt_name = false,
    },
    .{
        .month = 0,
        .day = 5,
        .day_of_year = gen.getDayOfYear(0, 5, COMMON[0..COMMON.len]),
        .day_of_leap_year = gen.getDayOfYear(0, 5, LEAP[0..LEAP.len]),
        .era_start_alt_name = false,
    },
    .{
        .month = 0,
        .day = 6,
        .day_of_year = gen.getDayOfYear(0, 6, LEAP[0..LEAP.len]),
        .day_of_leap_year = gen.getDayOfYear(0, 6, LEAP[0..LEAP.len]),
        .era_start_alt_name = false,
    },
};

var ic_var: [IC.len:null]?base.Intercalary = IC;
var common_var: [COMMON.len:null]?base.Segment = COMMON;
var leap_var: [LEAP.len:null]?base.Segment = LEAP;

pub const french_revolutionary_romme = base.Cal{
    .intercalary_list = @as([*:null]?base.Intercalary, &ic_var),
    .common_lookup_list = @as([*:null]?base.Segment, &common_var),
    .leap_lookup_list = @as([*:null]?base.Segment, &leap_var),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 0,
        .common_days = gen.dayCount(COMMON[0..COMMON.len]),
        .leap_days = gen.leapDayCount(COMMON[0..COMMON.len], LEAP[0..LEAP.len]),
        .offset_days = 0,
        .skip100 = true,
        .skip4000 = true,
        .symmetric = false,
    },
    .week = .{
        .start = @enumToInt(base.Weekday10.Primidi),
        .length = gen.lastOfEnum(base.Weekday10),
        .continuous = false,
    },
    .epoch_mjd = gen.mjdFromVcalc(-24160.5), //22 Sept, 1792 CE
    .common_month_max = gen.monthMax(COMMON[0..COMMON.len]),
    .leap_month_max = gen.monthMax(LEAP[0..LEAP.len]),
    .year0 = true,
};

pub const french_revolutionary_romme_sub1 = base.Cal{
    .intercalary_list = french_revolutionary_romme.intercalary_list,
    .common_lookup_list = french_revolutionary_romme.common_lookup_list,
    .leap_lookup_list = french_revolutionary_romme.leap_lookup_list,
    .leap_cycle = .{
        .year_count = french_revolutionary_romme.leap_cycle.year_count,
        .leap_year_count = french_revolutionary_romme.leap_cycle.leap_year_count,
        .offset_years = -1,
        .common_days = french_revolutionary_romme.leap_cycle.common_days,
        .leap_days = french_revolutionary_romme.leap_cycle.leap_days,
        .offset_days = -365,
        .skip100 = french_revolutionary_romme.leap_cycle.skip100,
        .skip4000 = french_revolutionary_romme.leap_cycle.skip4000,
        .symmetric = french_revolutionary_romme.leap_cycle.symmetric,
    },
    .week = french_revolutionary_romme.week,
    .epoch_mjd = french_revolutionary_romme.epoch_mjd,
    .common_month_max = french_revolutionary_romme.common_month_max,
    .leap_month_max = french_revolutionary_romme.leap_month_max,
    .year0 = french_revolutionary_romme.year0,
};
