#ifndef MON13_H
#define MON13_H

#include <stdint.h>
#include <stddef.h>

//Enumerations
enum mon13_Weekday7 {
    MON13_NO_WEEKDAY = 0,
    MON13_MONDAY = 1,
    MON13_TUESDAY = 2,
    MON13_WEDNESDAY = 3,
    MON13_THURSDAY = 4,
    MON13_FRIDAY = 5,
    MON13_SATURDAY = 6,
    MON13_SUNDAY = 7
};

enum mon13_Weekday10 {
    MON13_PRIMIDI = 1,
    MON13_DUODI = 2,
    MON13_TRIDI = 3,
    MON13_QUARTIDI = 4,
    MON13_QUINTIDI = 5,
    MON13_SEXTIDI = 6,
    MON13_SEPTIDI = 7,
    MON13_OCTIDI = 8,
    MON13_NONIDI = 9,
    MON13_DECADI = 10,
};

enum mon13_Error {
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
    MON13_ERROR_INVALID_UTF8 = -69,
    MON13_ERROR_INVALID_STATE = -70,
    MON13_ERROR_INVALID_SEQUENCE = -71,
    MON13_ERROR_FAILED_TO_INSERT_NULLCHAR = -72,
    MON13_ERROR_INVALID_DATE = -73,
    MON13_ERROR_INVALID_NAME_LIST = -74,
};

//Structures
struct mon13_NameList {
    char** month_list;
    char** weekday_list;
    char** era_list;
    char** intercalary_list;
    char** alt_intercalary_list;
    char* calendar_name;
};

struct mon13_Cal;

//Functions
int mon13_validYmd(
    const struct mon13_Cal* cal,
    int32_t year,
    uint8_t month,
    uint8_t day
);

int mon13_mjdFromYmd(
    const struct mon13_Cal* cal,
    int32_t year,
    uint8_t month,
    uint8_t day,
    int32_t* res_mjd
);
int mon13_mjdFromC99Tm(
    const struct mon13_Cal* cal,
    const void* tm,
    int32_t* res_mjd
);
int mon13_mjdFromUnix(
    int64_t u,
    int32_t* res_mjd
);
int mon13_mjdFromRd(
    int32_t rd,
    int32_t* res_mjd
);

int mon13_mjdToYmd(
    int32_t mjd,
    const struct mon13_Cal* cal,
    int32_t* res_year,
    uint8_t* res_month,
    uint8_t* res_day
);
int mon13_mjdToC99Tm(
    int32_t mjd,
    const struct mon13_Cal* cal,
    void* res_tm
);
int mon13_mjdToUnix(
    int32_t mjd,
    int64_t* res_unix
);
int mon13_mjdToRd(
    int32_t mjd,
    int32_t* res_rd
);
int mon13_mjdToIsLeapYear(
    int32_t mjd,
    const struct mon13_Cal* cal,
    int* res_isLeap
);
int mon13_mjdToDayOfWeek(
    int32_t mjd,
    const struct mon13_Cal* cal,
    uint8_t* res_weekday
);
int mon13_mjdToDayOfYear(
    int32_t mjd,
    const struct mon13_Cal* cal,
    uint16_t* res_yearday
);

int mon13_addMonths(
    int32_t mjd,
    const struct mon13_Cal* cal,
    int32_t offset,
    int32_t* res_mjd
);
int mon13_addYears(
    int32_t mjd,
    const struct mon13_Cal* cal,
    int32_t offset,
    int32_t* res_mjd
);

int mon13_diffMonths(
    int32_t mjd0,
    int32_t mjd1,
    const struct mon13_Cal* cal,
    int32_t* res_months
);
int mon13_diffYears(
    int32_t mjd0,
    int32_t mjd1,
    const struct mon13_Cal* cal,
    int32_t* res_months
);

int mon13_validNameList(
    const struct mon13_Cal* cal,
    const struct mon13_NameList* nlist
);

int mon13_format(
    int32_t mjd,
    const struct mon13_Cal* cal,
    const struct mon13_NameList* nlist,
    const char* fmt,
    char* buf,
    int32_t buflen
);

int mon13_parse(
    const struct mon13_Cal* cal,
    const struct mon13_NameList* nlist,
    const char* fmt,
    const char* buf,
    int32_t buflen,
    int32_t* res_mjd
);

const char* mon13_errorMessage(int errorCode);

//Predefined name lists
extern const struct mon13_NameList mon13_names_en_US_gregorian;
extern const struct mon13_NameList mon13_names_en_US_tranquility;
extern const struct mon13_NameList mon13_names_en_US_holocene;
extern const struct mon13_NameList mon13_names_en_US_cotsworth;
extern const struct mon13_NameList mon13_names_en_US_julian;
extern const struct mon13_NameList mon13_names_en_US_positivist;
extern const struct mon13_NameList mon13_names_en_US_symmetry454;
extern const struct mon13_NameList mon13_names_en_US_symmetry010;
extern const struct mon13_NameList mon13_names_en_US_ancient_egyptian;

extern const struct mon13_NameList mon13_names_en_GB_french_revolutionary;
extern const struct mon13_NameList mon13_names_en_GB_french_revolutionary_joke;

extern const struct mon13_NameList mon13_names_fr_FR_gregorian;
extern const struct mon13_NameList mon13_names_fr_FR_julian;
extern const struct mon13_NameList mon13_names_fr_FR_positivist;
extern const struct mon13_NameList mon13_names_fr_FR_french_revolutionary;

//Predefined calendars
extern const struct mon13_Cal mon13_gregorian;
extern const struct mon13_Cal mon13_gregorian_year_zero;
extern const struct mon13_Cal mon13_tranquility;
extern const struct mon13_Cal mon13_tranquility_year_zero;
extern const struct mon13_Cal mon13_holocene;
extern const struct mon13_Cal mon13_cotsworth;
extern const struct mon13_Cal mon13_julian;
extern const struct mon13_Cal mon13_positivist;
extern const struct mon13_Cal mon13_symmetry454;
extern const struct mon13_Cal mon13_symmetry010;
extern const struct mon13_Cal mon13_ancient_egyptian;
extern const struct mon13_Cal mon13_french_revolutionary_romme;
extern const struct mon13_Cal mon13_french_revolutionary_romme_sub1;

#endif //MON13_H