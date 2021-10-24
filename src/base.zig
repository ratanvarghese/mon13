pub const AddMode = extern enum {
    NONE,
    DAYS,
    MONTHS,
    YEARS,
};

pub const ExtractMode = extern enum {
    DAY_OF_YEAR,
    DAY_OF_WEEK,
    IS_LEAP_YEAR,
    MJD,
    UNIX,
    RD,
};

pub const Weekday = extern enum {
    MON13_NO_WEEKDAY = 0,
    MON13_MONDAY = 1,
    MON13_TUESDAY = 2,
    MON13_WEDNESDAY = 3,
    MON13_THURSDAY = 4,
    MON13_FRIDAY = 5,
    MON13_SATURDAY = 6,
    MON13_SUNDAY = 7,
};

pub const Date = extern struct {
    year: i32,
    month: u8,
    day: u8,
};

pub const Intercalary = packed struct {
    day_of_year: u16,
    day_of_leap_year: u16,
    month: u8,
    day: u8,
    IC_YEAR: bool,
    IC_ERA_START_ALT_NAME: bool,
    IC_LEAP: bool,
};

pub const Segment = packed struct {
    offset: u16,
    month: u8,
    day_start: u8,
    day_end: u8,
};

pub const LeapCycleInfo = packed struct {
    year_count: u16,
    leap_year_count: u16,
    offset_years: u16,
    offset_days: u16,
    common_days: u16,
    leap_days: u16,
    LEAP_GREGORIAN_SKIP: bool,
};

pub const Cal = extern struct {
    intercalary_list: ?[*:null]const ?Intercalary,
    common_lookup_list: [*:null]const ?Segment,
    leap_lookup_list: [*:null]const ?Segment,
    leap_cycle: LeapCycleInfo,
    epoch_mjd: i32,
    start_weekday: Weekday,
    week_length: u8,
    CAL_YEAR_ZERO: bool,
    CAL_PERENNIAL: bool,
};

pub const NameList = extern struct {
    month_list: [*:null]?[*:0]const u8,
    weekday_list: [*:null]?[*:0]const u8,
    era_list: [*:null]?[*:0]const u8,
    intercalary_list: ?[*:null]?[*:0]const u8,
    alt_intercalary_list: ?[*:null]?[*:0]const u8,
    calendar_name: [*:0]const u8,
};
