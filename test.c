#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include "theft.h"
#include "mon13.h"

struct test_1c1d {
	struct mon13_cal* c;
	struct mon13_date d;
	bool flip;
};

struct test_1c2d {
	struct mon13_cal* c;
	struct mon13_date d0;
	struct mon13_date d1;
	bool flip;
};

struct test_2c1d {
	struct mon13_cal* c0;
	struct mon13_cal* c1;
	struct mon13_date d;
	bool flip;
};

struct test_add {
	struct mon13_cal* c;
	struct mon13_date d;
	int32_t offset;
	bool flip;
};

#define TEST_QSORT_LEN 600
struct test_qsort {
	struct mon13_cal* c;
	struct mon13_date d[TEST_QSORT_LEN];
	int i0;
	int i1;
	int i2;
	bool fix_year;
	bool fix_month;
	bool flip;
};

static struct mon13_cal* random_cal(struct theft* t) {
	switch(theft_random_choice(t, 4)) {
		case 0: return &mon13_tranquility;
		case 1: return &mon13_international_fixed;
		case 2: return &mon13_positivist;
		case 3: return NULL;
		default: return NULL;
	}
}

static struct mon13_date random_date(struct theft* t, struct mon13_cal* c) {
	struct mon13_date res;
	res.year = theft_random_choice(t, INT32_MAX) + 1;
	if(theft_random_choice(t, 2)) {
		res.year = -(res.year);
	}
	if(c == NULL) {
		res.month = theft_random_choice(t, MON13_GREGORIAN_MONTH_PER_YEAR) + 1;
		switch(res.month) {
			case 2:
				if ((res.year%4 == 0 && res.year%100 != 0) || res.year%400 == 0) {
					res.day = theft_random_choice(t, 29) + 1;
				}
				else {
					res.day = theft_random_choice(t, 28) + 1;
				}
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				res.day = theft_random_choice(t, 30) + 1; break;
			default:
				res.day = theft_random_choice(t, 31) + 1; break;
		}
	}
	else {
		res.month = theft_random_choice(t, MON13_MONTH_PER_YEAR + 1);
		if(res.month == 0) {
			int ic_idx = theft_random_choice(t, c->intercalary_day_count);
			res.day = c->intercalary_days[ic_idx].day;
			res.month = c->intercalary_days[ic_idx].month;
			if(c->intercalary_days[ic_idx].flags & MON13_IC_ERA_START) {
				res.year = 0;
			}
		}
		else {
			res.day = theft_random_choice(t, MON13_DAY_PER_MONTH) + 1;
		}
	}
	return res;
}

enum theft_alloc_res alloc_leap_year(struct theft* t, void* data, void** instance) {
	int32_t* res = malloc(sizeof(int32_t));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	*res = theft_random_choice(t, INT_MAX);
	*res -= *res % 4;
	*res = (*res % 100 == 0 && *res % 400 != 0) ? (*res - 4) : *res;

	*instance = res;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_1c1d(struct theft* t, void* data, void** instance) {
	struct test_1c1d* res = malloc(sizeof(struct test_1c1d));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	res->c = random_cal(t);
	res->d = random_date(t, res->c);
	res->flip = theft_random_choice(t, 2);

	*instance = res;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_1c2d(struct theft* t, void* data, void** instance) {
	struct test_1c2d* res = malloc(sizeof(struct test_1c2d));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	res->c = random_cal(t);
	res->d0 = random_date(t, res->c);
	res->d1 = random_date(t, res->c);
	res->flip = theft_random_choice(t, 2);

	*instance = res;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_2c1d(struct theft* t, void* data, void** instance) {
	struct test_2c1d* res = malloc(sizeof(struct test_2c1d));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	res->c0 = random_cal(t);
	res->c1 = random_cal(t);
	res->d = random_date(t, res->c0);
	res->flip = theft_random_choice(t, 2);
	
	*instance = res;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_add(struct theft* t, void* data, void** instance) {
	struct test_add* res = malloc(sizeof(struct test_add));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	res->c = random_cal(t);
	res->d = random_date(t, res->c);
	res->offset = theft_random_choice(t, INT32_MAX);
	if(theft_random_choice(t, 2)) {
		res->offset = -(res->offset);
	}
	res->flip = theft_random_choice(t, 2);

	*instance = res;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_qsort(struct theft* t, void* data, void** instance) {
	struct test_qsort* res = malloc(sizeof(struct test_qsort));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	res->c = random_cal(t);
	res->i0 = theft_random_choice(t, TEST_QSORT_LEN/3) + 0*(TEST_QSORT_LEN/3);
	res->i1 = theft_random_choice(t, TEST_QSORT_LEN/3) + 1*(TEST_QSORT_LEN/3);
	res->i2 = theft_random_choice(t, TEST_QSORT_LEN/3) + 2*(TEST_QSORT_LEN/3);

	res->fix_year = theft_random_choice(0, 1);
	res->fix_month = theft_random_choice(0, 1);

	for(int i = 0; i < TEST_QSORT_LEN; i++) {
		res->d[i] = random_date(t, res->c);
		if(res->fix_year && i > 0) {
			res->d[i].year = res->d[0].year;
		}
		if(res->fix_month && i > 0) {
			if(res->d[0].month == 0) {
				res->d[0].month = 1;
			}
			res->d[i].month = res->d[0].month;
		}
	}

	res->flip = theft_random_choice(t, 2);
	*instance = res;
	return THEFT_ALLOC_OK;
}

void print_leap_year(FILE* f, const void* instance, void* env) {
	const int32_t* input = instance;
	fprintf(f, "%d", *input);
}

void print_1c1d(FILE* f, const void* instance, void* env) {
	const struct test_1c1d* input = instance;
	const struct mon13_date d = input->d;
	const char* name = (input->c == NULL) ? "Gregorian" : input->c->cal_name;
	const char* flip = input->flip ? "true" : "false";
	fprintf(
		f, "(%d-%02d-%02d) %s, flip = %s",
		d.year, d.month, d.day, name, flip
	);
}

void print_1c2d(FILE* f, const void* instance, void* env) {
	const struct test_1c2d* input = instance;
	const char* name = (input->c == NULL) ? "Gregorian" : input->c->cal_name;
	const char* flip = input->flip ? "true" : "false";
	fprintf(
		f, "(%d-%02d-%02d) (%d-%02d-%02d) %s, flip = %s",
		input->d0.year, input->d0.month, input->d0.day,
		input->d1.year, input->d1.month, input->d1.day,
		name, flip
	);
}

void print_2c1d(FILE* f, const void* instance, void* env) {
	const struct test_2c1d* input = instance;
	const char* name0 = (input->c0 == NULL) ? "Gregorian" : input->c0->cal_name;
	const char* name1 = (input->c1 == NULL) ? "Gregorian" : input->c1->cal_name;
	const char* flip = input->flip ? "true" : "false";
	fprintf(
		f, "(%d-%02d-%02d) %s, %s, flip = %s",
		input->d.year, input->d.month, input->d.day,
		name0, name1, flip
	);
}

void print_add(FILE* f, const void* instance, void* env) {
	const struct test_add* input = instance;
	const struct mon13_date d = input->d;
	const char* name = (input->c == NULL) ? "Gregorian" : input->c->cal_name;
	const char* flip = input->flip ? "true" : "false";
	fprintf(
		f, "(%d-%02d-%02d) %s, offset = %d, flip = %s",
		d.year, d.month, d.day, name, input->offset, flip
	);
}

enum theft_trial_res is_leap_year_gregorian(struct theft* t, void* test_input) {
	int32_t* input = test_input;
	struct mon13_date d = {.year = *input, .month = 1, .day = 1};
	d = mon13_add(d, 0, MON13_ADD_NONE, NULL);
	return (d.flags & MON13_DATE_IS_LEAP_YEAR) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res is_leap_year_gregorian_not(struct theft* t, void* test_input) {
	int32_t* input = test_input;
	struct mon13_date d = {.year = *input-1, .month = 1, .day = 1};
	d = mon13_add(d, 0, MON13_ADD_NONE, NULL);
	return (d.flags & MON13_DATE_IS_LEAP_YEAR) ? THEFT_TRIAL_FAIL : THEFT_TRIAL_PASS;
}

enum theft_trial_res convert_leap_tranquility(struct theft* t, void* test_input) {
	int32_t* input = test_input;
	struct mon13_date d_in = {.year = *input, .month = 2, .day = 29};
	struct mon13_date d_out = mon13_convert(NULL, &mon13_tranquility, d_in);
	
	int32_t tq_year = mon13_tranquility.era_start_gregorian.year;
	if(d_in.year < tq_year && d_out.year != (d_in.year - tq_year - 1)) {
		return THEFT_TRIAL_FAIL;
	}
	else if(d_in.year > tq_year && d_out.year != (d_in.year - tq_year)) {
		return THEFT_TRIAL_FAIL;
	}
	else if(d_in.year == tq_year) {
		return THEFT_TRIAL_SKIP;
	}

	for(int i = 0; i < mon13_tranquility.intercalary_day_count; i++) {
		struct mon13_intercalary* ic = &(mon13_tranquility.intercalary_days[i]);
		if(ic->flags & MON13_IC_LEAP) {
			if(d_out.day == ic->day && d_out.month == ic->month) {
				return THEFT_TRIAL_PASS;
			}
		}
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res convert_copies_input(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	struct mon13_date d_out = mon13_convert(input->c, input->c, input->d);
	if(mon13_compare(&(input->d), &(d_out), input->c) == 0) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res convert_era_start(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	if(input->c == NULL) {
		input->c = &mon13_tranquility;
	}
	struct mon13_date d_out = mon13_convert(input->c, NULL, input->c->era_start_gregorian);
	if(mon13_compare(&(input->c->era_start_gregorian), &(d_out), input->c) == 0) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res convert_round_trip(struct theft* t, void* test_input) {
	struct test_2c1d* input = test_input;
	struct mon13_date tmp = mon13_convert(input->c0, input->c1, input->d);
	struct mon13_date d_out = mon13_convert(input->c1, input->c0, tmp);
	if(mon13_compare(&(input->d), &(d_out), input->c0) == 0) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res compare_return_zero(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	if(mon13_compare(&(input->d), &(input->d), input->c) == 0) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res compare_greater_year(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	struct mon13_date greater = input->d;
	greater.year++;
	if(mon13_compare(&(greater), &(input->d), input->c) > 0) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res compare_lesser_year(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	struct mon13_date lesser = input->d;
	lesser.year--;
	if(mon13_compare(&(lesser), &(input->d), input->c) < 0) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res compare_intercalary(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	if(input->c == NULL) {
		return THEFT_TRIAL_SKIP;
	}
	struct mon13_date tmp = mon13_add(input->d, 0, MON13_ADD_NONE, input->c);
	bool is_leap = (tmp.flags & MON13_DATE_IS_LEAP_YEAR);

	for(int i = 0; i < input->c->intercalary_day_count; i++) {
		struct mon13_intercalary ic = input->c->intercalary_days[i];
		if(input->d.month == ic.month && input->d.day == ic.day) {
			continue;
		}
		if(input->d.month == ic.before_month && input->d.day == ic.before_day) {
			continue;
		}
		if(ic.flags & MON13_IC_ERA_START) {
			continue;
		}
		if((ic.flags & MON13_IC_LEAP) && !is_leap) {
			continue;
		}
		int32_t icd_y = (input->d.year == 0) ? 1 : input->d.year;

		struct mon13_date icd0 = {
			.year=icd_y, .month=ic.month, .day=ic.day
		};
		struct mon13_date icd1 = {
			.year=icd_y, .month=ic.before_month, .day=ic.before_day
		};
		int cmp0 = mon13_compare(&icd0, &(input->d), input->c);
		int cmp1 = mon13_compare(&icd1, &(input->d), input->c);
		if(cmp0 != cmp1) {
			return THEFT_TRIAL_FAIL;
		}
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res compare_reverse_arg(struct theft* t, void* test_input) {
	struct test_1c2d* input = test_input;
	int cmp0 = mon13_compare(&(input->d0), &(input->d1), input->c);
	int cmp1 = mon13_compare(&(input->d1), &(input->d0), input->c);
	if(cmp0 > 0 && cmp1 < 0) {
		return THEFT_TRIAL_PASS;
	}
	else if(cmp0 < 0 && cmp1 > 0) {
		return THEFT_TRIAL_PASS;
	}
	else if(cmp0 == 0 && cmp1 == 0) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

#define QSORT_F(fname, caddr) \
	int fname (const void* d0, const void* d1) { \
		return mon13_compare( \
			(const struct mon13_date*)d0, \
			(const struct mon13_date*)d1, \
			caddr \
		); \
	}

QSORT_F(compare_qsort_tq, &(mon13_tranquility))
QSORT_F(compare_qsort_if, &(mon13_international_fixed))
QSORT_F(compare_qsort_ps, &(mon13_positivist))
QSORT_F(compare_qsort_gr, NULL)

#define RUN_QSORT_F(c, d, fname, caddr) \
	if(c == caddr) { \
		qsort(d, TEST_QSORT_LEN, sizeof(struct mon13_date), fname); \
	}

enum theft_trial_res compare_qsort_result(struct test_qsort* input) {
	const struct mon13_date d0 = input->d[input->i0];
	const struct mon13_date d1 = input->d[input->i1];
	const struct mon13_date d2 = input->d[input->i2];
	if(d1.year < d0.year || d1.year > d2.year) {
		return THEFT_TRIAL_FAIL;
	}
	if(input->fix_year && (d1.month < d0.month || d1.month > d2.month)) {
		return THEFT_TRIAL_FAIL;
	}
	if(input->fix_year && input->fix_month && (d1.day < d0.day || d1.day > d2.day)) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res compare_qsort(struct theft* t, void* test_input) {
	struct test_qsort* input = test_input;
	RUN_QSORT_F(input->c, input->d, compare_qsort_tq, &(mon13_tranquility))
	RUN_QSORT_F(input->c, input->d, compare_qsort_if, &(mon13_international_fixed))
	RUN_QSORT_F(input->c, input->d, compare_qsort_ps, &(mon13_positivist))
	RUN_QSORT_F(input->c, input->d, compare_qsort_gr, NULL)
	return compare_qsort_result(input);
}

#if defined(__GLIBC__)
enum theft_trial_res compare_qsort_r(struct theft* t, void* test_input) {
	struct test_qsort* input = test_input;
	qsort_r(
		input->d, TEST_QSORT_LEN, sizeof(struct mon13_date),
		mon13_compare, input->c
	);
	return compare_qsort_result(input);
}
#endif

enum theft_trial_res get_weekday_start_month(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	input->d.day = 1;
	struct mon13_date tmp = mon13_add(input->d, 0, MON13_ADD_NONE, input->c);
	int weekday = tmp.weekday;
	if((input->c == NULL || input->d.month == 0) && weekday < 0) {
		return THEFT_TRIAL_PASS;
	}
	else if(input->c != NULL && weekday == 0) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res get_weekday_end_month(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	input->d.day = MON13_DAY_PER_MONTH;
	struct mon13_date tmp = mon13_add(input->d, 0, MON13_ADD_NONE, input->c);
	int weekday = tmp.weekday;
	if((input->c == NULL || input->d.month == 0) && weekday < 0) {
		return THEFT_TRIAL_PASS;
	}
	else if(input->c != NULL && weekday == (MON13_DAY_PER_WEEK - 1)) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res get_weekday_perennial(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	struct mon13_date tmp = mon13_add(input->d, 0, MON13_ADD_NONE, input->c);
	int weekday0 = tmp.weekday;
	
	bool leap = (input->c != NULL && (tmp.flags & MON13_DATE_IS_LEAP_YEAR));
	tmp = mon13_add(input->d, (tmp.day + tmp.month), MON13_ADD_YEARS, input->c);
	if(leap && weekday0 < 0) {
		while(!(tmp.flags & MON13_DATE_IS_LEAP_YEAR)) {
			tmp = mon13_add(tmp, 1, MON13_ADD_YEARS, input->c);
		}
	}

	int weekday1 = tmp.weekday;
	return (weekday0 == weekday1) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res get_weekday_range(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	struct mon13_date tmp = mon13_add(input->d, 0, MON13_ADD_NONE, input->c);
	int weekday = tmp.weekday;
	if(input->c == NULL || input->d.month == 0) {
		return (weekday < 0) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
	}
	else if(weekday >= 0 && weekday < MON13_DAY_PER_WEEK) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_year(struct theft* t, void* test_input) {
	struct test_add* input = test_input;
	struct mon13_date res = mon13_add(input->d, input->offset, MON13_ADD_YEARS, input->c);
	int32_t expected = input->d.year + input->offset;
 	//Year Zero boundary
	//test_add should not generate d.year == 0, but may generate input->offset == 0
	if(input->d.year < 0 && input->offset > 0 && (-input->d.year) <= input->offset) {
		expected++;
	}
	else if(input->d.year > 0 && input->offset < 0 && input->d.year <= (-input->offset)) {
		expected--;
	}
	return (res.year == expected) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_month_g(struct test_add* input, struct mon13_date res) {
	if(res.month < 1 || res.month > MON13_GREGORIAN_MONTH_PER_YEAR) {
		return THEFT_TRIAL_FAIL;
	}

	if((input->d.day <= 28) && (res.day != input->d.day)) {
		return THEFT_TRIAL_FAIL;
	}
	if((input->d.day > 28) && (res.month != 2) && (res.day <= 28)) {
		return THEFT_TRIAL_FAIL;
	}

	int yearDiff = res.year - input->d.year;
	int monthDiff = res.month - input->d.month;
	if((yearDiff*MON13_GREGORIAN_MONTH_PER_YEAR + monthDiff) == input->offset) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_month_simple(struct test_add* input, struct mon13_date res) {
	if(res.day != input->d.day) {
		return THEFT_TRIAL_FAIL;
	}
	if(res.month < 1 || res.month > MON13_MONTH_PER_YEAR) {
		return THEFT_TRIAL_FAIL;
	}

	int yearDiff = res.year - input->d.year;
	int monthDiff = res.month - input->d.month;
	if((yearDiff*MON13_MONTH_PER_YEAR + monthDiff) == input->offset) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_month_ic(struct test_add* input, struct mon13_date res) {
	int8_t start_month = input->d.month;
	int8_t start_day = input->d.day;
	while(start_month < 1 || start_day > MON13_DAY_PER_MONTH) {
		for(int i = 0; i < input->c->intercalary_day_count; i++) {
			struct mon13_intercalary ic = input->c->intercalary_days[i];
			if(input->d.year == 0 && !(ic.flags & MON13_IC_ERA_START)) {
				continue;
			}
			if(ic.month == start_month && ic.day == start_day) {
				start_month = ic.before_month;
				start_day = ic.before_day;
			}
		}
	}
	struct test_add new_add = {
		.c = input->c,
		.d = {.year=input->d.year, .month=start_month, .day=start_day},
		.offset = input->offset,
		.flip = input->flip
	};
	return add_month_simple(&new_add, res);
}

enum theft_trial_res add_month(struct theft* t, void* test_input) {
	struct test_add* input = test_input;
	struct mon13_date res = mon13_add(input->d, input->offset, MON13_ADD_MONTHS, input->c);
	if(input->c == NULL) {
		return add_month_g(input, res);
	}
	else if(input->d.month >= 1 && input->d.day <= MON13_DAY_PER_MONTH) {
		return add_month_simple(input, res);
	}
	else {
		return add_month_ic(input, res);
	}
}

enum theft_trial_res add_day_loop(struct theft* t, void* test_input) {
	struct test_add* input = test_input;
	int32_t mini_offset = (input->offset < 0) ? -1 : 1;
	input->offset = input->offset % 1000; //Don't want to loop too long!!
	struct mon13_date res0 = mon13_add(input->d, input->offset, MON13_ADD_DAYS, input->c);
	struct mon13_date res1 = input->d;
	for(int i = 0; i != input->offset; i += mini_offset) {
		res1 = mon13_add(res1, mini_offset, MON13_ADD_DAYS, input->c);
	}
	if(mon13_compare(&res0, &res1, input->c) == 0) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_day_round_trip(struct theft* t, void* test_input) {
	struct test_add* input = test_input;
	struct mon13_date tmp = mon13_add(input->d, input->offset, MON13_ADD_DAYS, input->c);
	struct mon13_date res = mon13_add(tmp, -(input->offset), MON13_ADD_DAYS, input->c);
	if(mon13_compare(&res, &(input->d), input->c) == 0) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_day_1_g(struct test_1c1d* input, struct mon13_date res) {
	if(res.month == input->d.month) {
		if(res.year != input->d.year) {
			return THEFT_TRIAL_FAIL;
		}
		if(res.day != (input->d.day + 1) || res.day > 31 || input->d.day > 30) {
			return THEFT_TRIAL_FAIL;
		}
		return THEFT_TRIAL_PASS;
	}
	else if(res.year == input->d.year) {
		if(res.month != (input->d.month + 1)) {
			return THEFT_TRIAL_FAIL;
		}
		if(res.day != 1) {
			return THEFT_TRIAL_FAIL;
		}
		return THEFT_TRIAL_PASS;
	}
	else {
		if(res.year != (input->d.year + 1) && res.year != 1) {
			return THEFT_TRIAL_FAIL;
		}
		else if(res.month != 1 || input->d.month != MON13_GREGORIAN_MONTH_PER_YEAR) {
			return THEFT_TRIAL_FAIL;
		}
		else if(res.day != 1 || input->d.day != 31) {
			return THEFT_TRIAL_FAIL;
		}
		return THEFT_TRIAL_PASS;
	}
}

enum theft_trial_res add_day_1_simple(struct test_1c1d* input, struct mon13_date res, bool skip) {
	if(!skip) {
		for(int i = 0; i < input->c->intercalary_day_count; i++) {
			struct mon13_intercalary ic = input->c->intercalary_days[i];
			if(input->d.year != -1 && (ic.flags & MON13_IC_ERA_START)) {
				continue;
			}
			if(!(res.flags & MON13_DATE_IS_LEAP_YEAR) && (ic.flags & MON13_IC_LEAP)){
				continue;
			}
			if(input->d.day == ic.before_day && input->d.month == ic.before_month) {
				if(res.month != ic.month || res.day != ic.day) {
					return THEFT_TRIAL_FAIL;
				}
				if(ic.flags & MON13_IC_YEAR_START) {
					if(res.year != (input->d.year + 1)) {
						return THEFT_TRIAL_FAIL;
					}
				}
				else if(res.year != input->d.year){
					return THEFT_TRIAL_FAIL;
				}
				return THEFT_TRIAL_PASS;
			}
		}
	}

	if(res.month == input->d.month) {
		if(res.year != input->d.year) {
			return THEFT_TRIAL_FAIL;
		}
		else if((res.day != (input->d.day + 1)) || res.day > MON13_DAY_PER_MONTH) {
			return THEFT_TRIAL_FAIL;
		}
		return THEFT_TRIAL_PASS;
	}
	else if(res.year == input->d.year) {
		if(res.month >= MON13_MONTH_PER_YEAR) {
			return THEFT_TRIAL_FAIL;
		}
		else if(res.day < MON13_DAY_PER_MONTH) {
			return THEFT_TRIAL_FAIL;
		}
		return THEFT_TRIAL_PASS;
	}
	else {
		if(res.year != (input->d.year + 1) && res.year != 1) {
			return THEFT_TRIAL_FAIL;
		}
		else if(res.month != 1 || input->d.month != MON13_MONTH_PER_YEAR) {
			return THEFT_TRIAL_FAIL;
		}
		else if(res.day != 1 || input->d.day != MON13_DAY_PER_MONTH) {
			return THEFT_TRIAL_FAIL;
		}
		return THEFT_TRIAL_PASS;
	}
}

enum theft_trial_res add_day_1_ic(struct test_1c1d* input, struct mon13_date res) {
	if(input->c == &mon13_positivist) { //Two intercalary days in a row on leap year.
		struct mon13_date norm_d = mon13_add(input->d, 0, MON13_ADD_NONE, input->c);
		if(norm_d.flags & MON13_DATE_IS_LEAP_YEAR) {
			if(input->d.day == 1) {
				if(res.day != 2 || res.month != 0 || res.year != input->d.year) {
					return THEFT_TRIAL_FAIL;
				}
				else {
					return THEFT_TRIAL_PASS;
				}
			}
			if(input->d.day == 2) {
				if(res.day != 1 || res.month != 1) {
					return THEFT_TRIAL_FAIL;
				}
				else if(res.year != (input->d.year + 1) && res.year != 1) {
					return THEFT_TRIAL_FAIL;
				}
				else {
					return THEFT_TRIAL_PASS;
				}
			}
		}
	}


	int32_t before_year = (input->d.year == 0) ? -1 : input->d.year;
	int8_t before_day, before_month;
	for(int i = 0; i < input->c->intercalary_day_count; i++) {
		struct mon13_intercalary ic = input->c->intercalary_days[i];
		if(input->d.year == 0 && !(ic.flags & MON13_IC_ERA_START)) {
			continue;
		}
		if(input->d.day == ic.day && input->d.month == ic.month) {
			before_day = ic.before_day;
			before_month = ic.before_month;
			if(ic.flags & MON13_IC_YEAR_START) {
				before_year--;
			}
		}
	}
	struct test_1c1d new_1c1d = {
		.c = input->c,
		.d = {.day = before_day, .month = before_month, .year = before_year}
	};
	return add_day_1_simple(&new_1c1d, res, true);
}

enum theft_trial_res add_day_1(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	struct mon13_date res = mon13_add(input->d, 1, MON13_ADD_DAYS, input->c);
	if(input->c == NULL) {
		return add_day_1_g(input, res);
	}
	else if(input->d.month >= 1 && input->d.day <= MON13_DAY_PER_MONTH) {
		return add_day_1_simple(input, res, false);
	}
	else {
		return add_day_1_ic(input, res);
	}
}

struct theft_type_info random_leap_year_info = {
	.alloc = alloc_leap_year,
	.free = theft_generic_free_cb,
	.print = print_leap_year,
	.autoshrink_config = {
		.enable = true
	}
};

struct theft_type_info random_1c1d_info = {
	.alloc = alloc_1c1d,
	.free = theft_generic_free_cb,
	.print = print_1c1d,
	.autoshrink_config = {
		.enable = true
	}
};

struct theft_type_info random_1c2d_info = {
	.alloc = alloc_1c2d,
	.free = theft_generic_free_cb,
	.print = print_1c2d,
	.autoshrink_config = {
		.enable = true
	}
};

struct theft_type_info random_2c1d_info = {
	.alloc = alloc_2c1d,
	.free = theft_generic_free_cb,
	.print = print_2c1d,
	.autoshrink_config = {
		.enable = true
	}
};

struct theft_type_info random_add_info = {
	.alloc = alloc_add,
	.free = theft_generic_free_cb,
	.print = print_add,
	.autoshrink_config = {
		.enable = true
	}
};

struct theft_type_info random_qsort_info = {
	.alloc = alloc_qsort,
	.free = theft_generic_free_cb,
	.autoshrink_config = {
		.enable = true
	}
};

int main() {
	theft_seed seed = theft_seed_of_time();
	struct theft_run_config config[] = {
		{
			.name = "mon13_is_leap_year: Gregorian true",
			.prop1 = is_leap_year_gregorian,
			.type_info = { &random_leap_year_info },
			.seed = seed
		},
		{
			.name = "mon13_is_leap_year: Gregorian false",
			.prop1 = is_leap_year_gregorian_not,
			.type_info = { &random_leap_year_info },
			.seed = seed
		},
		{
			.name = "mon13_convert: leap year Tranquility",
			.prop1 = convert_leap_tranquility,
			.type_info = { &random_leap_year_info },
			.seed = seed
		},
		{
			.name = "mon13_convert: copies input",
			.prop1 = convert_copies_input,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_convert: era start",
			.prop1 = convert_era_start,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_convert: round trip",
			.prop1 = convert_round_trip,
			.type_info = { &random_2c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_compare: return zero",
			.prop1 = compare_return_zero,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_compare: trivial greater year",
			.prop1 = compare_greater_year,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_compare: trivial lesser year",
			.prop1 = compare_lesser_year,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_compare: intercalary",
			.prop1 = compare_intercalary,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_compare: reverse arguments",
			.prop1 = compare_reverse_arg,
			.type_info = { &random_1c2d_info },
			.seed = seed
		},
		{
			.name = "mon13_compare: qsort",
			.prop1 = compare_qsort,
			.type_info = { &random_qsort_info },
			.seed = seed
		},
#if defined(__GLIBC__)
		{
			.name = "mon13_compare: qsort_r",
			.prop1 = compare_qsort_r,
			.type_info = { &random_qsort_info },
			.seed = seed
		},
#endif		
		{
			.name = "mon13_get_weekday: start month",
			.prop1 = get_weekday_start_month,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_get_weekday: end month",
			.prop1 = get_weekday_end_month,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_get_weekday: perennial",
			.prop1 = get_weekday_perennial,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_get_weekday: range",
			.prop1 = get_weekday_range,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_add: year",
			.prop1 = add_year,
			.type_info = { &random_add_info },
			.seed = seed
		},
		{
			.name = "mon13_add: month",
			.prop1 = add_month,
			.type_info = { &random_add_info },
			.seed = seed
		},
		{
			.name = "mon13_add: day in loop",
			.prop1 = add_day_loop,
			.type_info = { &random_add_info },
			.seed = seed
		},
		{
			.name = "mon13_add: day round trip",
			.prop1 = add_day_round_trip,
			.type_info = { &random_add_info },
			.seed = seed
		},
		{
			.name = "mon13_add: 1 day",
			.prop1 = add_day_1,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
	};
	int config_count = sizeof(config)/sizeof(struct theft_run_config);
	bool all_passed = true;
	for(int i = 0; i < config_count; i++) {
		if(theft_run(&(config[i])) != THEFT_RUN_PASS) {
			all_passed = false;
		}
	}
	return all_passed ? EXIT_SUCCESS : EXIT_FAILURE;
}