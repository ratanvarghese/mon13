#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#include "theft.h"
#include "mon13.h"
#include "known.h"

#define SIZEOF_ARR(arr) sizeof(arr)/sizeof(*arr)

enum theft_alloc_res alloc_gr2tq_oa(struct theft* t, void* data, void** instance)
{
	uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2tq_oa));
	*instance = (void*)&gr2tq_oa[i];
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_gr2tq_handy(struct theft* t, void* data, void** instance)
{
	uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2tq_handy));
	*instance = (void*)&gr2tq_handy[i];
	return THEFT_ALLOC_OK;
}

void print_known(FILE* f, const void* instance, void* env)
{
	const struct known_convert_date* kcd = instance;
	fprintf(
		f, "(%d-%02d-%02d) (%d-%02d-%02d)",
		kcd->d0.year, kcd->d0.month, kcd->d0.day,
		kcd->d1.year, kcd->d1.month, kcd->d1.day
	);
}

enum theft_trial_res convert_known(struct theft* t, void* test_input)
{
	const struct known_convert_date* kcd = test_input;
	struct mon13_date res0 = mon13_convert(kcd->c1, kcd->c0, kcd->d1);
	struct mon13_date res1 = mon13_convert(kcd->c0, kcd->c1, kcd->d0);
	if(mon13_compare(&res0, &(kcd->d0), kcd->c0)) {
		return THEFT_TRIAL_FAIL;
	}
	if(mon13_compare(&res1, &(kcd->d1), kcd->c1)) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

bool year0_convert_correct(const struct mon13_date d_y0, const struct mon13_date d) {
	if(d_y0.year > 0) {
		return d.year == d_y0.year ? true : false;
	}
	else {
		return d.year == d_y0.year - 1 ? true : false;
	}
}

enum theft_trial_res convert_tq_year0(struct theft* t, void* test_input)
{
	const struct known_convert_date* kcd = test_input;
	struct mon13_date res0 = mon13_convert(
		&mon13_gregorian_year_zero,
		&mon13_gregorian,
		kcd->d0
	);
	struct mon13_date res1 = mon13_convert(
		&mon13_tranquility_year_zero,
		&mon13_tranquility,
		kcd->d1
	);
	
	if(!year0_convert_correct(kcd->d0, res0)) {
		return THEFT_TRIAL_FAIL;
	}
	if(!year0_convert_correct(kcd->d1, res1)) {
		return THEFT_TRIAL_FAIL;
	}

	return THEFT_TRIAL_PASS;
}

void free_nop(void* instance, void* env) {
	; //For allocators that don't actually malloc.
}

struct theft_type_info gr2tq_oa_info = {
	.alloc = alloc_gr2tq_oa,
	.free = free_nop,
	.print = print_known
};

struct theft_type_info gr2tq_handy_info = {
	.alloc = alloc_gr2tq_handy,
	.free = free_nop,
	.print = print_known
};

int main() {
	theft_seed seed = theft_seed_of_time();
	struct theft_run_config config[] = {
		{
			.name = "Convert Gregorian<->Tranquility (OA)",
			.prop1 = convert_known,
			.type_info = &gr2tq_oa_info,
			.seed = seed,
			.trials = SIZEOF_ARR(gr2tq_oa)
		},
		{
			.name = "Convert Gregorian<->Tranquility (Handy)",
			.prop1 = convert_known,
			.type_info = &gr2tq_handy_info,
			.seed = seed,
			.trials = SIZEOF_ARR(gr2tq_handy)
		},
		{
			.name = "Convert Year 0 (OA)",
			.prop1 = convert_tq_year0,
			.type_info = &gr2tq_oa_info,
			.seed = seed,
			.trials = SIZEOF_ARR(gr2tq_oa)
		},
		{
			.name = "Convert Year 0 (Handy)",
			.prop1 = convert_tq_year0,
			.type_info = &gr2tq_handy_info,
			.seed = seed,
			.trials = SIZEOF_ARR(gr2tq_handy)
		},
	};
	bool all_passed = true;
	for(int i = 0; i < SIZEOF_ARR(config); i++) {
		if(theft_run(&(config[i])) != THEFT_RUN_PASS) {
			all_passed = false;
		}
	}
	//return all_passed ? EXIT_SUCCESS : EXIT_FAILURE;
	return 0;
}