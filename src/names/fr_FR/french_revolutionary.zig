const base = @import("../../base.zig");

var french_revolutionary_month_list_fr_FR = [_:null]?[*:0]u8{
    "Vendémiaire",
    "Brumaire",
    "Frimaire",
    "Nivôse",
    "Pluviôse",
    "Ventôse",
    "Germinal",
    "Floréal",
    "Prairial",
    "Messidor",
    "Thermidor",
    "Fructidor",
};

var french_revolutionary_weekday_list_fr_FR = [_:null]?[*:0]u8{
    "Primidi",
    "Duodi",
    "Tridi",
    "Quartidi",
    "Quintidi",
    "Sextidi",
    "Septidi",
    "Octidi",
    "Nonidi",
    "Decadi",
};

var french_revolutionary_era_list_fr_FR = [_:null]?[*:0]u8{
    "Avant la Liberté",
    "de la Liberté",
};

var french_revolutionary_intercalary_list_fr_FR = [_:null]?[*:0]u8{
    "La Fête de la Vertu",
    "La Fête du Génie",
    "La Fête du Travail",
    "La Fête de l'Opinion",
    "La Fête des Récompenses",
    "La Fête de la Révolution",
};

const NAME = "Révolutionnaire Français";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const french_revolutionary_names_fr_FR = base.NameList{
    .month_list = @ptrCast([*:null]?[*:0]u8, &french_revolutionary_month_list_fr_FR),
    .weekday_list = @ptrCast([*:null]?[*:0]u8, &french_revolutionary_weekday_list_fr_FR),
    .era_list = @ptrCast([*:null]?[*:0]u8, &french_revolutionary_era_list_fr_FR),
    .intercalary_list = @ptrCast([*:null]?[*:0]u8, &french_revolutionary_intercalary_list_fr_FR),
    .alt_intercalary_list = null,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
