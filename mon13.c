#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "mon13.h"
#include "cal.h"

//Helper structs
struct doy_date {
	int32_t year;
	uint16_t doy;
};

//Generic helpers
bool is_leap(int32_t year, const struct mon13_cal* cal) {
	if(year < 1 && (cal->flags & CAL_YEAR_ZERO) == 0) {
		year++;
	}
	if((cal->flags & CAL_GREGORIAN_LEAP) != 0) {
		year += cal->era_start_gregorian.year;
	}
	return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
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

uint16_t year_len(bool leap) {
	return leap ? 366 : 365;
}

struct mon13_date last_day_of_year(int32_t year, const struct mon13_cal* cal) {
	const bool leap = is_leap(year, cal);
	const struct lkup* res_lookup_list = leap ? cal->leap_lookup_list : cal->common_lookup_list;
	const unsigned doy_max = year_len(leap);
	unsigned doy_past = 0;
	size_t max_i = 0;
	for(size_t i = 0; doy_past < doy_max; i++) {
		const struct lkup segment = res_lookup_list[i];
		doy_past = segment.offset + segment_len(segment);
		max_i = i;
	}
	return last_day_of_segment(year, res_lookup_list[max_i]);
}

struct doy_date month_day_to_doy(struct mon13_date d, const struct mon13_cal* cal) {
	//Assuming d is normalized.
	const bool leap = is_leap(d.year, cal);
	const unsigned doy_max = year_len(leap);
	const struct lkup* lookup_list = leap ? cal->leap_lookup_list : cal->common_lookup_list;

	unsigned doy_past = 0;
	for(size_t i = 0; doy_past < doy_max; i++) {
		const struct lkup segment = lookup_list[i];
		if(d.month == segment.month) {
			if(d.day >= segment.day_start && d.day <= segment.day_end) {
				struct doy_date res;
				res.year = d.year;
				res.doy = segment.offset + (d.day - segment.day_start) + 1;
				return res;
			}
		}
		doy_past = segment.offset + segment_len(segment);
	}

	struct doy_date final;
	final.year = d.year;
	final.doy = doy_max;
	return final;
}

struct mon13_date doy_to_month_day(struct doy_date d, const struct mon13_cal* cal) {
	//Assuming d is normalized.
	const bool leap = is_leap(d.year, cal);
	const unsigned doy_max = year_len(leap);
	const struct lkup* lookup_list = leap ? cal->leap_lookup_list : cal->common_lookup_list;

	unsigned doy_past = 0;
	for(size_t i = 0; doy_past < doy_max; i++) {
		const struct lkup segment = lookup_list[i];
		doy_past = segment.offset + segment_len(segment);
		if(d.doy > segment.offset && d.doy <= doy_past) {
			struct mon13_date res; 
			res.year = d.year;
			res.month = segment.month;
			res.day = d.doy - segment.offset + segment.day_start - 1;
			return res;
		}
	}

	return last_day_of_year(d.year, cal);
}

//Normalization
int32_t norm_year(int32_t year, const struct mon13_cal* cal) {
	return (year == 0 && (cal->flags & CAL_YEAR_ZERO) == 0) ? -1 : year;
}

int32_t norm_year_sum(int32_t year_sum, const struct mon13_cal* cal) {
	return (year_sum == 0 && (cal->flags & CAL_YEAR_ZERO) == 0) ? 1 : year_sum;
}

struct mon13_date norm_month(struct mon13_date d, const struct mon13_cal* cal) {
	//Assuming d.year normalized
	const bool leap = is_leap(d.year, cal);
	const unsigned doy_max = year_len(leap);
	const struct lkup* lookup_list = leap ? cal->leap_lookup_list : cal->common_lookup_list;
	
	unsigned doy_past = 0;
	unsigned month_max = 0;
	for(size_t i = 0; doy_past < doy_max; i++) {
		const struct lkup segment = lookup_list[i];
		if(segment.month == d.month) {
			return d;
		}

		if(segment.month > month_max) {
			month_max = segment.month;
		}
		doy_past = segment.offset + segment_len(segment);
	}

	//If d.month == 0, but month 0 not found in lookup, convert to month_max;
	//If d.month > 0 but d.month < month_max, that's probably a bad calendar.
	uint16_t m = (d.month < month_max) ? month_max : d.month;

	struct mon13_date res;
	res.year = norm_year_sum(d.year + (m / month_max), cal);
	res.month = m + ((m - 1) % month_max) + 1; //Don't want res.month == 0.
	res.day = d.day;
	return res;
}

struct doy_date norm_day_of_year(struct doy_date d, const struct mon13_cal* cal) {
	uint16_t doy_done = 0;
	int32_t year = d.year;
	while(true) {
		uint16_t doy_sum = doy_done + year_len(is_leap(year, cal));
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
	struct doy_date res = {.year = norm_year_sum(year, cal), .doy = d.doy - doy_done};
	return res;
}

struct mon13_date norm_day(struct mon13_date d, const struct mon13_cal* cal) {
	//Assuming d.year and d.month normalized
	const bool leap = is_leap(d.year, cal);
	const unsigned doy_max = year_len(leap);
	const struct lkup* lookup_list = leap ? cal->leap_lookup_list : cal->common_lookup_list;

	unsigned doy_past = 0;
	size_t matching_i = 0;
	for(size_t i = 0; doy_past < doy_max; i++) {
		const struct lkup segment = lookup_list[i];
		if(segment.month == d.month) {
			if(d.day >= segment.day_start && d.day <= segment.day_end) {
				return d;
			}
			matching_i = i;
		}
		doy_past = segment.offset + segment_len(segment);
	}

	
	if(d.day == 0) {
		if(matching_i == 0) {
			return last_day_of_year(norm_year(d.year - 1, cal), cal);
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
	int32_t year = norm_year(d.year, cal);
	struct mon13_date d_norm_year = {.year = year, .month = d.month, .day = d.day};
	struct mon13_date d_norm_month = norm_month(d_norm_year, cal);
	struct mon13_date d_norm_day = norm_day(d_norm_month, cal);
	return d_norm_day;
}

//Addition and Subtraction
struct doy_date add_day_of_year(struct doy_date d0, struct doy_date d1, const struct mon13_cal* cal) {
	int32_t sum_year = norm_year_sum(d0.year + d1.year, cal);
	struct doy_date sum = {.year = sum_year, .doy = d0.doy + d1.doy};
	return norm_day_of_year(sum, cal);
}

struct doy_date sub_day_of_year(struct doy_date d0, struct doy_date d1, const struct mon13_cal* cal) {
	int32_t diff_year = norm_year(d0.year - d1.year, cal);
	uint16_t big_doy = d0.doy;
	if(d0.doy <= d1.doy) {
		diff_year = norm_year(diff_year - 1, cal);
		big_doy += year_len(is_leap(diff_year, cal));
	}
	struct doy_date diff = {.year = diff_year, .doy = big_doy - d1.doy};
	return norm_day_of_year(diff, cal);
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
	struct mon13_date d_norm = normalize(d, src);
	if(src == dest) {
		return d_norm;
	}

	const struct mon13_cal* mediator = &mon13_gregorian_year_zero;
	struct doy_date src_doy = month_day_to_doy(src->era_start_gregorian, mediator);
	struct doy_date dest_doy = month_day_to_doy(dest->era_start_gregorian, mediator);
	struct doy_date d_doy = month_day_to_doy(d, src);

	struct doy_date sum = add_day_of_year(d_doy, src_doy, mediator);
	struct doy_date diff = sub_day_of_year(sum, dest_doy, mediator);
	return doy_to_month_day(diff, dest);
}

struct mon13_date mon13_add(
	const struct mon13_date d,
	const struct mon13_cal* cal,
	const int32_t offset,
	const enum mon13_add_mode mode
) {
	struct mon13_date res = {.year = 0, .month = 0, .day = 9};
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
		d0_norm = normalize(*d0, cal);
		d1_norm = normalize(*d1, cal);
	}

	//TODO: if(d0->month == 0 || d1->month == 0)

	if(d0_norm.year != d1_norm.year) {
		return d0_norm.year - d1_norm.year;
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
	struct mon13_date norm_d = normalize(d, cal);
	switch(mode) {
		case MON13_IS_LEAP_YEAR: return is_leap(d.year, cal);
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