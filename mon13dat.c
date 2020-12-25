#include <stdbool.h>
#include "mon13.h"

#define MON13_IC_GREGORIAN (mon13_ic_leap_day | mon13_ic_has_month | mon13_ic_has_weekday)

struct mon13_cal mon13_gregorian = {
	.month_names = {
		"January", "February", "March", "April", "May",
		"June", "July", "August", "September", "October",
		"November", "December", NULL, NULL
	},
	.weekday_names = {
		"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
		"Friday", "Saturday"
	},
	.intercalary_day_names = { NULL, NULL },
	.cal_name = "Gregorian",
	.era_start_name = NULL,
	.before_era_name = "Before Common Era",
	.after_era_name = "Common Era",
	.intercalary_days = {
		{ .year = MON13_IC_GREGORIAN, .month = 2, .day = 29 },
		{ .year = mon13_ic_invalid, .month = 0, .day = 0}
	},
	.before_intercalary_days = {
		{ .year = mon13_ic_invalid, .month = 2, .day = 28},
		{ .year = mon13_ic_invalid, .month = 0, .day = 0}
	},
	.era_start_gregorian = {
		.year = 1, .month = 1, .day = 1
	},
	.leap_year_offset = 0,
	.month_length = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
		30, 31, 0, 0
	},
	.yearless_cal_start = false,
	.fixed_start_weekday = false
};

struct mon13_cal mon13_tranquility = {
	.month_names = {
		"Archimedes", "Brahe", "Copernicus", "Darwin", "Einstein",
		"Faraday", "Galileo", "Hippocrates", "Imhotep", "Jung",
		"Kepler", "Lavoisier", "Mendel", NULL
	},
	.weekday_names = {
		"Friday", "Saturday", "Sunday", "Monday", "Tuesday",
		"Wednesday", "Thursday"
	},
	.intercalary_day_names = {
		"Aldrin Day", "Armstrong Day"
	},
	.cal_name = "Tranquility",
	.era_start_name = "Moon Landing Day",
	.before_era_name = "Before Tranquility",
	.after_era_name = "After Tranquility",
	.intercalary_days = {
		{ .year = mon13_ic_leap_day, .month = 0, .day = 0 },
		{ .year = mon13_ic_year_day, .month = 0, .day = 1 }
	},
	.before_intercalary_days = {
		{ .year = mon13_ic_invalid, .month = 8, .day = 27 },
		{ .year = mon13_ic_invalid, .month = 13, .day = 28 }
	},
	.era_start_gregorian = {
		.year = 1969, .month = 7, .day = 20
	},
	.leap_year_offset = 1969,
	.month_length = {
		28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
		28, 28, 28, 0
	},
	.yearless_cal_start = true,
	.fixed_start_weekday = true
};

struct mon13_cal mon13_international_fixed = {
	.month_names = {
		"January", "February", "March", "April", "May",
		"June", "Sol", "July", "August", "September",
		"October", "November", "December", NULL
	},
	.intercalary_day_names = {
		"Leap Day", "Year Day"
	},
	.weekday_names = {
		"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
		"Friday", "Saturday"
	},
	.cal_name = "International Fixed",
	.era_start_name = NULL,
	.before_era_name = "Before Common Era (IFC)",
	.after_era_name = "Common Era (IFC)",
	.intercalary_days = {
		{ .year = mon13_ic_leap_day | mon13_ic_has_month, .month = 6, .day = 29 },
		{ .year = mon13_ic_year_day | mon13_ic_has_month, .month = 12, .day = 29 }
	},
	.before_intercalary_days = {
		{ .year = mon13_ic_invalid, .month = 6, .day = 28 },
		{ .year = mon13_ic_invalid, .month = 12, .day = 28 }
	},
	.era_start_gregorian = {
		.year = 1, .month = 1, .day = 1
	},
	.leap_year_offset = 0,
	.month_length = {
		28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
		28, 28, 28, 0
	},
	.yearless_cal_start = false,
	.fixed_start_weekday = true
};

struct mon13_cal mon13_positivist = {
	.month_names = {
		"Moses", "Homer", "Aristotle", "Archimedes", "Caesar",
		"Saint Paul", "Charlemagne", "Dante", "Gutenberg","Shakespeare",
		"Descartes", "Frederick", "Bichat", NULL
	},
	.intercalary_day_names = {
		"Festival of Holy Women", "Festival of the All the Dead"
	},
	.weekday_names = {
		"Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
		"Saturday", "Sunday"
	},
	.cal_name = "Positivist",
	.era_start_name = NULL,
	.before_era_name = "Before Revolution",
	.after_era_name = "After Revolution",
	.intercalary_days = {
		{ .year = mon13_ic_leap_day, .month = 0, .day = 0 },
		{ .year = mon13_ic_year_day, .month = 0, .day = 1 }
	},
	.before_intercalary_days = {
		{ .year = mon13_ic_year_day, .month = 0, .day = 1 },
		{ .year = mon13_ic_invalid, .month = 13, .day = 28 }
	},
	.era_start_gregorian = {
		.year = 1789, .month = 1, .day = 1
	},
	.leap_year_offset = 1789,
	.month_length = {
		28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
		28, 28, 28, 0
	},
	.yearless_cal_start = false,
	.fixed_start_weekday = true
};

struct mon13_cal mon13_world = {
	.month_names = {
		"January", "February", "March", "April", "May",
		"June", "July", "August", "September", "October",
		"November", "December", NULL, NULL
	},
	.intercalary_day_names = {
		"Leapyear Day", "Worldsday"
	},
	.weekday_names = {
		"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
		"Friday", "Saturday"
	},
	.cal_name = "World",
	.era_start_name = NULL,
	.before_era_name = "Before Common Era (WC)",
	.after_era_name = "Common Era (WC)",
	.intercalary_days = {
		{ .year = mon13_ic_leap_day, .month = 0, .day = 0 },
		{ .year = mon13_ic_year_day, .month = 0, .day = 1 }
	},
	.before_intercalary_days = {
		{ .year = mon13_ic_invalid, .month = 6, .day = 30 },
		{ .year = mon13_ic_invalid, .month = 12, .day = 30 },
	},
	.era_start_gregorian = {
		.year = 1, .month = 1, .day = 1
	},
	.leap_year_offset = 0,
	.month_length = {
		31, 30, 30, 31, 30, 30, 31, 30, 30, 31,
		30, 30, 0, 0
	},
	.yearless_cal_start = false,
	.fixed_start_weekday = true
};