const base = @import("../base.zig");
const gen = @import("../gen.zig");
const cal_gr = @import("gregorian.zig");

var tranquility_ic = [_:null]?base.Intercalary{
    .{
        .month = 0,
        .day = 1,
        .day_of_year = 365,
        .day_of_leap_year = 366,
        .IC_YEAR = true,
        .IC_ERA_START_ALT_NAME = true,
        .IC_LEAP = false,
    },
    .{
        .month = 0,
        .day = 2,
        .day_of_year = 224,
        .day_of_leap_year = 224,
        .IC_YEAR = false,
        .IC_ERA_START_ALT_NAME = false,
        .IC_LEAP = true,
    },
};

const COMMON = [_:null]?base.Segment{
    .{ .offset = 0, .month = 1, .day_start = 1, .day_end = 28 },
    .{ .offset = 28, .month = 2, .day_start = 1, .day_end = 28 },
    .{ .offset = 56, .month = 3, .day_start = 1, .day_end = 28 },
    .{ .offset = 84, .month = 4, .day_start = 1, .day_end = 28 },
    .{ .offset = 112, .month = 5, .day_start = 1, .day_end = 28 },
    .{ .offset = 140, .month = 6, .day_start = 1, .day_end = 28 },
    .{ .offset = 168, .month = 7, .day_start = 1, .day_end = 28 },
    .{ .offset = 196, .month = 8, .day_start = 1, .day_end = 28 },
    .{ .offset = 224, .month = 9, .day_start = 1, .day_end = 28 },
    .{ .offset = 252, .month = 10, .day_start = 1, .day_end = 28 },
    .{ .offset = 280, .month = 11, .day_start = 1, .day_end = 28 },
    .{ .offset = 308, .month = 12, .day_start = 1, .day_end = 28 },
    .{ .offset = 336, .month = 13, .day_start = 1, .day_end = 28 },
    .{ .offset = 364, .month = 0, .day_start = 1, .day_end = 1 },
};

const LEAP = [_:null]?base.Segment{
    .{ .offset = 0, .month = 1, .day_start = 1, .day_end = 28 },
    .{ .offset = 28, .month = 2, .day_start = 1, .day_end = 28 },
    .{ .offset = 56, .month = 3, .day_start = 1, .day_end = 28 },
    .{ .offset = 84, .month = 4, .day_start = 1, .day_end = 28 },
    .{ .offset = 112, .month = 5, .day_start = 1, .day_end = 28 },
    .{ .offset = 140, .month = 6, .day_start = 1, .day_end = 28 },
    .{ .offset = 168, .month = 7, .day_start = 1, .day_end = 28 },
    .{ .offset = 196, .month = 8, .day_start = 1, .day_end = 27 },
    .{ .offset = 223, .month = 0, .day_start = 2, .day_end = 2 },
    .{ .offset = 224, .month = 8, .day_start = 28, .day_end = 28 },
    .{ .offset = 225, .month = 9, .day_start = 1, .day_end = 28 },
    .{ .offset = 253, .month = 10, .day_start = 1, .day_end = 28 },
    .{ .offset = 281, .month = 11, .day_start = 1, .day_end = 28 },
    .{ .offset = 309, .month = 12, .day_start = 1, .day_end = 28 },
    .{ .offset = 337, .month = 13, .day_start = 1, .day_end = 28 },
    .{ .offset = 365, .month = 0, .day_start = 1, .day_end = 1 },
};

var common_var: [COMMON.len:null]?base.Segment = COMMON;
var leap_var: [LEAP.len:null]?base.Segment = LEAP;

pub const tranquility = base.Cal{
    .intercalary_list = @as([*:null]?base.Intercalary, &tranquility_ic),
    .common_lookup_list = @as([*:null]?base.Segment, &common_var),
    .leap_lookup_list = @as([*:null]?base.Segment, &leap_var),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 31,
        .common_days = gen.getDayCount(COMMON[0..COMMON.len]),
        .leap_days = gen.getLeapDayCount(COMMON[0..COMMON.len], LEAP[0..LEAP.len]),
        .offset_days = 11323,
        .LEAP_GREGORIAN_SKIP = true,
    },
    .epoch_mjd = 40423, //1 day after Moon Landing Day
    .start_weekday = base.Weekday.MON13_FRIDAY,
    .week_length = cal_gr.gregorian.week_length,
    .common_month_max = gen.getMonthMax(COMMON[0..COMMON.len]),
    .leap_month_max = gen.getMonthMax(LEAP[0..LEAP.len]),
    .CAL_YEAR_ZERO = false,
    .CAL_PERENNIAL = true,
};

pub const tranquility_year_zero = base.Cal{
    .intercalary_list = tranquility.intercalary_list,
    .common_lookup_list = tranquility.common_lookup_list,
    .leap_lookup_list = tranquility.leap_lookup_list,
    .leap_cycle = tranquility.leap_cycle,
    .epoch_mjd = tranquility.epoch_mjd,
    .start_weekday = tranquility.start_weekday,
    .week_length = cal_gr.gregorian.week_length,
    .common_month_max = tranquility.common_month_max,
    .leap_month_max = tranquility.leap_month_max,
    .CAL_YEAR_ZERO = true,
    .CAL_PERENNIAL = tranquility.CAL_PERENNIAL,
};
