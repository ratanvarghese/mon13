pub const Err = error{
    Overflow,
    BadCalendar,
    DateNotFound,
    DoyNotFound,
    BadMode,
    InvalidUtf8,
    BeyondEndState,
    InvalidSequence,
    FailedToInsertNullCharacter,
    InvalidDate,
};

pub const Weekday7 = enum(u4) {
    NoWeekday = 0,
    Monday = 1,
    Tuesday = 2,
    Wednesday = 3,
    Thursday = 4,
    Friday = 5,
    Saturday = 6,
    Sunday = 7,
};

pub const Weekday10 = enum(u4) {
    NoWeekday = 0,
    Primidi = 1,
    Duodi = 2,
    Tridi = 3,
    Quartidi = 4,
    Quintidi = 5,
    Sextidi = 6,
    Septidi = 7,
    Octidi = 8,
    Nonidi = 9,
    Decadi = 10,
};

pub const Date = struct {
    year: i32,
    month: u8,
    day: u8,
};

pub const Intercalary = packed struct {
    day_of_year: u16 = 0,
    day_of_leap_year: u16 = 0,
    month: u8,
    day: u8,
    era_start_alt_name: bool = false,
};

pub const Segment = packed struct {
    offset: u16,
    month: u8,
    day_start: u8,
    day_end: u8,
};

pub const LeapCycleInfo = packed struct {
    offset_days: i32,
    year_count: u16,
    leap_year_count: u16,
    offset_years: i16,
    common_days: u16,
    leap_days: u16,
    skip100: bool,
    skip4000: bool,
    symmetric: bool,
};

pub const WeekInfo = packed struct {
    length: u4,
    start: u4,
    continuous: bool,
};

pub const Cal = extern struct {
    intercalary_list: ?[*:null]const ?Intercalary,
    common_lookup_list: [*:null]const ?Segment,
    leap_lookup_list: [*:null]const ?Segment,
    leap_cycle: LeapCycleInfo,
    week: WeekInfo,
    epoch_mjd: i32,
    common_month_max: u8,
    leap_month_max: u8,
    year0: bool,
};

pub const NameList = extern struct {
    month_list: [*:null]?[*:0]const u8,
    weekday_list: [*:null]?[*:0]const u8,
    era_list: [*:null]?[*:0]const u8,
    intercalary_list: ?[*:null]?[*:0]const u8,
    alt_intercalary_list: ?[*:null]?[*:0]const u8,
    calendar_name: [*:0]const u8,
};
