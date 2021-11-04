const mon13 = @import("mon13");

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

pub export const mon13_gregorian_names_en_US = mon13.gregorian_names_en_US;
pub export const mon13_tranquility_names_en_US = mon13.tranquility_names_en_US;
pub export const mon13_holocene_names_en_US = mon13.holocene_names_en_US;
pub export const mon13_cotsworth_names_en_US = mon13.cotsworth_names_en_US;
pub export const mon13_julian_names_en_US = mon13.julian_names_en_US;
pub export const mon13_positivist_names_en_US = mon13.positivist_names_en_US;
pub export const mon13_symmetry454_names_en_US = mon13.symmetry454_names_en_US;
pub export const mon13_symmetry010_names_en_US = mon13.symmetry010_names_en_US;

pub export const mon13_gregorian_names_fr_FR = mon13.gregorian_names_fr_FR;
pub export const mon13_julian_names_fr_FR = mon13.julian_names_fr_FR;
pub export const mon13_positivist_names_fr_FR = mon13.positivist_names_fr_FR;

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
    BAD_MODE = -68,
    INVALID_UTF8 = -69,
    INVALID_STATE = -70,
    INVALID_SEQUENCE = -71,
    FAILED_TO_INSERT_NULLCHAR = -72,
    ERROR_INVALID_DATE = -73,

    fn make(err: mon13.Err) PublicError {
        return switch (err) {
            mon13.Err.Overflow => PublicError.OVERFLOW,
            mon13.Err.BadCalendar => PublicError.BAD_CALENDAR,
            mon13.Err.DateNotFound => PublicError.DATE_NOT_FOUND,
            mon13.Err.DoyNotFound => PublicError.DAY_OF_YEAR_NOT_FOUND,
            mon13.Err.BadMode => PublicError.BAD_MODE,
            mon13.Err.InvalidUtf8 => PublicError.INVALID_UTF8,
            mon13.Err.BeyondEndState => PublicError.INVALID_STATE,
            mon13.Err.InvalidSequence => PublicError.INVALID_SEQUENCE,
            mon13.Err.FailedToInsertNullCharacter => PublicError.FAILED_TO_INSERT_NULLCHAR,
            mon13.Err.InvalidDate => PublicError.ERROR_INVALID_DATE,
            else => PublicError.UNKNOWN,
        };
    }
};

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

    if (mon13.mjdFromYmd(cal, year, month, day)) |mjd| {
        res_mjd.* = mjd;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_mjdFromC99Tm(
    raw_cal: ?*const mon13.Cal,
    raw_tm: ?*const c_void,
    raw_mjd: ?*i32,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    const tm = raw_tm orelse return @enumToInt(PublicError.NULL_INPUT);
    var res_mjd = raw_mjd orelse return @enumToInt(PublicError.NULL_RESULT);

    if (mon13.mjdFromC99Tm(cal, tm)) |mjd| {
        res_mjd.* = mjd;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_mjdFromUnix(
    unix: i64,
    raw_mjd: ?*i32,
) c_int {
    var res_mjd = raw_mjd orelse return @enumToInt(PublicError.NULL_RESULT);

    if (mon13.mjdFromUnix(unix)) |mjd| {
        res_mjd.* = mjd;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_mjdFromRd(
    rd: i32,
    raw_mjd: ?*i32,
) c_int {
    var res_mjd = raw_mjd orelse return @enumToInt(PublicError.NULL_RESULT);

    if (mon13.mjdFromRd(rd)) |mjd| {
        res_mjd.* = mjd;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
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

    if (mon13.mjdToUnix(mjd)) |unix| {
        res_unix.* = unix;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_mjdToRd(
    mjd: i32,
    raw_rd: ?*i32,
) c_int {
    var res_rd = raw_rd orelse return @enumToInt(PublicError.NULL_INPUT);

    if (mon13.mjdToRd(mjd)) |rd| {
        res_rd.* = rd;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_mjdToIsLeapYear(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    raw_isLeap: ?*c_int,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_isLeap = raw_isLeap orelse return @enumToInt(PublicError.NULL_RESULT);
    if (mon13.mjdToIsLeapYear(mjd, cal)) |isLeap| {
        res_isLeap.* = @boolToInt(isLeap);
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_mjdToDayOfWeek(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    raw_weekday: ?*c_int,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_weekday = raw_weekday orelse return @enumToInt(PublicError.NULL_RESULT);
    if (mon13.mjdToDayOfWeek(mjd, cal)) |weekday| {
        res_weekday.* = @enumToInt(weekday);
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_mjdToDayOfYear(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    raw_yearday: ?*c_int,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_yearday = raw_yearday orelse return @enumToInt(PublicError.NULL_RESULT);
    if (mon13.mjdToDayOfYear(mjd, cal)) |yearday| {
        res_yearday.* = @intCast(c_int, yearday);
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_addMonths(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    offset: i32,
    raw_sum: ?*i32,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_sum = raw_sum orelse return @enumToInt(PublicError.NULL_RESULT);
    if (mon13.addMonths(mjd, cal, offset)) |sum| {
        res_sum.* = sum;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_addYears(
    mjd: i32,
    raw_cal: ?*const mon13.Cal,
    offset: i32,
    raw_sum: ?*i32,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_sum = raw_sum orelse return @enumToInt(PublicError.NULL_RESULT);
    if (mon13.addYears(mjd, cal, offset)) |sum| {
        res_sum.* = sum;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_diffMonths(
    mjd0: i32,
    mjd1: i32,
    raw_cal: ?*const mon13.Cal,
    raw_diff: ?*c_int,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_diff = raw_diff orelse return @enumToInt(PublicError.NULL_RESULT);
    if (mon13.diffMonths(mjd0, mjd1, cal)) |diff| {
        res_diff.* = diff;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_diffYears(
    mjd0: i32,
    mjd1: i32,
    raw_cal: ?*const mon13.Cal,
    raw_diff: ?*c_int,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    var res_diff = raw_diff orelse return @enumToInt(PublicError.NULL_RESULT);
    if (mon13.diffYears(mjd0, mjd1, cal)) |diff| {
        res_diff.* = diff;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
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
