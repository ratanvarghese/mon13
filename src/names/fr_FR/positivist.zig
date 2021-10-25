//http://gallica.bnf.fr/ark:/12148/bpt6k6108866f/f359.image
//https://gaz.wiki/wiki/fr/Positivist_calendar

const base = @import("../../base.zig");
const names_gr = @import("gregorian.zig");

var positivist_month_list_fr_FR = [_:null]?[*:0]const u8{
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

var positivist_era_list_fr_FR = [_:null]?[*:0]const u8{
    "Avant la Grande Crise",
    "De la Grande Crise", //Après la grande crise??
};

var positivist_intercalary_list_fr_FR = [_:null]?[*:0]const u8{
    "Fête Universelle des Morts",
    "Fête Générale des Saints Femmes",
};

pub const positivist_names_fr_FR = base.NameList{
    .month_list = @as([*:null]?[*:0]const u8, &positivist_month_list_fr_FR),
    .weekday_list = names_gr.gregorian_names_fr_FR.weekday_list,
    .era_list = @as([*:null]?[*:0]const u8, &positivist_era_list_fr_FR),
    .intercalary_list = @as([*:null]?[*:0]const u8, &positivist_intercalary_list_fr_FR),
    .alt_intercalary_list = null,
    .calendar_name = "Positiviste",
};
