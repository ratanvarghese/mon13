const base = @import("base.zig");

const WKDY_SUN = "Sunday";
const WKDY_MON = "Monday";
const WKDY_TUE = "Tuesday";
const WKDY_WED = "Wednesday";
const WKDY_THU = "Thursday";
const WKDY_FRI = "Friday";
const WKDY_SAT = "Saturday";

//Gregorian calendar
var gregorian_month_list = [_:null]?[*:0]const u8{
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

var gregorian_weekday_list = [_:null]?[*:0]const u8{
    WKDY_MON,
    WKDY_TUE,
    WKDY_WED,
    WKDY_THU,
    WKDY_FRI,
    WKDY_SAT,
    WKDY_SUN,
};

var gregorian_era_list = [_:null]?[*:0]const u8{
    "Before Common Era",
    "Common Era",
};

pub export const mon13_gregorian_names_en_US = base.mon13_name_list{
    .month_list = @as([*:null]?[*:0]const u8, &gregorian_month_list),
    .weekday_list = @as([*:null]?[*:0]const u8, &gregorian_weekday_list),
    .era_list = @as([*:null]?[*:0]const u8, &gregorian_era_list),
    .intercalary_list = null,
    .alt_intercalary_list = null,
    .calendar_name = "Gregorian",
};

var gregorian_month_list_fr_FR = [_:null]?[*:0]const u8{
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

var gregorian_weekday_list_fr_FR = [_:null]?[*:0]const u8{
    "Lundi",
    "Mardi",
    "Mercredi",
    "Jeudi",
    "Vendredi",
    "Samedi",
    "Dimanche",
};

var gregorian_era_list_fr_FR = [_:null]?[*:0]const u8{
    "Avant l'ère commune",
    "Ère commune",
};

pub export const mon13_gregorian_names_fr_FR = base.mon13_name_list{
    .month_list = @as([*:null]?[*:0]const u8, &gregorian_month_list_fr_FR),
    .weekday_list = @as([*:null]?[*:0]const u8, &gregorian_weekday_list_fr_FR),
    .era_list = @as([*:null]?[*:0]const u8, &gregorian_era_list_fr_FR),
    .intercalary_list = null,
    .alt_intercalary_list = null,
    .calendar_name = "Grégorien",
};

//Tranquility Calendar
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

var tranquility_weekday_list_en_US = [_:null]?[*:0]const u8{
    WKDY_MON,
    WKDY_TUE,
    WKDY_WED,
    WKDY_THU,
    WKDY_FRI,
    WKDY_SAT,
    WKDY_SUN,
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

pub export const mon13_tranquility_names_en_US = base.mon13_name_list{
    .month_list = @as([*:null]?[*:0]const u8, &tranquility_month_list_en_US),
    .weekday_list = @as([*:null]?[*:0]const u8, &tranquility_weekday_list_en_US),
    .era_list = @as([*:null]?[*:0]const u8, &tranquility_era_list_en_US),
    .intercalary_list = @as([*:null]?[*:0]const u8, &tranquility_intercalary_list_en_US),
    .alt_intercalary_list = @as([*:null]?[*:0]const u8, &tranquility_alt_intercalary_list_en_US),
    .calendar_name = "Tranquility",
};

//Holocene Calendar

var holocene_era_list_en_US = [_:null]?[*:0]const u8{
    "Before Human Era",
    "Human Era",
};

pub export const mon13_holocene_names_en_US = base.mon13_name_list{
    .month_list = @as([*:null]?[*:0]const u8, &gregorian_month_list),
    .weekday_list = @as([*:null]?[*:0]const u8, &gregorian_weekday_list),
    .era_list = @as([*:null]?[*:0]const u8, &holocene_era_list_en_US),
    .intercalary_list = null,
    .alt_intercalary_list = null,
    .calendar_name = "Holocene",
};

//Cotsworth Calendar

var cotsworth_month_list = [_:null]?[*:0]const u8{
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "Sol",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

var cotsworth_weekday_list = [_:null]?[*:0]const u8{
    WKDY_MON,
    WKDY_TUE,
    WKDY_WED,
    WKDY_THU,
    WKDY_FRI,
    WKDY_SAT,
    WKDY_SUN,
};

var cotsworth_era_list = [_:null]?[*:0]const u8{
    "Before Common Era",
    "Common Era",
};

pub export const mon13_cotsworth_names_en_US = base.mon13_name_list{
    .month_list = @as([*:null]?[*:0]const u8, &cotsworth_month_list),
    .weekday_list = @as([*:null]?[*:0]const u8, &cotsworth_weekday_list),
    .era_list = @as([*:null]?[*:0]const u8, &cotsworth_era_list),
    .intercalary_list = null,
    .alt_intercalary_list = null,
    .calendar_name = "Cotsworth",
};
