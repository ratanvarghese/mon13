const mon13 = @import("main.zig");

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
        const day0_name = "Sunday";
        for (day0_name) |c, i| {
            try expect(day0[i] == c);
        }
    } else {
        unreachable;
    }

    if (weekdays[1]) |day0| {
        const day0_name = "Monday";
        for (day0_name) |c, i| {
            try expect(day0[i] == c);
        }
    } else {
        unreachable;
    }

    if (weekdays[3]) |day0| {
        const day0_name = "Wednesday";
        for (day0_name) |c, i| {
            try expect(day0[i] == c);
        }
    } else {
        unreachable;
    }

    if (weekdays[6]) |day0| {
        const day0_name = "Saturday";
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

test "add zero years" {
    var d = mon13.mon13_date{ .year = -89712, .month = 2, .day = 29 };
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
