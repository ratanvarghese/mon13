#include <stdint.h>
#include <stddef.h>

#include "mon13.h"

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
	return 0;
}

int mon13_compare(
	const struct mon13_date* d0,
	const struct mon13_date* d1,
	const struct mon13_cal* cal
) {
	return 0;
}

void mon13_iter_step(
	const struct mon13_cal* cal,
	struct mon13_date* current,
	const struct mon13_date step,
	const bool skip_year_day,
	const bool skip_leap_day
) {
	return;
}