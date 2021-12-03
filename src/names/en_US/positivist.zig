const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

var positivist_month_list_en_US = [_:null]?[*:0]u8{
    "Moses",
    "Homer",
    "Aristotle",
    "Archimedes",
    "Caesar",
    "Saint Paul",
    "Charlemagne",
    "Dante",
    "Gutenberg",
    "Shakespeare",
    "Descartes",
    "Frederick",
    "Bichat",
};

var positivist_era_list_en_US = [_:null]?[*:0]u8{
    "Before the Great Crisis",
    "After the Great Crisis",
};

var positivist_intercalary_list_en_US = [_:null]?[*:0]u8{
    "Festival of All the Dead",
    "Festival of Holy Women",
};

const NAME = "Positivist";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const names_en_US_positivist = base.NameList{
    .month_list = @ptrCast([*:null]?[*:0]u8, &positivist_month_list_en_US),
    .weekday_list = names_gr.names_en_US_gregorian.weekday_list,
    .era_list = @ptrCast([*:null]?[*:0]u8, &positivist_era_list_en_US),
    .intercalary_list = @ptrCast([*:null]?[*:0]u8, &positivist_intercalary_list_en_US),
    .alt_intercalary_list = null,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
