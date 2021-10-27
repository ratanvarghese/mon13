const base = @import("base.zig");

pub fn getMonthMax(segments: []const ?base.Segment) u8 {
    //Assumes same months for leap and non-leap years.
    var month_max: u8 = 1;
    var si: u8 = 0;
    while (si < segments.len) : (si += 1) {
        if (segments[si]) |s| {
            if (s.month > month_max) {
                month_max = s.month;
            }
        }
    }
    return month_max;
}

pub fn getDayCount(segments: []const ?base.Segment) u16 {
    if (segments[segments.len - 1]) |final_segment| {
        return final_segment.offset + final_segment.day_end;
    }
    unreachable;
}

pub fn getLeapDayCount(common: []const ?base.Segment, leap: []const ?base.Segment) u16 {
    return getDayCount(leap) - getDayCount(common);
}
