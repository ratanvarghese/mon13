const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

var julian_era_list_en_US = [_:null]?[*:0]const u8{
    "Before Christ",
    "Anno Domini",
};

pub const julian_names_en_US = base.NameList{
    .month_list = names_gr.gregorian_names_en_US.month_list,
    .weekday_list = names_gr.gregorian_names_en_US.weekday_list,
    .era_list = @as([*:null]?[*:0]const u8, &julian_era_list_en_US),
    .intercalary_list = names_gr.gregorian_names_en_US.intercalary_list,
    .alt_intercalary_list = names_gr.gregorian_names_en_US.alt_intercalary_list,
    .calendar_name = "Julian",
};
