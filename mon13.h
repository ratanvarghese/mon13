#ifndef MON13_H
#define MON13_H

#include <stdint.h>
#include <stddef.h>

//Enumerations
enum mon13_add_mode {
	MON13_ADD_NONE,
	MON13_ADD_DAYS,
	MON13_ADD_MONTHS,
	MON13_ADD_YEARS
};

enum mon13_extract_mode {
	MON13_DAY_OF_YEAR,
	MON13_DAY_OF_WEEK,
	MON13_IS_LEAP_YEAR,
	MON13_IS_OK
};

//Structures
struct mon13_date {
	int32_t year;
	uint8_t month;
	uint8_t day;
};

struct mon13_cal;

//Functions
struct mon13_date mon13_convert(
	const struct mon13_date d,
	const struct mon13_cal* src,
	const struct mon13_cal* dest
);

struct mon13_date mon13_add(
	const struct mon13_date d,
	const struct mon13_cal* cal,
	const int32_t offset,
	const enum mon13_add_mode mode
);

int mon13_compare(
	const struct mon13_date* d0,
	const struct mon13_date* d1,
	const struct mon13_cal* cal
);

int mon13_extract(
	const struct mon13_date d,
	const struct mon13_cal* cal,
	const enum mon13_extract_mode mode
);

int mon13_format(
	const struct mon13_date d,
	const struct mon13_cal* cal,
	const char* fmt,
	char* buf,
	const size_t buflen
);

//Predefined calendars
extern const struct mon13_cal mon13_gregorian;
extern const struct mon13_cal mon13_gregorian_year_zero;
extern const struct mon13_cal mon13_tranquility;
extern const struct mon13_cal mon13_tranquility_year_zero;
//extern const struct mon13_cal mon13_cotsworth;
//extern const struct mon13_cal mon13_positivist;

#endif //MON13_H