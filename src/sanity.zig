const names = @import("names.zig");
const cal = @import("cal.zig");

const std = @import("std");
const expect = std.testing.expect;
const mem = std.mem;

test "Gregorian" {
    try expect(cal.mon13_gregorian.intercalary_list == cal.mon13_gregorian_year_zero.intercalary_list);
    try expect(cal.mon13_gregorian.epoch_mjd == cal.mon13_gregorian_year_zero.epoch_mjd);
}

test "Gregorian" {
    if (cal.mon13_tranquility.intercalary_list) |ic| {
        try expect(ic == cal.mon13_tranquility_year_zero.intercalary_list);
    } else {
        unreachable;
    }
}

test "Gregorian en_US" {
    try expect(names.mon13_gregorian_names_en_US.calendar_name[1] == 'r');
    try expect(names.mon13_gregorian_names_en_US.intercalary_list == null);

    if (names.mon13_gregorian_names_en_US.era_list[2]) |final_era| {
        unreachable;
    }

    var weekdays = names.mon13_gregorian_names_en_US.weekday_list;
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

test "Gregorian fr_FR" {
    if (names.mon13_gregorian_names_fr_FR.era_list[0]) |first_era| {
        const accent_e = "è";
        try expect(first_era[8] == accent_e[0]);
        try expect(first_era[9] == accent_e[1]);
    } else {
        unreachable;
    }
}

test "Tranquility en_US" {
    if (names.mon13_tranquility_names_en_US.intercalary_list) |ic_list| {
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
