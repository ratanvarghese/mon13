const mon13 = @import("mon13");

const std = @import("std");
const expect = std.testing.expect;
const mem = std.mem;

test "convert year 0" {
    const c0 = &mon13.tranquility_year_zero;
    const mjd = try mon13.mjdFromYmd(c0, -1639577, 7, 2);

    const c1 = &mon13.tranquility;
    var year: i32 = 0;
    var month: u8 = 0;
    var day: u8 = 0;
    try mon13.mjdToYmd(mjd, c1, &year, &month, &day);

    try expect(year == -1639578);
    try expect(month == 7);
    try expect(day == 2);
}

test "strange add gregorian" {
    const d_year: i32 = -2055615;
    const d_month: u8 = 08;
    const d_day: u8 = 19;
    const a2: i64 = 2763584833174830080;
    const c = &mon13.gregorian_year_zero;

    const mjd = try mon13.mjdFromYmd(c, d_year, d_month, d_day);

    const offset: i32 = @truncate(i32, a2);
    const res_mjd = mon13.addYears(mjd, c, offset) catch return;
    var res_year: i32 = 0;
    try mon13.mjdToYmd(mjd, c, &res_year, null, null);
    try expect(res_year == (d_year +% offset));
}

test "add zero years" {
    const d_year = -89713;
    const d_month = 2;
    const d_day = 29;
    const c = &mon13.gregorian;

    const mjd0 = try mon13.mjdFromYmd(c, d_year, d_month, d_day);
    const mjd1 = try mon13.addYears(mjd0, c, 0);
    try expect(mjd0 == mjd1);
}

fn skip_import(x: i64) bool {
    return (x > (std.math.maxInt(i32) / 2)) or (x < -(std.math.maxInt(i32) / 2));
}

test "strange convert" {
    const c = &mon13.tranquility_year_zero;
    const rd0: i64 = 5385873414131997696 % std.math.maxInt(i32);
    const offset: i32 = 6356633119034338304 % std.math.maxInt(i32);

    const mjd0 = try mon13.mjdFromRd(rd0);
    const mjd1 = mjd0 + offset;
    const rd1 = mon13.mjdToRd(mjd1) catch return;
}

test "Tranquility strange add" {
    const c_yz = &mon13.tranquility_year_zero;
    const offset: i32 = 544641169;

    const mjd = try mon13.mjdFromYmd(c_yz, -2796441, 0, 1);
    const res = mon13.addMonths(mjd, c_yz, offset) catch return;
}

test "holocene" {
    const mjd = try mon13.mjdFromYmd(&mon13.gregorian_year_zero, -9999, 1, 1);
    var hl_year: i32 = 0;
    var hl_month: u8 = 0;
    var hl_day: u8 = 0;
    try mon13.mjdToYmd(mjd, &mon13.holocene, &hl_year, &hl_month, &hl_day);

    try expect(hl_year == 1);
    try expect(hl_month == 1);
    try expect(hl_day == 1);
}

test "Cotsworth format" {
    const c = &mon13.cotsworth;
    const n = &mon13.cotsworth_names_en_US;
    const mjd = try mon13.mjdFromYmd(c, -2823980, 6, 29);

    const buf_size = 100;
    var buf = [_]u8{0} ** buf_size;
    const res = try mon13.format(mjd, c, n, "%A", buf[0..buf_size]);

    const expected = "Leap Day";
    for (expected) |ch, i| {
        try expect(buf[i] == ch);
    }
}

test "Cotsworth add many months" {
    const mjd = try mon13.mjdFromYmd(&mon13.cotsworth, 992456, 6, 29);
    const res0 = mon13.addMonths(mjd, &mon13.cotsworth, 209601470) catch return;
}

test "Valid" {
    try expect(mon13.validYmd(&mon13.tranquility, 1, 1, 1));
}

test "Invalid" {
    try expect(!mon13.validYmd(&mon13.tranquility, 1, 20, 20));
}

test "Tempting overflow" {
    const c = &mon13.tranquility_year_zero;
    const mjd = mon13.mjdFromYmd(c, -2147483641, 0, 2) catch return;
}

test "Day of Year" {
    const c = &mon13.holocene;
    const mjd = 383540235;
    try expect((try mon13.mjdToDayOfYear(mjd, c)) < 366);
}

test "Add Year Error 1" {
    const mjd0 = -1956022137;
    const offset = 257;
    const c = &mon13.tranquility_year_zero;
    const res = try mon13.addYears(mjd0, c, offset);
    try expect(res != 0);
}

test "Day not found? But why?" {
    const mjd0 = 1890882699;
    const c = &mon13.symmetry454;
    var y: i32 = 0;
    var m: u8 = 0;
    var d: u8 = 0;
    try mon13.mjdToYmd(mjd0, c, &y, &m, &d);
    try expect(m < 13);
}

test "Bad Calendar? But why?" {
    const mjd0 = -1347826664;
    const c = &mon13.symmetry454;
    var y: i32 = 0;
    var m: u8 = 0;
    var d: u8 = 0;
    try mon13.mjdToYmd(mjd0, c, &y, &m, &d);
    try expect(m < 13);
}

test "Date not found? But why?" {
    const mjd0 = -1786122197;
    const offset = -305900;
    const c = &mon13.symmetry454;
    const res = try mon13.addYears(mjd0, c, offset);
    try expect(res != 0);
}

test "Moon Landing Day" {
    const mjd = 40422;
    const c = &mon13.tranquility_year_zero;
    const n = &mon13.tranquility_names_en_US;
    const expected = "Moon Landing Day";

    var buf = [_]u8{0} ** 100;
    const res = try mon13.format(mjd, c, n, "%B", buf[0..buf.len]);
    try expect(res == expected.len);
    try expect(
        mem.eql(
            u8,
            expected[0..@intCast(usize, res)],
            buf[0..@intCast(usize, res)],
        ),
    );
}

test "Add Years on La Fête de la Révolution" {
    const mjd0 = -1294533640;
    const offset = 2568471;
    const c = &mon13.french_revolutionary_romme_sub1;

    const leap0 = try mon13.mjdToIsLeapYear(mjd0, c);
    try expect(leap0);

    var y0: i32 = 0;
    var m0: u8 = 0;
    var d0: u8 = 0;
    try mon13.mjdToYmd(mjd0, c, &y0, &m0, &d0);

    try expect(m0 == 0);
    try expect(d0 == 6);

    const mjd1 = try mon13.addYears(mjd0, c, offset);

    var y1: i32 = 0;
    var m1: u8 = 0;
    var d1: u8 = 0;
    try mon13.mjdToYmd(mjd1, c, &y1, &m1, &d1);

    const leap1 = try mon13.mjdToIsLeapYear(mjd1 - 2, c);
    try expect(!leap1);

    try expect(y1 == (y0 + offset + 1));
    try expect(m1 == 1);
    try expect(d1 == 1);
}

test "Rata Die Overflow Message" {
    const offset = 1389779633;
    const mjd = 757702997 + offset;
    if (mon13.mjdToRd(mjd)) |rd| {
        try expect(false);
    } else |err| {
        try expect(err == mon13.Err.Overflow);
        const msg = mon13.errorMessage(err);
        const expected = "Overflow occurred (internal)";
        try expect(mem.eql(
            u8,
            expected[0..expected.len],
            msg[0..expected.len],
        ));
    }
}
