#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "theft.h"
#include "mon13.h"

enum theft_alloc_res allocate_date(struct theft* t, void* data, void** instance) {
	struct mon13_date* d = malloc(sizeof(struct mon13_date));
	if(d == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	d->year = theft_random_choice(t, INT_MAX);
	d->month = (char)theft_random_choice(t, CHAR_MAX);
	d->day = (char)theft_random_choice(t, CHAR_MAX);
	*instance = d;
	return THEFT_ALLOC_OK;
}

void print_date(FILE* f, const void* instance, void* env) {
	const struct mon13_date* d = instance;
	fprintf(f, "%d-%d-%d", d->year, d->month, d->day);
}

enum theft_trial_res convert_copies_input(struct theft* t, void* test_input) {
	struct mon13_date* d_in = test_input;
	struct mon13_date d_out = mon13_convert(
		&mon13_gregorian, &mon13_gregorian, *d_in
	);
	if(d_out.year != d_in->year) {
		return THEFT_TRIAL_FAIL;
	}
	else if(d_out.month != d_in->month) {
		return THEFT_TRIAL_FAIL;
	}
	else if(d_out.day != d_in->day) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

struct theft_type_info random_date_info = {
	.alloc = allocate_date,
	.free = theft_generic_free_cb,
	.print = print_date,
};

int main() {
	theft_seed seed = theft_seed_of_time();
	struct theft_run_config config = {
		.name = __func__,
		.prop1 = convert_copies_input,
		.type_info = { &random_date_info },
		.seed = seed
	};
	return (theft_run(&config) == THEFT_RUN_PASS) ? EXIT_SUCCESS : EXIT_FAILURE;
}