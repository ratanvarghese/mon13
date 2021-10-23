const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

var tranquility_month_list_en_US = [_:null]?[*:0]const u8{
    "Archimedes",
    "Brahe",
    "Copernicus",
    "Darwin",
    "Einstein",
    "Faraday",
    "Galileo",
    "Hippocrates",
    "Imhotep",
    "Jung",
    "Kepler",
    "Lavoisier",
    "Mendel",
};

var tranquility_era_list_en_US = [_:null]?[*:0]const u8{
    "Before Tranquility",
    "After Tranquility",
};

var tranquility_intercalary_list_en_US = [_:null]?[*:0]const u8{
    "Armstrong Day",
    "Aldrin Day",
};

var tranquility_alt_intercalary_list_en_US = [_:null]?[*:0]const u8{
    "Moon Landing Day",
    "",
};

pub const tranquility_names_en_US = base.NameList{
    .month_list = @as([*:null]?[*:0]const u8, &tranquility_month_list_en_US),
    .weekday_list = names_gr.gregorian_names_en_US.weekday_list,
    .era_list = @as([*:null]?[*:0]const u8, &tranquility_era_list_en_US),
    .intercalary_list = @as([*:null]?[*:0]const u8, &tranquility_intercalary_list_en_US),
    .alt_intercalary_list = @as([*:null]?[*:0]const u8, &tranquility_alt_intercalary_list_en_US),
    .calendar_name = "Tranquility",
};
