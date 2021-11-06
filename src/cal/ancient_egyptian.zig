const base = @import("../base.zig");
const gen = @import("../gen.zig");

const COMMON = [_:null]?base.Segment{
    .{ .offset = 30 * 0, .month = 1, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 1, .month = 2, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 2, .month = 3, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 3, .month = 4, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 4, .month = 5, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 5, .month = 6, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 6, .month = 7, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 7, .month = 8, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 8, .month = 9, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 9, .month = 10, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 10, .month = 11, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 11, .month = 12, .day_start = 1, .day_end = 30 },
    .{ .offset = 30 * 12, .month = 0, .day_start = 1, .day_end = 5 },
};


