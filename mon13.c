#include <stdint.h>
#include <stddef.h>

#include "mon13.h"

enum mon13_validity mon13_bad_date(
	const struct mon13_cal* cal,
	const struct mon13_date d
) {
	enum mon13_validity res;
	return res;
}

bool mon13_is_leap_year(
	const struct mon13_cal* cal,
	const int32_t year
) {
	int32_t y = year;
	if(year < 0) { //Assuming no zero year
		y++;
	}
	if(cal != NULL && (cal->flags & MON13_CAL_GREGORIAN_LEAP_YEAR)) {
		y += cal->era_start_gregorian.year;
	}
	return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
}

struct mon13_date mon13_convert(
	const struct mon13_cal* src,
	const struct mon13_cal* dest,
	const struct mon13_date d
) {
	struct mon13_date res;
	return res;
}

int mon13_fmt(
	const struct mon13_cal* cal,
	const struct mon13_date d,
	const char* fmt,
	char* buf,
	const size_t buflen
) {
	int res;
	return res;
}

int mon13_compare(
	const struct mon13_date* d0,
	const struct mon13_date* d1,
	struct mon13_cal* cal
) {
	int res;
	return res;
}

struct mon13_date mon13_add(
	const struct mon13_cal* cal,
	const struct mon13_date a,
	const struct mon13_date b,
	const bool skip_intercalary_day
) {
	struct mon13_date res;
	return res;
}

int8_t mon13_get_weekday(
	const struct mon13_cal* cal,
	const struct mon13_date d
) {
	if(cal == NULL || d.month == 0) {
		return -1;
	}
	return (d.day - 1) % MON13_DAY_PER_WEEK;
}