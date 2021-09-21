const std = @import("std");

const base = @import("base.zig");
const logic = @import("logic.zig");

const FormatErr = error{
    InvalidUtf8,
    BeyondEndState,
    InvalidSequence,
};

const flag = enum(u8) {
    pad_none = '-',
    pad_space = '_',
    pad_zero = '0',
    absolute_value = '|',
};

const sequence = enum(u8) {
    percent = '%',
    weekday_name = 'A',
    month_name = 'B',
    day_of_month = 'd',
    calendar_name = 'f',
    day_of_year = 'j',
    month_number = 'm',
    newline = 'n',
    era_name = 'q',
    tab = 't',
    weekday_number = 'u',
    year = 'y',
};

const digit = enum(u8) {
    d0 = '0',
    d1 = '1',
    d2 = '2',
    d3 = '3',
    d4 = '4',
    d5 = '5',
    d6 = '6',
    d7 = '7',
    d8 = '8',
    d9 = '9',
};

fn valid_in_enum(comptime E: type, x: u8) bool {
    inline for (std.meta.fields(E)) |field| {
        if (field.value == x) {
            return true;
        }
    }
    return false;
}

const state = enum(u8) {
    start,
    end,
    fmt_prefix,
    fmt_width,
    fmt_flag,
    fmt_seq,
    copy,

    fn after_start(c: u8) state {
        return switch (c) {
            '%' => state.fmt_prefix,
            0 => state.end,
            else => state.copy,
        };
    }

    fn after_prefix_or_flag(c: u8) FormatErr!state {
        if (valid_in_enum(flag, c)) {
            return state.fmt_flag;
        } else if (valid_in_enum(digit, c)) {
            //Check digit after flag because '0' is a flag.
            return state.fmt_width;
        } else if (valid_in_enum(sequence, c)) {
            return state.fmt_seq;
        } else {
            return FormatErr.InvalidSequence;
        }
    }

    fn after_width(c: u8) FormatErr!state {
        if (valid_in_enum(digit, c)) {
            return state.fmt_width;
        } else if (valid_in_enum(sequence, c)) {
            return state.fmt_seq;
        } else {
            return FormatErr.InvalidSequence;
        }
    }

    fn next(self: state, c: u8) FormatErr!state {
        return switch (self) {
            .start => after_start(c),
            .end => FormatErr.BeyondEndState,
            .fmt_prefix, .fmt_flag => after_prefix_or_flag(c),
            .fmt_width => after_width(c),
            .fmt_seq => after_start(c),
            .copy => after_start(c),
        };
    }
};

fn copy_len(c: u8) FormatErr!u8 {
    return switch (c) {
        0b11110000...0b11110111 => 4,
        0b11100000...0b11101111 => 3,
        0b11000000...0b11011111 => 2,
        0b00000000...0b01111111 => 1,
        else => FormatErr.InvalidUtf8,
    };
}

pub export fn mon13_format(
    raw_d: ?*const base.mon13_date,
    raw_cal: ?*const base.mon13_cal,
    raw_nlist: ?*const base.mon13_name_list,
    raw_fmt: ?[*]const u8,
    buf: [*]u8,
    buflen: u32,
) c_int {
    const d = raw_d orelse return -1;
    const cal = raw_cal orelse return -2;
    const fmt = raw_fmt orelse return -3;
    if (buflen < 1) {
        return -4;
    }

    const buf_limit = buflen - 1; //Leave space for null character.
    var fmt_i: u32 = 0;
    var buf_i: u32 = 0;
    var s = state.start;

    while (fmt[fmt_i] != 0 and buf_i < buf_limit) {
        const c = fmt[fmt_i];
        s = s.next(c) catch return -5;

        if (s == state.copy) {
            const count = copy_len(c) catch return -6;
            const old_fmt_i = fmt_i;
            const old_buf_i = buf_i;
            fmt_i += count;
            buf_i += count;
            if (buf_i < buf_limit) {
                std.mem.copy(u8, buf[old_buf_i..buf_i], fmt[old_fmt_i..fmt_i]);
            } else {
                s = state.end;
                break;
            }
        } else {
            buf[buf_i] = c;
            buf_i += 1;
            fmt_i += 1;
        }
    }

    buf[buf_i] = 0;
    return @intCast(c_int, buf_i);
}

const testing = std.testing;
const expect = testing.expect;

test "mytest" {
    const res0 = try state.fmt_prefix.next('A');
    try expect(res0 == state.fmt_seq);
    const res1 = try state.fmt_prefix.next('0');
    try expect(res1 == state.fmt_flag);
    if (state.fmt_prefix.next('=')) |res2| {
        unreachable;
    } else |err| {}

    const res3 = try state.fmt_seq.next('%');
    const res4 = try state.copy.next('%');
    try expect(res3 == res4);
    try expect(res3 != state.start);

    const res5 = try copy_len('Z');
    try expect(res5 == 1);
    const res6 = try copy_len("é"[0]);
    try expect(res6 == 2);
}
