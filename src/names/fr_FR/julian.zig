const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

var julian_era_list_fr_FR = [_:null]?[*:0]u8{
    "Avant Jésus-Christ",
    "Anno Domini",
};

const NAME = "Julian";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const julian_names_fr_FR = base.NameList{
    .month_list = names_gr.gregorian_names_fr_FR.month_list,
    .weekday_list = names_gr.gregorian_names_fr_FR.weekday_list,
    .era_list = @ptrCast([*:null]?[*:0]u8, &julian_era_list_fr_FR),
    .intercalary_list = names_gr.gregorian_names_fr_FR.intercalary_list,
    .alt_intercalary_list = names_gr.gregorian_names_fr_FR.alt_intercalary_list,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
