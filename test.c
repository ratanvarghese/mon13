#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#include "theft.h"
#include "mon13.h"
#include "known.h"

#define SIZEOF_ARR(arr) sizeof(arr)/sizeof(*arr)

//Theft allocators
enum theft_alloc_res select_gr2tq_oa(struct theft* t, void* env, void** instance)
{
	uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2tq_oa));
	*instance = (void*)&gr2tq_oa[i];
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_gr2tq_handy(struct theft* t, void* env, void** instance)
{
	uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2tq_handy));
	*instance = (void*)&gr2tq_handy[i];
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_random(struct theft* t, void* env, void** instance)
{
	if(sizeof(uint64_t) != sizeof(void*)) {
		return THEFT_ALLOC_ERROR;
	}
	uint64_t x = theft_random_choice(t, UINT64_MAX);
	x = (env != NULL && x == 0) ? 1 : x;
	*instance = (void*)x;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_add_mode(struct theft* t, void* env, void** instance)
{
	if(sizeof(enum mon13_add_mode) > sizeof(void*)) {
		return THEFT_ALLOC_ERROR;
	}
	uint64_t x = theft_random_choice(t, (uint64_t)MON13_ADD_YEARS + 1);
	x = (env != NULL && x == MON13_ADD_NONE) ? 1 : x;
	*instance = (void*)x;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_env(struct theft* t, void* env, void** instance)
{
	*instance = env;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_year0(struct theft* t, void* env, void** instance)
{
	struct mon13_date* res = malloc(sizeof(struct mon13_date));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}

	struct known_convert_date* kcd;
	select_gr2tq_oa(t, NULL, (void**)&kcd);
	int32_t offset = theft_random_choice(t, INT32_MAX);
	if(env == kcd->c0) {
		*res = mon13_add(kcd->d0, kcd->c0, offset, MON13_ADD_DAYS);
	}
	else if(env == kcd->c1) {
		*res = mon13_add(kcd->d1, kcd->c1, offset, MON13_ADD_DAYS);
	}
	else {
		return THEFT_ALLOC_ERROR;
	}

	*instance = (void*)res;
	return THEFT_ALLOC_OK;
}

//Theft printers
void print_known(FILE* f, const void* instance, void* env)
{
	const struct known_convert_date* kcd = instance;
	fprintf(
		f, "(%d-%02d-%02d) (%d-%02d-%02d)",
		kcd->d0.year, kcd->d0.month, kcd->d0.day,
		kcd->d1.year, kcd->d1.month, kcd->d1.day
	);
}

void print_date(FILE* f, const void* instance, void* env)
{
	const struct mon13_date* d = instance;
	fprintf(f, "(%d-%02d-%02d)", d->year, d->month, d->day);
}

void print_random(FILE* f, const void* instance, void* env)
{
	uint64_t i = (uint64_t) instance;
	fprintf(f, "%u", i);
}

void print_add_mode(FILE* f, const void* instance, void* env)
{
	enum mon13_add_mode m = (enum mon13_add_mode) ((uint64_t)instance);
	char* m_str = NULL;
	switch(m) {
		case MON13_ADD_NONE:	m_str = "NONE";		break;
		case MON13_ADD_DAYS:	m_str = "DAYS";		break;
		case MON13_ADD_MONTHS:	m_str = "MONTHS";	break;
		case MON13_ADD_YEARS:	m_str = "YEARS";	break;
		default:				m_str = "INVALID";
	}
	fprintf(f, "%s", m_str);
}

//Theft trials: helpers
bool valid_tq(const struct mon13_date d) {
	if(d.month == 0) {
		return d.day == 1 || d.day == 2;
	}
	else {
		return d.month >= 1 && d.month <= 13 && d.day >= 1 && d.day <= 28;
	}
}

bool valid_gr(const struct mon13_date d) {
	uint8_t month_len[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if(d.month < 0 || d.month > 12) {
		return false;
	}
	else {
		return d.day >= 1 && d.day <= month_len[d.month - 1];
	}
}

bool year0_convert_correct(const struct mon13_date d_y0, const struct mon13_date d) {
	return (d_y0.year > 0) ? (d.year == d_y0.year) : (d.year == d_y0.year - 1);
}

bool add_1day_trivial(struct mon13_date d, struct mon13_date res) {
	bool stable_ym = (res.year == d.year) && (res.month == d.month);
	bool inc_day = (res.day == d.day + 1);
	return stable_ym && inc_day;
}

bool add_1day_month(struct mon13_date d, struct mon13_date res, bool end_of_month) {
	bool inc_month = (res.month == d.month + 1) && (res.year == d.year);
	bool start_month = res.day == 1;
	return end_of_month && inc_month && start_month;
}

bool add_1day_year(struct mon13_date d, struct mon13_date res, bool end_of_year) {
	bool inc_year = res.year == d.year + 1;
	bool start_year = (res.month == 1) && (res.day == 1);
	return end_of_year && inc_year && start_year; 
}

bool equal_year_month_day(struct mon13_date d0, struct mon13_date d1) {
	return d0.year == d1.year && d0.month == d1.month && d0.day == d1.day;
}

bool less_year_month_day(struct mon13_date d0, struct mon13_date d1) {
	//Does not consider days without months (ex. in Tranquility calendar)
	if(d0.year < d1.year) {
		return true;
	}
	else if(d0.month < d1.month) {
		return true;
	}
	else if(d0.day < d1.day) {
		return true;
	}
	return false;
}

//Theft trials: convert
enum theft_trial_res convert_known(struct theft* t, void* test_input)
{
	const struct known_convert_date* kcd = test_input;
	struct mon13_date res0 = mon13_convert(kcd->d1, kcd->c1, kcd->c0);
	struct mon13_date res1 = mon13_convert(kcd->d0, kcd->c0, kcd->c1);
	if(mon13_compare(&res0, &(kcd->d0), kcd->c0)) {
		return THEFT_TRIAL_FAIL;
	}
	if(mon13_compare(&res1, &(kcd->d1), kcd->c1)) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res convert_tq_year0(struct theft* t, void* test_input)
{
	const struct mon13_date* d = test_input;
	struct mon13_date res = mon13_convert(
		*d,
		&mon13_tranquility_year_zero,
		&mon13_tranquility
	);
	return year0_convert_correct(*d, res) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res convert_gr_year0(struct theft* t, void* test_input)
{
	const struct mon13_date* d = test_input;
	struct mon13_date res = mon13_convert(
		*d,
		&mon13_gregorian_year_zero,
		&mon13_gregorian
	);
	return year0_convert_correct(*d, res) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

//Theft trials: add
enum theft_trial_res add_1day_gr(struct theft* t, void* test_input)
{
	const struct mon13_date* d = test_input;
	const struct mon13_cal* c = &mon13_gregorian_year_zero;
	struct mon13_date res = mon13_add(*d, c, 1, MON13_ADD_DAYS);
	if(!valid_gr(res)) {
		return THEFT_TRIAL_FAIL;
	}

	bool correct_res = false;
	if(res.year != d->year) {
		bool end_of_year = (d->month == 12) && (d->day == 31);
		correct_res = add_1day_year(*d, res, end_of_year);
	}
	else if(res.month != d->month) {
		correct_res = add_1day_month(*d, res, d->day > 27);
	}
	else {
		correct_res = add_1day_trivial(*d, res);
	}
	return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_1day_tq(struct theft* t, void* test_input)
{
	const struct mon13_date* d = test_input;
	const struct mon13_cal* c = &mon13_tranquility_year_zero;
	struct mon13_date res = mon13_add(*d, c, 1, MON13_ADD_DAYS);
	if(!valid_tq(res)) {
		return THEFT_TRIAL_FAIL;
	}

	bool correct_res = false;
	if(res.year != d->year) {
		bool end_of_year = (d->month == 0) && (d->day == 1);
		correct_res = add_1day_year(*d, res, end_of_year);
	}
	else if(res.month == 0 && res.day == 1) {
		bool correct_start = (d->month == 13) && (d->day == 28);
		bool stable_ym = (res.year == d->year) && (res.month == d->month);
		correct_res = correct_start && stable_ym;

	}
	else if(res.month == 0 && res.day == 2) {
		bool correct_start = (d->month == 8) && (d->day == 27);
		bool leap = mon13_extract(res, c, MON13_IS_LEAP_YEAR);
		correct_res = (correct_start && leap); 
	}
	else if(res.month != d->month) {
		correct_res = add_1day_month(*d, res, d->day == 28);
	}
	else {
		correct_res = add_1day_trivial(*d, res);
	}
	return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_day_roundtrip(struct theft* t, void* a1, void* a2, void* a3)
{
	const struct mon13_date* d = a1;
	int32_t offset = (int32_t) ((int64_t)a2);
	const struct mon13_cal* c = a3;

	struct mon13_date res0 = mon13_add(*d, c, offset, MON13_ADD_DAYS);
	struct mon13_date res1 = mon13_add(res0, c, -offset, MON13_ADD_DAYS);
	return mon13_compare(d, &res0, c) ? THEFT_TRIAL_FAIL : THEFT_TRIAL_PASS;
}

enum theft_trial_res add_day_split(struct theft* t, void* a1, void* a2, void* a3)
{
	const struct mon13_date* d = a1;
	uint32_t offset = (uint32_t) ((uint64_t)a2);
	const struct mon13_cal* c = a3;

	struct mon13_date res0 = mon13_add(*d, c, offset, MON13_ADD_DAYS);

	struct mon13_date res1 = mon13_add(*d, c, offset/2, MON13_ADD_DAYS);
	struct mon13_date res2 = mon13_add(res1, c, offset/2, MON13_ADD_DAYS);
	struct mon13_date res3 = mon13_add(res2, c, offset%2, MON13_ADD_DAYS);
	return mon13_compare(&res0, &res3, c) ? THEFT_TRIAL_FAIL : THEFT_TRIAL_PASS;
}

enum theft_trial_res add_1month_gr(struct theft* t, void* test_input)
{
	const struct mon13_date* d = test_input;
	const struct mon13_cal* c = &mon13_gregorian_year_zero;
	struct mon13_date res = mon13_add(*d, c, 1, MON13_ADD_MONTHS);
	if(!valid_gr(res)) {
		return THEFT_TRIAL_FAIL;
	}

	bool correct_res = false;
	if(res.day != d->day && d->day < 29) {
		correct_res = false;
	}
	else if(res.year == d->year) {
		correct_res = (res.month == d->month + 1);
	}
	else {
		bool correct_start = (d->month == 12);
		bool inc_year = (res.year == d->year + 1) && (res.month == 1);
		correct_res = correct_start && inc_year;
	}
	return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_1month_tq(struct theft* t, void* test_input)
{
	const struct mon13_date* d = test_input;
	const struct mon13_cal* c = &mon13_tranquility_year_zero;
	if(d->month == 0) {
		return THEFT_TRIAL_SKIP;
	}

	struct mon13_date res = mon13_add(*d, c, 1, MON13_ADD_MONTHS);
	if(!valid_tq(res)) {
		return THEFT_TRIAL_FAIL;
	}

	bool correct_res = false;
	if(res.day != d->day) {
		correct_res = false;
	}
	else if(res.year == d->year) {
		correct_res = (res.month == d->month + 1);
	}
	else {
		bool correct_start = (d->month == 13);
		bool inc_year = (res.year == d->year + 1) && (res.month == 1);
		correct_res = correct_start && inc_year;
	}
	return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;	
}

enum theft_trial_res add_year(struct theft* t, void* a1, void* a2, void* a3)
{
	const struct mon13_date* d = a1;
	int32_t offset = (int32_t) ((int64_t)a2);
	const struct mon13_cal* c = a3;

	struct mon13_date res = mon13_add(*d, c, offset, MON13_ADD_YEARS);
	return res.year == (d->year + offset) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_zero(struct theft* t, void* a1, void* a2, void* a3)
{
	const struct mon13_date* d = a1;
	enum mon13_add_mode m = (enum mon13_add_mode) ((uint64_t)a2);
	const struct mon13_cal* c = a3;

	struct mon13_date res0 = mon13_add(*d, c, 0, m);
	struct mon13_date res1 = mon13_add(res0, c, 0, m);

	if(mon13_compare(&res0, &res1, c)) {
		return THEFT_TRIAL_FAIL;
	}
	else if(mon13_compare(d, &res0, c)) {
		return THEFT_TRIAL_FAIL;
	}
	else if(mon13_compare(d, &res1, c)) {
		return THEFT_TRIAL_FAIL;
	}
	else if(!equal_year_month_day(res0, res1)) {
		return THEFT_TRIAL_FAIL;
	}
	else if(!equal_year_month_day(res0, *d)) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

//Theft trials: compare
enum theft_trial_res compare_nearby(struct theft* t, void* a1, void* a2, void* a3, void* a4)
{
	const struct mon13_date* d = a1;
	int32_t offset = (int32_t) ((int64_t)a2);
	enum mon13_add_mode m = (enum mon13_add_mode) ((uint64_t)a3);
	const struct mon13_cal* c = a4;

	offset = offset >= 0 ? offset : -offset;

	struct mon13_date sum[5];
	sum[0] = mon13_add(*d, c, 2*(-offset), m);
	sum[1] = mon13_add(*d, c, 1*(-offset), m);
	sum[2] = mon13_add(*d, c, 0*(offset), m);
	sum[3] = mon13_add(*d, c, 1*(offset), m);
	sum[4] = mon13_add(*d, c, 2*(offset), m);

	int res[5];
	for(int i = 0; i < 5; i++) {
		res[i] = mon13_compare(d, &sum[i], c);
		if(offset == 0 && res[i] != 0) {
			return THEFT_TRIAL_FAIL;
		}
		else if(i == 2 && res[i] != 0) {
			return THEFT_TRIAL_FAIL;
		}
		else if(offset != 0 && i > 0 && res[i-1] >= res[i]) {
			return THEFT_TRIAL_FAIL;
		}
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res compare_random_gr(struct theft* t, void* a1, void* a2)
{
	const struct mon13_date* d0 = a1;
	const struct mon13_date* d1 = a2;

	int res = mon13_compare(d0, d1, &mon13_gregorian_year_zero);
	bool correct_res = false;
	if(res == 0) {
		correct_res = equal_year_month_day(*d0, *d1);
	}
	else if(res < 0) {
		correct_res = less_year_month_day(*d0, *d1);
	}
	else if(res > 0) {
		correct_res = less_year_month_day(*d1, *d0);
	}

	return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

//Theft trials: extract
enum theft_trial_res extract_is_ok(struct theft* t, void* a1, void* a2)
{
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;

	struct mon13_date sum = mon13_add(*d, c, 0, MON13_ADD_NONE);
	bool res = mon13_extract(sum, c, MON13_IS_OK);
	return res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res extract_is_leap(struct theft* t, void* a1, void* a2)
{
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;

	int leap_count = 0;
	for(int i = 1; i < 9; i++) {
		struct mon13_date sum = mon13_add(*d, c, i, MON13_ADD_YEARS);
		if(mon13_extract(sum, c, MON13_IS_LEAP_YEAR)) {
			leap_count++;
		}
	}
	bool res = (leap_count == 1) || (leap_count == 2);
	return res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res extract_day_of_week_gr(struct theft* t, void* test_input) {
	const struct mon13_date* d = test_input;
	const struct mon13_cal* c = &mon13_gregorian_year_zero;
	
	struct mon13_date sum0 = mon13_add(*d, c, 0, MON13_ADD_DAYS);
	struct mon13_date sum1 = mon13_add(*d, c, 1, MON13_ADD_DAYS);

	int dow0 = mon13_extract(sum0, c, MON13_DAY_OF_WEEK);
	int dow1 = mon13_extract(sum1, c, MON13_DAY_OF_WEEK);
	if(dow0 == 6) {
		return dow1 == 0 ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
	}
	else {
		return dow1 == (dow0 + 1) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
	}
}

enum theft_trial_res extract_day_of_week_tq(struct theft* t, void* test_input) {
	const struct mon13_date* d = test_input;
	const struct mon13_cal* c = &mon13_tranquility_year_zero;
	
	struct mon13_date sum = mon13_add(*d, c, 0, MON13_ADD_DAYS);

	int dow = mon13_extract(sum, c, MON13_DAY_OF_WEEK);
	if(sum.month == 0) {
		return dow == 0 ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
	}
	else {
		return dow == (sum.day % 7) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
	}
}

enum theft_trial_res extract_day_of_year_add_one(struct theft* t, void* a1, void* a2) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;

	struct mon13_date sum = mon13_add(*d, c, 1, MON13_ADD_DAYS);

	int doy0 = mon13_extract(*d, c, MON13_DAY_OF_YEAR);
	int doy1 = mon13_extract(sum, c, MON13_DAY_OF_YEAR);
	bool leap = mon13_extract(*d, c, MON13_IS_LEAP_YEAR);
	bool res = false;
	if(leap && doy0 == 366) {
		res = (doy1 == 1);
	}
	else if(!leap && doy0 == 365) {
		res = (doy1 == 1);
	}
	else {
		res = (doy1 == doy0 + 1) && (doy1 < 365) && (doy1 > 0);
	}
	return res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res extract_day_of_year_split(struct theft* t, void* a1, void* a2, void* a3)
{
	const struct mon13_date* d = a1;
	uint32_t offset = (uint32_t) ((uint64_t)a2);
	const struct mon13_cal* c = a3;

	struct mon13_date res0 = mon13_add(*d, c, offset, MON13_ADD_DAYS);

	struct mon13_date res1 = mon13_add(*d, c, offset/2, MON13_ADD_DAYS);
	struct mon13_date res2 = mon13_add(res1, c, offset/2, MON13_ADD_DAYS);
	struct mon13_date res3 = mon13_add(res2, c, offset%2, MON13_ADD_DAYS);

	int doy0 = mon13_extract(res0, c, MON13_DAY_OF_YEAR);
	int doy3 = mon13_extract(res3, c, MON13_DAY_OF_YEAR);

	return doy0 == doy3 ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

//Theft type info
struct theft_type_info gr2tq_oa_info = {
	.alloc = select_gr2tq_oa, //nothing to free
	.print = print_known
};

struct theft_type_info gr2tq_handy_info = {
	.alloc = select_gr2tq_handy, //nothing to free
	.print = print_known
};

struct theft_type_info random_info = {
	.alloc = select_random, //nothing to free
	.print = print_random,
	.env = NULL
};

struct theft_type_info random_nonzero_info = {
	.alloc = select_random, //nothing to free
	.print = print_random,
	.env = (void*)&random_info
};

struct theft_type_info add_mode_info = {
	.alloc = select_add_mode, //nothing to free
	.print = print_add_mode,
	.env = NULL
};

struct theft_type_info add_mode_nonzero_info = {
	.alloc = select_add_mode, //nothing to free
	.print = print_add_mode,
	.env = (void*)&add_mode_info
};

struct theft_type_info gr_year0_cal_info = {
	.alloc = select_env, //nothing to free
	.env = (void*)&mon13_gregorian_year_zero
};

struct theft_type_info tq_year0_cal_info = {
	.alloc = select_env, //nothing to free
	.env = (void*)&mon13_tranquility_year_zero
};

struct theft_type_info gr_year0_info = {
	.alloc = alloc_year0,
	.free = theft_generic_free_cb,
	.print = print_date,
	.env = (void*)&mon13_gregorian_year_zero
};

struct theft_type_info tq_year0_info = {
	.alloc = alloc_year0,
	.free = theft_generic_free_cb,
	.print = print_date,
	.env = (void*)&mon13_tranquility_year_zero
};

int main() {
	theft_seed seed = theft_seed_of_time();
	struct theft_run_config config[] = {
		{
			.name = "mon13_convert: Gregorian<->Tranquility (OA)",
			.prop1 = convert_known,
			.type_info = &gr2tq_oa_info,
			.seed = seed,
			.trials = SIZEOF_ARR(gr2tq_oa)
		},
		{
			.name = "mon13_convert: Gregorian<->Tranquility (Handy)",
			.prop1 = convert_known,
			.type_info = &gr2tq_handy_info,
			.seed = seed,
			.trials = SIZEOF_ARR(gr2tq_handy)
		},
		{
			.name = "mon13_convert: Gregorian Year 0",
			.prop1 = convert_gr_year0,
			.type_info = &gr_year0_info,
			.seed = seed
		},
		{
			.name = "mon13_convert: Tranquility Year 0",
			.prop1 = convert_tq_year0,
			.type_info = &tq_year0_info,
			.seed = seed
		},
		{
			.name = "mon13_add: 1 Day, Gregorian Year 0",
			.prop1 = add_1day_gr,
			.type_info = &gr_year0_info,
			.seed = seed
		},
		{
			.name = "mon13_add: 1 Day, Tranquility Year 0",
			.prop1 = add_1day_tq,
			.type_info = &tq_year0_info,
			.seed = seed
		},
		{
			.name = "mon13_add: Day Round Trip, Gregorian Year 0",
			.prop3 = add_day_roundtrip,
			.type_info = {
				&gr_year0_info,
				&random_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Day Round Trip, Tranquility Year 0",
			.prop3 = add_day_roundtrip,
			.type_info = {
				&tq_year0_info,
				&random_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Day Split, Gregorian Year 0",
			.prop3 = add_day_split,
			.type_info = {
				&gr_year0_info,
				&random_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Day Split, Tranquility Year 0",
			.prop3 = add_day_split,
			.type_info = {
				&tq_year0_info,
				&random_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: 1 Month, Gregorian Year 0",
			.prop1 = add_1month_gr,
			.type_info = &gr_year0_info,
			.seed = seed
		},
		{
			.name = "mon13_add: 1 Month, Tranquility Year 0",
			.prop1 = add_1month_tq,
			.type_info = &tq_year0_info,
			.seed = seed
		},
		{
			.name = "mon13_add: Year, Gregorian Year 0",
			.prop3 = add_year,
			.type_info = {
				&gr_year0_info,
				&random_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Year, Tranquility Year 0",
			.prop3 = add_year,
			.type_info = {
				&tq_year0_info,
				&random_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Zero, Gregorian Year 0",
			.prop3 = add_zero,
			.type_info = {
				&gr_year0_info,
				&add_mode_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Zero, Tranquility Year 0",
			.prop3 = add_zero,
			.type_info = {
				&tq_year0_info,
				&add_mode_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_compare: Nearby, Gregorian Year 0",
			.prop4 = compare_nearby,
			.type_info = {
				&gr_year0_info,
				&random_info,
				&add_mode_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_compare: Nearby, Tranquility Year 0",
			.prop4 = compare_nearby,
			.type_info = {
				&tq_year0_info,
				&random_nonzero_info,
				&add_mode_nonzero_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_compare: Random, Gregorian Year 0",
			.prop2 = compare_random_gr,
			.type_info = {
				&gr_year0_info,
				&gr_year0_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: IS_OK, Gregorian Year 0",
			.prop2 = extract_is_ok,
			.type_info = {
				&gr_year0_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: IS_OK, Tranquility Year 0",
			.prop2 = extract_is_ok,
			.type_info = {
				&tq_year0_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: IS_LEAP_YEAR, Gregorian Year 0",
			.prop2 = extract_is_leap,
			.type_info = {
				&gr_year0_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: IS_LEAP_YEAR, Tranquility Year 0",
			.prop2 = extract_is_leap,
			.type_info = {
				&tq_year0_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_WEEK, Gregorian Year 0",
			.prop1 = extract_day_of_week_gr,
			.type_info = &gr_year0_info,
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_WEEK, Tranquility Year 0",
			.prop1 = extract_day_of_week_tq,
			.type_info = &tq_year0_info,
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_YEAR, Gregorian Year 0",
			.prop2 = extract_day_of_year_add_one,
			.type_info = {
				&gr_year0_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_YEAR Plus 1, Tranquility Year 0",
			.prop2 = extract_day_of_year_add_one,
			.type_info = {
				&tq_year0_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_YEAR Split, Gregorian Year 0",
			.prop3 = extract_day_of_year_split,
			.type_info = {
				&gr_year0_info,
				&random_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_YEAR Split, Tranquility Year 0",
			.prop3 = extract_day_of_year_split,
			.type_info = {
				&tq_year0_info,
				&random_info,
				&tq_year0_cal_info
			},
			.seed = seed
		}
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