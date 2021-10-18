#ifndef MON13_H
#define MON13_H

#include <stdint.h>
#include <stddef.h>

//Enumerations
enum mon13_AddMode {
	MON13_ADD_NONE,
	MON13_ADD_DAYS,
	MON13_ADD_MONTHS,
	MON13_ADD_YEARS
};

enum mon13_ExtractMode {
	MON13_EXTRACT_DAY_OF_YEAR,
	MON13_EXTRACT_DAY_OF_WEEK,
	MON13_EXTRACT_IS_LEAP_YEAR,
	MON13_EXTRACT_MJD,
	MON13_EXTRACT_UNIX,
	MON13_EXTRACT_RD
};

enum mon13_ImportMode {
	MON13_IMPORT_MJD,
	MON13_IMPORT_UNIX,
	MON13_IMPORT_RD,
	MON13_IMPORT_C99_TM,
};

enum mon13_Weekday {
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
    MON13_ERROR_NONE = 0,
    MON13_ERROR_UNKNOWN = -1,
    MON13_ERROR_NULL_CALENDAR = -2,
    MON13_ERROR_NULL_NAME_LIST = -3,
    MON13_ERROR_NULL_FORMAT = -4,
    MON13_ERROR_NULL_INPUT = -5,
    MON13_ERROR_NULL_RESULT = -6,
    MON13_ERROR_NULL_DATE = -7,
	MON13_ERROR_OVERFLOW = -64,
	MON13_ERROR_BAD_CALENDAR = -65,
	MON13_ERROR_DATE_NOT_FOUND = -66,
	MON13_ERROR_DAY_OF_YEAR_NOT_FOUND = -67,
	MON13_ERROR_BAD_MODE = -68
};

//Structures
struct mon13_Date {
	int32_t year;
	uint8_t month;
	uint8_t day;
};

struct mon13_NameList {
	const char** month_list;
	const char** weekday_list;
	const char** era_list;
	const char** intercalary_list;
	const char** alt_intercalary_list;
	const char* calendar_name;
};

struct mon13_Cal;

//Functions
int mon13_import(
	const struct mon13_Cal* cal,
	const void* input,
	const enum mon13_ImportMode mode,
	struct mon13_Date* result
);

int mon13_convert(
	const struct mon13_Date* d,
	const struct mon13_Cal* src,
	const struct mon13_Cal* dest,
	struct mon13_Date* result
);

int mon13_add(
	const struct mon13_Date* d,
	const struct mon13_Cal* cal,
	const int32_t offset,
	const enum mon13_AddMode mode,
	struct mon13_Date* result
);

int mon13_compare(
	const struct mon13_Date* d0,
	const struct mon13_Date* d1,
	const struct mon13_Cal* cal
);

int64_t mon13_extract(
	const struct mon13_Date* d,
	const struct mon13_Cal* cal,
	const enum mon13_ExtractMode mode
);

int mon13_format(
	const struct mon13_Date* d,
	const struct mon13_Cal* cal,
	const struct mon13_NameList* nlist,
	const char* fmt,
	char* buf,
	const uint32_t buflen
);

//Predefined name lists
extern const struct mon13_NameList mon13_gregorian_names_en_US;
extern const struct mon13_NameList mon13_gregorian_names_fr_FR;

extern const struct mon13_NameList mon13_tranquility_names_en_US;

extern const struct mon13_NameList mon13_holocene_names_en_US;

extern const struct mon13_NameList mon13_cotsworth_names_en_US;

//Predefined calendars
extern const struct mon13_Cal mon13_gregorian;
extern const struct mon13_Cal mon13_gregorian_year_zero;
extern const struct mon13_Cal mon13_tranquility;
extern const struct mon13_Cal mon13_tranquility_year_zero;
extern const struct mon13_Cal mon13_holocene;
extern const struct mon13_Cal mon13_cotsworth;
//extern const struct mon13_Cal mon13_positivist;

#endif //MON13_H