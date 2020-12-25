#ifndef MON13_H
#define MON13_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MON13_VALID_EXTRA_DAY 1
#define MON13_INVALID_EXTRA_DAY 0
#define MON13_NO_MONTH -1

struct mon13_date {
	int32_t year;
	int8_t month;
	int8_t day;
};

struct mon13_cal {
	const char* month_names[14];
	const char* extra_day_names[2];
	const char* era_start_name;
	const char* before_era_name;
	const char* after_era_name;
	struct mon13_date extra_days[2];
	struct mon13_date before_extra_days[2];
	struct mon13_date era_start_gregorian;
	int16_t leap_year_offset;
	int8_t month_length[14];
	bool yearless_cal_start;
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

void mon13_iter_step(
	const struct mon13_cal* cal,
	struct mon13_date* current,
	const struct mon13_date step,
	const bool skip_year_day,
	const bool skip_leap_day
);

struct mon13_cal mon13_gregorian;
struct mon13_cal mon13_tranquility;
struct mon13_cal mon13_international_fixed;
struct mon13_cal mon13_positivist;
struct mon13_cal mon13_world;

#endif //MON13_H