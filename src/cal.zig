const base = @import("base.zig");

//Gregorian

var gregorian_common_lookup = [_:null]?base.lkup{
    .{ .offset = 0, .month = 1, .day_start = 1, .day_end = 31 },
    .{ .offset = 31, .month = 2, .day_start = 1, .day_end = 28 },
    .{ .offset = 59, .month = 3, .day_start = 1, .day_end = 31 },
    .{ .offset = 90, .month = 4, .day_start = 1, .day_end = 30 },
    .{ .offset = 120, .month = 5, .day_start = 1, .day_end = 31 },
    .{ .offset = 151, .month = 6, .day_start = 1, .day_end = 30 },
    .{ .offset = 181, .month = 7, .day_start = 1, .day_end = 31 },
    .{ .offset = 212, .month = 8, .day_start = 1, .day_end = 31 },
    .{ .offset = 243, .month = 9, .day_start = 1, .day_end = 30 },
    .{ .offset = 273, .month = 10, .day_start = 1, .day_end = 31 },
    .{ .offset = 304, .month = 11, .day_start = 1, .day_end = 30 },
    .{ .offset = 334, .month = 12, .day_start = 1, .day_end = 31 },
};

var gregorian_leap_lookup = [_:null]?base.lkup{
    .{ .offset = 0, .month = 1, .day_start = 1, .day_end = 31 },
    .{ .offset = 31, .month = 2, .day_start = 1, .day_end = 29 },
    .{ .offset = 60, .month = 3, .day_start = 1, .day_end = 31 },
    .{ .offset = 91, .month = 4, .day_start = 1, .day_end = 30 },
    .{ .offset = 121, .month = 5, .day_start = 1, .day_end = 31 },
    .{ .offset = 152, .month = 6, .day_start = 1, .day_end = 30 },
    .{ .offset = 182, .month = 7, .day_start = 1, .day_end = 31 },
    .{ .offset = 213, .month = 8, .day_start = 1, .day_end = 31 },
    .{ .offset = 244, .month = 9, .day_start = 1, .day_end = 30 },
    .{ .offset = 274, .month = 10, .day_start = 1, .day_end = 31 },
    .{ .offset = 305, .month = 11, .day_start = 1, .day_end = 30 },
    .{ .offset = 335, .month = 12, .day_start = 1, .day_end = 31 },
};

pub export const mon13_gregorian = base.mon13_cal{
    .intercalary_list = null,
    .common_lookup_list = @as([*:null]?base.lkup, &gregorian_common_lookup),
    .leap_lookup_list = @as([*:null]?base.lkup, &gregorian_leap_lookup),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 0,
        .common_days = 365,
        .leap_days = 1,
        .offset_days = 0,
        .LEAP_GREGORIAN_SKIP = true,
    },
    .week_length = 7,
    .start_weekday = base.mon13_weekday.MON13_NO_WEEKDAY,
    .epoch_mjd = -678575, //1 Jan, 1 CE
    .CAL_YEAR_ZERO = false,
    .CAL_PERENNIAL = false,
};

pub export const mon13_gregorian_year_zero = base.mon13_cal{
    .intercalary_list = null,
    .common_lookup_list = @as([*:null]?base.lkup, &gregorian_common_lookup),
    .leap_lookup_list = @as([*:null]?base.lkup, &gregorian_leap_lookup),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 0,
        .common_days = 365,
        .leap_days = 1,
        .offset_days = 0,
        .LEAP_GREGORIAN_SKIP = true,
    },
    .epoch_mjd = -678575, //1 Jan, 1 CE
    .week_length = 7,
    .start_weekday = base.mon13_weekday.MON13_NO_WEEKDAY,
    .CAL_YEAR_ZERO = true,
    .CAL_PERENNIAL = false,
};

//Tranquility

var tranquility_ic = [_:null]?base.intercalary{
    .{
        .month = 0,
        .day = 1,
        .day_of_year = 365,
        .IC_YEAR = true,
        .IC_ERA_START_ALT_NAME = true,
        .IC_LEAP = false,
    },
    .{
        .month = 0,
        .day = 2,
        .day_of_year = 224,
        .IC_YEAR = false,
        .IC_ERA_START_ALT_NAME = false,
        .IC_LEAP = true,
    },
};

var tranquility_common_lookup = [_:null]?base.lkup{
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

var tranquility_leap_lookup = [_:null]?base.lkup{
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

pub export const mon13_tranquility = base.mon13_cal{
    .intercalary_list = @as([*:null]?base.intercalary, &tranquility_ic),
    .common_lookup_list = @as([*:null]?base.lkup, &tranquility_common_lookup),
    .leap_lookup_list = @as([*:null]?base.lkup, &tranquility_leap_lookup),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 31,
        .common_days = 365,
        .leap_days = 1,
        .offset_days = 11323,
        .LEAP_GREGORIAN_SKIP = true,
    },
    .epoch_mjd = 40423, //1 day after Moon Landing Day
    .week_length = 7,
    .start_weekday = base.mon13_weekday.MON13_FRIDAY,
    .CAL_YEAR_ZERO = false,
    .CAL_PERENNIAL = true,
};

pub export const mon13_tranquility_year_zero = base.mon13_cal{
    .intercalary_list = @as([*:null]?base.intercalary, &tranquility_ic),
    .common_lookup_list = @as([*:null]?base.lkup, &tranquility_common_lookup),
    .leap_lookup_list = @as([*:null]?base.lkup, &tranquility_leap_lookup),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 31,
        .common_days = 365,
        .leap_days = 1,
        .offset_days = 11323,
        .LEAP_GREGORIAN_SKIP = true,
    },
    .epoch_mjd = 40423, //1 day after Moon Landing Day
    .week_length = 7,
    .start_weekday = base.mon13_weekday.MON13_FRIDAY,
    .CAL_YEAR_ZERO = true,
    .CAL_PERENNIAL = true,
};
