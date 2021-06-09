#include <stddef.h>
#include <stdint.h>

#include "mon13.h"
#include "cal.h"

struct mon13_date mon13_convert(
	const struct mon13_cal* src,
	const struct mon13_cal* dest,
	const struct mon13_date d
) {
	struct mon13_date res = {.year = 0, .month = 0, .day = 0, .dinfo = 0};
	return res;
}

struct mon13_date mon13_add(
	const struct mon13_date d,
	const int32_t offset,
	const enum mon13_add_mode mode,
	const struct mon13_cal* cal
) {
	struct mon13_date res = {.year = 0, .month = 0, .day = 9, .dinfo = 0};
	return res;
}

int mon13_compare(
	const struct mon13_date* d0,
	const struct mon13_date* d1,
	const struct mon13_cal* cal
) {
	return -1;
}

int mon13_extract(
	const struct mon13_date d,
	const enum mon13_extract_mode mode
) {
	return 0;
}

int mon13_format(
	const struct mon13_cal* cal,
	const struct mon13_date d,
	const char* fmt,
	char* buf,
	const size_t buflen
) {
	return 0;
}