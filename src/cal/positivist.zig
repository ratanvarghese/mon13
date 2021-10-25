const base = @import("../base.zig");
const cal_gr = @import("gregorian.zig");

var positivist_ic = [_:null]?base.Intercalary{
    .{
        .month = 0,
        .day = 1,
        .day_of_year = 365,
        .day_of_leap_year = 365,
        .IC_YEAR = true,
        .IC_ERA_START_ALT_NAME = false,
        .IC_LEAP = false,
    },
    .{
        .month = 0,
        .day = 2,
        .day_of_year = 366,
        .day_of_leap_year = 366,
        .IC_YEAR = false,
        .IC_ERA_START_ALT_NAME = false,
        .IC_LEAP = true,
    },
};

var positivist_common_lookup = [_:null]?base.Segment{
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

var positivist_leap_lookup = [_:null]?base.Segment{
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
    .{ .offset = 364, .month = 0, .day_start = 1, .day_end = 2 },
};

pub const positivist = base.Cal{
    .intercalary_list = @as([*:null]?base.Intercalary, &positivist_ic),
    .common_lookup_list = @as([*:null]?base.Segment, &positivist_common_lookup),
    .leap_lookup_list = @as([*:null]?base.Segment, &positivist_leap_lookup),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 212, //211
        .common_days = 365,
        .leap_days = 1,
        .offset_days = 77430, //77064
        .LEAP_GREGORIAN_SKIP = true,
    },
    .epoch_mjd = -25520, //1 Jan, 1789 CE
    .start_weekday = base.Weekday.MON13_MONDAY,
    .week_length = cal_gr.gregorian.week_length,
    .CAL_YEAR_ZERO = true,
    .CAL_PERENNIAL = true,
};
