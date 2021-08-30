const mon13 = @import("imp.zig").mon13;

pub export fn mon13_format(d: mon13.mon13_date, c: ?*const mon13.mon13_cal, nlist: ?*const mon13.mon13_name_list, fmt: ?[*:0]const u8, buf: ?[*]u8, buflen: u64) c_int {
    return d.day;
}
