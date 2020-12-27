#include <stdint.h>
#include <stddef.h>

#include "mon13.h"

static int max_day_gregorian(int8_t month, bool leap) {
	switch(month) {
		case 2:
			return leap ? 29 : 28;
		case 4:
		case 6:
		case 9:
		case 11:
			return 30;
		default:
			return 31;
	}
}

static const struct mon13_intercalary* get_ic(
	const struct mon13_cal* cal,
	const struct mon13_date d
) {
	for(int i = 0; i < cal->intercalary_day_count; i++) {
		struct mon13_intercalary ic = cal->intercalary_days[i];
		if(ic.month == d.month && ic.day == d.day) {
			if((ic.flags & MON13_IC_LEAP) && !mon13_is_leap_year(cal,d.year)) {
				continue;
			}
			return &(cal->intercalary_days[i]);
		}
	}
	return NULL;
}

enum mon13_validity mon13_bad_date(
	const struct mon13_cal* cal,
	const struct mon13_date d
) {
	if(d.year == 0) {
		return MON13_INVALID_YEAR_ZERO;
	}

	if(cal == NULL) {
		if(d.month < 1 || d.month > MON13_GREGORIAN_MONTH_PER_YEAR) {
			return MON13_INVALID_MONTH;
		}
		int md = max_day_gregorian(d.month, mon13_is_leap_year(NULL, d.year));
		if(d.day < 1 || d.day > md) {
			return MON13_INVALID_DAY;
		}
	}
	else {
		if(get_ic(cal, d) != NULL) {
			return MON13_VALID;
		}
		if(d.month < 1 || d.month > MON13_MONTH_PER_YEAR) {
			return MON13_INVALID_MONTH;
		}
		if(d.day < 1 || d.day > MON13_DAY_PER_MONTH) {
			return MON13_INVALID_DAY;
		}
	}
	return MON13_VALID;
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