const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

const NAME = "Symmetry454";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const names_en_US_symmetry454 = base.NameList{
    .month_list = names_gr.names_en_US_gregorian.month_list,
    .weekday_list = names_gr.names_en_US_gregorian.weekday_list,
    .era_list = names_gr.names_en_US_gregorian.era_list,
    .intercalary_list = names_gr.names_en_US_gregorian.intercalary_list,
    .alt_intercalary_list = names_gr.names_en_US_gregorian.alt_intercalary_list,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
