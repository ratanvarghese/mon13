const std = @import("std");

const base = @import("base.zig");
const logic = @import("logic.zig");

const Flag = enum(u8) {
    pad_none = '-',
    pad_space = '_',
    pad_zero = '0',
    absolute_value = '|',

    fn getChar(self: Flag) ?u8 {
        return switch (self) {
            .pad_space => ' ',
            .pad_zero => '0',
            else => null,
        };
    }
};

const Sequence = enum(u8) {
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
    year = 'Y',

    fn getNum(self: Sequence, d: base.Date, cal: *const base.Cal) ?i32 {
        return switch (self) {
            .day_of_month => d.day,
            .day_of_year => doy: {
                const res = logic.extract(d, cal, base.ExtractMode.DAY_OF_YEAR) catch break :doy null;
                return @intCast(i32, res);
            },
            .month_number => d.month,
            .weekday_number => weekday: {
                const res = logic.extract(d, cal, base.ExtractMode.DAY_OF_WEEK) catch break :weekday null;
                return @intCast(i32, res);
            },
            .year => d.year,
            else => null,
        };
    }

    fn getEraName(year: i32, nlist: *const base.NameList) ?[*:0]const u8 {
        const era_list = nlist.*.era_list;
        if (year < 0) {
            return era_list[0];
        } else {
            return era_list[1];
        }
    }

    fn getIcName(d: base.Date, cal: *const base.Cal, nlist: *const base.NameList) ?[*:0]const u8 {
        const icr = logic.seekIcRes(d, cal) orelse return null;
        if (icr.ic.IC_ERA_START_ALT_NAME and d.year == 0 and icr.ic.day_of_year == logic.yearLen(false, cal)) {
            const alt_ic_list = nlist.*.alt_intercalary_list orelse return null;
            return alt_ic_list[icr.ici];
        } else {
            const ic_list = nlist.*.intercalary_list orelse return null;
            return ic_list[icr.ici];
        }
    }

    fn getWeekdayName(d: base.Date, cal: *const base.Cal, nlist: *const base.NameList) ?[*:0]const u8 {
        const weekday_num = logic.extract(d, cal, base.ExtractMode.DAY_OF_WEEK) catch return null;
        const weekday = @intToEnum(base.Weekday, @intCast(c_int, weekday_num));
        if (weekday == base.Weekday.MON13_NO_WEEKDAY) {
            return getIcName(d, cal, nlist);
        } else {
            const i = @intCast(usize, weekday_num - 1);
            return nlist.*.weekday_list[i];
        }
    }

    fn getMonthName(d: base.Date, cal: *const base.Cal, nlist: *const base.NameList) ?[*:0]const u8 {
        if (d.month == 0) {
            return getIcName(d, cal, nlist);
        } else {
            const i = @intCast(usize, d.month - 1);
            return nlist.*.month_list[i];
        }
    }

    fn getName(self: Sequence, d: base.Date, cal: *const base.Cal, raw_nlist: ?*const base.NameList) ?[*:0]const u8 {
        const nlist = raw_nlist orelse return null;

        return switch (self) {
            .weekday_name => getWeekdayName(d, cal, nlist),
            .month_name => getMonthName(d, cal, nlist),
            .calendar_name => nlist.calendar_name,
            .era_name => getEraName(d.year, nlist),
            else => null,
        };
    }

    fn getChar(self: Sequence) ?u8 {
        return switch (self) {
            .percent => '%',
            .newline => '\n',
            .tab => '\t',
            else => null,
        };
    }

    fn defaultPaddingWidth(self: Sequence) u8 {
        return switch (self) {
            .day_of_month => 2,
            .day_of_year => 3,
            .month_number => 2,
            else => 0,
        };
    }
};

const Digit = enum(u8) {
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

fn validInEnum(comptime E: type, x: u8) bool {
    inline for (std.meta.fields(E)) |field| {
        if (field.value == x) {
            return true;
        }
    }
    return false;
}

const State = enum(u8) {
    start,
    end,
    spec_prefix,
    spec_width,
    spec_flag,
    spec_seq,
    copy,

    fn afterStart(c: u8) State {
        return switch (c) {
            '%' => State.spec_prefix,
            0 => State.end,
            else => State.copy,
        };
    }

    fn afterPrefixOrFlag(c: u8) logic.Err!State {
        if (validInEnum(Flag, c)) {
            return State.spec_flag;
        } else if (validInEnum(Digit, c)) {
            //Check digit after flag because '0' is a flag.
            return State.spec_width;
        } else if (validInEnum(Sequence, c)) {
            return State.spec_seq;
        } else {
            return logic.Err.InvalidSequence;
        }
    }

    fn afterWidth(c: u8) logic.Err!State {
        if (validInEnum(Digit, c)) {
            return State.spec_width;
        } else if (validInEnum(Sequence, c)) {
            return State.spec_seq;
        } else {
            return logic.Err.InvalidSequence;
        }
    }

    fn next(self: State, c: u8) logic.Err!State {
        return switch (self) {
            .start, .copy, .spec_seq => afterStart(c),
            .end => logic.Err.BeyondEndState,
            .spec_prefix, .spec_flag => afterPrefixOrFlag(c),
            .spec_width => afterWidth(c),
        };
    }
};

fn copyLenUtf8(c: u8) logic.Err!u8 {
    return switch (c) {
        0b11110000...0b11110111 => 4,
        0b11100000...0b11101111 => 3,
        0b11000000...0b11011111 => 2,
        0b00000000...0b01111111 => 1,
        else => logic.Err.InvalidUtf8,
    };
}

const DigitRes = struct {
    pub const radix = 10;
    denominator: u32,
    count: u8,
};

fn countDigits(n: u32) DigitRes {
    var res = DigitRes{ .denominator = 1, .count = 1 };
    var x = n;
    while (x >= DigitRes.radix) {
        x = x / DigitRes.radix;
        res.count += 1;
        res.denominator *= DigitRes.radix;
    }
    return res;
}

fn isDryRun(buf_i: usize, raw_buf: ?[]u8) bool {
    if (raw_buf) |buf| {
        if (buf.len == 0 or buf.len == 1) { //Leave space for null character
            return true;
        } else if (buf_i > (buf.len - 1)) { //Leave space for null character
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}

fn usableBuf(buf_i: usize, raw_buf: ?[]u8) ?[]u8 {
    if (isDryRun(buf_i, raw_buf)) {
        return null;
    } else {
        return raw_buf;
    }
}

const Specifier = struct {
    pad_width: u8 = 0,
    absolute_value: bool = false,
    seq: Sequence = Sequence.percent,
    pad_flag: ?Flag = null,
};

const Position = struct {
    fmt_i: usize = 0,
    buf_i: usize = 0,
    end_i: usize = 0,
};

fn doCopy(pos: *Position, fmt: [*]const u8, raw_buf: ?[]u8) logic.Err!void {
    const count = try copyLenUtf8(fmt[pos.*.fmt_i]);
    const old_pos = Position{ .fmt_i = pos.fmt_i, .buf_i = pos.buf_i };
    pos.*.fmt_i += count;
    pos.*.buf_i += count;
    if (usableBuf(pos.*.buf_i, raw_buf)) |buf| {
        std.mem.copy(u8, buf[old_pos.buf_i..pos.*.buf_i], fmt[old_pos.fmt_i..pos.*.fmt_i]);
        pos.*.end_i += count;
    }
}

fn doPrefix(pos: *Position) void {
    pos.*.fmt_i += 1;
}

fn doSpecWidth(spec: *Specifier, pos: *Position, fmt: [*]const u8) void {
    const c = fmt[pos.*.fmt_i];
    spec.*.pad_width = (spec.*.pad_width * 10) + (c - '0');
    pos.*.fmt_i += 1;
}

fn doFlag(spec: *Specifier, pos: *Position, fmt: [*]const u8) void {
    const c = fmt[pos.*.fmt_i];
    const f = @intToEnum(Flag, c);
    if (f == Flag.absolute_value) {
        spec.*.absolute_value = true;
    } else {
        spec.*.pad_flag = f;
    }
    pos.*.fmt_i += 1;
}

fn doChar(ch: u8, pos: *Position, raw_buf: ?[]u8) void {
    pos.*.buf_i += 1;
    if (usableBuf(pos.*.buf_i, raw_buf)) |buf| {
        buf[pos.*.buf_i - 1] = ch;
        pos.*.end_i += 1;
    }
}

fn doPad(x_digit: DigitRes, spec: Specifier, pos: *Position, raw_buf: ?[]u8) void {
    var pad_char: ?u8 = null;
    var pad_width: u8 = 0;
    if (spec.pad_flag) |pf| {
        pad_char = pf.getChar();
        pad_width = spec.pad_width;
    } else {
        pad_char = '0';
        pad_width = spec.seq.defaultPaddingWidth();
    }

    if (pad_char) |pc| {
        var pad_needed = pad_width;
        while (pad_needed > x_digit.count) {
            doChar(pc, pos, raw_buf);
            pad_needed -= 1;
        }
    }
}

fn doDigits(y_digit: DigitRes, y: u32, pos: *Position, raw_buf: ?[]u8) void {
    var x = y;
    var x_digit = y_digit;
    while (x_digit.count > 0) {
        const ch = @intCast(u8, (x / x_digit.denominator)) + '0';
        doChar(ch, pos, raw_buf);

        x %= x_digit.denominator;
        x_digit.denominator /= DigitRes.radix;
        x_digit.count -= 1;
    }
}

fn doNumber(n: i32, spec: Specifier, pos: *Position, raw_buf: ?[]u8) logic.Err!void {
    if (n < 0 and !spec.absolute_value) {
        doChar('-', pos, raw_buf);
    }
    const abs_n = std.math.absInt(n) catch return logic.Err.Overflow;
    const x: u32 = @intCast(u32, abs_n);
    const x_digit = countDigits(x);
    doPad(x_digit, spec, pos, raw_buf);
    doDigits(x_digit, x, pos, raw_buf);
}

fn doName(name: [*:0]const u8, pos: *Position, raw_buf: ?[]u8) logic.Err!void {
    var name_i: u32 = 0;
    while (name[name_i] != 0) {
        const name_c = name[name_i];
        const count = try copyLenUtf8(name_c);
        const old_pos = Position{ .fmt_i = pos.*.fmt_i, .buf_i = pos.*.buf_i };
        const old_name_i = name_i;
        name_i += count;
        pos.*.buf_i += count;
        if (usableBuf(pos.*.buf_i, raw_buf)) |buf| {
            std.mem.copy(u8, buf[old_pos.buf_i..pos.*.buf_i], name[old_name_i..name_i]);
            pos.*.end_i += count;
        }
    }
}

pub fn format(
    d: base.Date,
    cal: *const base.Cal,
    raw_nlist: ?*const base.NameList,
    fmt: [*]const u8,
    raw_buf: ?[]u8,
) logic.Err!c_int {
    var s = State.start;

    var spec = Specifier{};
    var pos = Position{};

    while (fmt[pos.fmt_i] != 0) {
        const c = fmt[pos.fmt_i];
        s = try s.next(c);

        switch (s) {
            .copy => try doCopy(&pos, fmt, raw_buf),
            .spec_prefix => doPrefix(&pos),
            .spec_width => doSpecWidth(&spec, &pos, fmt),
            .spec_flag => doFlag(&spec, &pos, fmt),
            .spec_seq => {
                spec.seq = @intToEnum(Sequence, c);
                if (spec.seq.getChar()) |ch| {
                    doChar(ch, &pos, raw_buf);
                } else if (spec.seq.getNum(d, cal)) |n| {
                    try doNumber(n, spec, &pos, raw_buf);
                } else if (spec.seq.getName(d, cal, raw_nlist)) |name| {
                    try doName(name, &pos, raw_buf);
                } else {
                    return logic.Err.InvalidSequence;
                }
                pos.fmt_i += 1;
                spec = Specifier{};
            },
            else => {},
        }
    }

    if (raw_buf) |buf| {
        if (buf.len > 0) {
            if (buf.len > pos.end_i) {
                buf[pos.end_i] = 0;
            } else {
                return logic.Err.FailedToInsertNullCharacter;
            }
        }
    }
    return @intCast(c_int, pos.buf_i);
}
