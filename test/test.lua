local mon13 = require("mon13")

do
	local d = {year = 13, month = 13, day = 13}
	assert(not mon13.validYmd(mon13.gregorian, d), "detect invalid ymd")
end

do
	local d = {year = 10, month = 10, day = 10}
	assert(mon13.validYmd(mon13.gregorian, d), "detect valid ymd")
end

do
	local d = {year = 13, month = 13, day = 13}
	local status, msg = pcall(mon13.mjdFromYmd, mon13.gregorian, d)
	assert(not status, "mjdFromYmd error status")
	local expected = "Invalid date"
	assert(msg:sub(-#expected) == expected, "mjdFromYmd error message")
end

do
	local d0 = {year = 10, month = 10, day = 10}
	local d1 = {year = d0.year, month = d0.month, day = d0.day + 1}
	local mjd0 = mon13.mjdFromYmd(mon13.tranquility, d0)
	local mjd1 = mon13.mjdFromYmd(mon13.tranquility, d1)
	assert(mjd1 == (mjd0 + 1), "add 1 to day, add 1 to MJD")
end

do
	local u0 = 234235423
	local u1 = u0 + (60 * 60 * 24)
	local mjd0 = mon13.mjdFromUnix(u0)
	local mjd1 = mon13.mjdFromUnix(u1)
	assert(mjd1 == (mjd0 + 1), "add 60 * 60 * 24 to Unix, add 1 to MJD")
end

do
	local rd0 = 1234123
	local rd1 = rd0 + 1
	local mjd0 = mon13.mjdFromRd(rd0)
	local mjd1 = mon13.mjdFromRd(rd1)
	assert(mjd1 == (mjd0 + 1), "add 1 to RD, add 1 to MJD")
end

do
	local mjd0 = 462342
	local ymd0 = mon13.mjdToYmd(mjd0, mon13.julian)
	local mjd1 = mon13.mjdFromYmd(mon13.julian, ymd0)
	assert(mjd1 == mjd0, "ymd round trip")
end

do
	local mjd0 = 462342
	local mjd1 = mon13.mjdFromUnix(mon13.mjdToUnix(mjd0))
	assert(mjd1 == mjd0, "Unix round trip")
end

do
	local mjd0 = 462342
	local mjd1 = mon13.mjdFromRd(mon13.mjdToRd(mjd0))
	assert(mjd1 == mjd0, "Rd round trip")
end

do
	local offset = 1389779633
	local mjd = 757702997 + offset
	local status, msg = pcall(mon13.mjdToRd, mjd)
	assert(not status, "mjdToRd error status")
	local expected = "Overflow occurred (internal)"
	assert(msg:sub(-#expected) == expected, "mjdToRd error message")
end

do
	local d = {year = 2000, month = 13, day = 13}
	local mjd = mon13.mjdFromYmd(mon13.cotsworth, d)
	local is_leap = mon13.mjdToIsLeapYear(mjd, mon13.cotsworth)
	assert(is_leap, "detect leap year")
end

do
	local d = {year = 2001, month = 13, day = 13}
	local mjd = mon13.mjdFromYmd(mon13.cotsworth, d)
	local is_leap = mon13.mjdToIsLeapYear(mjd, mon13.cotsworth)
	assert(not is_leap, "detect non-leap year")
end

do
	local mjd0 = 343542
	local mjd1 = mjd0 + 1
	local dow0 = mon13.mjdToDayOfWeek(mjd0, mon13.holocene)
	local dow1 = mon13.mjdToDayOfWeek(mjd1, mon13.holocene)
	assert(dow1 == (dow0 + 1), "add 1 to MJD, add 1 to day of week")
end

do
	local mjd0 = 343543
	local mjd1 = mjd0 + 1
	local dow0 = mon13.mjdToDayOfWeek(mjd0, mon13.holocene)
	local dow1 = mon13.mjdToDayOfWeek(mjd1, mon13.holocene)
	assert(dow0 == 7 and dow1 == 1, "add 1 to MJD, start next week")
	assert(dow0 == mon13.Weekday7.SUNDAY, "End week on Sunday")
	assert(dow1 == mon13.Weekday7.MONDAY, "Start week on Monday")
end

do
	local mjd0 = 343542
	local mjd1 = mjd0 + 1
	local doy0 = mon13.mjdToDayOfYear(mjd0, mon13.ancient_egyptian)
	local doy1 = mon13.mjdToDayOfYear(mjd1, mon13.ancient_egyptian)
	assert(doy1 == (doy0 + 1), "add 1 to MJD, add 1 to day of year")
end

do
	local d0 = {year = 60, month = 1, day = 4}
	local offset = 2
	local mjd0 = mon13.mjdFromYmd(mon13.positivist, d0)
	local mjd1 = mon13.addMonths(mjd0, mon13.positivist, offset)
	local d1 = mon13.mjdToYmd(mjd1, mon13.positivist)
	assert(d1.year == d0.year, "add 2 to month, same year")
	assert(d1.month == (d0.month + 2), "add 2 to month")
	assert(d1.day == d0.day, "add 2 to month, same day")
end

do
	local d0 = {year = 60, month = 1, day = 4}
	local offset = 2
	local mjd0 = mon13.mjdFromYmd(mon13.symmetry454, d0)
	local mjd1 = mon13.addYears(mjd0, mon13.symmetry454, offset)
	local d1 = mon13.mjdToYmd(mjd1, mon13.symmetry454)
	assert(d1.year == (d0.year + 2), "add 2 to year")
	assert(d1.month == d0.month, "add 2 to year, same month")
	assert(d1.day == d0.day, "add 2 to year, same day")
end

do
	local d0 = {year = 10, month = 10, day = 10}
	local d1 = {year = d0.year, month = d0.month + 3, day = d0.day}
	local mjd0 = mon13.mjdFromYmd(mon13.tranquility, d0)
	local mjd1 = mon13.mjdFromYmd(mon13.tranquility, d1)
	local diff = mon13.diffMonths(mjd1, mjd0, mon13.tranquility)
	assert(diff == 3, "diff months")
end

do
	local d0 = {year = 10, month = 10, day = 10}
	local d1 = {year = d0.year + 4, month = d0.month, day = d0.day}
	local mjd0 = mon13.mjdFromYmd(mon13.tranquility, d0)
	local mjd1 = mon13.mjdFromYmd(mon13.tranquility, d1)
	local diff = mon13.diffYears(mjd1, mjd0, mon13.tranquility)
	assert(diff == 4, "diff years")
end

do
	local d = {year = 1999, month = 4, day = 7}
	local mjd = mon13.mjdFromYmd(mon13.gregorian, d)
	local s = mon13.format(mjd, mon13.gregorian, "%Y-%m-%d")
	assert(s == "1999-04-07", "format, no name list")
end

do
	local d = {year = 2021, month = 11, day = 16}
	local mjd = mon13.mjdFromYmd(mon13.gregorian, d)
	local s = mon13.format(
		mjd,
		mon13.gregorian,
		mon13.names_en_US_gregorian,
		"%-d %B %Y, %q (%f)"
	)
	assert(s == "16 November 2021, Common Era (Gregorian)", "format")
end

do
	local nlist = {
		month_list = {
			"A",
			"B",
			"C",
			"D",
			"E",
			"F",
			"G",
			"H",
			"I",
			"J",
			"K",
			"L",
			"M"
		},
		weekday_list = mon13.names_en_US_tranquility.weekday_list,
		era_list = {
			"BT",
			"AT"
		},
		intercalary_list = {
			"ARM",
			"ALD"
		},
		alt_intercalary_list = {
			"MLD",
			""
		},
		calendar_name = mon13.names_en_US_tranquility.calendar_name
	}

	local d = {year = 50, month = 4, day = 1}
	local mjd = mon13.mjdFromYmd(mon13.tranquility, d)
	local s = mon13.format(mjd, mon13.tranquility, nlist, "%Y-%B%d (%f)")
	assert(s == "50-D01 (Tranquility)", "format, custom namelist")
end

do
	local nlist = {
		month_list = {
			"A",
			"B",
			"C",
			"D",
			"E",
			"F",
			"G",
			"H",
			"I",
			"J",
			"K",
			"L",
		},
		weekday_list = mon13.names_en_US_tranquility.weekday_list,
		era_list = {
			"BT",
			"AT"
		},
		intercalary_list = {
			"ARM",
			"ALD"
		},
		alt_intercalary_list = {
			"MLD",
			""
		},
		calendar_name = mon13.names_en_US_tranquility.calendar_name
	}

	local d = {year = 50, month = 4, day = 1}
	local mjd = mon13.mjdFromYmd(mon13.tranquility, d)
	local status, msg = pcall(
		mon13.format, mjd, mon13.tranquility, nlist, "%Y-%B%d (%f)"
	)
	assert(not status, "format, invalid namelist status")
	local expected = "Invalid name list"
	assert(msg:sub(-#expected) == expected, "format, invalid namelist message")
end

do
	local d = {year = 50, month = 4, day = 1}
	local mjd = mon13.mjdFromYmd(mon13.tranquility, d)
	local status, msg = pcall(
		mon13.format, mjd, mon13.tranquility, "%Y-%B%d (%f)"
	)
	assert(not status, "format, nil namelist error status")
	local expected = "Null name list"
	assert(msg:sub(-#expected) == expected, msg)--"format, nil namelist error message")
end

print("Lua API tests passed.")

-- local mjd = mon13.mjdFromUnix(os.time())
-- print(mon13.format(mjd, mon13.gregorian, "%Y-%m-%d"))

-- local fmt = "%-d %B %Y, %q (%f)"
-- print(mon13.format(mjd, mon13.gregorian, mon13.names_en_US_gregorian, fmt))
-- print(mon13.format(mjd, mon13.tranquility, mon13.names_en_US_tranquility, fmt))
-- print(mon13.format(
-- 	mjd,
-- 	mon13.french_revolutionary_romme_sub1,
-- 	mon13.names_fr_FR_french_revolutionary,
-- 	fmt
-- ))