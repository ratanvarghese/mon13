//http://gallica.bnf.fr/ark:/12148/bpt6k6108866f/f359.image
//https://gaz.wiki/wiki/fr/Positivist_calendar

const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

var positivist_month_list_fr_FR = [_:null]?[*:0]u8{
    "Moïse",
    "Homère",
    "Aristote",
    "Archimède",
    "César",
    "Saint Paul",
    "Charlemagne",
    "Dante",
    "Gutenberg",
    "Shakespeare",
    "Descartes",
    "Frédéric",
    "Bichat",
};

var positivist_era_list_fr_FR = [_:null]?[*:0]u8{
    "Avant la Grande Crise",
    "De la Grande Crise", //Après la grande crise??
};

var positivist_intercalary_list_fr_FR = [_:null]?[*:0]u8{
    "Fête Universelle des Morts",
    "Fête Générale des Saints Femmes",
};

const NAME = "Positiviste";
var name_var: [NAME.len:0]u8 = NAME.*;

pub const names_fr_FR_positivist = base.NameList{
    .month_list = @ptrCast([*:null]?[*:0]u8, &positivist_month_list_fr_FR),
    .weekday_list = names_gr.names_fr_FR_gregorian.weekday_list,
    .era_list = @ptrCast([*:null]?[*:0]u8, &positivist_era_list_fr_FR),
    .intercalary_list = @ptrCast([*:null]?[*:0]u8, &positivist_intercalary_list_fr_FR),
    .alt_intercalary_list = null,
    .calendar_name = @ptrCast([*:0]u8, &name_var),
};
