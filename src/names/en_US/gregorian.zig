const base = @import("../../base.zig");
const common = @import("common.zig");

var gregorian_month_list = [_:null]?[*:0]const u8{
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

var gregorian_weekday_list = [_:null]?[*:0]const u8{
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday",
};

var gregorian_era_list = [_:null]?[*:0]const u8{
    "Before Common Era",
    "Common Era",
};

pub const gregorian_names_en_US = base.NameList{
    .month_list = @as([*:null]?[*:0]const u8, &gregorian_month_list),
    .weekday_list = @as([*:null]?[*:0]const u8, &gregorian_weekday_list),
    .era_list = @as([*:null]?[*:0]const u8, &gregorian_era_list),
    .intercalary_list = null,
    .alt_intercalary_list = null,
    .calendar_name = "Gregorian",
};
