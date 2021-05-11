//This is supposed to be a private header, not exposed as part of the mon13 API.

#ifndef MON13_CAL_H
#define MON13_CAL_H

#include <stdint.h>
#include "mon13.h"

//Enumerations
enum intercalary_flags {
	IC_NONE = 0,
	IC_YEAR = 1 << 0,
	IC_ERA_START_ALT_NAME = 1 << 1,
	IC_LEAP = 1 << 2,
	IC_SENTINEL = 1 << 3
};

//Structures
struct lkup {
	uint16_t offset;
	uint8_t month;
	uint8_t day_start;
	uint8_t day_end;
};

struct intercalary {
	const char* name;
	const char* alt_name;
	int16_t flags;
	int16_t day_of_year;
	int8_t month;
	int8_t day;
};

struct mon13_cal {
	const struct intercalary* intercalary_list;
	const struct lkup* common_lookup_list;
	const struct lkup* leap_lookup_list;
	const char** month_list;
	const char** weekday_list;
	const char** era_list;
	const char* name;
	struct mon13_date era_start_gregorian;
};

#endif //MON13_CAL_H