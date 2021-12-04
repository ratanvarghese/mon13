import mon13

if True:
	d = mon13.Ymd(year = 13, month = 13, day = 13)
	assert (not mon13.validYmd(mon13.gregorian, d)), "detect invalid ymd"

if True:
	d = mon13.Ymd(year = 10, month = 10, day = 10)
	assert (mon13.validYmd(mon13.gregorian, d)), "detect valid ymd"

if True:
	d = mon13.Ymd(year = 13, month = 13, day = 13)
	try:
		mjd = mon13.mjdFromYmd(mon13.gregorian, d)
		assert (False), "mjdFromYmd, throw error"
	except mon13.Mon13Error as e:
		assert e.status is mon13.ErrorStatus.INVALID_DATE, "mjdFromYmd, error"

if True:
	d0 = mon13.Ymd(year = 10, month = 10, day = 10)
	d1 = mon13.Ymd(year = d0.year, month = d0.month, day = d0.day + 1)
	mjd0 = mon13.mjdFromYmd(mon13.tranquility, d0)
	mjd1 = mon13.mjdFromYmd(mon13.tranquility, d1)
	assert (mjd1 == (mjd0 + 1)), "add 1 to day, add 1 to MJD"

if True:
	u0 = 234235423
	u1 = u0 + (60 * 60 * 24)
	mjd0 = mon13.mjdFromUnix(u0)
	mjd1 = mon13.mjdFromUnix(u1)
	assert (mjd1 == (mjd0 + 1)), "add 60 * 60 * 24 to Unix, add 1 to MJD"

if True:
	rd0 = 1234123
	rd1 = rd0 + 1
	mjd0 = mon13.mjdFromRd(rd0)
	mjd1 = mon13.mjdFromRd(rd1)
	assert (mjd1 == (mjd0 + 1)), "add 1 to RD, add 1 to MJD"

if True:
	mjd0 = 462342
	ymd0 = mon13.mjdToYmd(mjd0, mon13.julian)
	mjd1 = mon13.mjdFromYmd(mon13.julian, ymd0)
	assert (mjd1 == mjd0), "ymd round trip"

if True:
	mjd0 = 462342
	u0 = mon13.mjdToUnix(mjd0)
	mjd1 = mon13.mjdFromUnix(u0)
	assert (mjd1 == mjd0), "Unix round trip"

if True:
	mjd0 = 462342
	rd0 = mon13.mjdToRd(mjd0)
	mjd1 = mon13.mjdFromRd(rd0)
	assert (mjd1 == mjd0), "Rd round trip"

if True:
	offset = 1389779633
	mjd = 757702997 + offset
	try:
		rd = mon13.mjdToRd(mjd)
		assert (False), "mjdToRd, throw error"
	except mon13.Mon13Error as e:
		assert e.status is mon13.ErrorStatus.OVERFLOW, "mjdToRd, error"

if True:
	d = mon13.Ymd(year = 2000, month = 13, day = 13)
	mjd = mon13.mjdFromYmd(mon13.cotsworth, d)
	is_leap = mon13.mjdToIsLeapYear(mjd, mon13.cotsworth)
	assert (is_leap), "detect leap year"

if True:
	d = mon13.Ymd(year = 2001, month = 13, day = 13)
	mjd = mon13.mjdFromYmd(mon13.cotsworth, d)
	is_leap = mon13.mjdToIsLeapYear(mjd, mon13.cotsworth)
	assert (not is_leap), "detect non-leap year"

if True:
	mjd0 = 343542
	mjd1 = mjd0 + 1
	dow0 = mon13.mjdToDayOfWeek(mjd0, mon13.holocene)
	dow1 = mon13.mjdToDayOfWeek(mjd1, mon13.holocene)
	assert (dow1 == (dow0 + 1)), "add 1 to MJD, add 1 to day of week"

if True:
	mjd0 = 343543
	mjd1 = mjd0 + 1
	dow0 = mon13.mjdToDayOfWeek(mjd0, mon13.holocene)
	dow1 = mon13.mjdToDayOfWeek(mjd1, mon13.holocene)
	assert (dow0 == 7 and dow1 == 1), "add 1 to MJD, start next week"
	assert (dow0 == mon13.Weekday7.SUNDAY.value), "End week on Sunday"
	assert (dow1 == mon13.Weekday7.MONDAY.value), "Start week on Monday"

if True:
	mjd0 = 343542
	mjd1 = mjd0 + 1
	doy0 = mon13.mjdToDayOfYear(mjd0, mon13.ancient_egyptian)
	doy1 = mon13.mjdToDayOfYear(mjd1, mon13.ancient_egyptian)
	assert (doy1 == (doy0 + 1)), "add 1 to MJD, add 1 to day of year"

if True:
	d0 = mon13.Ymd(year = 60, month = 1, day = 4)
	offset = 2
	mjd0 = mon13.mjdFromYmd(mon13.positivist, d0)
	mjd1 = mon13.addMonths(mjd0, mon13.positivist, offset)
	d1 = mon13.mjdToYmd(mjd1, mon13.positivist)
	assert (d1.year == d0.year), "add 2 to month, same year"
	assert (d1.month == (d0.month + 2)), "add 2 to month"
	assert (d1.day == d0.day), "add 2 to month, same day"

if True:
	d0 = mon13.Ymd(year = 60, month = 1, day = 4)
	offset = 2
	mjd0 = mon13.mjdFromYmd(mon13.symmetry454, d0)
	mjd1 = mon13.addYears(mjd0, mon13.symmetry454, offset)
	d1 = mon13.mjdToYmd(mjd1, mon13.symmetry454)
	assert (d1.year == (d0.year + 2)), "add 2 to year"
	assert (d1.month == d0.month), "add 2 to year, same month"
	assert (d1.day == d0.day), "add 2 to year, same day"

if True:
	d0 = mon13.Ymd(year = 10, month = 10, day = 10)
	d1 = mon13.Ymd(year = d0.year, month = d0.month + 3, day = d0.day)
	mjd0 = mon13.mjdFromYmd(mon13.tranquility, d0)
	mjd1 = mon13.mjdFromYmd(mon13.tranquility, d1)
	diff = mon13.diffMonths(mjd1, mjd0, mon13.tranquility)
	assert (diff == 3), "diff months"

if True:
	d0 = mon13.Ymd(year = 10, month = 10, day = 10)
	d1 = mon13.Ymd(year = d0.year + 4, month = d0.month, day = d0.day)
	mjd0 = mon13.mjdFromYmd(mon13.tranquility, d0)
	mjd1 = mon13.mjdFromYmd(mon13.tranquility, d1)
	diff = mon13.diffYears(mjd1, mjd0, mon13.tranquility)
	assert (diff == 4), "diff years"

if True:
	d = mon13.Ymd(year = 1999, month = 4, day = 7)
	mjd = mon13.mjdFromYmd(mon13.gregorian, d)
	s = mon13.format(mjd, mon13.gregorian, fmt=f"%Y-%m-%d")
	assert (s == "1999-04-07"), "format, no name list"

if True:
	d = mon13.Ymd(year = 2021, month = 11, day = 16)
	mjd = mon13.mjdFromYmd(mon13.gregorian, d)
	s = mon13.format(
		mjd,
		mon13.gregorian,
		mon13.names_en_US_gregorian,
		f"%-d %B %Y, %q (%f)"
	)
	assert (s == "16 November 2021, Common Era (Gregorian)"), "format"

if True:
	nlist = mon13.NameList(
		month_list = [
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
		],
		weekday_list = mon13.names_en_US_tranquility.weekday_list,
		era_list = [
			"BT",
			"AT"
		],
		intercalary_list = [
			"ARM",
			"ALD"
		],
		alt_intercalary_list = [
			"MLD",
			""
		],
		calendar_name = mon13.names_en_US_tranquility.calendar_name
	)

	d = mon13.Ymd(year = 50, month = 4, day = 1)
	mjd = mon13.mjdFromYmd(mon13.tranquility, d)
	s = mon13.format(mjd, mon13.tranquility, nlist, f"%Y-%B%d (%f)")
	assert (s == "50-D01 (Tranquility)"), "format, custom namelist"

if True:
	nlist = mon13.NameList(
		month_list = [
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
		],
		weekday_list = mon13.names_en_US_tranquility.weekday_list,
		era_list = [
			"BT",
			"AT"
		],
		intercalary_list = [
			"ARM",
			"ALD"
		],
		alt_intercalary_list = [
			"MLD",
			""
		],
		calendar_name = mon13.names_en_US_tranquility.calendar_name
	)

	d = mon13.Ymd(year = 50, month = 4, day = 1)
	mjd = mon13.mjdFromYmd(mon13.tranquility, d)
	try:
		s = mon13.format(mjd, mon13.tranquility, nlist, f"%Y-%B%d (%f)")
		assert (False), "format, invalid namelist, throw error"
	except mon13.Mon13Error as e:
		assert e.status is mon13.ErrorStatus.INVALID_NAME_LIST, "format, invalid namelist, error"
	assert (s == "50-D01 (Tranquility)"), "format, custom namelist"

if True:
	d = mon13.Ymd(year = 50, month = 4, day = 1)
	mjd = mon13.mjdFromYmd(mon13.tranquility, d)
	try:
		s = mon13.format(mjd, mon13.tranquility, None, f"%Y-%B%d (%f)")
		assert (False), "format, throw error"
	except mon13.Mon13Error as e:
		assert e.status is mon13.ErrorStatus.INVALID_SEQUENCE, "format, error"

print("Python API tests passed.")

# mjd0, status0 = mon13.mjdFromYmd(mon13.symmetry454, mon13.Ymd(year=2021, month=11, day=18))
# mjd1, status1 = mon13.mjdFromYmd(mon13.symmetry454, mon13.Ymd(year=2021, month=11, day=19))
# print(mjd0, mjd1)


# s2, status2 = mon13.format(mjd0, mon13.symmetry454, mon13.names_en_US_symmetry454, f"%-d %B %Y, %q (%f)")
# print(s2, status2)

# list3 = mon13.NameList(
# 	month_list = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"],
# 	weekday_list = mon13.names_en_US_symmetry454.weekday_list,
# 	era_list = ["BCE", "CE"],
# 	intercalary_list = mon13.names_en_US_symmetry454.intercalary_list,
# 	alt_intercalary_list = mon13.names_en_US_symmetry454.alt_intercalary_list,
# 	calendar_name = "Sym454"
# )

# s3, status3 = mon13.format(mjd0, mon13.symmetry454, list3, f"%-d %B %Y, %q (%f)")
# print(s3, status3)
