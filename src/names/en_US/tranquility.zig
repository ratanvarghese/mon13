const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

var tranquility_month_list_en_US = [_:null]?[*:0]u8{
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

var tranquility_era_list_en_US = [_:null]?[*:0]u8{
    "Before Tranquility",
    "After Tranquility",
};

var tranquility_intercalary_list_en_US = [_:null]?[*:0]u8{
    "Armstrong Day",
    "Aldrin Day",
};

var tranquility_alt_intercalary_list_en_US = [_:null]?[*:0]u8{
    "Moon Landing Day",
    "",
};

const NAME = "Tranquility";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const names_en_US_tranquility = base.NameList{
    .month_list = @ptrCast([*:null]?[*:0]u8, &tranquility_month_list_en_US),
    .weekday_list = names_gr.names_en_US_gregorian.weekday_list,
    .era_list = @ptrCast([*:null]?[*:0]u8, &tranquility_era_list_en_US),
    .intercalary_list = @ptrCast([*:null]?[*:0]u8, &tranquility_intercalary_list_en_US),
    .alt_intercalary_list = @ptrCast([*:null]?[*:0]u8, &tranquility_alt_intercalary_list_en_US),
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
