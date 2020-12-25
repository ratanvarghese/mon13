#include <stdlib.h>
#include <stdio.h>
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

static struct mon13_cal* random_cal(struct theft* t) {
	switch(theft_random_choice(t, 5)) {
		case 0: return &mon13_gregorian;
		case 1: return &mon13_tranquility;
		case 2: return &mon13_international_fixed;
		case 3: return &mon13_positivist;
		case 4: return &mon13_world;
		default: return NULL;
	}
}

static struct mon13_date random_date(struct theft* t, struct mon13_cal* c) {
	struct mon13_date res;
	int month_max = 0;
	while(c->month_length[month_max] > 0 && month_max < 14) {
		month_max++;
	}

	res.year = theft_random_choice(t, INT_MAX);
	res.month = theft_random_choice(t, month_max-1)+1;
	res.day = theft_random_choice(t, c->month_length[(res.month)-1])+1;
	return res;
}

enum theft_alloc_res alloc_1c1d(struct theft* t, void* data, void** instance) {
	struct test_1c1d* res = malloc(sizeof(struct test_1c1d));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	res->c = random_cal(t);
	if(res->c == NULL) {
		return THEFT_ALLOC_ERROR;
	}
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
	if(res->c == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	res->d0 = random_date(t, res->c);
	res->d1 = random_date(t, res->c);

	*instance = res;
	return THEFT_ALLOC_OK;
}

void print_1c1d(FILE* f, const void* instance, void* env) {
	const struct test_1c1d* input = instance;
	const struct mon13_date d = input->d;
	const char* name = input->c->cal_name;
	fprintf(f, "(%d-%d-%d) %s", d.year, d.month, d.day, name);
}

void print_1c2d(FILE* f, const void* instance, void* env) {
	const struct test_1c2d* input = instance;
	const struct mon13_date d0 = input->d0;
	const struct mon13_date d1 = input->d1;
	const char* name = input->c->cal_name;
	fprintf(
		f, "(%d-%d-%d) (%d-%d-%d) %s",
		d0.year, d0.month, d0.day,
		d1.year, d1.month, d1.day,
		name
	);
}

enum theft_trial_res convert_copies_input(struct theft* t, void* test_input) {
	struct test_1c1d* input = test_input;
	struct mon13_date d_out = mon13_convert(
		input->c, input->c, input->d
	);
	if(d_out.year != input->d.year) {
		return THEFT_TRIAL_FAIL;
	}
	else if(d_out.month != input->d.month) {
		return THEFT_TRIAL_FAIL;
	}
	else if(d_out.day != input->d.day) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
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

int main() {
	theft_seed seed = theft_seed_of_time();
	struct theft_run_config config[] = {
		{
			.name = "mon13_convert: copies input",
			.prop1 = convert_copies_input,
			.type_info = { &random_1c1d_info },
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
		}
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