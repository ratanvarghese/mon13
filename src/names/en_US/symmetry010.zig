const base = @import("../../base.zig");
const names_454 = @import("symmetry454.zig");

const NAME = "Symmetry010";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const symmetry010_names_en_US = base.NameList{
    .month_list = names_454.symmetry454_names_en_US.month_list,
    .weekday_list = names_454.symmetry454_names_en_US.weekday_list,
    .era_list = names_454.symmetry454_names_en_US.era_list,
    .intercalary_list = names_454.symmetry454_names_en_US.intercalary_list,
    .alt_intercalary_list = names_454.symmetry454_names_en_US.alt_intercalary_list,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
