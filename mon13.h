#ifndef MON13_H
#define MON13_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

enum mon13_ic_flags {
	MON13_IC_NONE = 0,
	MON13_IC_YEAR = 1 << 0,
	MON13_IC_LEAP = 1 << 1,
	MON13_IC_ERA_START = 1 << 2,
	MON13_IC_YEAR_START = 1 << 3
};

enum mon13_cal_flags {
	MON13_CAL_NONE = 0,
	MON13_CAL_YEARLESS_ERA_START = 1 << 0,
	MON13_CAL_GREGORIAN_LEAP_YEAR = 1 << 1
};

struct mon13_date {
	int32_t year;
	int8_t month;
	int8_t day;
};

struct mon13_intercalary {
	const char* name;
	int8_t month;
	int8_t day;
	int8_t before_month;
	int8_t before_day;
	int8_t flags;
};

struct mon13_cal {
	struct mon13_intercalary intercalary_days[3];
	const char* month_names[13];
	const char* weekday_names[7];
	const char* era_names[2];
	const char* cal_name;
	struct mon13_date era_start_gregorian;
	int32_t era_start_millisecond;
	int8_t intercalary_day_count;
	int8_t flags;
};

struct mon13_date mon13_convert(
	const struct mon13_cal* src,
	const struct mon13_cal* dest,
	const struct mon13_date d
);

int mon13_fmt(
	const struct mon13_cal* cal,
	const struct mon13_date d,
	const char* fmt,
	char* buf,
	const size_t buflen
);

int mon13_compare(
	const struct mon13_date* d0,
	const struct mon13_date* d1,
	const struct mon13_cal* cal
);

struct mon13_date mon13_add(
	const struct mon13_cal* cal,
	const struct mon13_date a,
	const struct mon13_date b,
	const bool skip_intercalary_day
);

int mon13_get_weekday(
	const struct mon13_cal* cal,
	const struct mon13_date d
);

struct mon13_cal mon13_tranquility;
struct mon13_cal mon13_international_fixed;
struct mon13_cal mon13_positivist;

#endif //MON13_H