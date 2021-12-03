const base = @import("../../base.zig");
const common = @import("common.zig");

var gregorian_month_list = [_:null]?[*:0]u8{
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

var gregorian_weekday_list = [_:null]?[*:0]u8{
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday",
};

var gregorian_era_list = [_:null]?[*:0]u8{
    "Before Common Era",
    "Common Era",
};

const NAME = "Gregorian";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const names_en_US_gregorian = base.NameList{
    .month_list = @ptrCast([*:null]?[*:0]u8, &gregorian_month_list),
    .weekday_list = @ptrCast([*:null]?[*:0]u8, &gregorian_weekday_list),
    .era_list = @ptrCast([*:null]?[*:0]u8, &gregorian_era_list),
    .intercalary_list = null,
    .alt_intercalary_list = null,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
