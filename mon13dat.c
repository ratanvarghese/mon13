#include <stdbool.h>
#include "mon13.h"

struct mon13_cal mon13_gregorian = {
	.month_names = {
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December",
		NULL,
		NULL
	},
	.extra_day_names = { NULL, NULL },
	.era_start_name = NULL,
	.before_era_name = "Before Common Era",
	.after_era_name = "After Common Era",
	.extra_days = {
		{ .year = MON13_VALID_EXTRA_DAY, .month = 2, .day = 29 },
		{ .year = MON13_INVALID_EXTRA_DAY, .month = 0, .day = 0}
	},
	.before_extra_days = {
		{ .year = MON13_VALID_EXTRA_DAY, .month = 2, .day = 28},
		{ .year = MON13_INVALID_EXTRA_DAY, .month = 0, .day = 0}
	},
	.era_start_gregorian = {
		.year = 1, .month = 1, .day = 1
	},
	.leap_year_offset = 0,
	.month_length = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
		30, 31, MON13_NO_MONTH, MON13_NO_MONTH
	}
};

struct mon13_cal mon13_tranquility = {
	.month_names = {
		"Archimedes",
		"Brahe",
		"Copernicus",
		"Darwin",
		"Einstein",
		"Faraday",
		"Galileo",
		"Hippocrates",
		"Imhotep",
		"Jung",
		"Kepler",
		"Lavoisier",
		"Mendel",
		NULL
	},
	.extra_day_names = {
		"Aldrin Day", "Armstrong Day"
	},
	.era_start_name = "Moon Landing Day",
	.before_era_name = "Before Tranquility",
	.after_era_name = "After Tranquility",
	.extra_days = {
		{ .year = MON13_VALID_EXTRA_DAY, .month = MON13_NO_MONTH, .day = 0 },
		{ .year = MON13_VALID_EXTRA_DAY, .month = MON13_NO_MONTH, .day = 1 }
	},
	.before_extra_days = {
		{ .year = MON13_VALID_EXTRA_DAY, .month = 8, .day = 27 },
		{ .year = MON13_VALID_EXTRA_DAY, .month = 13, .day = 28 }
	},
	.era_start_gregorian = {
		.year = 1969, .month = 7, .day = 20
	},
	.leap_year_offset = 1969,
	.month_length = {
		28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
		28, 28, 28, MON13_NO_MONTH
	},
	.yearless_cal_start = true,
};