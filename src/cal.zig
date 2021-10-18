const base = @import("base.zig");

//Gregorian

var gregorian_common_lookup = [_:null]?base.Segment{
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

var gregorian_leap_lookup = [_:null]?base.Segment{
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

pub const gregorian = base.Cal{
    .intercalary_list = null,
    .common_lookup_list = @as([*:null]?base.Segment, &gregorian_common_lookup),
    .leap_lookup_list = @as([*:null]?base.Segment, &gregorian_leap_lookup),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 0,
        .common_days = 365,
        .leap_days = 1,
        .offset_days = 0,
        .LEAP_GREGORIAN_SKIP = true,
    },
    .start_weekday = base.Weekday.MON13_NO_WEEKDAY,
    .epoch_mjd = -678575, //1 Jan, 1 CE
    .week_length = 7,
    .CAL_YEAR_ZERO = false,
    .CAL_PERENNIAL = false,
};

pub const gregorian_year_zero = base.Cal{
    .intercalary_list = null,
    .common_lookup_list = @as([*:null]?base.Segment, &gregorian_common_lookup),
    .leap_lookup_list = @as([*:null]?base.Segment, &gregorian_leap_lookup),
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
    .start_weekday = base.Weekday.MON13_NO_WEEKDAY,
    .week_length = 7,
    .CAL_YEAR_ZERO = true,
    .CAL_PERENNIAL = false,
};

//Tranquility

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

var tranquility_common_lookup = [_:null]?base.Segment{
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

var tranquility_leap_lookup = [_:null]?base.Segment{
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

pub const tranquility = base.Cal{
    .intercalary_list = @as([*:null]?base.Intercalary, &tranquility_ic),
    .common_lookup_list = @as([*:null]?base.Segment, &tranquility_common_lookup),
    .leap_lookup_list = @as([*:null]?base.Segment, &tranquility_leap_lookup),
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
    .start_weekday = base.Weekday.MON13_FRIDAY,
    .week_length = 7,
    .CAL_YEAR_ZERO = false,
    .CAL_PERENNIAL = true,
};

pub const tranquility_year_zero = base.Cal{
    .intercalary_list = @as([*:null]?base.Intercalary, &tranquility_ic),
    .common_lookup_list = @as([*:null]?base.Segment, &tranquility_common_lookup),
    .leap_lookup_list = @as([*:null]?base.Segment, &tranquility_leap_lookup),
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
    .start_weekday = base.Weekday.MON13_FRIDAY,
    .week_length = 7,
    .CAL_YEAR_ZERO = true,
    .CAL_PERENNIAL = true,
};

//Holocene

pub const holocene = base.Cal{
    .intercalary_list = null,
    .common_lookup_list = @as([*:null]?base.Segment, &gregorian_common_lookup),
    .leap_lookup_list = @as([*:null]?base.Segment, &gregorian_leap_lookup),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 0,
        .common_days = 365,
        .leap_days = 1,
        .offset_days = 0,
        .LEAP_GREGORIAN_SKIP = true,
    },
    .epoch_mjd = -4331000, //1 Jan, 1 HE
    .start_weekday = base.Weekday.MON13_NO_WEEKDAY,
    .week_length = 7,
    .CAL_YEAR_ZERO = true,
    .CAL_PERENNIAL = false,
};

//Cotsworth

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
    .start_weekday = base.Weekday.MON13_SUNDAY,
    .week_length = 7,
    .CAL_YEAR_ZERO = true,
    .CAL_PERENNIAL = true,
};
