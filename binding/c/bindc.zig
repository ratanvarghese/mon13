const mon13 = @import("mon13");

pub export const mon13_gregorian = mon13.gregorian;
pub export const mon13_gregorian_year_zero = mon13.gregorian_year_zero;
pub export const mon13_tranquility = mon13.tranquility;
pub export const mon13_tranquility_year_zero = mon13.tranquility_year_zero;
pub export const mon13_holocene = mon13.holocene;
pub export const mon13_cotsworth = mon13.cotsworth;

pub export const mon13_gregorian_names_en_US = mon13.gregorian_names_en_US;
pub export const mon13_gregorian_names_fr_FR = mon13.gregorian_names_fr_FR;
pub export const mon13_tranquility_names_en_US = mon13.tranquility_names_en_US;
pub export const mon13_holocene_names_en_US = mon13.holocene_names_en_US;
pub export const mon13_cotsworth_names_en_US = mon13.cotsworth_names_en_US;

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

    fn make(err: mon13.Err) PublicError {
        return switch (err) {
            mon13.Err.Overflow => PublicError.OVERFLOW,
            mon13.Err.BadCalendar => PublicError.BAD_CALENDAR,
            mon13.Err.DateNotFound => PublicError.DATE_NOT_FOUND,
            mon13.Err.DoyNotFound => PublicError.DAY_OF_YEAR_NOT_FOUND,
            mon13.Err.BadMode => PublicError.BAD_MODE,
            else => PublicError.UNKNOWN,
        };
    }
};

pub export fn mon13_import(
    raw_cal: ?*const mon13.Cal,
    raw_input: ?*const c_void,
    mode: mon13.ImportMode,
    raw_result: ?*mon13.Date,
) c_int {
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    const input = raw_input orelse return @enumToInt(PublicError.NULL_INPUT);
    const result = raw_result orelse return @enumToInt(PublicError.NULL_RESULT);

    if (mon13.import(cal, input, mode)) |imported| {
        result.* = imported;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_convert(
    raw_d: ?*const mon13.Date,
    raw_src: ?*const mon13.Cal,
    raw_dest: ?*const mon13.Cal,
    raw_result: ?*mon13.Date,
) c_int {
    const d = raw_d orelse return @enumToInt(PublicError.NULL_DATE);
    const src = raw_src orelse return @enumToInt(PublicError.NULL_CALENDAR);
    const dest = raw_dest orelse return @enumToInt(PublicError.NULL_CALENDAR);
    const result = raw_result orelse return @enumToInt(PublicError.NULL_RESULT);

    if (mon13.convert(d.*, src, dest)) |converted| {
        result.* = converted;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_add(
    raw_d: ?*const mon13.Date,
    raw_cal: ?*const mon13.Cal,
    offset: i32,
    mode: mon13.AddMode,
    raw_result: ?*mon13.Date,
) c_int {
    const d = raw_d orelse return @enumToInt(PublicError.NULL_DATE);
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    const result = raw_result orelse return @enumToInt(PublicError.NULL_RESULT);

    if (mon13.add(d.*, cal, offset, mode)) |sum| {
        result.* = sum;
        return @enumToInt(PublicError.NONE);
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}

pub export fn mon13_compare(
    raw_d0: ?*const mon13.Date,
    raw_d1: ?*const mon13.Date,
    raw_cal: ?*const mon13.Cal,
) c_int {
    const d0 = raw_d0 orelse return 0;
    const d1 = raw_d1 orelse return 0;
    return mon13.compare(d0.*, d1.*, raw_cal) catch 0;
}

pub export fn mon13_extract(
    raw_d: ?*const mon13.Date,
    raw_cal: ?*const mon13.Cal,
    mode: mon13.ExtractMode,
) i64 {
    return mon13.extract(raw_d, raw_cal, mode);
}

pub export fn mon13_format(
    raw_d: ?*const mon13.Date,
    raw_cal: ?*const mon13.Cal,
    raw_nlist: ?*const mon13.NameList,
    raw_fmt: ?[*]const u8,
    raw_buf: ?[*]u8,
    buflen: u32,
) c_int {
    const d = raw_d orelse return @enumToInt(PublicError.NULL_DATE);
    const cal = raw_cal orelse return @enumToInt(PublicError.NULL_CALENDAR);
    const fmt = raw_fmt orelse return @enumToInt(PublicError.NULL_FORMAT);

    if (mon13.format(d.*, cal, raw_nlist, fmt, raw_buf, buflen)) |bytes_used| {
        return bytes_used;
    } else |err| {
        return @enumToInt(PublicError.make(err));
    }
}
