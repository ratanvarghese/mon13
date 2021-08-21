#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "mon13.h"
#include "cal.h"

//Helper structs
struct floor_res {
	int quot;
	int rem;
};

struct doy_date {
	int32_t year;
	uint16_t doy;
};

//Generic helpers
struct floor_res floor_div(int32_t a, int32_t b) {
	struct floor_res res;
	res.quot = a / b;
	res.rem = a % b;

	//By C99, negative division tends to 0.
	//We want negative division tending towards -infinity.
	//See https://stackoverflow.com/questions/46265403/
	bool negative_division = (a < 0) ^ (b < 0);
	if(res.rem != 0 && negative_division) {
		res.quot--;
		res.rem += b;
	}
	return res;
}

unsigned clock_modulo(unsigned a, unsigned b) {
	return ((a - 1) % b) + 1;
}

bool is_leap(int32_t year, const struct mon13_cal* cal) {
	int y = year;
	int c = cal->leap_cycle.year_count;
	int l = cal->leap_cycle.leap_year_count;
	int A = cal->leap_cycle.offset_years;

	int adjusted = (l*y - A);
	struct floor_res f_simple = floor_div(adjusted, c);
	bool simple_res = f_simple.rem < l;
	if(cal->flags & LEAP_GREGORIAN_SKIP) {
		struct floor_res f_100 = floor_div(adjusted, 100);
		struct floor_res f_400 = floor_div(adjusted, 400);
		return simple_res && (f_100.rem != 0 || f_400.rem == 0);
	}
	else {
		return simple_res;
	}
}

struct mon13_date last_day_of_segment(int32_t year, struct lkup segment) {
	struct mon13_date res;
	res.year = year;
	res.month = segment.month;
	res.day = segment.day_end;
	return res;
}

uint8_t segment_len(const struct lkup segment) {
	return (segment.day_end - segment.day_start) + 1;
}

uint16_t year_len(bool leap, const struct mon13_cal* cal) {
	struct leap_cycle_info lc = cal->leap_cycle; 
	return leap ? lc.common_days + lc.leap_days : lc.common_days;
}

struct mon13_date last_day_of_year(int32_t year, const struct mon13_cal* cal) {
	const bool leap = is_leap(year, cal);
	const struct lkup* res_lookup_list = leap ? cal->leap_lookup_list : cal->common_lookup_list;
	size_t max_i = 0;
	for(size_t i = 0; (res_lookup_list[i].flags & LKUP_SENTINEL) == 0; i++) {
		max_i = i;
	}
	return last_day_of_segment(year, res_lookup_list[max_i]);
}

struct doy_date month_day_to_doy(struct mon13_date d, const struct mon13_cal* cal) {
	//Assuming d is normalized.
	const bool leap = is_leap(d.year, cal);
	const struct lkup* lookup_list = leap ? cal->leap_lookup_list : cal->common_lookup_list;

	for(size_t i = 0; (lookup_list[i].flags & LKUP_SENTINEL) == 0; i++) {
		const struct lkup segment = lookup_list[i];
		if(d.month == segment.month) {
			if(d.day >= segment.day_start && d.day <= segment.day_end) {
				struct doy_date res;
				res.year = d.year;
				res.doy = segment.offset + (d.day - segment.day_start) + 1;
				return res;
			}
		}
	}

	struct doy_date final;
	final.year = d.year;
	final.doy = year_len(leap, cal);
	return final;
}

struct mon13_date doy_to_month_day(struct doy_date d, const struct mon13_cal* cal) {
	//Assuming d is normalized.
	const bool leap = is_leap(d.year, cal);
	const struct lkup* lookup_list = leap ? cal->leap_lookup_list : cal->common_lookup_list;

	for(size_t i = 0; (lookup_list[i].flags & LKUP_SENTINEL) == 0; i++) {
		const struct lkup segment = lookup_list[i];
		unsigned segment_end = segment.offset + segment_len(segment);
		if(d.doy > segment.offset && d.doy <= segment_end) {
			struct mon13_date res;
			res.year = d.year;
			res.month = segment.month;
			res.day = d.doy - segment.offset + segment.day_start - 1;
			return res;
		}
	}

	return last_day_of_year(d.year, cal);
}

const struct intercalary* seek_ic(struct mon13_date d, const struct mon13_cal* cal) {
	for(int i = 0; (cal->intercalary_list[i].flags & IC_SENTINEL) == 0; i++) {
		const struct intercalary* res = &(cal->intercalary_list[i]);
		if(res->month == d.month && res->day == d.day) {
			return res;
		}
	}
	return NULL;
}

//MJD conversions
struct doy_date mjd_to_doy(int32_t mjd, const struct mon13_cal* cal) {
	const struct leap_cycle_info lc = cal->leap_cycle;

	const int day_total = mjd - cal->epoch_mjd - lc.offset_days;
	struct floor_res f_400, f_100, f_cycle, f_common;
	if(lc.flags & LEAP_GREGORIAN_SKIP) {
		const int common_400 = year_len(false, cal) * 400;
		const int leap_400 = (100 - 3) * lc.leap_days;
		f_400 = floor_div(day_total, common_400 + leap_400);

		const int common_100 = year_len(false, cal) * 100;
		const int leap_100 = ((100/lc.year_count) - 1) * lc.leap_days;
		f_100 = floor_div(f_400.rem, common_100 + leap_100);
	}
	else {
		f_400.quot = 0;
		f_400.rem = 0;
		f_100.quot = 0;
		f_100.rem = day_total;
	}

	const int common_cycle = year_len(false, cal) * lc.year_count;
	const int leap_cycle = lc.leap_year_count * lc.leap_days;
	f_cycle = floor_div(f_100.rem, common_cycle + leap_cycle);
	f_common = floor_div(f_cycle.rem, year_len(false, cal));

	struct doy_date res;
	res.year = 400*f_400.quot + 100*f_100.quot + lc.year_count*f_cycle.quot + f_common.quot;
	res.year += lc.offset_years;

	if(f_100.quot == lc.year_count || f_common.quot == lc.year_count) {
		res.doy = year_len(true, cal);
	}
	else {
		res.year++;
		res.doy = f_common.rem + 1;
	}
	return res;
}

int32_t doy_to_mjd(struct doy_date doy, const struct mon13_cal* cal) {
	const struct leap_cycle_info lc = cal->leap_cycle;

	const int32_t off_year = doy.year - 1 - lc.offset_years;
	const int32_t common_days = off_year * lc.common_days;
	
	struct floor_res f_leap = floor_div(off_year, lc.year_count);
	int32_t leap_days = lc.leap_year_count * f_leap.quot * lc.leap_days;

	if(lc.flags & LEAP_GREGORIAN_SKIP) {
		struct floor_res f_100 = floor_div(off_year, 100);
		struct floor_res f_400 = floor_div(off_year, 400);
		leap_days += ((f_400.quot - f_100.quot) * lc.leap_days);
	}

	const int32_t total_days = common_days + leap_days + lc.offset_days;
	const int32_t mjd = total_days + cal->epoch_mjd - 1;

	return mjd + doy.doy;
}

//Normalization
struct mon13_date norm_month(struct mon13_date d, const struct mon13_cal* cal) {
	//Assuming d.year normalized
	const bool leap = is_leap(d.year, cal);
	const struct lkup* lookup_list = leap ? cal->leap_lookup_list : cal->common_lookup_list;
	
	unsigned month_max = 0;
	for(size_t i = 0; (lookup_list[i].flags & LKUP_SENTINEL) == 0; i++) {
		const struct lkup segment = lookup_list[i];
		if(segment.month == d.month) {
			return d;
		}

		if(segment.month > month_max) {
			month_max = segment.month;
		}
	}

	//If d.month == 0, but month 0 not found in lookup, convert to month_max;
	//If d.month > 0 but d.month < month_max, that's probably a bad calendar.
	uint16_t m = (d.month < month_max) ? month_max : d.month;

	struct mon13_date res;
	res.year = d.year + (m / month_max);
	res.month = clock_modulo(m, month_max); //Don't want res.month == 0.
	res.day = d.day;
	return res;
}

struct doy_date norm_day_of_year(struct doy_date d, const struct mon13_cal* cal) {
	uint16_t doy_done = 0;
	int32_t year = d.year;
	while(true) {
		uint16_t doy_sum = doy_done + year_len(is_leap(year, cal), cal);
		if(doy_sum >= d.doy) {
			//doy_done never exceeds d.doy.
			//That makes it slightly easier to determine the result.
			//Use >= for 1-based doy, > for 0-based doy.
			break;
		}
		else {
			year++;
			doy_done = doy_sum;
		}
	}
	struct doy_date res = {.year = year, .doy = d.doy - doy_done};
	return res;
}

struct mon13_date norm_day(struct mon13_date d, const struct mon13_cal* cal) {
	//Assuming d.year and d.month normalized
	const bool leap = is_leap(d.year, cal);
	const struct lkup* lookup_list = leap ? cal->leap_lookup_list : cal->common_lookup_list;

	size_t matching_i = 0;
	for(size_t i = 0; (lookup_list[i].flags & LKUP_SENTINEL) == 0; i++) {
		const struct lkup segment = lookup_list[i];
		if(segment.month == d.month) {
			if(d.day >= segment.day_start && d.day <= segment.day_end) {
				return d;
			}
			matching_i = i;
		}
	}

	
	if(d.day == 0) {
		if(matching_i == 0) {
			return last_day_of_year(d.year - 1, cal);
		}
		else {
			return last_day_of_segment(d.year, lookup_list[matching_i - 1]);
		}
	}

	//At this point, assuming a valid cal, d.day is too big.
	const struct lkup matching = lookup_list[matching_i];
	uint16_t excess_days = d.day - matching.day_start;
	struct doy_date dd = {.year = d.year, .doy = matching.offset + excess_days};
	return doy_to_month_day(norm_day_of_year(dd, cal), cal);
}

struct mon13_date normalize(struct mon13_date d, const struct mon13_cal* cal) {
	struct mon13_date d_norm_month = norm_month(d, cal);
	struct mon13_date d_norm_day = norm_day(d_norm_month, cal);
	return d_norm_day;
}

//Year Zero adjustment

bool yz_needs_adjustment(int32_t y, const struct mon13_cal* cal) {
	return (cal->flags & CAL_YEAR_ZERO) == 0 && y < 1;
}

struct mon13_date yz_to_no_yz(struct mon13_date d, const struct mon13_cal* cal) {
	int32_t y = yz_needs_adjustment(d.year, cal) ? (d.year - 1) : d.year;
	struct mon13_date res = {.year = y, .month = d.month, .day = d.day};
	return res;
}

struct mon13_date no_yz_to_yz(struct mon13_date d, const struct mon13_cal* cal) {
	int32_t y = yz_needs_adjustment(d.year, cal) ? (d.year + 1) : d.year;
	struct mon13_date res = {.year = y, .month = d.month, .day = d.day};
	return res;
}

//Adding
struct mon13_date add_days(struct mon13_date d, int32_t offset, const struct mon13_cal* cal) {
	int32_t mjd = doy_to_mjd(month_day_to_doy(d, cal), cal);
	struct mon13_date res = doy_to_month_day(mjd_to_doy(mjd + offset, cal), cal);
	return res;
}

struct mon13_date add_years(struct mon13_date d, int32_t offset) {
	struct mon13_date res = {.year = d.year + offset, .month = d.month, .day = d.day};
	return res;
}

struct mon13_date add_months(struct mon13_date d, int32_t offset, const struct mon13_cal* cal) {
	bool must_change_month = (d.month == 0);
	uint8_t max_month = 0;
	size_t matching_i = 0;
	size_t pre_matching_i = 0;
	size_t post_matching_i = 0;
	const struct lkup* lookup_list = is_leap(d.year, cal) ? cal->leap_lookup_list : cal->common_lookup_list;
	for(size_t i = 0; (lookup_list[i].flags & LKUP_SENTINEL) == 0; i++) {
		const struct lkup segment = lookup_list[i];
		if(max_month < segment.month) {
			max_month = segment.month;
		}
		if(d.month == segment.month) {
			if(d.day >= segment.day_start && d.day <= segment.day_end) {
				matching_i = i;
			}
		}
		else if(must_change_month){
			if(matching_i == 0) {
				pre_matching_i = i;
			}
			else if(post_matching_i == 0) {
				post_matching_i = i;
			}
		}
	}

	struct floor_res f_year = floor_div(offset, max_month);
	struct mon13_date sum_year = add_years(d, f_year.quot);
	int month_diff = f_year.rem;
	if(month_diff == 0) {
		return sum_year;
	}

	if(must_change_month) {
		if(pre_matching_i == 0) {
			sum_year.month = lookup_list[post_matching_i].month;
			sum_year.day = lookup_list[post_matching_i].day_start;
		}
		else {
			sum_year.month = lookup_list[pre_matching_i].month;
			sum_year.day = lookup_list[pre_matching_i].day_end;
		}
	}

	int sum_month = sum_year.month + month_diff;
	struct mon13_date res;
	res.year = (sum_month > max_month) ? (sum_year.year + 1) : sum_year.year;
	res.month = clock_modulo(sum_month, max_month);
	res.day = sum_year.day;
	
	const struct lkup* res_lookup_list = is_leap(sum_year.year, cal) ? cal->leap_lookup_list : cal->common_lookup_list;
	for(size_t i = 0; (res_lookup_list[i].flags & LKUP_SENTINEL) == 0; i++) {
		const struct lkup segment = res_lookup_list[i];
		if(res.month == segment.month) {
			if(sum_year.day >= segment.day_start && sum_year.day <= segment.day_end) {
				res.day = sum_year.day;
			}
			else if(sum_year.day > segment.day_end) {
				res.day = segment.day_end;
			}
		}
	}
	return res;
}

//Day of Week
enum mon13_weekday get_day_of_week(struct mon13_date d, const struct mon13_cal* cal) {
	struct floor_res f_week;
	if((cal->flags & CAL_PERENNIAL) == 0) {
		int32_t mjd = doy_to_mjd(month_day_to_doy(d, cal), cal);
		f_week = floor_div(mjd, cal->week_length);
		return clock_modulo(f_week.rem + MON13_WEDNESDAY, cal->week_length);
	}
	else {
		const struct intercalary* ic = seek_ic(d, cal);
		if(ic != NULL) {
			return MON13_NO_WEEKDAY;
		}

		f_week = floor_div(d.day, cal->week_length);
		return clock_modulo(f_week.rem + cal->start_weekday - 1, cal->week_length);
	}
}


//Public functions
struct mon13_date mon13_convert(
	const struct mon13_date d,
	const struct mon13_cal* src,
	const struct mon13_cal* dest
) {
	if(src == NULL || dest == NULL) {
		return d;
	}

	struct mon13_date src_yz = no_yz_to_yz(d, src);
	struct mon13_date src_norm = normalize(src_yz, src);
	if(src == dest) {
		return src_norm;
	}

	int32_t mjd = doy_to_mjd(month_day_to_doy(src_norm, src), src);
	struct mon13_date res_yz = doy_to_month_day(mjd_to_doy(mjd, dest), dest);
	struct mon13_date res = yz_to_no_yz(res_yz, dest);
	return res;
}

struct mon13_date mon13_add(
	const struct mon13_date d,
	const struct mon13_cal* cal,
	const int32_t offset,
	const enum mon13_add_mode mode
) {
	
	struct mon13_date d_yz = no_yz_to_yz(d, cal);
	struct mon13_date d_norm = normalize(d_yz, cal);
	struct mon13_date res_yz;
	switch(mode) {
		case MON13_ADD_NONE: res_yz = d_norm;
		case MON13_ADD_DAYS: res_yz = add_days(d, offset, cal); break;
		case MON13_ADD_MONTHS: res_yz = add_months(d, offset, cal); break;
		case MON13_ADD_YEARS: res_yz = add_years(d, offset); break;
		default: {
			res_yz.year = 0;
			res_yz.month = 0;
			res_yz.day = 9;
		}
	}
	struct mon13_date res = yz_to_no_yz(res_yz, cal);
	return res;
}

int mon13_compare(
	const struct mon13_date* d0,
	const struct mon13_date* d1,
	const struct mon13_cal* cal
) {
	struct mon13_date d0_norm, d1_norm;
	if(cal == NULL) {
		d0_norm = *d0;
		d1_norm = *d1;
	}
	else {
		d0_norm = normalize(no_yz_to_yz(*d0, cal), cal);
		d1_norm = normalize(no_yz_to_yz(*d1, cal), cal);
	}

	if(d0_norm.year != d1_norm.year) {
		return d0_norm.year - d1_norm.year;
	}
	else if(cal != NULL && (d0->month == 0 || d1->month == 0)) {
		struct doy_date doy0 = month_day_to_doy(*d0, cal);
		struct doy_date doy1 = month_day_to_doy(*d1, cal);
		return doy0.doy - doy1.doy;
	}
	else if(d0_norm.month != d1_norm.month) {
		return d0_norm.month - d1_norm.month;
	}
	else {
		return d0_norm.day - d1_norm.day;
	}
}

int mon13_extract(
	const struct mon13_date d,
	const struct mon13_cal* cal,
	const enum mon13_extract_mode mode
) {
	struct mon13_date norm_d = normalize(no_yz_to_yz(d, cal), cal);
	switch(mode) {
		case MON13_DAY_OF_YEAR: return month_day_to_doy(norm_d, cal).doy;
		case MON13_DAY_OF_WEEK: return get_day_of_week(norm_d, cal);
		case MON13_IS_LEAP_YEAR: return is_leap(norm_d.year, cal);
		default: return 0;
	}
}

int mon13_format(
	const struct mon13_date d,
	const struct mon13_cal* cal,
	const char* fmt,
	char* buf,
	const size_t buflen
) {
	return 0;
}