const fmt = @import("format.zig");
const mon13 = @import("imp.zig").mon13;

const std = @import("std");
const expect = std.testing.expect;

test "basic mon13_format" {
    const d = mon13.mon13_date{ .year = 8, .month = 8, .day = 0 };
    try expect(fmt.mon13_format(d, null, null, null, null, 0) == 0);
}
