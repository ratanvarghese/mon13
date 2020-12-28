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

static const struct mon13_intercalary* get_ic(
	const struct mon13_cal* cal,
	const struct mon13_date d
) {
	if(cal == NULL) {
		return NULL;
	}

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

static struct mon13_date normalize(const struct mon13_date d, const struct mon13_cal* cal) {
	struct mon13_date res = {.year = d.year, .month = d.month, .day = d.day};
	res.weekday = (cal==NULL||res.month==0) ? -1 : (res.day-1)%MON13_DAY_PER_WEEK;
	res.flags = 0;
	if(mon13_is_leap_year(cal, res.year)) {
		res.flags |= MON13_DATE_IS_LEAP_YEAR;
	}
	return res;
}

static int compare_simple(int8_t month0, int8_t day0, int8_t month1, int8_t day1) {
	if(month0 < month1) {
		return -1;
	}
	if(month0 > month1) {
		return 1;
	}
	if(day0 < day1) {
		return -1;
	}
	if(day0 > day1) {
		return 1;
	}

	return 0;
}

static struct mon13_date before_intercalary(
	const struct mon13_date d,
	struct mon13_cal* cal,
	int* recurse_count
) {
	if(cal == NULL) {
		return d;
	}
	const struct mon13_intercalary* ic = get_ic(cal, d);
	if(ic == NULL || (ic->before_month == d.month && ic->before_day == d.day)) {
		return d; //before.* == d.* is an invalid calendar: no looping, quit fast
	}
	if(recurse_count != NULL) {
		*recurse_count++;
	}
	struct mon13_date res;
	res.year = d.year;
	res.month = ic->before_month;
	res.day = ic->before_day;
	return before_intercalary(res, cal, recurse_count);
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

	int simple_res = compare_simple(d0->month, d0->day, d1->month, d1->day);
	if(simple_res == 0 || (d0->month > 0 && d1->month > 0)) {
		return simple_res;
	}

	int rc0 = 0;
	int rc1 = 0;
	struct mon13_date bic0 = before_intercalary(*d0, cal, &rc0);
	struct mon13_date bic1 = before_intercalary(*d1, cal, &rc1);
	int bic_res = compare_simple(bic0.month, bic0.day, bic1.month, bic1.day);
	return (bic_res == 0) ? (rc0 - rc1) : bic_res;
}

struct mon13_date mon13_add(
	const struct mon13_date d,
	int offset,
	enum mon13_add_mode mode,
	const struct mon13_cal* cal
) {
	struct mon13_date res = {.year = d.year, .month = d.month, .day = d.day};
	switch(mode) {
		case MON13_ADD_DAYS: res.day += offset; break;
		case MON13_ADD_MONTHS: res.month += offset; break;
		case MON13_ADD_YEARS: res.year += offset; break;
		default: break;
	}
	return normalize(res, cal);
}