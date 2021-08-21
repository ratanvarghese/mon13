#include <stdint.h>
#include "mon13.h"
#include "cal.h"

#define WKDY_SUN "Sunday"
#define WKDY_MON "Monday"
#define WKDY_TUE "Tuesday"
#define WKDY_WED "Wednesday"
#define WKDY_THU "Thursday"
#define WKDY_FRI "Friday"
#define WKDY_SAT "Saturday"

//NAME LISTS

//Gregorian Calendar
const char* gregorian_month_list_en_US[] = {
	"January", "February", "March", "April",
	"May", "June", "July", "August",
	"September", "October", "November", "December", NULL
};

const char* gregorian_weekday_list_en_US[] = {
	WKDY_SUN, WKDY_MON, WKDY_TUE, WKDY_WED, WKDY_THU, WKDY_FRI, WKDY_SAT, NULL
};

const char* gregorian_era_list_en_US[] = {"Before Common Era", "Common Era", NULL};

const struct mon13_name_list mon13_gregorian_names_en_US = {
	.month_list = gregorian_month_list_en_US,
	.weekday_list = gregorian_weekday_list_en_US,
	.era_list = gregorian_era_list_en_US,
	.intercalary_list = NULL,
	.alt_intercalary_list = NULL,
	.calendar_name = "Gregorian"
};

const char* gregorian_month_list_fr_FR[] = {
	"Janvier", "Février", "Mars", "Avril",
	"Mai", "Juin", "Juillet", "Août",
	"Septembre", "Octobre", "Novembre", "Décembre", NULL
};

const char* gregorian_weekday_list_fr_FR[] = {
	"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche", NULL
};

const char* gregorian_era_list_fr_FR[] = {"Ère commune", "Avant l'ère commune", NULL};

const struct mon13_name_list mon13_gregorian_names_fr_FR = {
	.month_list = gregorian_month_list_fr_FR,
	.weekday_list = gregorian_weekday_list_fr_FR,
	.era_list = gregorian_era_list_fr_FR,
	.intercalary_list = NULL,
	.alt_intercalary_list = NULL,
	.calendar_name = "Grégorien"
};

//Tranquility Calendar
const char* tranquility_month_list_en_US[] = {
	"Archimedes", "Brahe", "Copernicus", "Darwin", "Einstein",
	"Faraday", "Galileo", "Hippocrates", "Imhotep", "Jung",
	"Kepler", "Lavoisier", "Mendel", NULL	
};

const char* tranquility_weekday_list_en_US[] = {
	WKDY_FRI, WKDY_SAT, WKDY_SUN, WKDY_MON, WKDY_TUE, WKDY_WED, WKDY_THU, NULL
};

const char* tranquility_era_list_en_US[] = {
	"Before Tranquility", "After Tranquility", NULL
};

const char* tranquility_intercalary_list_en_US[] = {
	"Armstrong Day", "Aldrin Day", NULL
};

const char* tranquility_alt_intercalary_list_en_US[] = {
	"Moon Landing Day", "", NULL
};

const struct mon13_name_list mon13_tranquility_names_en_US = {
	.month_list = tranquility_month_list_en_US,
	.weekday_list = tranquility_weekday_list_en_US,
	.era_list = tranquility_era_list_en_US,
	.intercalary_list = tranquility_intercalary_list_en_US,
	.alt_intercalary_list = tranquility_alt_intercalary_list_en_US,
	.calendar_name = "Tranquility"
};


//CALENDARS

//Gregorian Calendar
const struct intercalary gregorian_ic[] = {
	{
		.flags = IC_SENTINEL
	}
};

const struct lkup gregorian_common_lookup[] = {
	{offset: 0, month: 1, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 31, month: 2, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 59, month: 3, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 90, month: 4, day_start: 1, day_end: 30, flags: LKUP_NONE},
	{offset: 120, month: 5, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 151, month: 6, day_start: 1, day_end: 30, flags: LKUP_NONE},
	{offset: 181, month: 7, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 212, month: 8, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 243, month: 9, day_start: 1, day_end: 30, flags: LKUP_NONE},
	{offset: 273, month: 10, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 304, month: 11, day_start: 1, day_end: 30, flags: LKUP_NONE},
	{offset: 334, month: 12, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 0, month: 0, day_start: 0, day_end: 0, flags: LKUP_SENTINEL}
};

const struct lkup gregorian_leap_lookup[] = {
	{offset: 0, month: 1, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 31, month: 2, day_start: 1, day_end: 29, flags: LKUP_NONE},
	{offset: 60, month: 3, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 91, month: 4, day_start: 1, day_end: 30, flags: LKUP_NONE},
	{offset: 121, month: 5, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 152, month: 6, day_start: 1, day_end: 30, flags: LKUP_NONE},
	{offset: 182, month: 7, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 213, month: 8, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 244, month: 9, day_start: 1, day_end: 30, flags: LKUP_NONE},
	{offset: 274, month: 10, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 305, month: 11, day_start: 1, day_end: 30, flags: LKUP_NONE},
	{offset: 335, month: 12, day_start: 1, day_end: 31, flags: LKUP_NONE},
	{offset: 0, month: 0, day_start: 0, day_end: 0, flags: LKUP_SENTINEL}
};

const struct mon13_cal mon13_gregorian = {
	.intercalary_list = gregorian_ic,
	.common_lookup_list = gregorian_common_lookup,
	.leap_lookup_list = gregorian_leap_lookup,
	.leap_cycle = {
		.year_count = 4,
		.leap_year_count = 1,
		.offset_years = 0,
		.offset_days = 0,
		.flags = LEAP_GREGORIAN_SKIP,
		.common_days = 365,
		.leap_days = 1
	},
	.epoch_mjd = -678575, //1 Jan, 1 CE
	.flags = CAL_NONE,
	.week_length = 7,
	.start_weekday = MON13_NO_WEEKDAY
};

const struct mon13_cal mon13_gregorian_year_zero = {
	.intercalary_list = gregorian_ic,
	.common_lookup_list = gregorian_common_lookup,
	.leap_lookup_list = gregorian_leap_lookup,
	.leap_cycle = {
		.year_count = 4,
		.leap_year_count = 1,
		.offset_years = 0,
		.offset_days = 0,
		.flags = LEAP_GREGORIAN_SKIP,
		.common_days = 365,
		.leap_days = 1
	},
	.epoch_mjd = -678575, //1 Jan, 1 CE
	.flags = CAL_YEAR_ZERO,
	.week_length = 7,
	.start_weekday = MON13_NO_WEEKDAY
};

//Tranquility Calendar
const struct intercalary tranquility_ic[] = {
	{
		.flags = IC_YEAR | IC_ERA_START_ALT_NAME,
		.month = 0, .day = 1, .day_of_year = 365
	},
	{
		.flags = IC_LEAP,
		.month = 0, .day = 2, .day_of_year = 224
	},
	{
		.flags = IC_SENTINEL
	}
};

const struct lkup tranquility_common_lookup[] = {
	{offset: 0, month: 1, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 28, month: 2, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 56, month: 3, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 84, month: 4, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 112, month: 5, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 140, month: 6, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 168, month: 7, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 196, month: 8, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 224, month: 9, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 252, month: 10, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 280, month: 11, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 308, month: 12, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 336, month: 13, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 364, month: 0, day_start: 1, day_end: 1, flags: LKUP_NONE},
	{offset: 0, month: 0, day_start: 0, day_end: 0, flags: LKUP_SENTINEL}
};

const struct lkup tranquility_leap_lookup[] = {
	{offset: 0, month: 1, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 28, month: 2, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 56, month: 3, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 84, month: 4, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 112, month: 5, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 140, month: 6, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 168, month: 7, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 196, month: 8, day_start: 1, day_end: 27, flags: LKUP_NONE},
	{offset: 223, month: 0, day_start: 2, day_end: 2, flags: LKUP_NONE},
	{offset: 224, month: 8, day_start: 28, day_end: 28, flags: LKUP_NONE},
	{offset: 225, month: 9, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 253, month: 10, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 281, month: 11, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 309, month: 12, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 337, month: 13, day_start: 1, day_end: 28, flags: LKUP_NONE},
	{offset: 365, month: 0, day_start: 1, day_end: 1, flags: LKUP_NONE},
	{offset: 0, month: 0, day_start: 0, day_end: 0, flags: LKUP_SENTINEL}
};

const struct mon13_cal mon13_tranquility = {
	.intercalary_list = tranquility_ic,
	.common_lookup_list = tranquility_common_lookup,
	.leap_lookup_list = tranquility_leap_lookup,
	.leap_cycle = {
		.year_count = 4,
		.leap_year_count = 1,
		.offset_years = 31,
		.offset_days = 11323,
		.flags = LEAP_GREGORIAN_SKIP,
		.common_days = 365,
		.leap_days = 1
	},
	.epoch_mjd = 40423, //1 day after Moon Landing Day
	.flags = CAL_PERENNIAL,
	.week_length = 7,
	.start_weekday = MON13_FRIDAY
};

const struct mon13_cal mon13_tranquility_year_zero = {
	.intercalary_list = tranquility_ic,
	.common_lookup_list = tranquility_common_lookup,
	.leap_lookup_list = tranquility_leap_lookup,
	.leap_cycle = {
		.year_count = 4,
		.leap_year_count = 1,
		.offset_years = 31,
		.offset_days = 11323,
		.flags = LEAP_GREGORIAN_SKIP,
		.common_days = 365,
		.leap_days = 1
	},
	.epoch_mjd = 40423, //1 day after Moon Landing Day
	.flags = CAL_YEAR_ZERO | CAL_PERENNIAL,
	.week_length = 7,
	.start_weekday = MON13_FRIDAY
};