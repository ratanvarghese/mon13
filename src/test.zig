const mon13 = @import("main.zig");

const std = @import("std");
const expect = std.testing.expect;

test "import mjd" {
    const mjd0: i64 = -279196381;
    const offset: i32 = 0;
    const c = &mon13.mon13_tranquility_year_zero;

    var d0 = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    var d1 = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    var impres = mon13.mon13_import(
        c,
        &mjd0,
        mon13.mon13_import_mode.MON13_IMPORT_MJD,
        &d0,
    );
    try expect(impres == 0);
    var addres = mon13.mon13_add(
        &d0,
        c,
        offset,
        mon13.mon13_add_mode.MON13_ADD_DAYS,
        &d1,
    );
    try expect(addres == 0);
    var mjd1 = mon13.mon13_extract(
        &d1,
        c,
        mon13.mon13_extract_mode.MON13_EXTRACT_MJD,
    );
    try expect((mjd1 - mjd0) == offset);
}

test "convert year 0" {
    var d = mon13.mon13_date{ .year = -1639577, .month = 7, .day = 2 };
    var res = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    const status = mon13.mon13_convert(
        &d,
        &mon13.mon13_tranquility_year_zero,
        &mon13.mon13_tranquility,
        &res,
    );

    try expect(status == 0);
    try expect(res.year == -1639578);
    try expect(res.month == 7);
    try expect(res.day == 2);
}

test "strange add gregorian" {
    var d = mon13.mon13_date{ .year = -2055615, .month = 08, .day = 19 };
    const a2: i64 = 2763584833174830080;
    const c = &mon13.mon13_gregorian_year_zero;

    const offset: i32 = @truncate(i32, a2);
    var res = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    var status = mon13.mon13_add(
        &d,
        c,
        offset,
        mon13.mon13_add_mode.MON13_ADD_YEARS,
        &res,
    );
    if (status == 0) {
        try expect(res.year == (d.year + offset));
    }
}
