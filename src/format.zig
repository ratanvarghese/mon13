const mon13 = @cImport({
    @cInclude("cal.h");
});

export fn mon13_format(d: mon13.mon13_date, c: *const ?mon13.mon13_cal, nlist: *const ?mon13.mon13_name_list, fmt: [*:0]const u8, buf: ?[*]u8, buflen: u64) c_int {
    return 0;
}

test "basic mon13_format" {
    const d = mon13.mon13_date{ .year = 8, .month = 8, .day = 8 };
    try mon13_format(d, null, null, null, null, 0) == 0;
}
