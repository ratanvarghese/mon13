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
	.after_era_name = "Common Era",
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
	},
	.yearless_cal_start = false
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

struct mon13_cal mon13_international_fixed = {
	.month_names = {
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"Sol",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December",
		NULL
	},
	.extra_day_names = {
		"Leap Day", "Year Day"
	},
	.era_start_name = NULL,
	.before_era_name = "Before Common Era (IFC)",
	.after_era_name = "Common Era (IFC)",
	.extra_days = {
		{ .year = MON13_VALID_EXTRA_DAY, .month = 6, .day = 29 },
		{ .year = MON13_VALID_EXTRA_DAY, .month = 12, .day = 29 }
	},
	.before_extra_days = {
		{ .year = MON13_VALID_EXTRA_DAY, .month = 6, .day = 28 },
		{ .year = MON13_VALID_EXTRA_DAY, .month = 12, .day = 28 }
	},
	.era_start_gregorian = {
		.year = 1, .month = 1, .day = 1
	},
	.leap_year_offset = 0,
	.month_length = {
		28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
		28, 28, 28, MON13_NO_MONTH
	},
	.yearless_cal_start = false
};

struct mon13_cal mon13_positivist = {
	.month_names = {
		"Moses",
		"Homer",
		"Aristotle",
		"Archimedes",
		"Caesar",
		"Saint Paul",
		"Charlemagne",
		"Dante",
		"Gutenberg",
		"Shakespeare",
		"Descartes",
		"Frederick",
		"Bichat",
		NULL
	},
	.extra_day_names = {
		"Festival of Holy Women", "Festival of the All the Dead"
	},
	.era_start_name = NULL,
	.before_era_name = "Before Revolution",
	.after_era_name = "After Revolution",
	.extra_days = {
		{ .year = MON13_VALID_EXTRA_DAY, .month = MON13_NO_MONTH, .day = 0 },
		{ .year = MON13_VALID_EXTRA_DAY, .month = MON13_NO_MONTH, .day = 1 }
	},
	.before_extra_days = {
		{ .year = MON13_VALID_EXTRA_DAY, .month = MON13_NO_MONTH, .day = 1 },
		{ .year = MON13_VALID_EXTRA_DAY, .month = 13, .day = 28 }
	},
	.era_start_gregorian = {
		.year = 1789, .month = 1, .day = 1
	},
	.leap_year_offset = 1789,
	.month_length = {
		28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
		28, 28, 28, MON13_NO_MONTH
	},
	.yearless_cal_start = false
};

struct mon13_cal mon13_world = {
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
	.extra_day_names = {
		"Leapyear Day", "Worldsday"
	},
	.era_start_name = NULL,
	.before_era_name = "Before Common Era (WC)",
	.after_era_name = "Common Era (WC)",
	.extra_days = {
		{ .year = MON13_VALID_EXTRA_DAY, .month = MON13_NO_MONTH, .day = 0 },
		{ .year = MON13_VALID_EXTRA_DAY, .month = MON13_NO_MONTH, .day = 1 }
	},
	.before_extra_days = {
		{ .year = MON13_VALID_EXTRA_DAY, .month = 6, .day = 30 },
		{ .year = MON13_VALID_EXTRA_DAY, .month = 12, .day = 30 },
	},
	.era_start_gregorian = {
		.year = 1, .month = 1, .day = 1
	},
	.leap_year_offset = 0,
	.month_length = {
		31, 30, 30, 31, 30, 30, 31, 30, 30, 31,
		30, 30, MON13_NO_MONTH, MON13_NO_MONTH
	},
	.yearless_cal_start = false
};