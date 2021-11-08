const std = @import("std");

const gen = @import("gen.zig");
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

    fn getNum(self: Sequence, mjd: i32, cal: *const base.Cal) base.Err!?i32 {
        return switch (self) {
            .day_of_month => day: {
                var res: u8 = 0;
                try logic.mjdToYmd(mjd, cal, null, null, &res);
                break :day @intCast(i32, res);
            },
            .day_of_year => doy: {
                const res = try logic.mjdToDayOfYear(mjd, cal);
                break :doy @intCast(i32, res);
            },
            .month_number => month: {
                var res: u8 = 0;
                try logic.mjdToYmd(mjd, cal, null, &res, null);
                break :month @intCast(i32, res);
            },
            .weekday_number => weekday: {
                const res = try logic.mjdToDayOfWeek(mjd, cal);
                break :weekday @intCast(i32, res);
            },
            .year => year: {
                var res: i32 = 0;
                try logic.mjdToYmd(mjd, cal, &res, null, null);
                break :year res;
            },
            else => null,
        };
    }

    fn getEraName(mjd: i32, cal: *const base.Cal, nlist: *const base.NameList) base.Err!?[*:0]const u8 {
        var year: i32 = 0;
        try logic.mjdToYmd(mjd, cal, &year, null, null);
        const era_list = nlist.*.era_list;
        if (year < 0) {
            return era_list[0];
        } else {
            return era_list[1];
        }
    }

    fn getIcName(mjd: i32, cal: *const base.Cal, nlist: *const base.NameList) base.Err!?[*:0]const u8 {
        var d = base.Date{ .year = 0, .month = 0, .day = 0 };
        try logic.mjdToYmd(mjd, cal, &d.year, &d.month, &d.day);
        const icr = logic.seekIcRes(d, cal) orelse return null;
        if (icr.ic.era_start_alt_name and d.year == 0 and icr.ic.day_of_year == logic.yearLen(false, cal)) {
            const alt_ic_list = nlist.*.alt_intercalary_list orelse return null;
            return alt_ic_list[icr.ici];
        } else {
            const ic_list = nlist.*.intercalary_list orelse return null;
            return ic_list[icr.ici];
        }
    }

    fn getWeekdayName(mjd: i32, cal: *const base.Cal, nlist: *const base.NameList) base.Err!?[*:0]const u8 {
        const weekday = try logic.mjdToDayOfWeek(mjd, cal);
        if (weekday == @enumToInt(base.Weekday7.NoWeekday)) {
            return getIcName(mjd, cal, nlist);
        } else {
            const i = @intCast(usize, weekday - 1);
            return nlist.*.weekday_list[i];
        }
    }

    fn getMonthName(mjd: i32, cal: *const base.Cal, nlist: *const base.NameList) base.Err!?[*:0]const u8 {
        var month: u8 = 0;
        try logic.mjdToYmd(mjd, cal, null, &month, null);
        if (month == 0) {
            return getIcName(mjd, cal, nlist);
        } else {
            const i = @intCast(usize, month - 1);
            return nlist.*.month_list[i];
        }
    }

    fn getName(self: Sequence, mjd: i32, cal: *const base.Cal, raw_nlist: ?*const base.NameList) base.Err!?[*:0]const u8 {
        const nlist = raw_nlist orelse return null;
        return switch (self) {
            .weekday_name => try getWeekdayName(mjd, cal, nlist),
            .month_name => try getMonthName(mjd, cal, nlist),
            .calendar_name => nlist.calendar_name,
            .era_name => try getEraName(mjd, cal, nlist),
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

const DigitChar = enum(u8) {
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

    fn afterPrefixOrFlag(c: u8) base.Err!State {
        if (gen.validInEnum(Flag, c)) {
            return State.spec_flag;
        } else if (gen.validInEnum(DigitChar, c)) {
            //Check digit after flag because '0' is a flag.
            return State.spec_width;
        } else if (gen.validInEnum(Sequence, c)) {
            return State.spec_seq;
        } else {
            return base.Err.InvalidSequence;
        }
    }

    fn afterWidth(c: u8) base.Err!State {
        if (gen.validInEnum(DigitChar, c)) {
            return State.spec_width;
        } else if (gen.validInEnum(Sequence, c)) {
            return State.spec_seq;
        } else {
            return base.Err.InvalidSequence;
        }
    }

    fn next(self: State, c: u8) base.Err!State {
        return switch (self) {
            .start, .copy, .spec_seq => afterStart(c),
            .end => base.Err.BeyondEndState,
            .spec_prefix, .spec_flag => afterPrefixOrFlag(c),
            .spec_width => afterWidth(c),
        };
    }
};

fn copyLenUtf8(c: u8) base.Err!u8 {
    return switch (c) {
        0b11110000...0b11110111 => 4,
        0b11100000...0b11101111 => 3,
        0b11000000...0b11011111 => 2,
        0b00000000...0b01111111 => 1,
        else => base.Err.InvalidUtf8,
    };
}

const DigitInfo = struct {
    pub const radix = 10;
    denominator: u32,
    count: u8,
    value: u32,

    fn fromInt(m: i32) base.Err!DigitInfo {
        const abs_m = std.math.absInt(m) catch return base.Err.Overflow;
        var x: u32 = @intCast(u32, abs_m);
        var res = DigitInfo{ .denominator = 1, .count = 1, .value = x };
        while (x >= DigitInfo.radix) {
            x = x / DigitInfo.radix;
            res.count += 1;
            res.denominator *= DigitInfo.radix;
        }
        return res;
    }

    fn getChar(self: DigitInfo) u8 {
        return @intCast(u8, (self.value / self.denominator)) + '0';
    }

    fn removeTopDigit(self: DigitInfo) DigitInfo {
        const d = self.denominator; //pull out to prevent modulo by zero
        return DigitInfo{
            .denominator = d / DigitInfo.radix,
            .count = self.count - 1,
            .value = self.value % d,
        };
    }
};

const Specifier = struct {
    pad_width: u8 = 0,
    absolute_value: bool = false,
    seq: Sequence = Sequence.percent,
    pad_flag: ?Flag = null,

    fn getPadChar(self: Specifier) ?u8 {
        if (self.pad_flag) |pf| {
            return pf.getChar();
        } else {
            return '0';
        }
    }

    fn getPadWidth(self: Specifier) u8 {
        if (self.pad_flag) |pf| {
            return self.pad_width;
        } else {
            return self.seq.defaultPaddingWidth();
        }
    }
};

const Position = struct {
    fmt_i: usize = 0,
    buf_i: usize = 0,
    end_i: usize = 0,

    fn isDryRun(self: Position, raw_buf: ?[]u8) bool {
        if (raw_buf) |buf| {
            if (buf.len == 0 or buf.len == 1) { //Leave space for null character
                return true;
            } else if (self.buf_i > (buf.len - 1)) { //Leave space for null character
                return true;
            } else {
                return false;
            }
        } else {
            return true;
        }
    }

    fn usableBuf(self: Position, raw_buf: ?[]u8) ?[]u8 {
        if (self.isDryRun(raw_buf)) {
            return null;
        } else {
            return raw_buf;
        }
    }

    fn similar(self: Position, fmt_shift: usize, buf_shift: usize) Position {
        return Position{
            .fmt_i = self.fmt_i + fmt_shift,
            .buf_i = self.buf_i + buf_shift,
            .end_i = self.end_i,
        };
    }

    fn doCopy(self: Position, fmt: [*]const u8, raw_buf: ?[]u8) base.Err!Position {
        const count = try copyLenUtf8(fmt[self.fmt_i]);
        var res = self.similar(count, count);
        if (res.usableBuf(raw_buf)) |buf| {
            var dest = buf[self.buf_i..res.buf_i];
            var src = fmt[self.fmt_i..res.fmt_i];
            std.mem.copy(u8, dest, src);
            res.end_i += count;
        }
        return res;
    }

    fn doPrefix(self: Position) Position {
        return self.similar(1, 0);
    }

    fn doSpecWidth(self: Position, spec: *Specifier, fmt: [*]const u8) Position {
        const c = fmt[self.fmt_i];
        spec.*.pad_width = (spec.*.pad_width * 10) + (c - '0');
        return self.similar(1, 0);
    }

    fn doFlag(self: Position, spec: *Specifier, fmt: [*]const u8) Position {
        const c = fmt[self.fmt_i];
        const f = @intToEnum(Flag, c);
        if (f == Flag.absolute_value) {
            spec.*.absolute_value = true;
        } else {
            spec.*.pad_flag = f;
        }
        return self.similar(1, 0);
    }

    fn doChar(self: Position, ch: u8, raw_buf: ?[]u8) Position {
        var res = self.similar(0, 1);
        if (res.usableBuf(raw_buf)) |buf| {
            buf[self.buf_i] = ch;
            res.end_i += 1;
        }
        return res;
    }

    fn doNumber(self: Position, n: i32, spec: Specifier, raw_buf: ?[]u8) base.Err!Position {
        var res = self;
        if (n < 0 and !spec.absolute_value) {
            res = res.doChar('-', raw_buf);
        }
        const x_digit = try DigitInfo.fromInt(n);
        res = res.doPad(x_digit, spec, raw_buf);
        res = res.doDigits(x_digit, raw_buf);
        return res;
    }

    fn doPad(self: Position, x_digit: DigitInfo, spec: Specifier, raw_buf: ?[]u8) Position {
        var res = self;
        if (spec.getPadChar()) |pc| {
            var pad_needed = spec.getPadWidth();
            while (pad_needed > x_digit.count) {
                res = res.doChar(pc, raw_buf);
                pad_needed -= 1;
            }
        }
        return res;
    }

    fn doDigits(self: Position, y_digit: DigitInfo, raw_buf: ?[]u8) Position {
        var res = self;
        var x_digit = y_digit;
        while (x_digit.count > 0) {
            res = res.doChar(x_digit.getChar(), raw_buf);
            x_digit = x_digit.removeTopDigit();
        }
        return res;
    }

    fn doName(self: Position, name: [*:0]const u8, raw_buf: ?[]u8) base.Err!Position {
        var name_i: u32 = 0;
        var res = self;
        while (name[name_i] != 0) {
            const name_c = name[name_i];
            const count = try copyLenUtf8(name_c);
            const next_name_i = name_i + count;
            var next_res = res.similar(0, count);
            if (next_res.usableBuf(raw_buf)) |buf| {
                var dest = buf[res.buf_i..next_res.buf_i];
                var src = name[name_i..next_name_i];
                std.mem.copy(u8, dest, src);
                next_res.end_i += count;
            }
            name_i = next_name_i;
            res = next_res;
        }
        return res;
    }

    fn doTerminator(self: Position, raw_buf: ?[]u8) base.Err!void {
        if (raw_buf) |buf| {
            if (buf.len > 0) {
                if (buf.len > self.end_i) {
                    buf[self.end_i] = 0;
                } else {
                    return base.Err.FailedToInsertNullCharacter;
                }
            }
        }
    }
};

pub fn format(
    mjd: i32,
    cal: *const base.Cal,
    raw_nlist: ?*const base.NameList,
    fmt: [*]const u8,
    raw_buf: ?[]u8,
) base.Err!c_int {
    var s = State.start;

    var spec = Specifier{};
    var pos = Position{};

    while (fmt[pos.fmt_i] != 0) {
        const c = fmt[pos.fmt_i];
        s = try s.next(c);

        pos = switch (s) {
            .copy => try pos.doCopy(fmt, raw_buf),
            .spec_prefix => pos.doPrefix(),
            .spec_width => pos.doSpecWidth(&spec, fmt),
            .spec_flag => pos.doFlag(&spec, fmt),
            .spec_seq => seq: {
                var res = Position{};
                spec.seq = @intToEnum(Sequence, c);
                if (spec.seq.getChar()) |ch| {
                    res = pos.doChar(ch, raw_buf);
                } else if (try spec.seq.getNum(mjd, cal)) |n| {
                    res = try pos.doNumber(n, spec, raw_buf);
                } else if (try spec.seq.getName(mjd, cal, raw_nlist)) |name| {
                    res = try pos.doName(name, raw_buf);
                } else {
                    return base.Err.InvalidSequence;
                }
                res.fmt_i = pos.fmt_i + 1;
                spec = Specifier{};
                break :seq res;
            },
            else => pos,
        };
    }

    try pos.doTerminator(raw_buf);
    return @intCast(c_int, pos.buf_i);
}
