const base = @import("../../base.zig");

var gregorian_month_list_fr_FR = [_:null]?[*:0]u8{
    "Janvier",
    "Février",
    "Mars",
    "Avril",
    "Mai",
    "Juin",
    "Juillet",
    "Août",
    "Septembre",
    "Octobre",
    "Novembre",
    "Décembre",
};

var gregorian_weekday_list_fr_FR = [_:null]?[*:0]u8{
    "Lundi",
    "Mardi",
    "Mercredi",
    "Jeudi",
    "Vendredi",
    "Samedi",
    "Dimanche",
};

var gregorian_era_list_fr_FR = [_:null]?[*:0]u8{
    "Avant l'ère commune",
    "Ère commune",
};

const NAME = "Grégorien";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const names_fr_FR_gregorian = base.NameList{
    .month_list = @ptrCast([*:null]?[*:0]u8, &gregorian_month_list_fr_FR),
    .weekday_list = @ptrCast([*:null]?[*:0]u8, &gregorian_weekday_list_fr_FR),
    .era_list = @ptrCast([*:null]?[*:0]u8, &gregorian_era_list_fr_FR),
    .intercalary_list = null,
    .alt_intercalary_list = null,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
