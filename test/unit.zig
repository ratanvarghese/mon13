const mon13 = @import("mon13");

const std = @import("std");
const expect = std.testing.expect;
const mem = std.mem;

test "import mjd" {
    const mjd0: i64 = -279196381;
    const offset: i32 = 0;
    const c = &mon13.tranquility_year_zero;

    const d0 = try mon13.importMjd(c, &mjd0);

    const d1 = try mon13.addDays(d0, c, offset);
    var mjd1 = try mon13.extractMjd(d1, c);
    try expect((mjd1 - mjd0) == offset);
}

test "convert year 0" {
    const d = mon13.Date{ .year = -1639577, .month = 7, .day = 2 };
    const res = try mon13.convert(
        d,
        &mon13.tranquility_year_zero,
        &mon13.tranquility,
    );

    try expect(res.year == -1639578);
    try expect(res.month == 7);
    try expect(res.day == 2);
}

test "strange add gregorian" {
    const d = mon13.Date{ .year = -2055615, .month = 08, .day = 19 };
    const a2: i64 = 2763584833174830080;
    const c = &mon13.gregorian_year_zero;

    const offset: i32 = @truncate(i32, a2);
    const res = mon13.addYears(d, c, offset) catch return;
    try expect(res.year == (d.year +% offset));
}

test "add zero years" {
    const d = mon13.Date{ .year = -89713, .month = 2, .day = 29 };
    const c = &mon13.gregorian;
    const res = try mon13.addYears(
        d,
        c,
        0,
    );
    try expect((try mon13.compare(d, res, c)) == 0);
}

fn skip_import(x: i64) bool {
    return (x > (std.math.maxInt(i32) / 2)) or (x < -(std.math.maxInt(i32) / 2));
}

test "strange convert" {
    const c = &mon13.tranquility_year_zero;
    const rd0: i64 = 5385873414131997696 % std.math.maxInt(i32);
    const offset: i32 = 6356633119034338304 % std.math.maxInt(i32);

    const d0 = try mon13.importRd(c, &rd0);
    const d1 = try mon13.addDays(d0, c, offset);
    const rd1: i64 = try mon13.extractRd(d1, c);
    try expect((rd1 - rd0) == offset);
}

test "Tranquility strange add" {
    const d_yz = mon13.Date{ .year = -2796441, .month = 0, .day = 1 };
    const c_yz = &mon13.tranquility_year_zero;
    const offset: i32 = 544641169;

    const res = try mon13.addMonths(d_yz, c_yz, offset);
}

test "holocene" {
    const d_gr = mon13.Date{ .year = -9999, .month = 1, .day = 1 };
    const c_gr = &mon13.gregorian_year_zero;
    const c_hl = &mon13.holocene;
    const d_hl = try mon13.convert(d_gr, c_gr, c_hl);
    try expect(d_hl.year == 1);
    try expect(d_hl.month == 1);
    try expect(d_hl.day == 1);
}

test "avoid overflow in compare" {
    const kcd_d0 = mon13.Date{ .year = 1928, .month = 5, .day = 28 };
    const kcd_d1 = mon13.Date{ .year = 1928, .month = 5, .day = 19 };
    const kcd_c0 = &mon13.gregorian_year_zero;
    const kcd_c1 = &mon13.cotsworth;

    const res0 = try mon13.convert(kcd_d1, kcd_c1, kcd_c0);
    const res1 = try mon13.convert(kcd_d0, kcd_c0, kcd_c1);

    const cmp_res0 = try mon13.compare(res0, kcd_d0, kcd_c0);
    const cmp_res1 = try mon13.compare(res1, kcd_d1, kcd_c1);
    if (cmp_res0 == 0) {
        try expect(cmp_res1 == 0);
    } else if (cmp_res0 > 0) {
        try expect(cmp_res1 < 0);
    } else {
        try expect(cmp_res1 > 0);
    }
}

test "Cotsworth format" {
    const d = mon13.Date{ .year = -2823980, .month = 06, .day = 29 };
    const c = &mon13.cotsworth;
    const n = &mon13.cotsworth_names_en_US;

    const buf_size = 100;
    var buf = [_]u8{0} ** buf_size;
    const res = try mon13.format(d, c, n, "%A", buf[0..buf_size]);

    const expected = "Leap Day";
    for (expected) |ch, i| {
        try expect(buf[i] == ch);
    }
}

test "Cotsworth add many months" {
    const d = mon13.Date{ .year = 992456, .month = 06, .day = 29 };
    const c = &mon13.cotsworth;
    const offset = 209601470;

    const res0 = try mon13.addMonths(d, c, offset);
}

test "Valid" {
    const d = mon13.Date{ .year = 1, .month = 1, .day = 1 };
    const c = &mon13.tranquility;
    try expect(mon13.valid(d, c));
}

test "Invalid" {
    const d = mon13.Date{ .year = 1, .month = 20, .day = 20 };
    const c = &mon13.tranquility;
    try expect(!mon13.valid(d, c));
}

test "Tempting overflow" {
    const d = mon13.Date{ .year = -2147483641, .month = 0, .day = 2 };
    const c = &mon13.tranquility_year_zero;

    const res = try mon13.extractIsLeapYear(d, c);
    try expect(res != 0);
}

test "Add like other cal" {
    const d = mon13.Date{ .year = 2302144, .month = 06, .day = 25 };
    const c_yz = &mon13.gregorian_year_zero;
    const c = &mon13.gregorian;
    const offset: i32 = (11113292707091716096 % std.math.maxInt(i32));

    const d_yz = try mon13.convert(d, c, c_yz);
    if (mon13.addDays(d_yz, c_yz, offset)) |res| {} else |err| {
        try expect(err == mon13.Err.Overflow);
    }
}
