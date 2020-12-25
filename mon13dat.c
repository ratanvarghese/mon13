#include <stdbool.h>
#include "mon13.h"

struct mon13_cal mon13_tranquility = {
	.intercalary_days = {
		{
			.name = "Armstrong Day",
			.month = 0, .day = 1,
			.before_month = 13, .before_day = 28,
			.flags = MON13_IC_YEAR
		},
		{
			.name = "Aldrin Day",
			.month = 0, .day = 2,
			.before_month = 8, .before_day = 27,
			.flags = MON13_IC_LEAP
		},
		{
			.name = "Moon Landing Day",
			.month = 0, .day = 3,
			.before_month = 13, .before_day = 28,
			.flags = MON13_IC_ERA_START
		}
	},
	.month_names = {
		"Archimedes", "Brahe", "Copernicus", "Darwin", "Einstein",
		"Faraday", "Galileo", "Hippocrates", "Imhotep", "Jung",
		"Kepler", "Lavoisier", "Mendel"
	},
	.weekday_names = {
		"Friday", "Saturday", "Sunday", "Monday", "Tuesday",
		"Wednesday", "Thursday"
	},
	.era_names = {
		"Before Tranquility", "After Tranquility"
	},
	.cal_name = "Tranquility",
	.era_start_gregorian = {
		.year = 1969, .month = 7, .day = 20
	},
	.era_start_millisecond = (200 + 1000*(1 + 60*(18 + 60*(20)))),
	.intercalary_day_count = 3,
	.flags = (MON13_CAL_YEARLESS_ERA_START | MON13_CAL_GREGORIAN_LEAP_YEAR)
};

struct mon13_cal mon13_international_fixed = {
	.intercalary_days = {
		{
			.name = "Year Day",
			.month = 13, .day = 29,
			.before_month = 13, .before_day = 28,
			.flags = MON13_IC_YEAR
		},
		{
			.name = "Leap Day",
			.month = 6, .day = 29,
			.before_month = 6, .before_day = 28,
			.flags = MON13_IC_LEAP
		}
	},
	.month_names = {
		"January", "February", "March", "April", "May",
		"June", "Sol", "July", "August", "September",
		"October", "November", "December"
	},
	.weekday_names = {
		"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
		"Friday", "Saturday"
	},
	.era_names = {
		"Before Common Era", "Common Era"
	},
	.cal_name = "International Fixed",
	.era_start_gregorian = {
		.year = 1, .month = 1, .day = 1
	},
	.era_start_millisecond = 0,
	.intercalary_day_count = 2,
	.flags = MON13_CAL_GREGORIAN_LEAP_YEAR
};

struct mon13_cal mon13_positivist = {
	.intercalary_days = {
		{
			.name = "Festival of the All the Dead",
			.month = 0, .day = 1,
			.before_month = 13, .before_day = 28,
			.flags = MON13_IC_YEAR
		},
		{
			.name = "Festival of Holy Women",
			.month = 0, .day = 2,
			.before_month = 0, .before_day = 1,
			.flags = MON13_IC_LEAP
		}
	},
	.month_names = {
		"Moses", "Homer", "Aristotle", "Archimedes", "Caesar",
		"Saint Paul", "Charlemagne", "Dante", "Gutenberg","Shakespeare",
		"Descartes", "Frederick", "Bichat"
	},
	.weekday_names = {
		"Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
		"Saturday", "Sunday"
	},
	.era_names = {
		"Before Revolution", "After Revolution"
	},
	.cal_name = "Positivist",
	.era_start_gregorian = {
		.year = 1789, .month = 1, .day = 1
	},
	.era_start_millisecond = 0,
	.intercalary_day_count = 2,
	.flags = MON13_CAL_GREGORIAN_LEAP_YEAR
};