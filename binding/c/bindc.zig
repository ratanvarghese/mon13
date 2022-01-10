const mon13 = @import("mon13");
const std = @import("std");

pub export const mon13_gregorian = mon13.gregorian;
pub export const mon13_gregorian_year_zero = mon13.gregorian_year_zero;
pub export const mon13_tranquility = mon13.tranquility;
pub export const mon13_tranquility_year_zero = mon13.tranquility_year_zero;
pub export const mon13_holocene = mon13.holocene;
pub export const mon13_cotsworth = mon13.cotsworth;
pub export const mon13_julian = mon13.julian;
pub export const mon13_positivist = mon13.positivist;
pub export const mon13_symmetry454 = mon13.symmetry454;
pub export const mon13_symmetry010 = mon13.symmetry010;
pub export const mon13_ancient_egyptian = mon13.ancient_egyptian;
pub export const mon13_french_revolutionary_romme = mon13.french_revolutionary_romme;
pub export const mon13_french_revolutionary_romme_sub1 = mon13.french_revolutionary_romme_sub1;

pub export const mon13_names_en_US_gregorian = mon13.names_en_US_gregorian;
pub export const mon13_names_en_US_tranquility = mon13.names_en_US_tranquility;
pub export const mon13_names_en_US_holocene = mon13.names_en_US_holocene;
pub export const mon13_names_en_US_cotsworth = mon13.names_en_US_cotsworth;
pub export const mon13_names_en_US_julian = mon13.names_en_US_julian;
pub export const mon13_names_en_US_positivist = mon13.names_en_US_positivist;
pub export const mon13_names_en_US_symmetry454 = mon13.names_en_US_symmetry454;
pub export const mon13_names_en_US_symmetry010 = mon13.names_en_US_symmetry010;
pub export const mon13_names_en_US_ancient_egyptian = mon13.names_en_US_ancient_egyptian;

pub export const mon13_names_en_GB_french_revolutionary = mon13.names_en_GB_french_revolutionary;
pub export const mon13_names_en_GB_french_revolutionary_joke = mon13.names_en_GB_french_revolutionary_joke;

pub export const mon13_names_fr_FR_gregorian = mon13.names_fr_FR_gregorian;
pub export const mon13_names_fr_FR_julian = mon13.names_fr_FR_julian;
pub export const mon13_names_fr_FR_positivist = mon13.names_fr_FR_positivist;
pub export const mon13_names_fr_FR_french_revolutionary = mon13.names_fr_FR_french_revolutionary;

pub const PublicError = extern enum {
    NONE = 0,
    UNKNOWN = -1,
    NULL_CALENDAR = -2,
    NULL_NAME_LIST = -3,
    NULL_FORMAT = -4,
    NULL_INPUT = -5,
    NULL_RESULT = -6,
    NULL_DATE = -7,
    OVERFLOW = -64,
    BAD_CALENDAR = -65,
    DATE_NOT_FOUND = -66,
    DAY_OF_YEAR_NOT_FOUND = -67,
    INVALID_UTF8 = -69,
    INVALID_STATE = -70,
    INVALID_SEQUENCE = -71,
    FAILED_TO_INSERT_NULLCHAR = -72,
    INVALID_DATE = -73,
    INVALID_NAME_LIST = -74,

    fn make(err: anyerror) PublicError {
        return switch (err) {
            mon13.Err.Unknown => PublicError.UNKNOWN,
            mon13.Err.NullCalendar => PublicError.NULL_CALENDAR,
            mon13.Err.NullNameList => PublicError.NULL_NAME_LIST,
            mon13.Err.NullFormat => PublicError.NULL_FORMAT,
            mon13.Err.NullInput => PublicError.NULL_INPUT,
            mon13.Err.NullResult => PublicError.NULL_RESULT,
            mon13.Err.NullDate => PublicError.NULL_DATE,
            mon13.Err.Overflow => PublicError.OVERFLOW,
            mon13.Err.BadCalendar => PublicError.BAD_CALENDAR,
            mon13.Err.DateNotFound => PublicError.DATE_NOT_FOUND,
            mon13.Err.DoyNotFound => PublicError.DAY_OF_YEAR_NOT_FOUND,
            mon13.Err.InvalidUtf8 => PublicError.INVALID_UTF8,
            mon13.Err.BeyondEndState => PublicError.INVALID_STATE,
            mon13.Err.InvalidSequence => PublicError.INVALID_SEQUENCE,
            mon13.Err.FailedToInsertNullCharacter => PublicError.FAILED_TO_INSERT_NULLCHAR,
            mon13.Err.InvalidDate => PublicError.INVALID_DATE,
            mon13.Err.InvalidNameList => PublicError.INVALID_NAME_LIST,
            else => PublicError.UNKNOWN,
        };
    }

    fn toErr(self: PublicError) mon13.Err {
        return switch (self) {
            PublicError.UNKNOWN => mon13.Err.Unknown,
            PublicError.NULL_CALENDAR => mon13.Err.NullCalendar,
            PublicError.NULL_NAME_LIST => mon13.Err.NullNameList,
            PublicError.NULL_FORMAT => mon13.Err.NullFormat,
            PublicError.NULL_INPUT => mon13.Err.NullInput,
            PublicError.NULL_RESULT => mon13.Err.NullResult,
            PublicError.NULL_DATE => mon13.Err.NullDate,
            PublicError.OVERFLOW => mon13.Err.Overflow,
            PublicError.BAD_CALENDAR => mon13.Err.BadCalendar,
            PublicError.DATE_NOT_FOUND => mon13.Err.DateNotFound,
            PublicError.DAY_OF_YEAR_NOT_FOUND => mon13.Err.DoyNotFound,
            PublicError.INVALID_UTF8 => mon13.Err.InvalidUtf8,
            PublicError.INVALID_STATE => mon13.Err.BeyondEndState,
            PublicError.INVALID_SEQUENCE => mon13.Err.InvalidSequence,
            PublicError.FAILED_TO_INSERT_NULLCHAR => mon13.Err.FailedToInsertNullCharacter,
            PublicError.INVALID_DATE => mon13.Err.InvalidDate,
            PublicError.INVALID_NAME_LIST => mon13.Err.InvalidNameList,
            else => mon13.Err.Unknown,
        };
    }
};

fn tail(comptime T: type, res: *T, raw: anyerror!T) c_int {
    if (raw) |x| {
        res.* = x;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_validYmd(
    raw_cal: ?*const mon13.Cal,
    year: i32,
    month: u8,
    day: u8,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    return @boolToInt(mon13.validYmd(cal, year, month, day));
}

pub export fn mon13_mjdFromYmd(
    raw_cal: ?*const mon13.Cal,
    year: i32,
    month: u8,
    day: u8,
    raw_mjd: ?*i32,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_mjd = raw_mjd orelse return @enumToInt(PublicError.NULL_RESULT);
    return tail(i32, res_mjd, mon13.mjdFromYmd(cal, year, month, day));
}

pub export fn mon13_mjdFromC99Tm(
    raw_cal: ?*const mon13.Cal,
    raw_tm: ?*const c_void,
    raw_mjd: ?*i32,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    const tm = raw_tm orelse return @enumToInt(PublicError.NULL_INPUT);
    var res_mjd = raw_mjd orelse return @enumToInt(PublicError.NULL_RESULT);
    return tail(i32, res_mjd, mon13.mjdFromC99Tm(cal, tm));
}

pub export fn mon13_mjdFromUnix(
    unix: i64,
    raw_mjd: ?*i32,
) c_int {
    var res_mjd = raw_mjd orelse return @enumToInt(PublicError.NULL_RESULT);
    return tail(i32, res_mjd, mon13.mjdFromUnix(unix));
}

pub export fn mon13_mjdFromRd(
    rd: i32,
    raw_mjd: ?*i32,
) c_int {
    var res_mjd = raw_mjd orelse return @enumToInt(PublicError.NULL_RESULT);
    return tail(i32, res_mjd, mon13.mjdFromRd(rd));
}

pub export fn mon13_mjdToYmd(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    year: ?*i32,
    month: ?*u8,
    day: ?*u8,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    if (mon13.mjdToYmd(mjd, cal, year, month, day)) {
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_mjdToC99Tm(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    raw_tm: ?*c_void,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var tm = raw_tm orelse return @enumToInt(PublicError.NULL_INPUT);

    if (mon13.mjdToC99Tm(mjd, cal, tm)) {
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_mjdToUnix(
    mjd: i32,
    raw_unix: ?*i64,
) c_int {
    var res_unix = raw_unix orelse return @enumToInt(PublicError.NULL_INPUT);
    return tail(i64, res_unix, mon13.mjdToUnix(mjd));
}

pub export fn mon13_mjdToRd(
    mjd: i32,
    raw_rd: ?*i32,
) c_int {
    var res_rd = raw_rd orelse return @enumToInt(PublicError.NULL_INPUT);
    return tail(i32, res_rd, mon13.mjdToRd(mjd));
}

pub export fn mon13_mjdToIsLeapYear(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    raw_isLeap: ?*c_int,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_isLeap = raw_isLeap orelse return @enumToInt(PublicError.NULL_RESULT);
    if (mon13.mjdToIsLeapYear(mjd, cal)) |x| {
        res_isLeap.* = @boolToInt(x);
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_mjdToDayOfWeek(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    raw_weekday: ?*u8,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_weekday = raw_weekday orelse return @enumToInt(PublicError.NULL_RESULT);
    return tail(u8, res_weekday, mon13.mjdToDayOfWeek(mjd, cal));
}

pub export fn mon13_mjdToDayOfYear(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    raw_yearday: ?*u16,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_yearday = raw_yearday orelse return @enumToInt(PublicError.NULL_RESULT);
    return tail(u16, res_yearday, mon13.mjdToDayOfYear(mjd, cal));
}

pub export fn mon13_addMonths(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    offset: i32,
    raw_sum: ?*i32,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_sum = raw_sum orelse return @enumToInt(PublicError.NULL_RESULT);
    return tail(i32, res_sum, mon13.addMonths(mjd, cal, offset));
}

pub export fn mon13_addYears(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    offset: i32,
    raw_sum: ?*i32,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_sum = raw_sum orelse return @enumToInt(PublicError.NULL_RESULT);
    return tail(i32, res_sum, mon13.addYears(mjd, cal, offset));
}

pub export fn mon13_diffMonths(
    mjd0: i32,
    mjd1: i32,
    raw_cal: ?*const mon13.Cal,
    raw_diff: ?*c_int,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_diff = raw_diff orelse return @enumToInt(PublicError.NULL_RESULT);
    return tail(c_int, res_diff, mon13.diffMonths(mjd0, mjd1, cal));
}

pub export fn mon13_diffYears(
    mjd0: i32,
    mjd1: i32,
    raw_cal: ?*const mon13.Cal,
    raw_diff: ?*c_int,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_diff = raw_diff orelse return @enumToInt(PublicError.NULL_RESULT);
    return tail(c_int, res_diff, mon13.diffYears(mjd0, mjd1, cal));
}

pub export fn mon13_validNameList(
    raw_cal: ?*const mon13.Cal,
    raw_nlist: ?*const mon13.NameList,
) c_int {
    const cal = raw_cal orelse return @boolToInt(false);
    if (raw_nlist) |nlist| {
        if (@ptrCast(?*c_void, nlist.month_list)) |months| {} else {
            return @boolToInt(false);
        }
        if (@ptrCast(?*c_void, nlist.weekday_list)) |weekdays| {} else {
            return @boolToInt(false);
        }
        if (@ptrCast(?*c_void, nlist.era_list)) |eras| {} else {
            return @boolToInt(false);
        }
        if (@ptrCast(?*c_void, nlist.calendar_name)) |name| {} else {
            return @boolToInt(false);
        }
    }
    return @boolToInt(mon13.validNameList(cal, raw_nlist));
}

pub export fn mon13_format(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    raw_nlist: ?*const mon13.NameList,
    raw_fmt: ?[*]const u8,
    raw_buf: ?[*]u8,
    buflen: u32,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    const fmt = raw_fmt orelse return @enumToInt(PublicError.NULL_FORMAT);

    var slice_buf: ?[]u8 = null;
    if (buflen > 0) {
        if (raw_buf) |ptr_buf| {
            slice_buf = ptr_buf[0..buflen];
        }
    }

    if (mon13.format(mjd, cal, raw_nlist, fmt, slice_buf)) |bytes_used| {
        return bytes_used;
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_parse(
    raw_cal: ?*const mon13.Cal,
    raw_nlist: ?*const mon13.NameList,
    raw_fmt: ?[*]const u8,
    raw_buf: ?[*]u8,
    buflen: u32,
    raw_mjd: ?*i32,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    const fmt = raw_fmt orelse return @enumToInt(PublicError.NULL_FORMAT);
    const res_mjd = raw_mjd orelse return @enumToInt(PublicError.NULL_RESULT);
    const ptr_buf = raw_buf orelse return @enumToInt(PublicError.NULL_INPUT);

    if (buflen < 1) {
        return @enumToInt(PublicError.DATE_NOT_FOUND);
    }

    var slice_buf: []u8 = ptr_buf[0..buflen];
    if (mon13.parse(cal, raw_nlist, fmt, slice_buf, res_mjd)) |bytes_used| {
        return bytes_used;
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_errorMessage(errorCode: c_int) [*:0]const u8 {
    if (errorCode > std.math.minInt(i32) and errorCode < std.math.maxInt(i32)) {
        if (mon13.utils.validInEnum(PublicError, @intCast(i32, errorCode))) {
            const e = @intToEnum(PublicError, @intCast(i32, errorCode));
            return mon13.errorMessage(e.toErr());
        }
    }
    return mon13.errorMessage(mon13.Err.Unknown);
}
