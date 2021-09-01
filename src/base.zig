pub const mon13_add_mode = extern enum {
    MON13_ADD_NONE,
    MON13_ADD_DAYS,
    MON13_ADD_MONTHS,
    MON13_ADD_YEARS,
};

pub const mon13_extract_mode = extern enum {
    MON13_EXTRACT_DAY_OF_YEAR,
    MON13_EXTRACT_DAY_OF_WEEK,
    MON13_EXTRACT_IS_LEAP_YEAR,
    MON13_EXTRACT_MJD,
    MON13_EXTRACT_UNIX,
    MON13_EXTRACT_RD,
};

pub const mon13_import_mode = extern enum {
    MON13_IMPORT_MJD,
    MON13_IMPORT_UNIX,
    MON13_IMPORT_RD,
};

pub const mon13_error = extern enum {
    MON13_ERROR_NULL_CALENDAR = -1,
    MON13_ERROR_NULL_NAME_LIST = -2,
    MON13_ERROR_NULL_FORMAT = -3,
};

pub const mon13_weekday = extern enum {
    MON13_NO_WEEKDAY = 0,
    MON13_MONDAY = 1,
    MON13_TUESDAY = 2,
    MON13_WEDNESDAY = 3,
    MON13_THURSDAY = 4,
    MON13_FRIDAY = 5,
    MON13_SATURDAY = 6,
    MON13_SUNDAY = 7,
};

pub const daycount_t = i32;
pub const mjd_t = daycount_t;

pub const mon13_date = extern struct {
    year: i32,
    month: u8,
    day: u8,
};

pub const intercalary = packed struct {
    day_of_year: u16,
    month: u8,
    day: u8,
    IC_YEAR: bool,
    IC_ERA_START_ALT_NAME: bool,
    IC_LEAP: bool,
};

pub const lkup = packed struct {
    offset: u16,
    month: u8,
    day_start: u8,
    day_end: u8,
};

pub const leap_cycle_info = packed struct {
    year_count: u16,
    leap_year_count: u16,
    offset_years: u16,
    offset_days: u16,
    common_days: u16,
    leap_days: u16,
    LEAP_GREGORIAN_SKIP: bool,
};

pub const mon13_cal = extern struct {
    intercalary_list: ?[*:null]const ?intercalary,
    common_lookup_list: [*:null]const ?lkup,
    leap_lookup_list: [*:null]const ?lkup,
    leap_cycle: leap_cycle_info,
    epoch_mjd: mjd_t,
    week_length: u8,
    start_weekday: mon13_weekday,
    CAL_YEAR_ZERO: bool,
    CAL_PERENNIAL: bool,
};

pub const mon13_name_list = extern struct {
    month_list: [*:null]?[*:0]const u8,
    weekday_list: [*:null]?[*:0]const u8,
    era_list: [*:null]?[*:0]const u8,
    intercalary_list: ?[*:null]?[*:0]const u8,
    alt_intercalary_list: ?[*:null]?[*:0]const u8,
    calendar_name: [*:0]const u8,
};

comptime {
    @export(mon13_add_mode, .{ .name = "mon13_add_mode", .linkage = .Strong });
    @export(mon13_extract_mode, .{ .name = "mon13_extract_mode", .linkage = .Strong });
    @export(mon13_import_mode, .{ .name = "mon13_import_mode", .linkage = .Strong });
    @export(mon13_error, .{ .name = "mon13_error", .linkage = .Strong });
    @export(mon13_weekday, .{ .name = "mon13_weekday", .linkage = .Strong });
    @export(mon13_date, .{ .name = "mon13_date", .linkage = .Strong });
    @export(mon13_cal, .{ .name = "mon13_cal", .linkage = .Strong });
    @export(mon13_name_list, .{ .name = "mon13_name_list", .linkage = .Strong });
}
