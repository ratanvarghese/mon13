const base = @import("../../base.zig");

//Recorded in Clavis Calendaria: Or, A Compendious Analysis of the Calendar
//by John Brady
//https://books.google.ca/books?id=pKjhAAAAMAAJ&pg=PA38&redir_esc=y#v=onepage&q&f=false
var french_revolutionary_month_list_en_GB_joke = [_:null]?[*:0]const u8{
    "Wheezy",
    "Sneezy",
    "Freezy",
    "Slippy",
    "Drippy",
    "Nippy",
    "Showery",
    "Flowery",
    "Bowery",
    "Hoppy",
    "Croppy",
    "Poppy",
};

//Suggested in The French Revolution, A History
//By Thomas Carlyle
//https://books.google.ca/books?id=81sQAAAAYAAJ&pg=PP7&source=gbs_selected_pages&cad=2#v=onepage&q=Vintagearious&f=false
var french_revolutionary_month_list_en_GB_carlyle = [_:null]?[*:0]const u8{
    "Vintagearious",
    "Fogarious",
    "Frostarious",
    "Snowous",
    "Rainous",
    "Windous",
    "Buddal",
    "Floweral",
    "Meadowal",
    "Reapidor",
    "Heatidor",
    "Fruitidor",
};

var french_revolutionary_weekday_list_en_GB = [_:null]?[*:0]const u8{
    "First Day",
    "Second Day",
    "Third Day",
    "Fourth Day",
    "Fifth Day",
    "Sixth Day",
    "Seventh Day",
    "Eighth Day",
    "Ninth Day",
    "Tenth Day",
};

var french_revolutionary_era_list_en_GB = [_:null]?[*:0]const u8{
    "Before Liberty",
    "of Liberty",
};

var french_revolutionary_intercalary_list_en_GB = [_:null]?[*:0]const u8{
    "Festival of Virtue",
    "Festival of Genius",
    "Festival of Labour",
    "Festival of Opinion",
    "Festival of Rewards",
    "Festival of the Revolution",
};

pub const french_revolutionary_names_en_GB = base.NameList{
    .month_list = @as([*:null]?[*:0]const u8, &french_revolutionary_month_list_en_GB_carlyle),
    .weekday_list = @as([*:null]?[*:0]const u8, &french_revolutionary_weekday_list_en_GB),
    .era_list = @as([*:null]?[*:0]const u8, &french_revolutionary_era_list_en_GB),
    .intercalary_list = @as([*:null]?[*:0]const u8, &french_revolutionary_intercalary_list_en_GB),
    .alt_intercalary_list = null,
    .calendar_name = "French Revolutionary",
};

pub const french_revolutionary_names_en_GB_joke = base.NameList{
    .month_list = @as([*:null]?[*:0]const u8, &french_revolutionary_month_list_en_GB_joke),
    .weekday_list = french_revolutionary_names_en_GB.weekday_list,
    .era_list = french_revolutionary_names_en_GB.era_list,
    .intercalary_list = french_revolutionary_names_en_GB.intercalary_list,
    .alt_intercalary_list = french_revolutionary_names_en_GB.alt_intercalary_list,
    .calendar_name = french_revolutionary_names_en_GB.calendar_name,
};
