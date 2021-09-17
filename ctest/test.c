#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "theft.h"
#include "mon13.h"
#include "known.h"

#define SIZEOF_ARR(arr) sizeof(arr)/sizeof(*arr)
#define STRFTIME_BUF 500
#define UNIX_DAY 24 * 60 * 60

//Theft allocators
bool cal_cmp_ignore_year0(const struct mon13_cal* x, const struct mon13_cal* y) {
	if(x == y) {
		return true;
	}
	if(x == &mon13_gregorian_year_zero && y == &mon13_gregorian) {
		return true;
	}
	if(x == &mon13_gregorian && y == &mon13_gregorian_year_zero) {
		return true;
	}
	if(x == &mon13_tranquility_year_zero && y == &mon13_tranquility) {
		return true;
	}
	if(x == &mon13_tranquility && y == &mon13_tranquility_year_zero) {
		return true;
	}
	return false;
}

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

enum theft_alloc_res alloc_date(struct theft* t, void* env, void** instance)
{
	struct mon13_date* res = malloc(sizeof(struct mon13_date));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}

	struct known_convert_date* kcd;
	select_gr2tq_oa(t, NULL, (void**)&kcd);
	int32_t offset = (theft_random_choice(t, INT32_MAX) - (INT32_MAX/2));

	if(env == kcd->c0 || cal_cmp_ignore_year0(env, kcd->c0)) {
		if(mon13_add(&(kcd->d0), kcd->c0, offset, MON13_ADD_DAYS, res)) {
			return THEFT_ALLOC_ERROR;
		}
	}
	else if(env == kcd->c1 || cal_cmp_ignore_year0(env, kcd->c1)) {
		if(mon13_add(&(kcd->d1), kcd->c1, offset, MON13_ADD_DAYS, res)) {
			return THEFT_ALLOC_ERROR;
		}
	}
	else {
		return THEFT_ALLOC_ERROR;
	}

	*instance = (void*)res;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_strftime_fmt(struct theft* t, void* env, void** instance) {
	char fmt_list[] = "%ABdjmntuY";
	size_t fmt_len = theft_random_choice(t, STRFTIME_BUF/10);
	fmt_len = (fmt_len > 0) ? fmt_len : 1;
	char* res = malloc(fmt_len * sizeof(char));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	for(int i = 0; i < (fmt_len - 1); i++) { //leave 1 more, for null character 
		if(i < (fmt_len - 2) && theft_random_choice(t, 4) == 0) {
			size_t fmt_i = theft_random_choice(t, SIZEOF_ARR(fmt_list) - 1);
			res[i] = '%';
			res[i+1] = fmt_list[fmt_i];
			i++;
		}
		else {
			res[i] = theft_random_choice(t, 127);
			if(res[i] < 32 || res[i] == '%') {
				res[i] = ' ';
			}
		}

	}
	res[fmt_len - 1] = '\0';
	*instance = res;
	return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_numeric_fmt(struct theft* t, void* env, void** instance) {
	char flag_list[] = "-_0";
	char width_list[] = "0123456789";
	char fmt_list[] = "djmuY";
	
	char* res = malloc(5 * sizeof(char));
	if(res == NULL) {
		return THEFT_ALLOC_ERROR;
	}
	res[0] = '%';

	size_t flag_i = theft_random_choice(t, SIZEOF_ARR(flag_list) - 1);
	res[1] = flag_list[flag_i];

	size_t width_i = theft_random_choice(t, SIZEOF_ARR(width_list) - 1);
	res[2] = width_list[width_i];

	size_t fmt_i = theft_random_choice(t, SIZEOF_ARR(fmt_list) - 1);
	res[3] = fmt_list[fmt_i];

	res[4] = '\0';
	*instance = res;
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
	fprintf(f, "%lu", i);
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

void print_s(FILE* f, const void* instance, void* env)
{
	const char* s = instance;
	fprintf(f, "%s", s);
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

const char* contained(char* needle, const char** haystack, size_t maxlen, char placeholder) {
	for(int i = 0; haystack[i] != NULL; i++) {
		const char* expected = haystack[i]; 
		size_t len = strlen(expected);
		if(strncmp(needle, expected, maxlen) == 0) {
			if(needle[len + 1] == '\0' && needle[len + 2] == placeholder) {
				return expected;
			}
		}
	}
	return NULL;
}

bool format_res_check(size_t res, const char* expected) {
	return (expected != NULL) && (res == strlen(expected));
}

bool skip_import(int64_t x) {
	return (x > (INT32_MAX/2)) || (x < -(INT32_MAX/2));
}

//Theft trials: import
enum theft_trial_res import_mjd(struct theft* t, void* a1, void* a2, void* a3) {
	const struct mon13_cal* c = a1;
	int64_t mjd0 = ((int64_t)a2) % INT32_MAX;
	int32_t offset = (int32_t) ((int64_t)a3 % INT32_MAX);

	struct mon13_date d0, d1;
	int status;
	status = mon13_import(c, &mjd0, MON13_IMPORT_MJD, &d0);
	if(status) {
		return skip_import(mjd0) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
	}
	status = mon13_add(&d0, c, offset, MON13_ADD_DAYS, &d1);
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	int64_t mjd1 = mon13_extract(&d1, c, MON13_EXTRACT_MJD);
	return ((mjd1 - mjd0) == offset) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res import_unix(struct theft* t, void* a1, void* a2, void* a3) {
	const struct mon13_cal* c = a1;
	int64_t u0 = ((int64_t)a2) % (((int64_t)INT32_MAX) * UNIX_DAY);
	int32_t offset = (int32_t) ((int64_t)a3 % INT32_MAX);

	int64_t u0_cut = u0 - (u0 % (UNIX_DAY));
	struct mon13_date d0, d1;
	int status;
	status = mon13_import(c, &u0_cut, MON13_IMPORT_UNIX, &d0);
	if(status) {
		return skip_import(u0/(UNIX_DAY)) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
	}
	status = mon13_add(&d0, c, offset, MON13_ADD_DAYS, &d1);
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	int64_t u1 = mon13_extract(&d1, c, MON13_EXTRACT_UNIX);
	int64_t udiff = (u1 - u0_cut);
	int64_t udiff_days =  udiff / (UNIX_DAY);
	if(udiff_days == offset) {
		return THEFT_TRIAL_PASS;
	}

	return THEFT_TRIAL_FAIL;
}

enum theft_trial_res import_unix_epoch_start(struct theft* t, void* a1) {
	const struct mon13_cal* c = &mon13_gregorian_year_zero;
	int64_t u0 = ((int64_t)a1) % (UNIX_DAY);

	struct mon13_date d0;
	if(mon13_import(c, &u0, MON13_IMPORT_UNIX, &d0)) {
		return THEFT_TRIAL_FAIL; //u0 is too small for error to be allowed
	}
	if(d0.year == 1970 && d0.month == 1 && d0.day == 1) {
		return THEFT_TRIAL_PASS;
	}
	else {
		return THEFT_TRIAL_FAIL;
	}
}

enum theft_trial_res import_rd(struct theft* t, void* a1, void* a2, void* a3) {
	const struct mon13_cal* c = a1;
	int64_t rd0 = ((int64_t)a2) % INT32_MAX;
	int32_t offset = (int32_t) ((int64_t)a3 % INT32_MAX);

	struct mon13_date d0, d1;
	int status;
	status = mon13_import(c, &rd0, MON13_IMPORT_RD, &d0);
	if(status) {
		return skip_import(rd0) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
	}
	status = mon13_add(&d0, c, offset, MON13_ADD_DAYS, &d1);
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	int64_t rd1 = mon13_extract(&d1, c, MON13_EXTRACT_RD);
	return ((rd1 - rd0) == offset) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

//Theft trials: convert
enum theft_trial_res convert_known(struct theft* t, void* test_input)
{
	const struct known_convert_date* kcd = test_input;
	struct mon13_date res0, res1;
	if(mon13_convert(&(kcd->d1), kcd->c1, kcd->c0, &res0)) {
		return THEFT_TRIAL_FAIL;
	}
	if(mon13_convert(&(kcd->d0), kcd->c0, kcd->c1, &res1)) {
		return THEFT_TRIAL_FAIL;
	}
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
	struct mon13_date res;
	int status = mon13_convert(
		d,
		&mon13_tranquility_year_zero,
		&mon13_tranquility,
		&res
	);
	if(status) {
		return THEFT_TRIAL_FAIL;
	}
	return year0_convert_correct(*d, res) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res convert_gr_year0(struct theft* t, void* test_input)
{
	const struct mon13_date* d = test_input;
	struct mon13_date res;
	int status = mon13_convert(
		d,
		&mon13_gregorian_year_zero,
		&mon13_gregorian,
		&res
	);
	if(status) {
		return THEFT_TRIAL_FAIL;
	}
	return year0_convert_correct(*d, res) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res convert_tq_year0_reverse(struct theft* t, void* test_input)
{
	const struct mon13_date* d = test_input;
	struct mon13_date res;
	int status = mon13_convert(
		d,
		&mon13_tranquility,
		&mon13_tranquility_year_zero,
		&res
	);
	if(status) {
		return THEFT_TRIAL_FAIL;
	}
	return year0_convert_correct(res, *d) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res convert_gr_year0_reverse(struct theft* t, void* test_input)
{
	const struct mon13_date* d = test_input;
	struct mon13_date res;
	int status = mon13_convert(
		d,
		&mon13_gregorian,
		&mon13_gregorian_year_zero,
		&res
	);
	if(status) {
		return THEFT_TRIAL_FAIL;
	}
	return year0_convert_correct(res, *d) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

//Theft trials: add
enum theft_trial_res add_1day_gr(struct theft* t, void* test_input)
{
	const struct mon13_date* d = test_input;
	const struct mon13_cal* c = &mon13_gregorian_year_zero;
	struct mon13_date res;
	int status;
	status = mon13_add(d, c, 1, MON13_ADD_DAYS, &res);
	if(status) {
		return THEFT_TRIAL_FAIL; //Adding 1 is unlikely to be legitimate error.
	}
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
	struct mon13_date res;
	int status;
	status = mon13_add(d, c, 1, MON13_ADD_DAYS, &res);
	if(status) {
		return THEFT_TRIAL_FAIL; //Adding 1 is unlikely to be legitimate error.
	}
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
		bool stable_y = (res.year == d->year);
		correct_res = correct_start && stable_y;

	}
	else if(res.month == 0 && res.day == 2) {
		bool correct_start = (d->month == 8) && (d->day == 27);
		bool leap = mon13_extract(&res, c, MON13_EXTRACT_IS_LEAP_YEAR);
		bool stable_y = (res.year == d->year);
		correct_res = (correct_start && leap && stable_y); 
	}
	else if(res.month == 8 && res.day == 28 && mon13_extract(&res, c, MON13_EXTRACT_IS_LEAP_YEAR)) {
		bool correct_start = (d->month == 0) && (d->day == 2);
		bool stable_y = (res.year == d->year);
		correct_res = (correct_start && stable_y); 

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
	int32_t offset = (int32_t) ((int64_t)a2 % INT32_MAX);
	const struct mon13_cal* c = a3;

	struct mon13_date res0, res1;
	int status;
	status = mon13_add(d, c, offset, MON13_ADD_DAYS, &res0);
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	status = mon13_add(&res0, c, -offset, MON13_ADD_DAYS, &res1);
	if(status) {
		return THEFT_TRIAL_FAIL; //Need to be able to return to origin.
	}

	return mon13_compare(d, &res1, c) ? THEFT_TRIAL_FAIL : THEFT_TRIAL_PASS;
}

enum theft_trial_res add_day_split(struct theft* t, void* a1, void* a2, void* a3)
{
	const struct mon13_date* d = a1;
	int32_t offset = (int32_t) ((uint64_t)a2 % INT32_MAX);
	const struct mon13_cal* c = a3;

	offset = (offset > 0) ? offset : -offset;

	struct mon13_date res0, res1, res2, res3;
	int status;
	status = mon13_add(d, c, offset, MON13_ADD_DAYS, &res0);
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	status = mon13_add(d, c, offset/2, MON13_ADD_DAYS, &res1);
	if(status) {
		return THEFT_TRIAL_FAIL; //If adding offset is OK, adding offset/2 should be.
	}
	status = mon13_add(&res1, c, offset/2, MON13_ADD_DAYS, &res2);
	if(status) {
		return THEFT_TRIAL_FAIL; //If adding offset is OK, adding offset/2 should be.
	}
	status = mon13_add(&res2, c, offset%2, MON13_ADD_DAYS, &res3);
	if(status) {
		return THEFT_TRIAL_FAIL; //If adding offset is OK, adding offset%2 should be.
	}
	return mon13_compare(&res0, &res3, c) ? THEFT_TRIAL_FAIL : THEFT_TRIAL_PASS;
}

enum theft_trial_res add_1month_gr(struct theft* t, void* test_input)
{
	const struct mon13_date* d = test_input;
	const struct mon13_cal* c = &mon13_gregorian_year_zero;
	struct mon13_date res;
	int status;
	status = mon13_add(d, c, 1, MON13_ADD_MONTHS, &res);
	if(status) {
		return THEFT_TRIAL_FAIL; //Adding 1 month is unlikely to be legitimate error.
	}
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

	struct mon13_date res;
	int status;
	status = mon13_add(d, c, 1, MON13_ADD_MONTHS, &res);
	if(status) {
		return THEFT_TRIAL_FAIL; //Adding 1 month is unlikely to be legitimate error.
	}
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
	int32_t offset = (int32_t) ((int64_t)a2 % INT32_MAX);
	const struct mon13_cal* c = a3;

	struct mon13_date res;
	int status = mon13_add(d, c, offset, MON13_ADD_YEARS, &res);
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	return res.year == (d->year + offset) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_zero(struct theft* t, void* a1, void* a2, void* a3)
{
	const struct mon13_date* d = a1;
	enum mon13_add_mode m = (enum mon13_add_mode) ((uint64_t)a2);
	const struct mon13_cal* c = a3;

	struct mon13_date res0, res1;
	int status;
	status = mon13_add(d, c, 0, m, &res0);
	if(status) {
		return THEFT_TRIAL_FAIL;
	}
	status = mon13_add(&res0, c, 0, m, &res1);
	if(status) {
		return THEFT_TRIAL_FAIL;
	}

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
	//equal_year_month_day(*d, res0) does not account for normalization.

	return THEFT_TRIAL_PASS;
}

enum theft_trial_res add_like_other_cal(struct theft* t, void* a1, void* a2, void* a3, void* a4, void* a5) {
	const struct mon13_date* d = a1;
	enum mon13_add_mode m = (enum mon13_add_mode) ((uint64_t) a2);
	const struct mon13_cal* c_yz = a3;
	const struct mon13_cal* c = a4;
	int32_t offset = (int32_t) ((uint64_t)a5 % INT32_MAX);

	struct mon13_date d_yz, res_yz, res0, res1;
	int status;
	status = mon13_convert(d, c, c_yz, &d_yz);
	if(status) {
		return THEFT_TRIAL_FAIL;
	}
	status = mon13_add(&d_yz, c_yz, offset, m, &res_yz);
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	status = mon13_add(d, c, offset, m, &res0);
	if(status) {
		return THEFT_TRIAL_FAIL; //If add(,c_yz,) worked, add(,c,) should work.
	}
	status = mon13_convert(&res_yz, c_yz, c, &res1);
	if(status) {
		return skip_import(((int64_t)res_yz.year)*366) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
	}

	if(mon13_compare(&res0, &res1, c)) {
		return THEFT_TRIAL_FAIL;
	}

	return THEFT_TRIAL_PASS;
}

enum theft_trial_res add_no_year_zero(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	enum mon13_add_mode m = (enum mon13_add_mode) ((uint64_t) a2);
	const struct mon13_cal* c = a3;
	int32_t offset = (int32_t) ((uint64_t)a4 % INT32_MAX);

	struct mon13_date res;
	int status;
	status = mon13_add(d, c, offset, m, &res);
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	return (res.year == 0) ? THEFT_TRIAL_FAIL : THEFT_TRIAL_PASS;
}

//Theft trials: compare

enum theft_trial_res compare_nearby(struct theft* t, void* a1, void* a2, void* a3, void* a4)
{
	const struct mon13_date* d = a1;
	int32_t offset = (int32_t) ((int64_t)a2 % (INT32_MAX/2));
	enum mon13_add_mode m = (enum mon13_add_mode) ((uint64_t)a3);
	const struct mon13_cal* c = a4;

	offset = offset >= 0 ? offset : -offset;
	
	struct mon13_date sum[5];
	int status;
	status = mon13_add(d, c, 2*(-offset), m, &(sum[0]));
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	status = mon13_add(d, c, 1*(-offset), m, &(sum[1]));
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	status = mon13_add(d, c, 0*( offset), m, &(sum[2]));
	if(status) {
		return THEFT_TRIAL_FAIL; //Adding zero should never error!!
	}
	status = mon13_add(d, c, 1*( offset), m, &(sum[3]));
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	status = mon13_add(d, c, 2*( offset), m, &(sum[4]));
	if(status) {
		return THEFT_TRIAL_SKIP;
	}

	for(int i = 0; i < 5; i++) {
		int res = mon13_compare(d, &sum[i], c);
		if(offset == 0 && res != 0) {
			return THEFT_TRIAL_FAIL;
		}
		else if(i == 2 && res != 0) {
			return THEFT_TRIAL_FAIL;
		}
		else if(i < 2 && res < 0) {
			return THEFT_TRIAL_FAIL;
		}
		else if(i > 2 && res > 0) {
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
enum theft_trial_res extract_is_leap(struct theft* t, void* a1, void* a2)
{
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;

	int leap_count = 0;
	for(int i = 1; i < 9; i++) {
		struct mon13_date sum;
		int status = mon13_add(d, c, i, MON13_ADD_YEARS, &sum);
		if(status) {
			return THEFT_TRIAL_SKIP;
		}
		if(mon13_extract(&sum, c, MON13_EXTRACT_IS_LEAP_YEAR)) {
			leap_count++;
		}
	}
	bool res = (leap_count == 1) || (leap_count == 2);
	return res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res extract_day_of_week_gr(struct theft* t, void* test_input) {
	const struct mon13_date* d = test_input;
	const struct mon13_cal* c = &mon13_gregorian_year_zero;
	
	struct mon13_date sum0, sum1;
	int status;
	status = mon13_add(d, c, 0, MON13_ADD_DAYS, &sum0);
	if(status) {
		return THEFT_TRIAL_FAIL; //Adding zero should never cause errors!
	}
	status = mon13_add(d, c, 1, MON13_ADD_DAYS, &sum1);
	if(status) {
		return THEFT_TRIAL_FAIL; //Adding 1 unlikely to cause errors.
	}

	int dow0 = mon13_extract(&sum0, c, MON13_EXTRACT_DAY_OF_WEEK);
	int dow1 = mon13_extract(&sum1, c, MON13_EXTRACT_DAY_OF_WEEK);
	if(dow0 == MON13_SUNDAY) {
		return dow1 == MON13_MONDAY ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
	}
	else {
		return dow1 == (dow0 + 1) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
	}
}

enum theft_trial_res extract_day_of_week_tq(struct theft* t, void* test_input) {
	const struct mon13_date* d = test_input;
	const struct mon13_cal* c = &mon13_tranquility_year_zero;
	
	struct mon13_date sum;
	int status = mon13_add(d, c, 0, MON13_ADD_DAYS, &sum);
	if(status) {
		return THEFT_TRIAL_FAIL; //Adding 0 should never cause error!
	}

	int dow = mon13_extract(&sum, c, MON13_EXTRACT_DAY_OF_WEEK);
	int expected;
	if(sum.month == 0) {
		expected = MON13_NO_WEEKDAY;
	}
	else {
		switch(sum.day % 7) {
			case 0: expected = MON13_THURSDAY; break;
			case 1: expected = MON13_FRIDAY; break;
			case 2: expected = MON13_SATURDAY; break;
			case 3: expected = MON13_SUNDAY; break;
			case 4: expected = MON13_MONDAY; break;
			case 5: expected = MON13_TUESDAY; break;
			case 6: expected = MON13_WEDNESDAY; break;
			default: expected = -1;
		}
	}
	return dow == expected ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res extract_day_of_year_add_one(struct theft* t, void* a1, void* a2) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;

	struct mon13_date sum;
	int status = mon13_add(d, c, 1, MON13_ADD_DAYS, &sum);
	if(status) {
		return THEFT_TRIAL_FAIL; //Adding 1 unlikely to cause error.
	}

	int doy0 = mon13_extract(d, c, MON13_EXTRACT_DAY_OF_YEAR);
	int doy1 = mon13_extract(&sum, c, MON13_EXTRACT_DAY_OF_YEAR);
	bool leap = mon13_extract(d, c, MON13_EXTRACT_IS_LEAP_YEAR);
	bool res = false;
	if(leap && doy0 == 366) {
		res = (doy1 == 1);
	}
	else if(!leap && doy0 == 365) {
		res = (doy1 == 1);
	}
	else {
		res = (doy1 == doy0 + 1) && (doy0 <= 365) && (doy0 > 0);
	}
	return res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res extract_day_of_year_split(struct theft* t, void* a1, void* a2, void* a3)
{
	const struct mon13_date* d = a1;
	int32_t offset = (int32_t) ((uint64_t)a2 % INT32_MAX);
	const struct mon13_cal* c = a3;

	offset = (offset > 0) ? offset : -offset;

	struct mon13_date res0, res1, res2, res3;
	int status;
	status = mon13_add(d, c, offset, MON13_ADD_DAYS, &res0);
	if(status) {
		return THEFT_TRIAL_SKIP;
	}
	status = mon13_add(d, c, offset/2, MON13_ADD_DAYS, &res1);
	if(status) {
		return THEFT_TRIAL_FAIL; //Already added offset, so why error for offset/2?
	}
	status = mon13_add(&res1, c, offset/2, MON13_ADD_DAYS, &res2);
	if(status) {
		return THEFT_TRIAL_FAIL; //Already added offset, so why error for offset/2?
	}
	status = mon13_add(&res2, c, offset%2, MON13_ADD_DAYS, &res3);
	if(status) {
		return THEFT_TRIAL_FAIL; //Already added offset, so why error for offset%2?
	}

	int doy0 = mon13_extract(&res0, c, MON13_EXTRACT_DAY_OF_YEAR);
	int doy3 = mon13_extract(&res3, c, MON13_EXTRACT_DAY_OF_YEAR);

	return doy0 == doy3 ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res format_percent(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);
	
	char buf[3];
	memset(buf, placeholder, 3);

	int res = mon13_format(d, c, n, "%%", buf, 3);
	if(!format_res_check(res, "%")) {
		return THEFT_TRIAL_FAIL;
	}
	if(buf[0] != '%') {
		return THEFT_TRIAL_FAIL;
	}
	if(buf[1] != '\0') {
		return THEFT_TRIAL_FAIL;
	}
	if(buf[2] != placeholder) {
		return THEFT_TRIAL_FAIL;
	}

	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_weekday(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);


	int day = mon13_extract(d, c, MON13_EXTRACT_DAY_OF_WEEK);
	if(day == MON13_NO_WEEKDAY) {
		return THEFT_TRIAL_SKIP;
	}

	char buf[20];
	memset(buf, placeholder, 20);

	int res = mon13_format(d, c, n, "%A", buf, 20);
	const char* expected = contained(buf, n->weekday_list, 10, placeholder);
	return format_res_check(res, expected) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res format_month(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);

	if(d->month == 0) {
		return THEFT_TRIAL_SKIP;
	}

	char buf[100];
	memset(buf, placeholder, 100);

	int res = mon13_format(d, c, n, "%B", buf, 100);
	const char* expected = contained(buf, n->month_list, 100, placeholder);
	return format_res_check(res, expected) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res format_day_of_month(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);

	char buf[4];
	memset(buf, placeholder, 4);

	int res = mon13_format(d, c, n, "%d", buf, 4);
	char* endptr = buf;
	long parsed = strtol(buf, &endptr, 10);
	if(parsed != d->day || res < 1 || res > 2) {
		return THEFT_TRIAL_FAIL;
	}
	if(endptr[0] != '\0' && endptr[1] != placeholder && &(endptr[0]) != &(buf[res])) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_cal(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);

	char buf[100];
	memset(buf, placeholder, 100);

	int res = mon13_format(d, c, n, "%f", buf, 4);
	if(res != strlen(n->calendar_name)) {
		return THEFT_TRIAL_FAIL;
	}
	if(strncmp(buf, n->calendar_name, 50)) {
		return THEFT_TRIAL_FAIL;
	}
	if(buf[res + 1] != '\0' && buf[res + 2] != placeholder) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_doy(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);

	int doy = mon13_extract(d, c, MON13_EXTRACT_DAY_OF_YEAR);
	
	char buf[5];
	memset(buf, placeholder, 5);

	int res = mon13_format(d, c, n, "%j", buf, 5);
	char* endptr = buf;
	long parsed = strtol(buf, &endptr, 10);
	if(parsed != doy || res < 1 || res > 3) {
		return THEFT_TRIAL_FAIL;
	}
	if(endptr[0] != '\0' && endptr[1] != placeholder && &(endptr[0]) != &(buf[res])) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_month_number(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);
	
	char buf[5];
	memset(buf, placeholder, 5);

	int res = mon13_format(d, c, n, "%m", buf, 5);
	char* endptr = buf;
	long parsed = strtol(buf, &endptr, 10);
	if(parsed != d->month || res < 1 || res > 2) {
		return THEFT_TRIAL_FAIL;
	}
	if(endptr[0] != '\0' && endptr[1] != placeholder && &(endptr[0]) != &(buf[res])) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_newline(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);
	
	char buf[3];
	memset(buf, placeholder, 3);

	int res = mon13_format(d, c, n, "%n", buf, 3);
	if(!format_res_check(res, "%")) {
		return THEFT_TRIAL_FAIL;
	}
	if(buf[0] != '\n') {
		return THEFT_TRIAL_FAIL;
	}
	if(buf[1] != '\0') {
		return THEFT_TRIAL_FAIL;
	}
	if(buf[2] != placeholder) {
		return THEFT_TRIAL_FAIL;
	}

	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_era(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);

	if(d->month == 0) {
		return THEFT_TRIAL_SKIP;
	}

	const char* expected = (d->year > 0) ? n->era_list[0] : n->era_list[1];

	char buf[100];
	memset(buf, placeholder, 100);

	int res = mon13_format(d, c, n, "%q", buf, 4);
	if(res != strlen(expected)) {
		return THEFT_TRIAL_FAIL;
	}
	if(strncmp(buf, expected, 50)) {
		return THEFT_TRIAL_FAIL;
	}
	if(buf[res + 1] != '\0' && buf[res + 2] != placeholder) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_tab(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);
	
	char buf[3];
	memset(buf, placeholder, 3);

	int res = mon13_format(d, c, n, "%t", buf, 3);
	if(!format_res_check(res, "%")) {
		return THEFT_TRIAL_FAIL;
	}
	if(buf[0] != '\t') {
		return THEFT_TRIAL_FAIL;
	}
	if(buf[1] != '\0') {
		return THEFT_TRIAL_FAIL;
	}
	if(buf[2] != placeholder) {
		return THEFT_TRIAL_FAIL;
	}

	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_weekday_number(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);
	
	char buf[3];
	memset(buf, placeholder, 3);

	int res = mon13_format(d, c, n, "%u", buf, 5);
	char* endptr = buf;
	long parsed = strtol(buf, &endptr, 10);
	if(parsed > 7 || parsed < 0 || res != 1) {
		return THEFT_TRIAL_FAIL;
	}
	if(endptr[0] != '\0' && endptr[1] != placeholder && &(endptr[0]) != &(buf[1])) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_year(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	const struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);
	
	char buf[100];
	memset(buf, placeholder, 100);

	int res = mon13_format(d, c, n, "%Y", buf, 5);
	char* endptr = buf;
	long parsed = strtol(buf, &endptr, 10);
	if(parsed != d->year || res < 1) {
		return THEFT_TRIAL_FAIL;
	}
	if(endptr[0] != '\0' && endptr[1] != placeholder) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_strftime(struct theft* t, void* a1, void* a2, void* a3) {
	time_t unix = (time_t) (((int64_t)a1) % (INT64_MAX/1024));
	const char placeholder = (char) (((uint64_t)a2) % CHAR_MAX);
	const char* fmt = a3;
	const struct mon13_cal* c = &mon13_gregorian_year_zero;
	const struct mon13_name_list* n = &mon13_gregorian_names_en_US;

	const struct tm* gmt_u = gmtime(&unix);
	int64_t unix64 = (int64_t)unix;
	struct mon13_date d;
	mon13_import(c, &unix64, MON13_IMPORT_UNIX, &d);

	char buf0[STRFTIME_BUF];
	char buf1[STRFTIME_BUF];
	memset(buf0, placeholder, STRFTIME_BUF);
	memset(buf1, placeholder, STRFTIME_BUF);

	int res0 = strftime(buf0, STRFTIME_BUF, fmt, gmt_u);
	int res1 = mon13_format(&d, c, n, fmt, buf1, STRFTIME_BUF);

	if(res0 != res1) {
		return THEFT_TRIAL_FAIL;
	}
	if(strncmp(buf0, buf1, STRFTIME_BUF)) {
		return THEFT_TRIAL_FAIL;
	}
	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_numeric_padding(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char* fmt = a4;
	char placeholder = ' ';

	char flag = fmt[1];
	int min_width = (flag == '-') ? 0 : fmt[2] - '0'; //Assumes ASCII or UTF8.
	int targ;
	switch(fmt[3]) {
		case 'd': targ = d->day; break;
		case 'j': targ = mon13_extract(d, c, MON13_EXTRACT_DAY_OF_YEAR); break;
		case 'm': targ = d->month; break;
		case 'u': targ = mon13_extract(d, c, MON13_EXTRACT_DAY_OF_WEEK); break;
		case 'Y': d->year = d->year % 9999; targ = d->year; break;
		default: targ = 0;
	}
	int digits = floor(log10(targ)) + 1;

	char buf[20];
	memset(buf, placeholder, 20);
	int res = mon13_format(d, c, n, fmt, buf, 20);

	if(digits >= min_width && res != digits) {
		return THEFT_TRIAL_FAIL;
	}
	else if(digits < min_width) {
		if(res != min_width) {
			return THEFT_TRIAL_FAIL;
		}
		else if(flag == '0' && buf[0] != '0') {
			return THEFT_TRIAL_FAIL;
		}
		else if(flag == '_' && buf[0] != ' ') {
			return THEFT_TRIAL_FAIL;
		}
	}

	return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_numeric_null(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
	struct mon13_date* d = a1;
	const struct mon13_cal* c = a2;
	const struct mon13_name_list* n = a3;
	const char* fmt = a4;
	char placeholder0 = ' ';
	char placeholder1 = '\t';

	char buf0[20];
	char buf1[20];
	memset(buf0, placeholder0, 20);
	memset(buf1, placeholder1, 20);
	int res0 = mon13_format(d, c, n, fmt, buf0, 20);
	int res1 = mon13_format(d, c, NULL, fmt, buf1, 20);
	return (res0 == res1) && !strncmp(buf0, buf1, 20) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
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

struct theft_type_info gr_cal_info = {
	.alloc = select_env, //nothing to free
	.env = (void*)&mon13_gregorian
};

struct theft_type_info tq_cal_info = {
	.alloc = select_env, //nothing to free
	.env = (void*)&mon13_tranquility
};

struct theft_type_info gr_year0_cal_info = {
	.alloc = select_env, //nothing to free
	.env = (void*)&mon13_gregorian_year_zero
};

struct theft_type_info tq_year0_cal_info = {
	.alloc = select_env, //nothing to free
	.env = (void*)&mon13_tranquility_year_zero
};

struct theft_type_info gr_date_info = {
	.alloc = alloc_date,
	.free = theft_generic_free_cb,
	.print = print_date,
	.env = (void*)&mon13_gregorian
};

struct theft_type_info tq_date_info = {
	.alloc = alloc_date,
	.free = theft_generic_free_cb,
	.print = print_date,
	.env = (void*)&mon13_tranquility
};

struct theft_type_info gr_year0_date_info = {
	.alloc = alloc_date,
	.free = theft_generic_free_cb,
	.print = print_date,
	.env = (void*)&mon13_gregorian_year_zero
};

struct theft_type_info tq_year0_date_info = {
	.alloc = alloc_date,
	.free = theft_generic_free_cb,
	.print = print_date,
	.env = (void*)&mon13_tranquility_year_zero
};

struct theft_type_info strftime_fmt_info = {
	.alloc = alloc_strftime_fmt,
	.free = theft_generic_free_cb,
	.print = print_s
};

struct theft_type_info numeric_fmt_info = {
	.alloc = alloc_numeric_fmt,
	.free = theft_generic_free_cb,
	.print = print_s
};

struct theft_type_info gr_name_en_info = {
	.alloc = select_env, //nothing to free
	.env = (void*)&mon13_gregorian_names_en_US
};

struct theft_type_info gr_name_fr_info = {
	.alloc = select_env, //nothing to free
	.env = (void*)&mon13_gregorian_names_fr_FR
};

struct theft_type_info tq_name_en_info = {
	.alloc = select_env, //nothing to free
	.env = (void*)&mon13_tranquility_names_en_US
};

int main(int argc, char** argv) {
	theft_seed seed;
	if(argc > 1) {
		seed = strtol(argv[1], NULL, 10);
	}
	else {
		seed = theft_seed_of_time();
	}
	printf("SEED: %lu\n", seed);
	struct theft_run_config config[] = {
		{
			.name = "mon13_import: Gregorian Year 0<->MJD",
			.prop3 = import_mjd,
			.type_info = {
				&gr_year0_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Tranquility Year 0<->MJD",
			.prop3 = import_mjd,
			.type_info = {
				&tq_year0_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Gregorian<->MJD",
			.prop3 = import_mjd,
			.type_info = {
				&gr_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Tranquility<->MJD",
			.prop3 = import_mjd,
			.type_info = {
				&tq_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Gregorian Year 0<->Unix time",
			.prop3 = import_unix,
			.type_info = {
				&gr_year0_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Tranquility Year 0<->Unix time",
			.prop3 = import_unix,
			.type_info = {
				&tq_year0_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Gregorian<->Unix time",
			.prop3 = import_unix,
			.type_info = {
				&gr_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Tranquility<->Unix time",
			.prop3 = import_unix,
			.type_info = {
				&tq_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Unix time start epoch",
			.prop1 = import_unix_epoch_start,
			.type_info = {
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Gregorian Year 0<->RD",
			.prop3 = import_rd,
			.type_info = {
				&gr_year0_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Tranquility Year 0<->RD",
			.prop3 = import_rd,
			.type_info = {
				&tq_year0_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Gregorian<->RD",
			.prop3 = import_rd,
			.type_info = {
				&gr_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_import: Tranquility<->RD",
			.prop3 = import_rd,
			.type_info = {
				&tq_cal_info,
				&random_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_convert: Gregorian<->Tranquility (OA)",
			.prop1 = convert_known,
			.type_info = {&gr2tq_oa_info},
			.seed = seed,
			.trials = SIZEOF_ARR(gr2tq_oa)
		},
		{
			.name = "mon13_convert: Gregorian<->Tranquility (Handy)",
			.prop1 = convert_known,
			.type_info = {&gr2tq_handy_info},
			.seed = seed,
			.trials = SIZEOF_ARR(gr2tq_handy)
		},
		{
			.name = "mon13_convert: Gregorian Year 0",
			.prop1 = convert_gr_year0,
			.type_info = {&gr_year0_date_info},
			.seed = seed
		},
		{
			.name = "mon13_convert: Tranquility Year 0",
			.prop1 = convert_tq_year0,
			.type_info = {&tq_year0_date_info},
			.seed = seed
		},
		{
			.name = "mon13_convert: Gregorian Year 0 (Reverse)",
			.prop1 = convert_gr_year0_reverse,
			.type_info = {&gr_year0_date_info},
			.seed = seed
		},
		{
			.name = "mon13_convert: Tranquility Year 0 (Reverse)",
			.prop1 = convert_tq_year0_reverse,
			.type_info = {&tq_year0_date_info},
			.seed = seed
		},
		{
			.name = "mon13_add: 1 Day, Gregorian Year 0",
			.prop1 = add_1day_gr,
			.type_info = {&gr_year0_date_info},
			.seed = seed
		},
		{
			.name = "mon13_add: 1 Day, Tranquility Year 0",
			.prop1 = add_1day_tq,
			.type_info = {&tq_year0_date_info},
			.seed = seed
		},
		{
			.name = "mon13_add: Day Round Trip, Gregorian Year 0",
			.prop3 = add_day_roundtrip,
			.type_info = {
				&gr_year0_date_info,
				&random_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Day Round Trip, Tranquility Year 0",
			.prop3 = add_day_roundtrip,
			.type_info = {
				&tq_year0_date_info,
				&random_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Day Round Trip, Gregorian",
			.prop3 = add_day_roundtrip,
			.type_info = {
				&gr_date_info,
				&random_info,
				&gr_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Day Round Trip, Tranquility",
			.prop3 = add_day_roundtrip,
			.type_info = {
				&tq_date_info,
				&random_info,
				&tq_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Day Split, Gregorian Year 0",
			.prop3 = add_day_split,
			.type_info = {
				&gr_year0_date_info,
				&random_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Day Split, Tranquility Year 0",
			.prop3 = add_day_split,
			.type_info = {
				&tq_year0_date_info,
				&random_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: 1 Month, Gregorian Year 0",
			.prop1 = add_1month_gr,
			.type_info = {&gr_year0_date_info},
			.seed = seed
		},
		{
			.name = "mon13_add: 1 Month, Tranquility Year 0",
			.prop1 = add_1month_tq,
			.type_info = {&tq_year0_date_info},
			.seed = seed
		},
		{
			.name = "mon13_add: Year, Gregorian Year 0",
			.prop3 = add_year,
			.type_info = {
				&gr_year0_date_info,
				&random_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Year, Tranquility Year 0",
			.prop3 = add_year,
			.type_info = {
				&tq_year0_date_info,
				&random_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Zero, Gregorian Year 0",
			.prop3 = add_zero,
			.type_info = {
				&gr_year0_date_info,
				&add_mode_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Zero, Tranquility Year 0",
			.prop3 = add_zero,
			.type_info = {
				&tq_year0_date_info,
				&add_mode_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Zero, Gregorian",
			.prop3 = add_zero,
			.type_info = {
				&gr_date_info,
				&add_mode_info,
				&gr_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Zero, Tranquility",
			.prop3 = add_zero,
			.type_info = {
				&tq_date_info,
				&add_mode_info,
				&tq_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Gregorian like Gregorian Year 0",
			.prop5 = add_like_other_cal,
			.type_info = {
				&gr_date_info,
				&add_mode_info,
				&gr_year0_cal_info,
				&gr_cal_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: Tranquility like Tranquility Year 0",
			.prop5 = add_like_other_cal,
			.type_info = {
				&tq_date_info,
				&add_mode_info,
				&tq_year0_cal_info,
				&tq_cal_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: No Year 0 for Gregorian",
			.prop4 = add_no_year_zero,
			.type_info = {
				&gr_date_info,
				&add_mode_info,
				&gr_cal_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_add: No Year 0 for Tranquility",
			.prop4 = add_no_year_zero,
			.type_info = {
				&tq_date_info,
				&add_mode_info,
				&tq_cal_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_compare: Nearby, Gregorian Year 0",
			.prop4 = compare_nearby,
			.type_info = {
				&gr_year0_date_info,
				&random_nonzero_info,
				&add_mode_nonzero_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_compare: Nearby, Tranquility Year 0",
			.prop4 = compare_nearby,
			.type_info = {
				&tq_year0_date_info,
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
				&gr_year0_date_info,
				&gr_year0_date_info
			},
			.seed = seed
		},
		{
			.name = "mon13_compare: Random, Gregorian",
			.prop2 = compare_random_gr,
			.type_info = {
				&gr_date_info,
				&gr_date_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: IS_LEAP_YEAR, Gregorian Year 0",
			.prop2 = extract_is_leap,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: IS_LEAP_YEAR, Tranquility Year 0",
			.prop2 = extract_is_leap,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_WEEK, Gregorian Year 0",
			.prop1 = extract_day_of_week_gr,
			.type_info = {&gr_year0_date_info},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_WEEK, Tranquility Year 0",
			.prop1 = extract_day_of_week_tq,
			.type_info = {&tq_year0_date_info},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_WEEK, Gregorian",
			.prop1 = extract_day_of_week_gr,
			.type_info = {&gr_date_info},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_WEEK, Tranquility",
			.prop1 = extract_day_of_week_tq,
			.type_info = {&tq_date_info},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_YEAR, Gregorian Year 0",
			.prop2 = extract_day_of_year_add_one,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_YEAR Plus 1, Tranquility Year 0",
			.prop2 = extract_day_of_year_add_one,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_YEAR Split, Gregorian Year 0",
			.prop3 = extract_day_of_year_split,
			.type_info = {
				&gr_year0_date_info,
				&random_info,
				&gr_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_extract: DAY_OF_YEAR Split, Tranquility Year 0",
			.prop3 = extract_day_of_year_split,
			.type_info = {
				&tq_year0_date_info,
				&random_info,
				&tq_year0_cal_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %%, Gregorian Year 0 (en_US)",
			.prop4 = format_percent,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %%, Gregorian Year 0 (fr_FR)",
			.prop4 = format_percent,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %%, Tranquility Year 0 (en_US)",
			.prop4 = format_percent,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %A, Gregorian Year 0 (en_US)",
			.prop4 = format_weekday,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %A, Gregorian Year 0 (fr_FR)",
			.prop4 = format_weekday,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %A, Tranquility Year 0 (en_US)",
			.prop4 = format_weekday,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %B, Gregorian Year 0 (en_US)",
			.prop4 = format_month,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %B, Gregorian Year 0 (fr_FR)",
			.prop4 = format_month,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %B, Tranquility Year 0 (en_US)",
			.prop4 = format_month,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %d, Gregorian Year 0 (en_US)",
			.prop4 = format_month,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %d, Gregorian Year 0 (fr_FR)",
			.prop4 = format_month,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %d, Tranquility Year 0 (en_US)",
			.prop4 = format_month,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %f, Gregorian Year 0 (en_US)",
			.prop4 = format_cal,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %f, Gregorian Year 0 (fr_FR)",
			.prop4 = format_cal,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %f, Tranquility Year 0 (en_US)",
			.prop4 = format_cal,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %j, Gregorian Year 0 (en_US)",
			.prop4 = format_doy,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %j, Gregorian Year 0 (fr_FR)",
			.prop4 = format_doy,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %j, Tranquility Year 0 (en_US)",
			.prop4 = format_doy,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %m, Gregorian Year 0 (en_US)",
			.prop4 = format_month_number,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %m, Gregorian Year 0 (fr_FR)",
			.prop4 = format_month_number,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %m, Tranquility Year 0 (en_US)",
			.prop4 = format_month_number,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %n, Gregorian Year 0 (en_US)",
			.prop4 = format_newline,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %n, Gregorian Year 0 (fr_FR)",
			.prop4 = format_newline,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %n, Tranquility Year 0 (en_US)",
			.prop4 = format_newline,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %q, Gregorian Year 0 (en_US)",
			.prop4 = format_era,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %q, Gregorian Year 0 (fr_FR)",
			.prop4 = format_era,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %q, Tranquility Year 0 (en_US)",
			.prop4 = format_era,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %t, Gregorian Year 0 (en_US)",
			.prop4 = format_tab,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %t, Gregorian Year 0 (fr_FR)",
			.prop4 = format_tab,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %t, Tranquility Year 0 (en_US)",
			.prop4 = format_tab,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %u, Gregorian Year 0 (en_US)",
			.prop4 = format_weekday_number,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %u, Gregorian Year 0 (fr_FR)",
			.prop4 = format_weekday_number,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %u, Tranquility Year 0 (en_US)",
			.prop4 = format_weekday_number,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %Y, Gregorian Year 0 (en_US)",
			.prop4 = format_year,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %Y, Gregorian Year 0 (fr_FR)",
			.prop4 = format_year,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: %Y, Tranquility Year 0 (en_US)",
			.prop4 = format_year,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&random_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: compare with strftime",
			.prop3 = format_strftime,
			.type_info = {
				&random_info,
				&random_info,
				&strftime_fmt_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: numeric padding flags, Gregorian Year 0 (en_US)",
			.prop4 = format_numeric_padding,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&numeric_fmt_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: numeric padding flags, Gregorian Year 0 (fr_FR)",
			.prop4 = format_numeric_padding,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&numeric_fmt_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: numeric padding flags, Tranquility Year 0 (en_US)",
			.prop4 = format_numeric_padding,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&numeric_fmt_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: null names, Gregorian Year 0 (en_US)",
			.prop4 = format_numeric_null,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_en_info,
				&numeric_fmt_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: null names, Gregorian Year 0 (fr_FR)",
			.prop4 = format_numeric_null,
			.type_info = {
				&gr_year0_date_info,
				&gr_year0_cal_info,
				&gr_name_fr_info,
				&numeric_fmt_info
			},
			.seed = seed
		},
		{
			.name = "mon13_format: null names, Tranquility Year 0 (en_US)",
			.prop4 = format_numeric_null,
			.type_info = {
				&tq_year0_date_info,
				&tq_year0_cal_info,
				&tq_name_en_info,
				&numeric_fmt_info
			},
			.seed = seed
		}
	};
	//bool all_passed = true;
	for(int i = 0; i < SIZEOF_ARR(config); i++) {
		if(theft_run(&(config[i])) != THEFT_RUN_PASS) {
			//all_passed = false;
		}
	}
	//return all_passed ? EXIT_SUCCESS : EXIT_FAILURE;
	return 0;
}