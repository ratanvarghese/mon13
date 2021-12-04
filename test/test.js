const fs = require('fs');
const mon13 = require('../binding/js_wasm/mon13');

const source = fs.readFileSync(process.argv[2] + "/mon13.wasm");
const typedArray = new Uint8Array(source);

function my_assert(cond, msg) {
	if(!cond) {
		throw new Error(msg);
	}
}

WebAssembly.instantiate(typedArray,{}).then(lib => {
	const m = new mon13(lib);

	{
		const d = {year:13, month:13, day: 13}
		my_assert(!m.validYmd(m.gregorian, d), "detect invalid ymd")
	}

	{
		const d = {year:10, month:10, day: 10}
		my_assert(m.validYmd(m.gregorian, d), "detect valid ymd")
	}

	{
		const d = {year:13, month:13, day: 13}
		var msg = "";
		try {
			m.mjdFromYmd(m.gregorian, d);
		}
		catch(e) {
			msg = e.message;
		}
		my_assert(msg == "Invalid date", "mjdFromYmd error message")
	}

	{
		const d0 = {year:10, month:10, day: 10}
		const d1 = {year:d0.year, month:d0.month, day: d0.day + 1}
		const mjd0 = m.mjdFromYmd(m.tranquility, d0)
		const mjd1 = m.mjdFromYmd(m.tranquility, d1)
		my_assert(mjd1 == (mjd0 + 1), "add 1 to day, add 1 to MJD")
	}

	{
		const u0 = 234235423
		const u1 = u0 + (60 * 60 * 24)
		const mjd0 = m.mjdFromUnix(u0)
		const mjd1 = m.mjdFromUnix(u1)
		my_assert(mjd1 == (mjd0 + 1), "add 60 * 60 * 24 to Unix, add 1 to MJD")
	}

	{
		const rd0 = 1234123
		const rd1 = rd0 + 1
		const mjd0 = m.mjdFromRd(rd0)
		const mjd1 = m.mjdFromRd(rd1)
		my_assert(mjd1 == (mjd0 + 1), "add 1 to RD, add 1 to MJD")
	}

	{
		const mjd0 = 462342
		const ymd0 = m.mjdToYmd(mjd0, m.julian)
		const mjd1 = m.mjdFromYmd(m.julian, ymd0)
		my_assert(mjd1 == mjd0, "ymd round trip")
	}

	{
		const mjd0 = 462342
		const mjd1 = m.mjdFromUnix(m.mjdToUnix(mjd0))
		my_assert(mjd1 == mjd0, "Unix round trip")
	}

	{
		const mjd0 = 462342
		const mjd1 = m.mjdFromRd(m.mjdToRd(mjd0))
		my_assert(mjd1 == mjd0, "Rd round trip")
	}

	{
		const offset = 1389779633
		const mjd = 757702997 + offset
		var msg = "";
		try {
			m.mjdToRd(mjd);
		} catch(e) {
			msg = e.message;
		}
		const expected = "Overflow occurred (internal)"
		my_assert(msg == expected, "mjdToRd error message")
	}

	{
		const d = {year:2000, month:13, day: 13}
		const mjd = m.mjdFromYmd(m.cotsworth, d)
		const is_leap = m.mjdToIsLeapYear(mjd, m.cotsworth)
		my_assert(is_leap, "detect leap year")
	}

	{
		const d = {year:2001, month:13, day: 13}
		const mjd = m.mjdFromYmd(m.cotsworth, d)
		const is_leap = m.mjdToIsLeapYear(mjd, m.cotsworth)
		my_assert(!is_leap, "detect non-leap year")
	}

	{
		const mjd0 = 343542
		const mjd1 = mjd0 + 1
		const dow0 = m.mjdToDayOfWeek(mjd0, m.holocene)
		const dow1 = m.mjdToDayOfWeek(mjd1, m.holocene)
		my_assert(dow1 == (dow0 + 1), "add 1 to MJD, add 1 to day of week")
	}

	{
		const mjd0 = 343543
		const mjd1 = mjd0 + 1
		const dow0 = m.mjdToDayOfWeek(mjd0, m.holocene)
		const dow1 = m.mjdToDayOfWeek(mjd1, m.holocene)
		my_assert(dow0 == 7 && dow1 == 1, "add 1 to MJD, start next week")
		my_assert(dow0 == m.Weekday7.SUNDAY, "End week on Sunday")
		my_assert(dow1 == m.Weekday7.MONDAY, "Start week on Monday")
	}

	{
		const mjd0 = 343542
		const mjd1 = mjd0 + 1
		const doy0 = m.mjdToDayOfYear(mjd0, m.ancient_egyptian)
		const doy1 = m.mjdToDayOfYear(mjd1, m.ancient_egyptian)
		my_assert(doy1 == (doy0 + 1), "add 1 to MJD, add 1 to day of year")
	}

	{
		const d0 = {year:60, month:1, day: 4}
		const offset = 2
		const mjd0 = m.mjdFromYmd(m.positivist, d0)
		const mjd1 = m.addMonths(mjd0, m.positivist, offset)
		const d1 = m.mjdToYmd(mjd1, m.positivist)
		my_assert(d1.year == d0.year, "add 2 to month, same year")
		my_assert(d1.month == (d0.month + 2), "add 2 to month")
		my_assert(d1.day == d0.day, "add 2 to month, same day")
	}

	{
		const d0 = {year:60, month:1, day: 4}
		const offset = 2
		const mjd0 = m.mjdFromYmd(m.symmetry454, d0)
		const mjd1 = m.addYears(mjd0, m.symmetry454, offset)
		const d1 = m.mjdToYmd(mjd1, m.symmetry454)
		my_assert(d1.year == (d0.year + 2), "add 2 to year")
		my_assert(d1.month == d0.month, "add 2 to year, same month")
		my_assert(d1.day == d0.day, "add 2 to year, same day")
	}

	{
		const d0 = {year:10, month:10, day: 10}
		const d1 = {year:d0.year, month:d0.month + 3, day: d0.day}
		const mjd0 = m.mjdFromYmd(m.tranquility, d0)
		const mjd1 = m.mjdFromYmd(m.tranquility, d1)
		const diff = m.diffMonths(mjd1, mjd0, m.tranquility)
		my_assert(diff == 3, "diff months")
	}

	{
		const d0 = {year:10, month:10, day: 10}
		const d1 = {year:d0.year + 4, month:d0.month, day: d0.day}
		const mjd0 = m.mjdFromYmd(m.tranquility, d0)
		const mjd1 = m.mjdFromYmd(m.tranquility, d1)
		const diff = m.diffYears(mjd1, mjd0, m.tranquility)
		my_assert(diff == 4, "diff years")
	}

	{
		const d = {year:1999, month:4, day: 7}
		const mjd = m.mjdFromYmd(m.gregorian, d)
		const s = m.format(mjd, m.gregorian, "%Y-%m-%d")
		my_assert(s == "1999-04-07", "format, no name list")
	}

	{
		const d = {year:2021, month:11, day: 16}
		const mjd = m.mjdFromYmd(m.gregorian, d)
		const s = m.format(
			mjd,
			m.gregorian,
			m.names_en_US_gregorian,
			"%-d %B %Y, %q (%f)"
		)
		my_assert(s == "16 November 2021, Common Era (Gregorian)", "format")
	}

	{
		const nlist = {
			month_list: [
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
			weekday_list: [
				"Mon",
				"Tue",
				"Wed",
				"Thu",
				"Fri",
				"Sat",
				"Sun",
			],
			era_list: [
				"BT",
				"AT"
			],
			intercalary_list: [
				"ARM",
				"ALD"
			],
			alt_intercalary_list: [
				"MLD",
				""
			],
			calendar_name: "TRQ"
		}
	
		const d = {year: 50, month: 4, day: 1}
		const mjd = m.mjdFromYmd(m.tranquility, d)
		const s = m.format(mjd, m.tranquility, nlist, "%Y-%B%d (%f)")
		my_assert(s == "50-D01 (TRQ)", "format, custom namelist")
	}

	{
		const nlist = {
			month_list: [
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
			weekday_list: [
				"Mon",
				"Tue",
				"Wed",
				"Thu",
				"Fri",
				"Sat",
				"Sun",
			],
			era_list: [
				"BT",
				"AT"
			],
			intercalary_list: [
				"ARM",
				"ALD"
			],
			alt_intercalary_list: [
				"MLD",
				""
			],
			calendar_name: "TRQ"
		}
	
		const d = {year: 50, month: 4, day: 1}
		const mjd = m.mjdFromYmd(m.tranquility, d)
		var msg = ""
		try {
			m.format(mjd, m.tranquility, nlist, "%Y-%B%d (%f)")
		} catch(e) {
			msg = e.message
		}
		my_assert(msg === "Invalid name list", "format, invalid namelist")
	}

	{
		const d = {year: 50, month: 4, day: 1}
		const mjd = m.mjdFromYmd(m.tranquility, d)
		var msg = ""
		try {
			m.format(mjd, m.tranquility, "%Y-%B%d (%f)")
		} catch (e) {
			msg = e.message;
		}
		const expected = "Invalid format sequence"
		my_assert(msg === expected, "format, missing namelist error message")
	}

	console.log("JavaScript API tests passed.")
})