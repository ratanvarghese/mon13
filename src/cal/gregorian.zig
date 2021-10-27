const base = @import("../base.zig");
const gen = @import("../gen.zig");

const COMMON = [_:null]?base.Segment{
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

const LEAP = [_:null]?base.Segment{
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

var common_var: [COMMON.len:null]?base.Segment = COMMON;
var leap_var: [LEAP.len:null]?base.Segment = LEAP;

pub const gregorian = base.Cal{
    .intercalary_list = null,
    .common_lookup_list = @ptrCast([*:null]?base.Segment, &common_var),
    .leap_lookup_list = @as([*:null]?base.Segment, &leap_var),
    .leap_cycle = .{
        .year_count = 4,
        .leap_year_count = 1,
        .offset_years = 0,
        .common_days = gen.getDayCount(COMMON[0..COMMON.len]),
        .leap_days = gen.getLeapDayCount(COMMON[0..COMMON.len], LEAP[0..LEAP.len]),
        .offset_days = 0,
        .LEAP_GREGORIAN_SKIP = true,
    },
    .start_weekday = base.Weekday.MON13_NO_WEEKDAY,
    .epoch_mjd = -678575, //1 Jan, 1 CE
    .week_length = 7,
    .common_month_max = gen.getMonthMax(COMMON[0..COMMON.len]),
    .leap_month_max = gen.getMonthMax(LEAP[0..LEAP.len]),
    .CAL_YEAR_ZERO = false,
    .CAL_PERENNIAL = false,
};

pub const gregorian_year_zero = base.Cal{
    .intercalary_list = gregorian.intercalary_list,
    .common_lookup_list = gregorian.common_lookup_list,
    .leap_lookup_list = gregorian.leap_lookup_list,
    .leap_cycle = gregorian.leap_cycle,
    .epoch_mjd = gregorian.epoch_mjd,
    .start_weekday = gregorian.start_weekday,
    .week_length = gregorian.week_length,
    .common_month_max = gregorian.common_month_max,
    .leap_month_max = gregorian.leap_month_max,
    .CAL_YEAR_ZERO = true,
    .CAL_PERENNIAL = gregorian.CAL_PERENNIAL,
};
