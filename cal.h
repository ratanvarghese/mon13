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

enum cal_flags {
	CAL_NONE = 0,
	CAL_YEAR_ZERO = 1 << 0,
	CAL_PERENNIAL = 1 << 1
};

enum leap_flags {
	LEAP_NONE = 0,
	LEAP_GREGORIAN_SKIP = 1 << 0
};

enum lkup_flags {
	LKUP_NONE = 0,
	LKUP_SENTINEL = 1 << 0
};

//Structures
struct lkup {
	uint16_t offset;
	uint8_t month;
	uint8_t day_start;
	uint8_t day_end;
	uint8_t flags;
};

struct intercalary {
	int16_t flags;
	int16_t day_of_year;
	int8_t month;
	int8_t day;
};

struct leap_cycle_info {
	uint16_t year_count;
	uint16_t leap_year_count;
	uint16_t offset_years;
	uint16_t offset_days;
	uint16_t flags;
	uint16_t common_days;
	uint16_t leap_days;
};

struct mon13_cal {
	const struct intercalary* intercalary_list;
	const struct lkup* common_lookup_list;
	const struct lkup* leap_lookup_list;
	struct leap_cycle_info leap_cycle;
	int32_t epoch_mjd;
	int32_t flags;
	uint8_t week_length;
	uint8_t start_weekday;
};

#endif //MON13_CAL_H