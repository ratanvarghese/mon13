const base = @import("base.zig");
const std = @import("std");

pub fn monthMax(segments: []const ?base.Segment) u8 {
    //Assumes same months for leap and non-leap years.
    var month_max: u8 = 1;
    for (segments) |raw_s| {
        const s = raw_s orelse unreachable;
        if (s.month > month_max) {
            month_max = s.month;
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

pub fn lastOfEnum(comptime E: type) comptime_int {
    var raw_res: ?comptime_int = null;
    inline for (std.meta.fields(E)) |field| {
        if (raw_res) |res| {
            if (res < field.value) {
                raw_res = field.value;
            }
        } else {
            raw_res = field.value;
        }
    }
    const final = raw_res orelse unreachable;
    return final;
}

pub fn getDayOfYearFromSegment(month: u8, day: u8, s: base.Segment) ?u16 {
    if (s.month == month and s.day_start <= day and s.day_end >= day) {
        const day_of_month = day - s.day_start;
        return s.offset + day_of_month + 1;
    } else {
        return null;
    }
}

pub fn getDayOfYear(month: u8, day: u8, segments: []const ?base.Segment) u16 {
    for (segments) |raw_s| {
        const s = raw_s orelse unreachable;
        if (getDayOfYearFromSegment(month, day, s)) |res| {
            return res;
        }
    }
    unreachable;
}
