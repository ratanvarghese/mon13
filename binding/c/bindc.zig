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

pub export fn mon13_import(
    raw_cal: ?*const mon13.Cal,
    raw_input: ?*const c_void,
    mode: mon13.ImportMode,
    raw_result: ?*mon13.Date,
) c_int {
    return mon13.import(raw_cal, raw_input, mode, raw_result);
}

pub export fn mon13_convert(
    raw_d: ?*const mon13.Date,
    raw_src: ?*const mon13.Cal,
    raw_dest: ?*const mon13.Cal,
    raw_result: ?*mon13.Date,
) c_int {
    return mon13.convert(raw_d, raw_src, raw_dest, raw_result);
}

pub export fn mon13_add(
    raw_d: ?*const mon13.Date,
    raw_cal: ?*const mon13.Cal,
    offset: i32,
    mode: mon13.AddMode,
    raw_result: ?*mon13.Date,
) c_int {
    return mon13.add(raw_d, raw_cal, offset, mode, raw_result);
}

pub export fn mon13_compare(
    raw_d0: ?*const mon13.Date,
    raw_d1: ?*const mon13.Date,
    raw_cal: ?*const mon13.Cal,
) c_int {
    return mon13.compare(raw_d0, raw_d1, raw_cal);
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
    return mon13.format(raw_d, raw_cal, raw_nlist, raw_fmt, raw_buf, buflen);
}
