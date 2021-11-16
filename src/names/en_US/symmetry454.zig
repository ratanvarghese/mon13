const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

const NAME = "Symmetry454";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const symmetry454_names_en_US = base.NameList{
    .month_list = names_gr.gregorian_names_en_US.month_list,
    .weekday_list = names_gr.gregorian_names_en_US.weekday_list,
    .era_list = names_gr.gregorian_names_en_US.era_list,
    .intercalary_list = names_gr.gregorian_names_en_US.intercalary_list,
    .alt_intercalary_list = names_gr.gregorian_names_en_US.alt_intercalary_list,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
