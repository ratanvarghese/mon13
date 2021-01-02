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

static bool is_leap_year(
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

static int16_t to_doy_g(const struct mon13_date d, bool leap) {
	int16_t res = d.day;
	for(int8_t i = 1; i < d.month; i++) {
		res += max_day_gregorian(i, leap);
	}
	return res;
}

static int cmp(int x, int y) {
	//https://stackoverflow.com/a/1903975
	return (x > y) - (x < y);
}

static int16_t to_doy(
	const struct mon13_date d,
	const struct mon13_cal* cal
) {
	bool leap = is_leap_year(cal, d.year);
	if(cal == NULL) {
		return to_doy_g(d, leap);
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
			if(!is_leap_day || leap) {
				res = ic.day_of_year;
				res_is_ic = true;
			}
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

static struct mon13_date from_doy_g(
	const int32_t year,
	const int16_t doy,
	const bool leap
) {
	struct mon13_date res = {.year = year, .month = 1, .day = 1};
	int16_t acc = 0;
	int16_t old_acc = 0;
	for(int8_t i = 1; i <= MON13_GREGORIAN_MONTH_PER_YEAR; i++) {
		acc += max_day_gregorian(i, leap);
		if(doy <= acc) {
			res.month = i;
			res.day = doy - old_acc;
			break;
		}
		old_acc = acc;
	}
	return res;
}

static struct mon13_date from_doy(
	const int32_t year,
	const int16_t doy,
	const struct mon13_cal* cal
) {
	bool leap = is_leap_year(cal, year);
	if(cal == NULL) {
		return from_doy_g(year, doy, leap);
	}

	struct mon13_date res = {.year = year, .month = 1, .day = 1};
	int16_t leap_boundary = 367; //Intentionally beyond length of year.
	const struct mon13_intercalary* year_day = NULL;
	for(int i = 0; i < cal->intercalary_day_count; i++) {
		struct mon13_intercalary ic = cal->intercalary_days[i];
		if(ic.flags & MON13_IC_YEAR) {
			year_day = &(cal->intercalary_days[i]);
		}

		bool res_era = year == 0 && (ic.flags & MON13_IC_ERA_START);
		bool ic_leap = ic.flags & MON13_IC_LEAP;
		bool res_leap = leap && ic_leap && (ic.day_of_year == doy);
		if(res_era || res_leap) {
			res.month = ic.month;
			res.day = ic.day;
			return res;
		}
		else if(leap && ic_leap) {
			leap_boundary = ic.day_of_year;
		}
	}
	int16_t n_doy = (doy > leap_boundary) ? (doy - 1) : doy;
	if(year_day != NULL) {
		if(year_day->day_of_year == n_doy) {
			res.month = year_day->month;
			res.day = year_day->day;
			return res;
		}
		else {
			n_doy = (n_doy >= year_day->day_of_year) ? (n_doy - 1) : n_doy;
		}
	}

	res.day = ((n_doy - 1) % MON13_DAY_PER_MONTH) + 1;
	res.month = ((n_doy - 1) / MON13_DAY_PER_MONTH) + 1;
	return res;
}

static struct mon13_date add_years(const struct mon13_date d, int32_t offset) {
	struct mon13_date res = d;
	res.year += offset;
	if(d.year < 0 && res.year >= 0) {
		res.year++;
	}
	else if(d.year >= 0 && res.year <= 0) {
		res.year--;
	}
	return res;
}

static struct mon13_date add_months(
	const struct mon13_date d,
	int32_t offset,
	const struct mon13_cal* c
) {
	int32_t max_month = (c == NULL) ? MON13_GREGORIAN_MONTH_PER_YEAR : MON13_MONTH_PER_YEAR;
	struct mon13_date res = add_years(d, offset / max_month);
	int8_t offset_months = offset % max_month;

	if(c != NULL) {
		while(res.month == 0) {
			int i = (res.day <= c->intercalary_day_count) ? (res.day - 1) : 0;
			struct mon13_intercalary ic = c->intercalary_days[i];
			res.month = ic.before_month;
			res.day = ic.before_day;
		}
	}

	res.month += offset_months;
	if(res.month < 1) {
		res.year--;
		res.month += max_month;
	}
	else if(res.month > max_month) {
		res.year++;
		res.month -= max_month;
	}
	if(res.year == 0) {
		res.year--;
	}

	bool leap = is_leap_year(c, res.year);
	int max_day = (c == NULL) ? max_day_gregorian(res.month, leap) : MON13_DAY_PER_MONTH;
	res.day = (res.day > max_day) ? max_day : res.day;
	return res;
}

static struct mon13_date add_days(
	const struct mon13_date d,
	int32_t offset,
	const struct mon13_cal* c
) {
	return d;
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
	int sgn_year = cmp(d0->year, d1->year);
	if(sgn_year != 0) {
		return sgn_year;
	}
	return cmp(to_doy(*d0, cal),to_doy(*d1, cal));
}

struct mon13_date mon13_add(
	const struct mon13_date d,
	int32_t offset,
	enum mon13_add_mode mode,
	const struct mon13_cal* cal
) {
	struct mon13_date res = d;
	if(offset != 0) {
		switch(mode) {
			case MON13_ADD_YEARS: res = add_years(d, offset); break;
			case MON13_ADD_MONTHS: res = add_months(d, offset, cal); break;
			case MON13_ADD_DAYS: res = add_days(d, offset, cal); break;
		}
	}

	res.weekday = (cal==NULL||res.month==0) ? -1 : (res.day-1)%MON13_DAY_PER_WEEK;
	res.flags = 0;
	if(is_leap_year(cal, res.year)) {
		res.flags |= MON13_DATE_IS_LEAP_YEAR;
	}
	return res;
}