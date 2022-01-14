const std = @import("std");

const gen = @import("gen.zig");
const base = @import("base.zig");
const logic = @import("logic.zig");

const RADIX = 10;
const MAX_LIST_LEN = 32;

pub fn validNameList(
    cal: *const base.Cal,
    raw_nlist: ?*const base.NameList,
) bool {
    const nlist = raw_nlist orelse return true;

    const months = gen.listLen([*:0]u8, nlist.*.month_list);
    if (months != cal.*.common_month_max and months != cal.*.leap_month_max) {
        return false;
    }
    if (months >= MAX_LIST_LEN) {
        return false;
    }

    const weekdays = gen.listLen([*:0]u8, nlist.*.weekday_list);
    if (weekdays != cal.*.week.length or weekdays >= MAX_LIST_LEN) {
        return false;
    }

    const eras = gen.listLen([*:0]u8, nlist.*.era_list);
    if (eras != 2 or eras >= MAX_LIST_LEN) { //Reconsider for calendars with regnal era
        return false;
    }

    if (cal.*.intercalary_list) |ic_list| {
        const ic_count = gen.listLen(base.Intercalary, ic_list);
        if ((ic_count * 2 + weekdays) >= MAX_LIST_LEN) {
            return false;
        }
        if ((ic_count * 2 + months) >= MAX_LIST_LEN) {
            return false;
        }

        if (nlist.*.intercalary_list) |nic_list| {
            if (gen.listLen([*:0]u8, nic_list) != ic_count) {
                return false;
            }
        } else {
            return false;
        }

        if (nlist.*.alt_intercalary_list) |alt_nic_list| {
            if (gen.listLen([*:0]u8, alt_nic_list) != ic_count) {
                return false;
            }
        }
    } else {
        if (nlist.*.intercalary_list) |nic_list| {
            return false;
        }
        if (nlist.*.alt_intercalary_list) |alt_nic_list| {
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

    fn canBeIntercalary(self: Sequence) bool {
        return switch (self) {
            .weekday_name, .month_name => true,
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
        } else if (std.ascii.isDigit(c)) {
            //Check digit after flag because '0' is a flag.
            return State.spec_width;
        } else if (gen.validInEnum(Sequence, c)) {
            return State.spec_seq;
        } else {
            return base.Err.InvalidSequence;
        }
    }

    fn afterWidth(c: u8) base.Err!State {
        if (std.ascii.isDigit(c)) {
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

    fn writePadding(self: Specifier, name: [*:0]const u8, writer: anytype) !void {
        const name_len = std.mem.len(name);
        const pad_width = self.getPadWidth();
        if (self.getPadChar()) |c| {
            if (pad_width > name_len) {
                try writer.writeByteNTimes(c, pad_width - name_len);
            }
        }
    }

    fn readPadding(self: Specifier, ps: anytype) !u8 {
        const pad_char = self.getPadChar() orelse ' ';
        const pad_width = self.getPadWidth();
        if (pad_width > 0) {
            var c = pad_char;
            while (c == pad_char) {
                c = try ps.*.reader().readByte();
            }
            return c;
        } else {
            return ps.*.reader().readByte();
        }
    }
};

fn checkInt(comptime T: type, negative: bool, n: u32) base.Err!T {
    if (negative) {
        return base.Err.InvalidDate;
    }
    if (n > std.math.maxInt(T)) {
        return base.Err.Overflow;
    }
    return @intCast(T, n);
}

fn readUnsignedInt(comptime T: type, start_c: u8, ps: anytype) !T {
    var n: T = 0;
    var put_back = true;
    var read_digit = false;
    var c = start_c;
    while (std.ascii.isDigit(c)) {
        const digit = try std.fmt.charToDigit(c, RADIX);
        n = try std.math.mul(T, n, RADIX);
        n = try std.math.add(T, n, digit);
        read_digit = true;
        c = ps.*.reader().readByte() catch |err| err_blk: {
            if (err == error.EndOfStream) {
                put_back = false;
                break :err_blk 0;
            } else {
                return err;
            }
        };
    }
    if (put_back) {
        try ps.*.putBackByte(c);
    }
    if (!read_digit) {
        return base.Err.DateNotFound;
    }
    return n;
}

fn readNameInArr(arr: []?[*:0]u8, ps: anytype) !usize {
    if (arr.len >= MAX_LIST_LEN) {
        return base.Err.InvalidNameList;
    }
    var done = std.bit_set.IntegerBitSet(MAX_LIST_LEN).initEmpty();
    var matches = std.bit_set.IntegerBitSet(MAX_LIST_LEN).initEmpty();
    var match_i: usize = 0;
    while (match_i < arr.len) : (match_i += 1) {
        if (arr[match_i]) |name| {
            matches.set(match_i);
        }
    }

    var name_i: usize = 0;
    var res_i: usize = 0;
    var last_c: ?u8 = null;
    while (matches.count() > done.count()) : (name_i += 1) {
        const c = ps.*.reader().readByte() catch |err| {
            if (err == error.EndOfStream) {
                last_c = null;
                break;
            } else {
                return err;
            }
        };
        match_i = 0;
        while (match_i < arr.len) : (match_i += 1) {
            if (matches.isSet(match_i) and !done.isSet(match_i)) {
                if (arr[match_i]) |name| {
                    const nc = name[name_i];
                    if (nc == 0) {
                        done.set(match_i);
                    } else {
                        if (nc == c) {
                            res_i = match_i;
                        } else {
                            matches.unset(match_i);
                        }
                    }
                }
            }
        }
        last_c = c;
    }
    if (last_c) |c| {
        try ps.*.putBackByte(c);
    }
    if (matches.count() < 1) {
        return base.Err.InvalidDate;
    }
    return res_i;
}

const DateData = struct {
    year_abs: ?u32 = null,
    month: ?u8 = null,
    day_of_month: ?u8 = null,
    day_of_year: ?u16 = null,
    day_of_week: ?u8 = null,
    after_epoch: ?bool = null,

    fn setYmdi(self: DateData, mjd: i32, cal: *const base.Cal) base.Err!DateData {
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
                        break :weekday dd.setYmdi(mjd, cal);
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
            else => try self.setYmdi(mjd, cal),
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
            try std.fmt.formatInt(abs, RADIX, false, opt, writer);
        } else {
            try std.fmt.formatInt(n, RADIX, false, opt, writer);
        }
    }

    fn toIcName(
        self: DateData,
        cal: *const base.Cal,
        nlist: *const base.NameList,
    ) base.Err![*:0]const u8 {
        const y = self.year_abs orelse return base.Err.DateNotFound;
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
            .calendar_name => nlist.*.calendar_name,
            .era_name => era: {
                const after_epoch = self.after_epoch orelse unreachable;
                const era_idx: usize = if (after_epoch) 1 else 0;
                const era_list = nlist.*.era_list;
                break :era era_list[era_idx] orelse return base.Err.InvalidNameList;
            },
            else => unreachable,
        };

        try spec.writePadding(name, writer);
        var name_i: u32 = 0;
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

    fn readNumber(self: DateData, spec: Specifier, ps: anytype) !DateData {
        var c = try spec.readPadding(ps);
        const negative = (c == '-');
        if (negative) {
            if (spec.absolute_value) {
                try ps.*.putBackByte(c);
                return self;
            }
            c = ps.*.reader().readByte() catch |err| {
                if (err == error.EndOfStream) {
                    return self;
                } else {
                    return err;
                }
            };
        }
        const n = try readUnsignedInt(u32, c, ps);

        var dd = self;
        switch (spec.seq) {
            .weekday_number => {
                dd.day_of_week = try checkInt(u8, negative, n);
            },
            .day_of_month => {
                dd.day_of_month = try checkInt(u8, negative, n);
            },
            .day_of_year => {
                dd.day_of_year = try checkInt(u16, negative, n);
            },
            .month_number => {
                dd.month = try checkInt(u8, negative, n);
            },
            .year => {
                dd.year_abs = n;
                if (negative or n == 0) {
                    dd.after_epoch = false;
                } else if (!spec.absolute_value) {
                    dd.after_epoch = true;
                }
            },
            else => unreachable,
        }
        return dd;
    }

    fn setIntercalary(
        self: DateData,
        cal: *const base.Cal,
        raw_name_i: usize,
        ic_count: usize,
    ) !DateData {
        const ic_name_i = raw_name_i % ic_count;
        const alt_name = raw_name_i >= ic_count;
        if (cal.*.intercalary_list) |ic_list| {
            var i: usize = 0;
            while (ic_list[i]) |ic| : (i += 1) {
                if (ic.name_i == ic_name_i) {
                    var res = self;
                    res.day_of_week = 0;
                    res.month = ic.month;
                    res.day_of_month = ic.day;
                    if (alt_name) {
                        if (ic.era_start_alt_name) {
                            res.year_abs = if (cal.*.year0) 0 else 1;
                            res.after_epoch = false;
                        }
                    }
                    return res;
                }
            }
        }
        return base.Err.DateNotFound;
    }

    fn readName(
        self: DateData,
        spec: Specifier,
        cal: *const base.Cal,
        nlist: *const base.NameList,
        ps: anytype,
    ) !DateData {
        var name_arr: [MAX_LIST_LEN]?[*:0]u8 = undefined;
        var i: usize = 0;
        switch (spec.seq) {
            .calendar_name => {
                name_arr[0] = nlist.*.calendar_name;
                i = 1;
            },
            .era_name => {
                while (i < 2) : (i += 1) {
                    name_arr[i] = nlist.*.era_list[i];
                }
            },
            .weekday_name => {
                while (i < cal.*.week.length) : (i += 1) {
                    name_arr[i] = nlist.*.weekday_list[i];
                }
            },
            .month_name => {
                if (cal.*.common_month_max != cal.*.leap_month_max) {
                    return base.Err.BadCalendar;
                }
                while (i < cal.*.common_month_max) : (i += 1) {
                    name_arr[i] = nlist.*.month_list[i];
                }
            },
            else => unreachable,
        }

        var ic_count: usize = 0;
        if (cal.*.intercalary_list) |ic_list| {
            if (spec.seq.canBeIntercalary()) {
                ic_count = gen.listLen(base.Intercalary, ic_list);

                if (nlist.*.intercalary_list) |nic_list| {
                    const boundary_i = i;
                    while ((i - boundary_i) < ic_count) : (i += 1) {
                        name_arr[i] = nic_list[i - boundary_i];
                    }
                }
                if (nlist.*.alt_intercalary_list) |alt_nic_list| {
                    const boundary_i = i;
                    while ((i - boundary_i) < ic_count) : (i += 1) {
                        name_arr[i] = alt_nic_list[i - boundary_i];
                    }
                }
            }
        }

        const match_i = try readNameInArr(name_arr[0..i], ps);
        var res = self;
        switch (spec.seq) {
            .calendar_name => {},
            .era_name => {
                res.after_epoch = (match_i > 0);
            },
            .weekday_name => {
                if (match_i < cal.*.week.length) {
                    res.day_of_week = (@intCast(u8, match_i) + 1);
                } else if (ic_count > 0) {
                    const ic_i = (match_i - cal.*.week.length);
                    res = try self.setIntercalary(cal, ic_i, ic_count);
                }
            },
            .month_name => {
                if (match_i < cal.*.common_month_max) {
                    res.month = (@intCast(u8, match_i) + 1);
                } else if (ic_count > 0) {
                    const ic_i = (match_i - cal.*.common_month_max);
                    res = try self.setIntercalary(cal, ic_i, ic_count);
                }
            },
            else => unreachable,
        }
        return res;
    }

    fn toMjd(self: DateData, cal: *const base.Cal) base.Err!i32 {
        const after_epoch = self.after_epoch orelse true;
        const year_abs = self.year_abs orelse return base.Err.DateNotFound;
        if (year_abs > std.math.maxInt(i32)) {
            return base.Err.Overflow;
        }
        const y = if (after_epoch) @intCast(i32, year_abs) else -@intCast(i32, year_abs);
        if (self.day_of_year) |day_of_year| {
            return logic.mjdFromDayOfYear(cal, y, day_of_year);
        } else {
            const month = self.month orelse return base.Err.DateNotFound;
            const day_of_month = self.day_of_month orelse return base.Err.DateNotFound;
            return logic.mjdFromYmd(cal, y, month, day_of_month);
        }
    }
};

fn writeCopy(c: u8, fmt_reader: anytype, writer: anytype) !void {
    const count = std.unicode.utf8ByteSequenceLength(c) catch {
        return base.Err.InvalidUtf8;
    };
    if (count > 4) {
        unreachable;
    }
    var buf: [4]u8 = undefined;
    buf[0] = c;
    const read_bytes = try fmt_reader.read(buf[1..count]);
    if (read_bytes != (count - 1)) {
        return base.Err.InvalidSequence;
    }
    _ = try writer.write(buf[0..count]);
}

fn doPrefix() void {}

fn doSpecWidth(c: u8, spec: *Specifier) void {
    spec.*.pad_width = (spec.*.pad_width * RADIX) + (c - '0');
}

fn doFlag(c: u8, spec: *Specifier) void {
    const f = @intToEnum(Flag, c);
    if (f == Flag.absolute_value) {
        spec.*.absolute_value = true;
    } else {
        spec.*.pad_flag = f;
    }
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

fn readCopy(c: u8, reader: anytype) !void {
    const count = std.unicode.utf8ByteSequenceLength(c) catch {
        return base.Err.InvalidUtf8;
    };
    if (count > 4) {
        unreachable;
    }

    var buf: [4]u8 = undefined;
    const read_bytes = try reader.read(buf[0..count]);
    if (read_bytes != count) {
        return base.Err.InvalidSequence;
    }
    if (buf[0] != c) { //Caused by digit after numeric sequence
        return base.Err.InvalidSequence;
    }
}

pub fn formatW(
    mjd: i32,
    cal: *const base.Cal,
    raw_nlist: ?*const base.NameList,
    fmt: []const u8,
    writer: anytype,
) !void {
    if (!validNameList(cal, raw_nlist)) {
        return base.Err.InvalidNameList;
    }

    var s = State.start;
    var fmt_reader = std.io.fixedBufferStream(fmt).reader();

    var spec = Specifier{};
    var dd = DateData{};
    while (true) {
        const c = fmt_reader.readByte() catch |err| {
            if (err == error.EndOfStream) {
                break;
            } else {
                return err;
            }
        };
        s = try s.next(c);

        switch (s) {
            .copy => try writeCopy(c, fmt_reader, writer),
            .spec_prefix => doPrefix(),
            .spec_width => doSpecWidth(c, &spec),
            .spec_flag => doFlag(c, &spec),
            .spec_seq => {
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
            },
            .end => break,
            .start => return base.Err.InvalidSequence,
        }
    }
    writer.writeByte(0) catch return base.Err.FailedToInsertNullCharacter;
}

pub fn format(
    mjd: i32,
    cal: *const base.Cal,
    raw_nlist: ?*const base.NameList,
    fmt: []const u8,
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

pub fn parseR(
    cal: *const base.Cal,
    raw_nlist: ?*const base.NameList,
    fmt: []const u8,
    raw_reader: anytype,
    mjd_res: *i32,
) !usize {
    if (!validNameList(cal, raw_nlist)) {
        return base.Err.InvalidNameList;
    }

    var s = State.start;
    var fmt_reader = std.io.fixedBufferStream(fmt).reader();

    var spec = Specifier{};
    var dd = DateData{};
    var ps = std.io.peekStream(1, raw_reader);
    while (true) {
        const c = fmt_reader.readByte() catch |err| {
            if (err == error.EndOfStream) {
                break;
            } else {
                return err;
            }
        };
        const prev_s = s;
        s = try s.next(c);
        if (prev_s == State.spec_seq) {
            if (s == State.spec_prefix) {
                return base.Err.DateNotFound; //Ambiguous
            }
            if (s == State.copy and std.ascii.isDigit(c)) {
                return base.Err.DateNotFound; //Ambiguous
            }
        }

        switch (s) {
            .copy => try readCopy(c, ps.reader()),
            .spec_prefix => doPrefix(),
            .spec_width => doSpecWidth(c, &spec),
            .spec_flag => doFlag(c, &spec),
            .spec_seq => {
                spec.seq = @intToEnum(Sequence, c);
                if (spec.seq.isChar()) {
                    try ps.reader().skipBytes(1, .{ .buf_size = 1 });
                } else if (spec.seq.isNumeric()) {
                    dd = try dd.readNumber(spec, &ps);
                } else if (spec.seq.isName()) {
                    const nlist = raw_nlist orelse return base.Err.NullNameList;
                    dd = try dd.readName(spec, cal, nlist, &ps);
                } else {
                    return base.Err.InvalidSequence;
                }
                spec = Specifier{};
            },
            .end => break,
            .start => return base.Err.InvalidSequence,
        }
    }

    mjd_res.* = try dd.toMjd(cal);
    return ps.fifo.readableLength();
}

pub fn parse(
    cal: *const base.Cal,
    raw_nlist: ?*const base.NameList,
    fmt: []const u8,
    buf: []const u8,
    mjd_res: *i32,
) !c_int {
    var fbs = std.io.fixedBufferStream(buf);
    const off = try parseR(cal, raw_nlist, fmt, fbs.reader(), mjd_res);
    const pos = try fbs.getPos();
    if (pos > std.math.maxInt(c_int)) {
        return base.Err.Overflow;
    }
    return @intCast(c_int, pos) - @intCast(c_int, off);
}
