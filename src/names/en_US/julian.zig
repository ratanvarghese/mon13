const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

var julian_era_list_en_US = [_:null]?[*:0]u8{
    "Before Christ",
    "Anno Domini",
};

const NAME = "Julian";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const names_en_US_julian = base.NameList{
    .month_list = names_gr.names_en_US_gregorian.month_list,
    .weekday_list = names_gr.names_en_US_gregorian.weekday_list,
    .era_list = @ptrCast([*:null]?[*:0]u8, &julian_era_list_en_US),
    .intercalary_list = names_gr.names_en_US_gregorian.intercalary_list,
    .alt_intercalary_list = names_gr.names_en_US_gregorian.alt_intercalary_list,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
