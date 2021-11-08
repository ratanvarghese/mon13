const base = @import("base.zig");
const std = @import("std");

pub fn monthMax(segments: []const ?base.Segment) u8 {
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

pub fn dayCount(segments: []const ?base.Segment) u16 {
    if (segments[segments.len - 1]) |final_segment| {
        return final_segment.offset + final_segment.day_end;
    }
    unreachable;
}

pub fn leapDayCount(common: []const ?base.Segment, leap: []const ?base.Segment) u16 {
    return dayCount(leap) - dayCount(common);
}

//For MJD converted by
//https://www.vcalc.com/equation/?uuid=e900a382-77a1-11e5-a3bb-bc764e2038f2
pub fn mjdFromVcalc(vcalc: f32) i32 {
    return @floatToInt(i32, std.math.floor(vcalc));
}

pub fn validInEnum(comptime E: type, x: u8) bool {
    inline for (std.meta.fields(E)) |field| {
        if (field.value == x) {
            return true;
        }
    }
    return false;
}
