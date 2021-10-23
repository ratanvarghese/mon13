const base = @import("../base.zig");
const cal_gr = @import("gregorian.zig");

var cotsworth_ic = [_:null]?base.Intercalary{
    .{
        .month = 13,
        .day = 29,
        .day_of_year = 365,
        .day_of_leap_year = 366,
        .IC_YEAR = true,
        .IC_ERA_START_ALT_NAME = false,
        .IC_LEAP = false,
    },
    .{
        .month = 6,
        .day = 29,
        .day_of_year = 169,
        .day_of_leap_year = 169,
        .IC_YEAR = false,
        .IC_ERA_START_ALT_NAME = false,
        .IC_LEAP = true,
    },
};

var cotsworth_common_lookup = [_:null]?base.Segment{
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
    .{ .offset = 336, .month = 13, .day_start = 1, .day_end = 29 },
};

var cotsworth_leap_lookup = [_:null]?base.Segment{
    .{ .offset = 0, .month = 1, .day_start = 1, .day_end = 28 },
    .{ .offset = 28, .month = 2, .day_start = 1, .day_end = 28 },
    .{ .offset = 56, .month = 3, .day_start = 1, .day_end = 28 },
    .{ .offset = 84, .month = 4, .day_start = 1, .day_end = 28 },
    .{ .offset = 112, .month = 5, .day_start = 1, .day_end = 28 },
    .{ .offset = 140, .month = 6, .day_start = 1, .day_end = 29 },
    .{ .offset = 169, .month = 7, .day_start = 1, .day_end = 28 },
    .{ .offset = 197, .month = 8, .day_start = 1, .day_end = 28 },
    .{ .offset = 225, .month = 9, .day_start = 1, .day_end = 28 },
    .{ .offset = 253, .month = 10, .day_start = 1, .day_end = 28 },
    .{ .offset = 281, .month = 11, .day_start = 1, .day_end = 28 },
    .{ .offset = 309, .month = 12, .day_start = 1, .day_end = 28 },
    .{ .offset = 337, .month = 13, .day_start = 1, .day_end = 29 },
};

pub const cotsworth = base.Cal{
    .intercalary_list = @as([*:null]?base.Intercalary, &cotsworth_ic),
    .common_lookup_list = @as([*:null]?base.Segment, &cotsworth_common_lookup),
    .leap_lookup_list = @as([*:null]?base.Segment, &cotsworth_leap_lookup),
    .leap_cycle = cal_gr.gregorian.leap_cycle,
    .epoch_mjd = cal_gr.gregorian.epoch_mjd,
    .start_weekday = base.Weekday.MON13_SUNDAY,
    .week_length = cal_gr.gregorian.week_length,
    .CAL_YEAR_ZERO = true,
    .CAL_PERENNIAL = true,
};
