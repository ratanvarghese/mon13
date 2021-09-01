const base = @import("base.zig");

pub export fn mon13_import(
    cal: ?*const base.mon13_cal,
    input: ?*const c_void,
    mode: base.mon13_import_mode,
) base.mon13_date {
    return base.mon13_date{ .year = 0, .month = 0, .day = 9 };
}

pub export fn mon13_convert(
    d: base.mon13_date,
    src: ?*const base.mon13_cal,
    dest: ?*const base.mon13_cal,
) base.mon13_date {
    return base.mon13_date{ .year = 0, .month = 0, .day = 9 };
}
pub export fn mon13_add(
    d: base.mon13_date,
    cal: ?*const base.mon13_cal,
    offset: i32,
    mode: base.mon13_add_mode,
) base.mon13_date {
    return base.mon13_date{ .year = 0, .month = 0, .day = 9 };
}
pub export fn mon13_compare(
    d0: ?*const base.mon13_date,
    d1: ?*const base.mon13_date,
    cal: ?*const base.mon13_cal,
) c_int {
    return 0;
}
pub export fn mon13_extract(
    d: base.mon13_date,
    cal: ?*const base.mon13_cal,
    mode: base.mon13_extract_mode,
) i64 {
    return 0;
}
pub export fn mon13_format(
    d: base.mon13_date,
    cal: ?*const base.mon13_cal,
    nlist: ?*const base.mon13_name_list,
    fmt: [*]const u8,
    buf: [*]u8,
    buflen: u64,
) c_int {
    return 0;
}
