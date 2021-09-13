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
	MON13_EXTRACT_DAY_OF_YEAR,
	MON13_EXTRACT_DAY_OF_WEEK,
	MON13_EXTRACT_IS_LEAP_YEAR,
	MON13_EXTRACT_MJD,
	MON13_EXTRACT_UNIX,
	MON13_EXTRACT_RD
};

enum mon13_import_mode {
	MON13_IMPORT_MJD,
	MON13_IMPORT_UNIX,
	MON13_IMPORT_RD
};

enum mon13_weekday {
	MON13_NO_WEEKDAY = 0,
	MON13_MONDAY = 1,
	MON13_TUESDAY = 2,
	MON13_WEDNESDAY = 3,
	MON13_THURSDAY = 4,
	MON13_FRIDAY = 5,
	MON13_SATURDAY = 6,
	MON13_SUNDAY = 7
};

enum mon13_error {
	MON13_ERROR_NULL_CALENDAR = -1,
	MON13_ERROR_NULL_NAME_LIST = -2,
	MON13_ERROR_NULL_FORMAT = -3
};

//Structures
struct mon13_date {
	int32_t year;
	uint32_t reserved3;
	uint32_t reserved4;
	uint32_t reserved5;
	uint8_t month;
	uint8_t day;
	uint8_t reserved1;
	uint8_t reserved2;
};

struct mon13_name_list {
	const char** month_list;
	const char** weekday_list;
	const char** era_list;
	const char** intercalary_list;
	const char** alt_intercalary_list;
	const char* calendar_name;
};

struct mon13_cal;

//Functions
struct mon13_date mon13_import(
	const struct mon13_cal* cal,
	const void* input,
	const enum mon13_import_mode mode
);

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

int64_t mon13_extract(
	const struct mon13_date d,
	const struct mon13_cal* cal,
	const enum mon13_extract_mode mode
);

int mon13_format(
	const struct mon13_date d,
	const struct mon13_cal* cal,
	const struct mon13_name_list* nlist,
	const char* fmt,
	char* buf,
	const uint64_t buflen
);

//Predefined name lists
extern const struct mon13_name_list mon13_gregorian_names_en_US;
extern const struct mon13_name_list mon13_gregorian_names_fr_FR;

extern const struct mon13_name_list mon13_tranquility_names_en_US;

//Predefined calendars
extern const struct mon13_cal mon13_gregorian;
extern const struct mon13_cal mon13_gregorian_year_zero;
extern const struct mon13_cal mon13_tranquility;
extern const struct mon13_cal mon13_tranquility_year_zero;
//extern const struct mon13_cal mon13_cotsworth;
//extern const struct mon13_cal mon13_positivist;

#endif //MON13_H