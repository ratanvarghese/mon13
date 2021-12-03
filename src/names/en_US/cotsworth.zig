const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

var cotsworth_month_list = [_:null]?[*:0]u8{
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "Sol",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

var cotsworth_era_list = [_:null]?[*:0]u8{
    "Before Common Era",
    "Common Era",
};

var cotsworth_intercalary_list = [_:null]?[*:0]u8{
    "Year Day",
    "Leap Day",
};

const NAME = "Cotsworth";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const names_en_US_cotsworth = base.NameList{
    .month_list = @ptrCast([*:null]?[*:0]u8, &cotsworth_month_list),
    .weekday_list = names_gr.names_en_US_gregorian.weekday_list,
    .era_list = names_gr.names_en_US_gregorian.era_list,
    .intercalary_list = @ptrCast([*:null]?[*:0]u8, &cotsworth_intercalary_list),
    .alt_intercalary_list = null,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
