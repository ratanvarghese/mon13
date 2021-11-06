const base = @import("../../base.zig");
const common = @import("common.zig");

//From Calendrical Calculations: The Ultimate Edition
var ancient_egyptian_month_list = [_:null]?[*:0]const u8{
    "Thoth",
    "Phaophi",
    "Athyr",
    "Choiak",
    "Tybi",
    "Mechir",
    "Phamenoth",
    "Pharmuthi",
    "Pachon",
    "Payni",
    "Epiphi",
    "Mesori",
};

//No idea what these would be
var ancient_egyptian_weekday_list = [_:null]?[*:0]const u8{
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
    "?",
};

//The Egyptians had eras for all their pharohs, but we only bother with 1 for now
var ancient_egyptian_era_list = [_:null]?[*:0]const u8{
    "Before Nabonassar Era",
    "Nabonassar Era",
};

var ancient_egyptian_intercalary_list = [_:null]?[*:0]const u8{
    "Birthday of Osiris",
    "Birthday of Horus",
    "Birthday of Set",
    "Birthday of Isis",
    "Birthday of Nephthys",
};

pub const ancient_egyptian_names_en_US = base.NameList{
    .month_list = @as([*:null]?[*:0]const u8, &ancient_egyptian_month_list),
    .weekday_list = @as([*:null]?[*:0]const u8, &ancient_egyptian_weekday_list),
    .era_list = @as([*:null]?[*:0]const u8, &ancient_egyptian_era_list),
    .intercalary_list = @as([*:null]?[*:0]const u8, &ancient_egyptian_intercalary_list),
    .alt_intercalary_list = null,
    .calendar_name = "Ancient Egyptian",
};
