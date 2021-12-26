const std = @import("std");

const gen = @import("gen.zig");
const base = @import("base.zig");
const logic = @import("logic.zig");

pub fn validNameList(
    cal: *const base.Cal,
    raw_nlist: ?*const base.NameList,
) bool {
    const nlist = raw_nlist orelse return true;

    const months = gen.listLen([*:0]u8, nlist.month_list);
    if (months != cal.*.common_month_max and months != cal.*.leap_month_max) {
        return false;
    }

    if (gen.listLen([*:0]u8, nlist.weekday_list) != cal.*.week.length) {
        return false;
    }

    if (gen.listLen([*:0]u8, nlist.era_list) != 2) { //Reconsider for calendars with regnal era
        return false;
    }

    if (cal.*.intercalary_list) |ic_list| {
        const ic_count = gen.listLen(base.Intercalary, ic_list);

        if (nlist.intercalary_list) |nic_list| {
            if (gen.listLen([*:0]u8, nic_list) != ic_count) {
                return false;
            }
        } else {
            return false;
        }

        if (nlist.alt_intercalary_list) |alt_nic_list| {
            if (gen.listLen([*:0]u8, alt_nic_list) != ic_count) {
                return false;
            }
        }
    } else {
        if (nlist.intercalary_list) |nic_list| {
            return false;
        }
        if (nlist.alt_intercalary_list) |alt_nic_list| {
            return false;
        }
    }

    return true;
}

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

    fn isChar(self: Sequence) bool {
        return switch (self) {
            .percent, .newline, .tab => true,
            else => false,
        };
    }

    fn isNumeric(self: Sequence) bool {
        return switch (self) {
            .day_of_month, .day_of_year, .month_number, .weekday_number, .year => true,
            else => false,
        };
    }

    fn isName(self: Sequence) bool {
        return switch (self) {
            .weekday_name, .month_name, .calendar_name, .era_name => true,
            else => false,
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

const Specifier = struct {
    pad_width: usize = 0,
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

    fn getPadWidth(self: Specifier) usize {
        if (self.pad_flag) |pf| {
            if (pf == Flag.pad_none) {
                return 0;
            } else {
                return self.pad_width;
            }
        } else {
            return self.seq.defaultPaddingWidth();
        }
    }

    fn toStdFmtOptions(self: Specifier) std.fmt.FormatOptions {
        var res = std.fmt.FormatOptions{};
        const width = self.getPadWidth();
        if (width > 0) {
            res.width = width;
            if (self.getPadChar()) |fill| {
                res.fill = fill;
            }
        }
        return res;
    }
};

const DateData = struct {
    year_abs: ?u32 = null,
    month: ?u8 = null,
    day_of_month: ?u8 = null,
    day_of_year: ?u16 = null,
    day_of_week: ?u8 = null,
    after_epoch: ?bool = null,

    fn setYmd(self: DateData, mjd: i32, cal: *const base.Cal) base.Err!DateData {
        if (self.year_abs) |sy| {
            return self;
        } else {
            var raw_y: i32 = 0;
            var raw_m: u8 = 0;
            var raw_d: u8 = 0;
            try logic.mjdToYmd(mjd, cal, &raw_y, &raw_m, &raw_d);
            var dd = self;
            dd.month = raw_m;
            dd.day_of_month = raw_d;
            dd.after_epoch = raw_y > 0;
            dd.year_abs = @intCast(u32, if (raw_y > 0) raw_y else -raw_y);
            return dd;
        }
    }

    fn setFields(self: DateData, spec: Specifier, mjd: i32, cal: *const base.Cal) base.Err!DateData {
        return switch (spec.seq) {
            .percent, .tab, .calendar_name, .newline => self,
            .weekday_name, .weekday_number => weekday: {
                if (self.day_of_week) |sw| {
                    break :weekday self;
                } else {
                    var dd = self;
                    const weekday = try logic.mjdToDayOfWeek(mjd, cal);
                    dd.day_of_week = weekday;
                    if (weekday == 0) {
                        break :weekday dd.setYmd(mjd, cal);
                    } else {
                        break :weekday dd;
                    }
                }
            },
            .day_of_year => doy: {
                if (self.day_of_year) |sdoy| {
                    break :doy self;
                } else {
                    var dd = self;
                    dd.day_of_year = try logic.mjdToDayOfYear(mjd, cal);
                    break :doy dd;
                }
            },
            else => try self.setYmd(mjd, cal),
        };
    }

    fn writeNumber(self: DateData, spec: Specifier, writer: anytype) !void {
        const n = switch (spec.seq) {
            .day_of_month => @intCast(i32, self.day_of_month orelse unreachable),
            .day_of_year => @intCast(i32, self.day_of_year orelse unreachable),
            .month_number => @intCast(i32, self.month orelse unreachable),
            .weekday_number => @intCast(i32, self.day_of_week orelse unreachable),
            .year => year: {
                const after_epoch = self.after_epoch orelse unreachable;
                const y = @intCast(i32, self.year_abs orelse unreachable);
                if (after_epoch) {
                    break :year y;
                } else {
                    break :year -y;
                }
            },
            else => unreachable,
        };

        const x = if (spec.absolute_value and n < 0) -n else n;
        const opt = spec.toStdFmtOptions();
        if (x >= 0) {
            const abs = @intCast(u32, x);
            //Force the + sign to be omitted
            try std.fmt.formatInt(abs, 10, false, opt, writer);
        } else {
            try std.fmt.formatInt(n, 10, false, opt, writer);
        }
    }

    fn toIcName(
        self: DateData,
        cal: *const base.Cal,
        nlist: *const base.NameList,
    ) base.Err![*:0]const u8 {
        const y = self.year_abs orelse unreachable;
        const m = self.month orelse unreachable;
        const d = self.day_of_month orelse unreachable;
        const raw_ic = gen.seekIc(m, d, cal);
        const ic = raw_ic orelse return base.Err.BadCalendar;
        const use_alt = ic.era_start_alt_name and y == 0 and ic.day_of_year == gen.yearLen(false, cal);
        const raw_list = if (use_alt) nlist.*.alt_intercalary_list else nlist.*.intercalary_list;
        const list = raw_list orelse return base.Err.InvalidNameList;
        return list[ic.name_i] orelse base.Err.InvalidNameList;
    }

    fn writeName(
        self: DateData,
        spec: Specifier,
        cal: *const base.Cal,
        nlist: *const base.NameList,
        writer: anytype,
    ) !void {
        const name = switch (spec.seq) {
            .weekday_name => weekday: {
                const day_of_week = self.day_of_week orelse unreachable;
                if (day_of_week == @enumToInt(base.Weekday7.NoWeekday)) {
                    break :weekday try self.toIcName(cal, nlist);
                } else {
                    const i = @intCast(usize, day_of_week - 1);
                    break :weekday nlist.*.weekday_list[i] orelse return base.Err.InvalidNameList;
                }
            },
            .month_name => month: {
                const m = self.month orelse unreachable;
                if (m == 0) {
                    break :month try self.toIcName(cal, nlist);
                } else {
                    const i = @intCast(usize, m - 1);
                    break :month nlist.*.month_list[i] orelse return base.Err.InvalidNameList;
                }
            },
            .calendar_name => nlist.calendar_name,
            .era_name => era: {
                const after_epoch = self.after_epoch orelse unreachable;
                const era_idx: usize = if (after_epoch) 1 else 0;
                const era_list = nlist.*.era_list;
                break :era era_list[era_idx] orelse return base.Err.InvalidNameList;
            },
            else => unreachable,
        };

        var name_i: u32 = 0;
        const name_len = std.mem.len(name);
        const pad_width = spec.getPadWidth();
        if (spec.getPadChar()) |c| {
            if (pad_width > name_len) {
                try writer.writeByteNTimes(c, pad_width - name_len);
            }
        }
        while (name[name_i] != 0) {
            const name_c = name[name_i];
            const count = std.unicode.utf8ByteSequenceLength(name_c) catch {
                return base.Err.InvalidUtf8;
            };
            const next_name_i = name_i + count;
            _ = try writer.write(name[name_i..next_name_i]);
            name_i = next_name_i;
        }
    }
};

fn writeCopy(fmt: [*]const u8, fmt_i: usize, writer: anytype) !usize {
    const count = std.unicode.utf8ByteSequenceLength(fmt[fmt_i]) catch {
        return base.Err.InvalidUtf8;
    };
    const res_i = fmt_i + count;
    _ = try writer.write(fmt[fmt_i..res_i]);
    return res_i;
}

fn doPrefix(fmt_i: usize) usize {
    return fmt_i + 1;
}

fn doSpecWidth(fmt: [*]const u8, fmt_i: usize, spec: *Specifier) usize {
    const c = fmt[fmt_i];
    spec.*.pad_width = (spec.*.pad_width * 10) + (c - '0');
    return fmt_i + 1;
}

fn doFlag(fmt: [*]const u8, fmt_i: usize, spec: *Specifier) usize {
    const c = fmt[fmt_i];
    const f = @intToEnum(Flag, c);
    if (f == Flag.absolute_value) {
        spec.*.absolute_value = true;
    } else {
        spec.*.pad_flag = f;
    }
    return fmt_i + 1;
}

fn writeChar(spec: Specifier, writer: anytype) !void {
    const ch: u8 = switch (spec.seq) {
        .percent => '%',
        .newline => '\n',
        .tab => '\t',
        else => unreachable,
    };
    try writer.writeByte(ch);
}

fn readCopy(fmt: [*]const u8, fmt_i: usize, reader: anytype) !usize {
    const count = std.unicode.utf8ByteSequenceLength(fmt[fmt_i]) catch {
        return base.Err.InvalidUtf8;
    };
    const res_i = fmt_i + count;
    try reader.skipBytes(count, .{});
    return res_i;
}

pub fn formatW(
    mjd: i32,
    cal: *const base.Cal,
    raw_nlist: ?*const base.NameList,
    fmt: [*]const u8,
    writer: anytype,
) !void {
    if (!validNameList(cal, raw_nlist)) {
        return base.Err.InvalidNameList;
    }

    var s = State.start;
    var fmt_i: usize = 0;

    var spec = Specifier{};
    var dd = DateData{};
    while (fmt[fmt_i] != 0) {
        const c = fmt[fmt_i];
        s = try s.next(c);

        fmt_i = switch (s) {
            .copy => try writeCopy(fmt, fmt_i, writer),
            .spec_prefix => doPrefix(fmt_i),
            .spec_width => doSpecWidth(fmt, fmt_i, &spec),
            .spec_flag => doFlag(fmt, fmt_i, &spec),
            .spec_seq => seq: {
                spec.seq = @intToEnum(Sequence, c);
                if (spec.seq.isChar()) {
                    try writeChar(spec, writer);
                } else if (spec.seq.isNumeric()) {
                    dd = try dd.setFields(spec, mjd, cal);
                    try dd.writeNumber(spec, writer);
                } else if (spec.seq.isName()) {
                    const nlist = raw_nlist orelse return base.Err.NullNameList;
                    dd = try dd.setFields(spec, mjd, cal);
                    try dd.writeName(spec, cal, nlist, writer);
                } else {
                    return base.Err.InvalidSequence;
                }
                spec = Specifier{};
                break :seq fmt_i + 1;
            },
            else => fmt_i,
        };
    }
    writer.writeByte(0) catch return base.Err.FailedToInsertNullCharacter;
}

pub fn format(
    mjd: i32,
    cal: *const base.Cal,
    raw_nlist: ?*const base.NameList,
    fmt: [*]const u8,
    raw_buf: ?[]u8,
) !c_int {
    if (raw_buf) |buf| {
        if (buf.len > 0) {
            var bufWriter = std.io.fixedBufferStream(buf);
            formatW(mjd, cal, raw_nlist, fmt, bufWriter.writer()) catch |err| {
                if (err == error.NoSpaceLeft or err == base.Err.FailedToInsertNullCharacter) {
                    buf[buf.len - 1] = 0;
                }
                return format(mjd, cal, raw_nlist, fmt, null);
            };
            const res = try bufWriter.getPos();
            return @intCast(c_int, res) - 1;
        }
    }
    var counter = std.io.countingWriter(std.io.null_writer);
    try formatW(mjd, cal, raw_nlist, fmt, counter.writer());
    return @intCast(c_int, counter.bytes_written) - 1;
}
