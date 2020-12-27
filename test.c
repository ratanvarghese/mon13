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
};

struct test_1c2d {
	struct mon13_cal* c;
	struct mon13_date d0;
	struct mon13_date d1;
};

struct test_2c1d {
	struct mon13_cal* c0;
	struct mon13_cal* c1;
	struct mon13_date d;
};

struct test_add {
	struct mon13_cal* c;
	struct mon13_date d0;
	struct mon13_date d1;
	bool skip;
};

#define TEST_QSORT_LEN 100
struct test_qsort {
	struct mon13_cal* c;
	struct mon13_date d[TEST_QSORT_LEN];
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
	res.year = theft_random_choice(t, INT_MAX) + 1;
	if(theft_random_choice(t, 2)) {
		res.year = -res.year;
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
			res.day = theft_random_choice(t, c->intercalary_day_count) + 1;
			if(c->intercalary_days[res.day-1].flags & MON13_IC_ERA_START) {
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
	
	*instance = res;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_add(struct theft* t, void* data, void** instance) {
	struct test_add* res = malloc(sizeof(struct test_add));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	res->c = random_cal(t);
	res->d0 = random_date(t, res->c);
	res->d1.year = theft_random_choice(t, INT_MAX);
	if(theft_random_choice(t, 2)) {
		res->d1.year = -(res->d1.year);
	}
	res->d1.month = theft_random_choice(t, INT_MAX);
	if(theft_random_choice(t, 2)) {
		res->d1.month = -(res->d1.month);
	}
	res->d1.day = theft_random_choice(t, INT_MAX);
	if(theft_random_choice(t, 2)) {
		res->d1.day = -(res->d1.day);
	}
	res->skip = (theft_random_choice(t, 2) == 0);

	*instance = res;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_qsort(struct theft* t, void* data, void** instance) {
	struct test_qsort* res = malloc(sizeof(struct test_qsort));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	res->c = random_cal(t);
	for(int i = 0; i < TEST_QSORT_LEN; i++) {
		res->d[i] = random_date(t, res->c);
	}
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
	fprintf(f, "(%d-%02d-%02d) %s", d.year, d.month, d.day, name);
}

void print_1c2d(FILE* f, const void* instance, void* env) {
	const struct test_1c2d* input = instance;
	const char* name = (input->c == NULL) ? "Gregorian" : input->c->cal_name;
	fprintf(
		f, "(%d-%02d-%02d) (%d-%02d-%02d) %s",
		input->d0.year, input->d0.month, input->d0.day,
		input->d1.year, input->d1.month, input->d1.day,
		name
	);
}

void print_2c1d(FILE* f, const void* instance, void* env) {
	const struct test_2c1d* input = instance;
	const char* name0 = (input->c0 == NULL) ? "Gregorian" : input->c0->cal_name;
	const char* name1 = (input->c1 == NULL) ? "Gregorian" : input->c1->cal_name;
	fprintf(
		f, "(%d-%02d-%02d) %s, %s",
		input->d.year, input->d.month, input->d.day,
		name0, name1
	);
}

void print_add(FILE* f, const void* instance, void* env) {
	const struct test_add* input = instance;
	const char* name = (input->c == NULL) ? "Gregorian" : input->c->cal_name;
	const char* skip = input->skip ? "true" : "false";
	fprintf(
		f, "(%d-%02d-%02d) (%d-%02d-%02d) %s, skip = %s",
		input->d0.year, input->d0.month, input->d0.day,
		input->d1.year, input->d1.month, input->d1.day,
		name, skip
	);
}

enum theft_trial_res bad_date_month(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	input->d.month += (MON13_MONTH_PER_YEAR + 1);
	if(mon13_bad_date(input->c, input->d)) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res bad_date_day(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	input->d.day += (input->c != NULL) ? MON13_DAY_PER_MONTH : 31;
	if(mon13_bad_date(input->c, input->d)) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res is_leap_year_gregorian(struct theft* t, void* test_input) {
	int32_t* input = test_input;
	return mon13_is_leap_year(NULL, *input) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res is_leap_year_gregorian_not(struct theft* t, void* test_input) {
	int32_t* input = test_input;
	*input = *input - 1;
	return mon13_is_leap_year(NULL, *input) ? THEFT_TRIAL_FAIL : THEFT_TRIAL_PASS;
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

enum theft_trial_res compare_qsort(struct theft* t, void* test_input) {
	struct test_qsort* input = test_input;
	RUN_QSORT_F(input->c, input->d, compare_qsort_tq, &(mon13_tranquility))
	RUN_QSORT_F(input->c, input->d, compare_qsort_if, &(mon13_international_fixed))
	RUN_QSORT_F(input->c, input->d, compare_qsort_ps, &(mon13_positivist))
	RUN_QSORT_F(input->c, input->d, compare_qsort_gr, NULL)

	if(input->d[0].year <= input->d[TEST_QSORT_LEN-1].year) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

#if defined(__GLIBC__)
enum theft_trial_res compare_qsort_r(struct theft* t, void* test_input) {
	struct test_qsort* input = test_input;
	qsort_r(
		input->d, TEST_QSORT_LEN, sizeof(struct mon13_date),
		mon13_compare, input->c
	);

	if(input->d[0].year <= input->d[TEST_QSORT_LEN-1].year) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}
#endif

enum theft_trial_res add_reverse_arg(struct theft* t, void* test_input) {
	struct test_add* input = test_input;
	struct mon13_date sum0 = mon13_add(input->c, input->d0, input->d1, input->skip);
	struct mon13_date sum1 = mon13_add(input->c, input->d1, input->d0, input->skip);
	if(mon13_compare(&(input->d0), &(input->d1), input->c) == 0) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_advance_weekday(struct theft* t, void* test_input) {
	struct test_add* input = test_input;
	input->d1.year = 0;
	input->d1.month = 0;
	input->d1.day = (input->d1.day % MON13_DAY_PER_WEEK);
	struct mon13_date sum = mon13_add(input->c, input->d0, input->d1, true);
	int weekday0 = mon13_get_weekday(input->c, input->d0);
	int weekday1 = mon13_get_weekday(input->c, sum);
	if((weekday1 - weekday0) == input->d1.day) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_advance_month(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	if(input->c == NULL || input->d.month == 0) {
		return THEFT_TRIAL_SKIP;
	}
	input->d.day = MON13_DAY_PER_MONTH;
	struct mon13_date one_day = {.year = 0, .month = 0, .day = 1};
	struct mon13_date sum = mon13_add(input->c, input->d, one_day, true);
	if(sum.day == 1) {
		if(input->d.month == MON13_MONTH_PER_YEAR && sum.month == 1) {
			return THEFT_TRIAL_PASS;
		}
		else if(sum.month == (input->d.month + 1)) {
			return THEFT_TRIAL_PASS;
		}
	}
	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res get_weekday_start_month(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	input->d.day = 1;
	int weekday = mon13_get_weekday(input->c, input->d);
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
	int weekday = mon13_get_weekday(input->c, input->d);
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
	int weekday0 = mon13_get_weekday(input->c, input->d);
	
	bool leap = (input->c != NULL && mon13_is_leap_year(input->c, input->d.year));
	input->d.year += (input->d.day + input->d.month);
	while(leap && weekday0 < 0 && !mon13_is_leap_year(input->c, input->d.year)) {
		input->d.year++;
	}

	int weekday1 = mon13_get_weekday(input->c, input->d);
	return (weekday0 == weekday1) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res get_weekday_range(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	int weekday = mon13_get_weekday(input->c, input->d);
	if(input->c == NULL || input->d.month == 0) {
		return (weekday < 0) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
	}
	else if(weekday >= 0 && weekday < MON13_DAY_PER_WEEK) {
		return THEFT_TRIAL_PASS;
	}
	return THEFT_TRIAL_FAIL;
}

struct theft_type_info random_leap_year_info = {
	.alloc = alloc_leap_year,
	.free = theft_generic_free_cb,
	.print = print_leap_year,
};

struct theft_type_info random_1c1d_info = {
	.alloc = alloc_1c1d,
	.free = theft_generic_free_cb,
	.print = print_1c1d,
};

struct theft_type_info random_1c2d_info = {
	.alloc = alloc_1c2d,
	.free = theft_generic_free_cb,
	.print = print_1c2d,
};

struct theft_type_info random_2c1d_info = {
	.alloc = alloc_2c1d,
	.free = theft_generic_free_cb,
	.print = print_2c1d,
};

struct theft_type_info random_add_info = {
	.alloc = alloc_add,
	.free = theft_generic_free_cb,
	.print = print_add,
};

struct theft_type_info random_qsort_info = {
	.alloc = alloc_qsort,
	.free = theft_generic_free_cb
};

int main() {
	theft_seed seed = theft_seed_of_time();
	struct theft_run_config config[] = {
		{
			.name = "mon13_bad_date: bad month",
			.prop1 = bad_date_month,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
		{
			.name = "mon13_bad_date: bad day",
			.prop1 = bad_date_day,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
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
			.name = "mon13_add: reverse arguments",
			.prop1 = add_reverse_arg,
			.type_info = { &random_add_info },
			.seed = seed
		},
		{
			.name = "mon13_add: advance weekday",
			.prop1 = add_advance_weekday,
			.type_info = { &random_add_info },
			.seed = seed
		},
		{
			.name = "mon13_add: advance month",
			.prop1 = add_advance_month,
			.type_info = { &random_1c1d_info },
			.seed = seed
		},
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