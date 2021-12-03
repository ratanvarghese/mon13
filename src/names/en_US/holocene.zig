const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

var holocene_era_list_en_US = [_:null]?[*:0]u8{
    "Before Human Era",
    "Human Era",
};

const NAME = "Holocene";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const names_en_US_holocene = base.NameList{
    .month_list = names_gr.names_en_US_gregorian.month_list,
    .weekday_list = names_gr.names_en_US_gregorian.weekday_list,
    .era_list = @ptrCast([*:null]?[*:0]u8, &holocene_era_list_en_US),
    .intercalary_list = names_gr.names_en_US_gregorian.intercalary_list,
    .alt_intercalary_list = names_gr.names_en_US_gregorian.alt_intercalary_list,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
