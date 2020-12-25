#ifndef MON13_H
#define MON13_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

enum mon13_intercalary {
	mon13_ic_invalid = 0,
	mon13_ic_leap_day = 1,
	mon13_ic_year_day = 2,
	mon13_ic_has_month = 4,
	mon13_ic_has_weekday = 8,
};

struct mon13_date {
	int32_t year;
	int8_t month;
	int8_t day;
};

struct mon13_cal {
	const char* month_names[14];
	const char* weekday_names[7];
	const char* intercalary_day_names[2];
	const char* cal_name;
	const char* era_start_name;
	const char* before_era_name;
	const char* after_era_name;
	struct mon13_date intercalary_days[2];
	struct mon13_date before_intercalary_days[2];
	struct mon13_date era_start_gregorian;
	int16_t leap_year_offset;
	int8_t month_length[14];
	bool yearless_cal_start;
	bool fixed_start_weekday;
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

struct mon13_cal mon13_gregorian;
struct mon13_cal mon13_tranquility;
struct mon13_cal mon13_international_fixed;
struct mon13_cal mon13_positivist;
struct mon13_cal mon13_world;

#endif //MON13_H