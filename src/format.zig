const std = @import("std");

const base = @import("base.zig");
const logic = @import("logic.zig");

const FormatErr = error{
    InvalidUtf8,
    BeyondEndState,
    InvalidSequence,
};

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

    fn getNum(self: Sequence, d: *const base.Date, cal: *const base.Cal) ?i32 {
        return switch (self) {
            .day_of_month => d.*.day,
            .day_of_year => @intCast(i32, logic.mon13_extract(d, cal, base.ExtractMode.DAY_OF_YEAR)),
            .month_number => d.*.month,
            .weekday_number => @intCast(i32, logic.mon13_extract(d, cal, base.ExtractMode.DAY_OF_WEEK)),
            .year => d.*.year,
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

    fn getIcName(d: *const base.Date, cal: *const base.Cal, nlist: *const base.NameList) ?[*:0]const u8 {
        const icr = logic.seekIcRes(d.*, cal) orelse return null;
        if (icr.ic.IC_ERA_START_ALT_NAME and d.year == 0 and icr.ic.day_of_year == logic.yearLen(false, cal)) {
            const alt_ic_list = nlist.*.alt_intercalary_list orelse return null;
            return alt_ic_list[icr.ici];
        } else {
            const ic_list = nlist.*.intercalary_list orelse return null;
            return ic_list[icr.ici];
        }
    }

    fn getWeekdayName(d: *const base.Date, cal: *const base.Cal, nlist: *const base.NameList) ?[*:0]const u8 {
        const weekday_num = logic.mon13_extract(d, cal, base.ExtractMode.DAY_OF_WEEK);
        const weekday = @intToEnum(base.Weekday, @intCast(c_int, weekday_num));
        if (weekday == base.Weekday.MON13_NO_WEEKDAY) {
            return getIcName(d, cal, nlist);
        } else {
            const i = @intCast(usize, weekday_num - 1);
            return nlist.*.weekday_list[i];
        }
    }

    fn getMonthName(d: *const base.Date, cal: *const base.Cal, nlist: *const base.NameList) ?[*:0]const u8 {
        if (d.*.month == 0) {
            return getIcName(d, cal, nlist);
        } else {
            const i = @intCast(usize, d.*.month - 1);
            return nlist.*.month_list[i];
        }
    }

    fn getName(self: Sequence, d: *const base.Date, cal: *const base.Cal, raw_nlist: ?*const base.NameList) ?[*:0]const u8 {
        const nlist = raw_nlist orelse return null;

        return switch (self) {
            .weekday_name => getWeekdayName(d, cal, nlist),
            .month_name => getMonthName(d, cal, nlist),
            .calendar_name => nlist.calendar_name,
            .era_name => getEraName(d.*.year, nlist),
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
    fmt_prefix,
    fmt_width,
    fmt_flag,
    fmt_seq,
    copy,

    fn afterStart(c: u8) State {
        return switch (c) {
            '%' => State.fmt_prefix,
            0 => State.end,
            else => State.copy,
        };
    }

    fn afterPrefixOrFlag(c: u8) FormatErr!State {
        if (validInEnum(Flag, c)) {
            return State.fmt_flag;
        } else if (validInEnum(Digit, c)) {
            //Check digit after flag because '0' is a flag.
            return State.fmt_width;
        } else if (validInEnum(Sequence, c)) {
            return State.fmt_seq;
        } else {
            return FormatErr.InvalidSequence;
        }
    }

    fn afterWidth(c: u8) FormatErr!State {
        if (validInEnum(Digit, c)) {
            return State.fmt_width;
        } else if (validInEnum(Sequence, c)) {
            return State.fmt_seq;
        } else {
            return FormatErr.InvalidSequence;
        }
    }

    fn next(self: State, c: u8) FormatErr!State {
        return switch (self) {
            .start => afterStart(c),
            .end => FormatErr.BeyondEndState,
            .fmt_prefix, .fmt_flag => afterPrefixOrFlag(c),
            .fmt_width => afterWidth(c),
            .fmt_seq => afterStart(c),
            .copy => afterStart(c),
        };
    }
};

fn copyLenUtf8(c: u8) FormatErr!u8 {
    return switch (c) {
        0b11110000...0b11110111 => 4,
        0b11100000...0b11101111 => 3,
        0b11000000...0b11011111 => 2,
        0b00000000...0b01111111 => 1,
        else => FormatErr.InvalidUtf8,
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

fn isDryRun(buflen: u32, raw_buf: ?[*]u8) bool {
    if (buflen == 0) {
        return true;
    } else if (raw_buf) |buf| {
        return false;
    } else {
        return true;
    }
}

fn usableBuf(dry_run: bool, raw_buf: ?[*]u8) ?[*]u8 {
    if (dry_run) {
        return null;
    } else {
        return raw_buf;
    }
}

const fmt_info = struct {
    pad_width: u8 = 0,
    absolute_value: bool = false,
    seq: Sequence = Sequence.percent,
    pad_flag: ?Flag = null,
};

pub export fn mon13_format(
    raw_d: ?*const base.Date,
    raw_cal: ?*const base.Cal,
    raw_nlist: ?*const base.NameList,
    raw_fmt: ?[*]const u8,
    raw_buf: ?[*]u8,
    buflen: u32,
) c_int {
    const d = raw_d orelse return -1;
    const cal = raw_cal orelse return -2;
    const fmt = raw_fmt orelse return -3;
    const dry_run = isDryRun(buflen, raw_buf);

    const buf_limit = buflen -% 1; //Leave space for null character.
    var fmt_i: usize = 0;
    var buf_i: usize = 0;
    var s = State.start;

    var info = fmt_info{};

    while (fmt[fmt_i] != 0 and (dry_run or buf_i < buf_limit)) {
        const c = fmt[fmt_i];
        s = s.next(c) catch return -5;

        if (s == State.copy) {
            const count = copyLenUtf8(c) catch return -6;
            const old_fmt_i = fmt_i;
            const old_buf_i = buf_i;
            fmt_i += count;
            buf_i += count;
            if (buf_i <= buf_limit) {
                if (usableBuf(dry_run, raw_buf)) |buf| {
                    std.mem.copy(u8, buf[old_buf_i..buf_i], fmt[old_fmt_i..fmt_i]);
                }
            } else {
                buf_i = old_buf_i;
                s = State.end;
                break;
            }
        } else if (s == State.fmt_prefix) {
            fmt_i += 1;
        } else if (s == State.fmt_width) {
            info.pad_width = (info.pad_width * 10) + (c - '0');
            fmt_i += 1;
        } else if (s == State.fmt_flag) {
            const f = @intToEnum(Flag, c);
            if (f == Flag.absolute_value) {
                info.absolute_value = true;
            } else {
                info.pad_flag = f;
            }
            fmt_i += 1;
        } else if (s == State.fmt_seq) {
            info.seq = @intToEnum(Sequence, c);
            if (info.seq.getChar()) |ch| {
                if (usableBuf(dry_run, raw_buf)) |buf| {
                    buf[buf_i] = ch;
                }
                buf_i += 1;
            } else if (info.seq.getNum(d, cal)) |n| {
                var x: u32 = 0;
                if (n < 0) {
                    x = @intCast(u32, -n);
                    if (!info.absolute_value) {
                        if (usableBuf(dry_run, raw_buf)) |buf| {
                            buf[buf_i] = '-';
                        }
                        buf_i += 1;
                    }
                } else {
                    x = @intCast(u32, n);
                }
                var x_digit = countDigits(x);

                var pad_char: ?u8 = null;
                var pad_width: u8 = 0;
                if (info.pad_flag) |pf| {
                    pad_char = pf.getChar();
                    pad_width = info.pad_width;
                } else {
                    pad_char = '0';
                    pad_width = info.seq.defaultPaddingWidth();
                }

                if (pad_char) |pc| {
                    var pad_needed = pad_width;
                    while (pad_needed > x_digit.count and buf_i <= buf_limit) {
                        if (usableBuf(dry_run, raw_buf)) |buf| {
                            buf[buf_i] = pc;
                        }
                        buf_i += 1;
                        pad_needed -= 1;
                    }
                }
                while (x_digit.count > 0 and buf_i <= buf_limit) {
                    if (usableBuf(dry_run, raw_buf)) |buf| {
                        buf[buf_i] = @intCast(u8, (x / x_digit.denominator)) + '0';
                    }
                    x %= x_digit.denominator;
                    x_digit.denominator /= DigitRes.radix;
                    buf_i += 1;
                    x_digit.count -= 1;
                }
            } else if (info.seq.getName(d, cal, raw_nlist)) |name| {
                var name_i: u32 = 0;
                while (name[name_i] != 0) {
                    const name_c = name[name_i];
                    const count = copyLenUtf8(name_c) catch return -7;
                    const old_buf_i = buf_i;
                    const old_name_i = name_i;
                    name_i += count;
                    buf_i += count;
                    if (buf_i <= buf_limit) {
                        if (usableBuf(dry_run, raw_buf)) |buf| {
                            std.mem.copy(u8, buf[old_buf_i..buf_i], name[old_name_i..name_i]);
                        }
                    } else {
                        buf_i = old_buf_i;
                        s = State.end;
                        break;
                    }
                }
            } else {
                //return -7;
            }
            fmt_i += 1;
            info = fmt_info{};
        }
    }

    if (usableBuf(dry_run, raw_buf)) |buf| {
        buf[buf_i] = 0;
    }
    return @intCast(c_int, buf_i);
}
