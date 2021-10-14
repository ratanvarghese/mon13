const mon13 = @import("mon13");

const std = @import("std");
const expect = std.testing.expect;
const mem = std.mem;

test "Gregorian Sanity" {
    try expect(mon13.mon13_gregorian.intercalary_list == mon13.mon13_gregorian_year_zero.intercalary_list);
    try expect(mon13.mon13_gregorian.epoch_mjd == mon13.mon13_gregorian_year_zero.epoch_mjd);
}

test "Tranquility Sanity" {
    if (mon13.mon13_tranquility.intercalary_list) |ic| {
        try expect(ic == mon13.mon13_tranquility_year_zero.intercalary_list);
    } else {
        unreachable;
    }
}

test "Gregorian en_US Sanity" {
    try expect(mon13.mon13_gregorian_names_en_US.calendar_name[1] == 'r');
    try expect(mon13.mon13_gregorian_names_en_US.intercalary_list == null);

    if (mon13.mon13_gregorian_names_en_US.era_list[2]) |final_era| {
        unreachable;
    }

    const weekdays = mon13.mon13_gregorian_names_en_US.weekday_list;
    var di: u8 = 0;
    while (weekdays[di]) |d| : (di += 1) {
        try expect(d[0] == 'S' or d[0] == 'M' or d[0] == 'T' or d[0] == 'W' or d[0] == 'F');
    }

    if (weekdays[0]) |day0| {
        const day0_name = "Monday";
        for (day0_name) |c, i| {
            try expect(day0[i] == c);
        }
    } else {
        unreachable;
    }

    if (weekdays[1]) |day0| {
        const day0_name = "Tuesday";
        for (day0_name) |c, i| {
            try expect(day0[i] == c);
        }
    } else {
        unreachable;
    }

    if (weekdays[3]) |day0| {
        const day0_name = "Thursday";
        for (day0_name) |c, i| {
            try expect(day0[i] == c);
        }
    } else {
        unreachable;
    }

    if (weekdays[6]) |day0| {
        const day0_name = "Sunday";
        for (day0_name) |c, i| {
            try expect(day0[i] == c);
        }
    } else {
        unreachable;
    }

    if (weekdays[7]) |day7| {
        unreachable;
    }
}

test "Gregorian fr_FR Sanity" {
    if (mon13.mon13_gregorian_names_fr_FR.era_list[0]) |first_era| {
        const accent_e = "è";
        try expect(first_era[8] == accent_e[0]);
        try expect(first_era[9] == accent_e[1]);
    } else {
        unreachable;
    }
}

test "Tranquility en_US Sanity" {
    if (mon13.mon13_tranquility_names_en_US.intercalary_list) |ic_list| {
        if (ic_list[1]) |day0| {
            try expect(day0[0] == 'A');
            try expect(day0[1] == 'l');
        } else {
            unreachable;
        }
    } else {
        unreachable;
    }
}

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
    const d = mon13.mon13_date{ .year = -1639577, .month = 7, .day = 2 };
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
    const d = mon13.mon13_date{ .year = -2055615, .month = 08, .day = 19 };
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
        try expect(res.year == (d.year +% offset));
    }
}

test "add zero years" {
    const d = mon13.mon13_date{ .year = -89712, .month = 2, .day = 29 };
    const c = &mon13.mon13_gregorian;
    var res = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    var status = mon13.mon13_add(
        &d,
        c,
        0,
        mon13.mon13_add_mode.MON13_ADD_YEARS,
        &res,
    );
    if (status == 0) {
        try expect(mon13.mon13_compare(&d, &res, c) == 0);
    } else {
        unreachable;
    }
}

fn skip_import(x: i64) bool {
    return (x > (std.math.maxInt(i32) / 2)) or (x < -(std.math.maxInt(i32) / 2));
}

test "strange convert" {
    const c = &mon13.mon13_tranquility_year_zero;
    const rd0: i64 = 5385873414131997696 % std.math.maxInt(i32);
    const offset: i32 = 6356633119034338304 % std.math.maxInt(i32);

    var d0 = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    var d1 = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    var status: c_int = 0;
    status = mon13.mon13_import(c, &rd0, mon13.mon13_import_mode.MON13_IMPORT_RD, &d0);
    if (status != 0) {
        try expect(skip_import(rd0));
        return;
    }
    status = mon13.mon13_add(&d0, c, offset, mon13.mon13_add_mode.MON13_ADD_DAYS, &d1);
    if (status != 0) {
        return;
    }
    const rd1: i64 = mon13.mon13_extract(&d1, c, mon13.mon13_extract_mode.MON13_EXTRACT_RD);

    // const stdout = std.io.getStdOut().writer();
    // try stdout.print("\nd0: .year = {d}, .month = {d}, .day = {d}", .{ d0.year, d0.month, d0.day });
    // try stdout.print("\nd1: .year = {d}, .month = {d}, .day = {d}", .{ d1.year, d1.month, d1.day });
    // try stdout.print("\nrd0: {d}\nrd1: {d}\noffset: {d}\n", .{ rd0, rd1, offset });
    try expect((rd1 - rd0) == offset);
}

test "Tranquility strange add" {
    const d_yz = mon13.mon13_date{ .year = -2796441, .month = 0, .day = 1 };
    const c_yz = &mon13.mon13_tranquility_year_zero;
    const offset: i32 = 544641169;
    const m = mon13.mon13_add_mode.MON13_ADD_MONTHS;

    var res = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    var status: c_int = 0;
    status = mon13.mon13_add(&d_yz, c_yz, offset, m, &res);
    try expect(status == 0);
    // const stdout = std.io.getStdOut().writer();
    // try stdout.print("\nd0: .year = {d}, .month = {d}, .day = {d}\n", .{ res.year, res.month, res.day });
}

test "holocene" {
    const d_gr = mon13.mon13_date{ .year = -9999, .month = 1, .day = 1 };
    var d_hl = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    const c_gr = &mon13.mon13_gregorian_year_zero;
    const c_hl = &mon13.mon13_holocene;
    const status = mon13.mon13_convert(&d_gr, c_gr, c_hl, &d_hl);
    try expect(status == 0);
    try expect(d_hl.year == 1);
    try expect(d_hl.month == 1);
    try expect(d_hl.day == 1);
}

test "avoid overflow in compare" {
    const kcd_d0 = mon13.mon13_date{ .year = 1928, .month = 5, .day = 28 };
    const kcd_d1 = mon13.mon13_date{ .year = 1928, .month = 5, .day = 19 };
    const kcd_c0 = &mon13.mon13_gregorian_year_zero;
    const kcd_c1 = &mon13.mon13_cotsworth;

    var res0 = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    var res1 = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    var status: c_int = 0;
    status = mon13.mon13_convert(&kcd_d1, kcd_c1, kcd_c0, &res0);
    try expect(status == 0);
    status = mon13.mon13_convert(&kcd_d0, kcd_c0, kcd_c1, &res1);
    try expect(status == 0);

    const cmp_res0 = mon13.mon13_compare(&res0, &kcd_d0, kcd_c0);
    const cmp_res1 = mon13.mon13_compare(&res1, &kcd_d1, kcd_c1);
    if (cmp_res0 == 0) {
        try expect(cmp_res1 == 0);
    } else if (cmp_res0 > 0) {
        try expect(cmp_res1 < 0);
    } else {
        try expect(cmp_res1 > 0);
    }
}

test "Cotsworth format" {
    const d = mon13.mon13_date{ .year = -2823980, .month = 06, .day = 29 };
    const c = &mon13.mon13_cotsworth;
    const n = &mon13.mon13_cotsworth_names_en_US;

    const buf_size = 100;
    var buf = [_]u8{0} ** buf_size;
    const res = mon13.mon13_format(&d, c, n, "%A", @as([*]u8, &buf), buf_size);

    const expected = "Leap Day";
    for (expected) |ch, i| {
        try expect(buf[i] == ch);
    }
}

test "Cotsworth add many months" {
    const d = mon13.mon13_date{ .year = 992456, .month = 06, .day = 29 };
    const m = mon13.mon13_add_mode.MON13_ADD_MONTHS;
    const c = &mon13.mon13_cotsworth;
    const offset = 209601470;

    var res0 = mon13.mon13_date{ .year = 0, .month = 0, .day = 0 };
    const status = mon13.mon13_add(&d, c, offset, m, &res0);
    try expect(status == 0);
    //    try expect(res0.month == d.month);
    //    try expect(res0.day == d.day);
}
