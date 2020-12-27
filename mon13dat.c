#include <stdbool.h>
#include "mon13.h"

#define WKDY_SUN "Sunday"
#define WKDY_MON "Monday"
#define WKDY_TUE "Tuesday"
#define WKDY_WED "Wednesday"
#define WKDY_THU "Thursday"
#define WKDY_FRI "Friday"
#define WKDY_SAT "Saturday"

const char* mon13_wkdy_names[7] = {
	WKDY_SUN, WKDY_MON, WKDY_TUE, WKDY_WED, WKDY_THU, WKDY_FRI, WKDY_SAT
};

struct mon13_cal mon13_tranquility = {
	.intercalary_days = {
		{
			.name = "Armstrong Day",
			.flags = MON13_IC_YEAR,
			.month = 0, .day = 1,
			.before_month = 13, .before_day = 28,
		},
		{
			.name = "Aldrin Day",
			.flags = MON13_IC_LEAP,
			.month = 0, .day = 2,
			.before_month = 8, .before_day = 27,
		},
		{
			.name = "Moon Landing Day",
			.flags = MON13_IC_ERA_START,
			.month = 0, .day = 3,
			.before_month = 13, .before_day = 28,
		}
	},
	.month_names = {
		"Archimedes", "Brahe", "Copernicus", "Darwin", "Einstein",
		"Faraday", "Galileo", "Hippocrates", "Imhotep", "Jung",
		"Kepler", "Lavoisier", "Mendel"
	},
	.weekday_names = {
		WKDY_FRI, WKDY_SAT, WKDY_SUN, WKDY_MON, WKDY_TUE, WKDY_WED, WKDY_THU
	},
	.era_names = { "Before Tranquility", "After Tranquility" },
	.cal_name = "Tranquility",
	.era_start_gregorian = { .year = 1969, .month = 7, .day = 20 },
	.era_start_millisecond = (200 + 1000*(1 + 60*(18 + 60*(20)))),
	.flags = (
		MON13_CAL_YEARLESS_ERA_START |
		MON13_CAL_GREGORIAN_LEAP_YEAR |
		MON13_CAL_GREGORIAN_WKDY_NAMES
	),
	.intercalary_day_count = 3,
	.week_info = 5
};

struct mon13_cal mon13_international_fixed = {
	.intercalary_days = {
		{
			.name = "Year Day",
			.flags = MON13_IC_YEAR,
			.month = 13, .day = 29,
			.before_month = 13, .before_day = 28,
		},
		{
			.name = "Leap Day",
			.flags = MON13_IC_LEAP,
			.month = 6, .day = 29,
			.before_month = 6, .before_day = 28,
		}
	},
	.month_names = {
		"January", "February", "March", "April", "May",
		"June", "Sol", "July", "August", "September",
		"October", "November", "December"
	},
	.weekday_names = {
		WKDY_SUN, WKDY_MON, WKDY_TUE, WKDY_WED, WKDY_THU, WKDY_FRI, WKDY_SAT
	},
	.era_names = { "Before Common Era", "Common Era" },
	.cal_name = "International Fixed",
	.era_start_gregorian = { .year = 1, .month = 1, .day = 1 },
	.era_start_millisecond = 0,
	.flags = MON13_CAL_GREGORIAN_LEAP_YEAR | MON13_CAL_GREGORIAN_WKDY_NAMES,
	.intercalary_day_count = 2,
	.week_info = 0
};

struct mon13_cal mon13_positivist = {
	.intercalary_days = {
		{
			.name = "Festival of the All the Dead",
			.flags = MON13_IC_YEAR,
			.month = 0, .day = 1,
			.before_month = 13, .before_day = 28,
		},
		{
			.name = "Festival of Holy Women",
			.flags = MON13_IC_LEAP,
			.month = 0, .day = 2,
			.before_month = 0, .before_day = 1,
		}
	},
	.month_names = {
		"Moses", "Homer", "Aristotle", "Archimedes", "Caesar",
		"Saint Paul", "Charlemagne", "Dante", "Gutenberg","Shakespeare",
		"Descartes", "Frederick", "Bichat"
	},
	.weekday_names = {
		WKDY_MON, WKDY_TUE, WKDY_WED, WKDY_THU, WKDY_FRI, WKDY_SAT, WKDY_SUN
	},
	.era_names = { "Before Positivist Era", "After Positivist Era" }, //unknown
	.cal_name = "Positivist",
	.era_start_gregorian = { .year = 1789, .month = 1, .day = 1 },
	.era_start_millisecond = 0,
	.flags = MON13_CAL_GREGORIAN_LEAP_YEAR | MON13_CAL_GREGORIAN_WKDY_NAMES,
	.intercalary_day_count = 2,
	.week_info = 1
};