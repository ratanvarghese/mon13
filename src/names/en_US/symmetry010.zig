const base = @import("../../base.zig");
const names_454 = @import("symmetry454.zig");

const NAME = "Symmetry010";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const names_en_US_symmetry010 = base.NameList{
    .month_list = names_454.names_en_US_symmetry454.month_list,
    .weekday_list = names_454.names_en_US_symmetry454.weekday_list,
    .era_list = names_454.names_en_US_symmetry454.era_list,
    .intercalary_list = names_454.names_en_US_symmetry454.intercalary_list,
    .alt_intercalary_list = names_454.names_en_US_symmetry454.alt_intercalary_list,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
