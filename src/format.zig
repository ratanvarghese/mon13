const base = @import("base.zig");
const logic = @import("logic.zig");

pub export fn mon13_format(
    d: *base.mon13_date,
    cal: ?*const base.mon13_cal,
    nlist: ?*const base.mon13_name_list,
    fmt: [*]const u8,
    buf: [*]u8,
    buflen: u64,
) c_int {
    return 0;
}
