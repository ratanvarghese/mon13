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

static bool mon13_is_leap_year(
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

static struct mon13_date normalize(int y, int m, int d, const struct mon13_cal* cal) {
	struct mon13_date res = {.year = y, .month = m, .day = d};
	res.weekday = (cal==NULL||res.month==0) ? -1 : (res.day-1)%MON13_DAY_PER_WEEK;
	res.flags = 0;
	if(mon13_is_leap_year(cal, res.year)) {
		res.flags |= MON13_DATE_IS_LEAP_YEAR;
	}
	return res;
}

static int16_t day_of_year_g(const struct mon13_date d, bool leap) {
	int16_t res = d.day;
	for(int8_t i = 1; i < d.month; i++) {
		res += max_day_gregorian(i, leap);
	}
	return res;
}

static int16_t day_of_year(
	const struct mon13_date d,
	const struct mon13_cal* cal
) {
	bool leap = mon13_is_leap_year(cal, d.year);
	if(cal == NULL) {
		return day_of_year_g(d, leap);
	}

	int16_t leap_boundary = 367; //Intentionally beyond length of year.
	int16_t res = d.day + ((d.month - 1) * MON13_DAY_PER_MONTH);
	bool res_is_ic = false;
	for(int i = 0; i < cal->intercalary_day_count; i++) {
		struct mon13_intercalary ic = cal->intercalary_days[i];
		bool is_leap_day = (ic.flags & MON13_IC_LEAP);
		bool is_era_start = (ic.flags & MON13_IC_ERA_START);
		if(leap && is_leap_day) {
			leap_boundary = ic.day_of_year;
		}

		if(is_era_start && d.year != 0) {
			continue;
		}
		else if(d.day == ic.day && d.month == ic.month) {
			res = ic.day_of_year;
			res_is_ic = true;
		}
		else if(!res_is_ic && !is_leap_day && res >= ic.day_of_year) {
			res++;
		}
	}
	if(res >= leap_boundary) {
		res++;
	}
	return res;
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
	if(d0->year < d1->year) {
		return -1;
	}
	if(d0->year > d1->year) {
		return 1;
	}

	int16_t day_of_year_0 = day_of_year(*d0, cal);
	int16_t day_of_year_1 = day_of_year(*d1, cal);
	if(day_of_year_0 < day_of_year_1) {
		return -1;
	}
	if(day_of_year_0 > day_of_year_1) {
		return 1;
	}
	return 0;
}

struct mon13_date mon13_add(
	const struct mon13_date d,
	int32_t offset,
	enum mon13_add_mode mode,
	const struct mon13_cal* cal
) {
	int year = d.year;
	int month = d.month;
	int day = d.day;
	switch(mode) {
		case MON13_ADD_DAYS: day += offset; break;
		case MON13_ADD_MONTHS: month += offset; break;
		case MON13_ADD_YEARS: year += offset; break;
		default: break;
	}
	return normalize(year, month, day, cal);
}