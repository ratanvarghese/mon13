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
#define ASCII_COPY_BUF 500
#define UNIX_DAY 24 * 60 * 60

enum mon13_AddMode {
    MON13_ADD_NONE,
    MON13_ADD_DAYS,
    MON13_ADD_MONTHS,
    MON13_ADD_YEARS
};


//Theft printers
void print_known(FILE* f, const void* instance, void* env) {
    const struct known_convert_date* kcd = instance;
    fprintf(
        f, "(%d-%02d-%02d) (%d-%02d-%02d)",
        kcd->d0.year, kcd->d0.month, kcd->d0.day,
        kcd->d1.year, kcd->d1.month, kcd->d1.day
    );
}

void print_cal(FILE* f, const void* instance, void* env) {
    const struct mon13_Cal* c = instance;
    if(c == NULL) {
        fprintf(f, "NULL");
    }
    else if(c == &mon13_gregorian) {
        fprintf(f, "%s", mon13_gregorian_names_en_US.calendar_name);
    }
    else if(c == &mon13_gregorian_year_zero) {
        fprintf(f, "%s (Year Zero)", mon13_gregorian_names_en_US.calendar_name);
    }
    else if(c == &mon13_tranquility) {
        fprintf(f, "%s", mon13_tranquility_names_en_US.calendar_name);
    }
    else if(c == &mon13_tranquility_year_zero) {
        fprintf(f, "%s (Year Zero)", mon13_tranquility_names_en_US.calendar_name);
    }
    else if(c == &mon13_holocene) {
        fprintf(f, "%s", mon13_holocene_names_en_US.calendar_name);
    }
    else if(c == &mon13_cotsworth) {
        fprintf(f, "%s", mon13_cotsworth_names_en_US.calendar_name);
    }
    else if(c == &mon13_julian) {
        fprintf(f, "%s", mon13_julian_names_en_US.calendar_name);
    }
    else if(c == &mon13_positivist) {
        fprintf(f, "%s", mon13_positivist_names_en_US.calendar_name);
    }
    else {
        fprintf(f, "UNKNOWN");
    }
}

void print_date(FILE* f, const void* instance, void* env) {
    const struct mon13_Date* d = instance;
    fprintf(f, "(%d-%02d-%02d)", d->year, d->month, d->day);
}

void print_random(FILE* f, const void* instance, void* env) {
    uint64_t i = (uint64_t) instance;
    fprintf(f, "%lu", i);
}

void print_add_mode(FILE* f, const void* instance, void* env) {
    enum mon13_AddMode m = (enum mon13_AddMode) ((uint64_t)instance);
    char* m_str = NULL;
    switch(m) {
        case MON13_ADD_NONE:    m_str = "NONE";        break;
        case MON13_ADD_DAYS:    m_str = "DAYS";        break;
        case MON13_ADD_MONTHS:    m_str = "MONTHS";    break;
        case MON13_ADD_YEARS:    m_str = "YEARS";    break;
        default:                m_str = "INVALID";
    }
    fprintf(f, "%s", m_str);
}

void print_s(FILE* f, const void* instance, void* env) {
    const char* s = instance;
    fprintf(f, "%s", s);
}

//Theft trials: helpers
bool valid_tq(const struct mon13_Date d) {
    if(d.month == 0) {
        return d.day == 1 || d.day == 2;
    }
    else {
        return d.month >= 1 && d.month <= 13 && d.day >= 1 && d.day <= 28;
    }
}

bool valid_gr(const struct mon13_Date d) {
    uint8_t month_len[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if(d.month < 0 || d.month > 12) {
        return false;
    }
    else {
        return d.day >= 1 && d.day <= month_len[d.month - 1];
    }
}

bool year0_convert_correct(const struct mon13_Date d_y0, const struct mon13_Date d) {
    return (d_y0.year > 0) ? (d.year == d_y0.year) : (d.year == d_y0.year - 1);
}

bool add_1day_trivial(struct mon13_Date d, struct mon13_Date res) {
    bool stable_ym = (res.year == d.year) && (res.month == d.month);
    bool inc_day = (res.day == d.day + 1);
    return stable_ym && inc_day;
}

bool add_1day_month(struct mon13_Date d, struct mon13_Date res, bool end_of_month) {
    bool inc_month = (res.month == d.month + 1) && (res.year == d.year);
    bool start_month = res.day == 1;
    return end_of_month && inc_month && start_month;
}

bool add_1day_year(struct mon13_Date d, struct mon13_Date res, bool end_of_year) {
    bool inc_year = res.year == d.year + 1;
    bool start_year = (res.month == 1) && (res.day == 1);
    return end_of_year && inc_year && start_year; 
}

bool equal_year_month_day(struct mon13_Date d0, struct mon13_Date d1) {
    return d0.year == d1.year && d0.month == d1.month && d0.day == d1.day;
}

bool less_year_month_day(struct mon13_Date d0, struct mon13_Date d1) {
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
            if(needle[len] == '\0' && needle[len + 1] == placeholder) {
                return expected;
            }
        }
    }
    return NULL;
}

const char* contained_ic(char* needle, const struct mon13_NameList* nlist, size_t maxlen, char placeholder) {
    const char* expected = contained(needle, nlist->intercalary_list, maxlen, placeholder);
    if(expected == NULL && nlist->alt_intercalary_list != NULL) {
        return contained(needle, nlist->alt_intercalary_list, maxlen, placeholder);
    }
    return expected;
}

bool format_res_check(size_t res, const char* expected) {
    return (expected != NULL) && (res == strlen(expected));
}

bool format_res_check_nonblank(size_t res, const char* expected) {
    return format_res_check(res, expected) && expected[0] != '\0';
}

bool skip_import(int64_t x) {
    return (x > (INT32_MAX/2)) || (x < -(INT32_MAX/2));
}

bool is_leap_day(struct mon13_Date d, const struct mon13_Cal* c) {
    int64_t is_leap = 0;
    if(mon13_extractIsLeapYear(&d, c, &is_leap)) {
        return false;
    }
    if(!is_leap) {
        return false;
    }
    if(c == &mon13_gregorian_year_zero || c == &mon13_gregorian) {
        return d.month == 2 && d.day == 29;
    }
    if(c == &mon13_julian) {
        return d.month == 2 && d.day == 29;
    }
    if(c == &mon13_tranquility_year_zero || c == &mon13_tranquility) {
        return d.month == 0 && d.day == 2;
    }
    if(c == &mon13_holocene) {
        return d.month == 2 && d.day == 29;
    }
    if(c == &mon13_cotsworth) {
        return d.month == 6 && d.day == 29;
    }
    if(c == &mon13_positivist) {
        return d.month == 0 && d.day == 2;
    }
    return false;
}

//Theft allocators
enum theft_alloc_res select_gr2tq_oa(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2tq_oa));
    *instance = (void*)&gr2tq_oa[i];
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_gr2tq_handy(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2tq_handy));
    *instance = (void*)&gr2tq_handy[i];
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_gr2ct_wiki(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2ct_wiki));
    *instance = (void*)&gr2ct_wiki[i];
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_gr2jl_wiki(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2jl_wiki));
    *instance = (void*)&gr2jl_wiki[i];
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_random(struct theft* t, void* env, void** instance) {
    if(sizeof(uint64_t) != sizeof(void*)) {
        return THEFT_ALLOC_ERROR;
    }
    uint64_t x = theft_random_choice(t, UINT64_MAX);
    x = (env != NULL && x == 0) ? 1 : x;
    *instance = (void*)x;
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_add_mode(struct theft* t, void* env, void** instance) {
    if(sizeof(enum mon13_AddMode) > sizeof(void*)) {
        return THEFT_ALLOC_ERROR;
    }
    uint64_t x = theft_random_choice(t, (uint64_t)MON13_ADD_YEARS + 1);
    x = (env != NULL && x == MON13_ADD_NONE) ? 1 : x;
    *instance = (void*)x;
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_env(struct theft* t, void* env, void** instance) {
    *instance = env;
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_date(struct theft* t, void* env, void** instance) {
    struct mon13_Date* res = malloc(sizeof(struct mon13_Date));
    if(res == NULL) {
        return THEFT_ALLOC_ERROR;
    }

    struct known_convert_date* kcd;
    select_gr2tq_oa(t, NULL, (void**)&kcd);
    int32_t offset = (theft_random_choice(t, INT32_MAX) - (INT32_MAX/2));

    if(env == &mon13_gregorian_year_zero) {
        if(mon13_addDays(&(kcd->d0), kcd->c0, offset, res)) {
            return THEFT_ALLOC_ERROR;
        }
    }
    else if(env == &mon13_gregorian) {
        if(mon13_addDays(&(kcd->d0), &mon13_gregorian, offset, res)) {
            return THEFT_ALLOC_ERROR;
        }
    }
    else if(env == &mon13_tranquility_year_zero) {
        if(mon13_addDays(&(kcd->d1), kcd->c1, offset, res)) {
            return THEFT_ALLOC_ERROR;
        }
    }
    else if(env == &mon13_tranquility) {
        if(mon13_addDays(&(kcd->d1), &mon13_tranquility, offset, res)) {
            return THEFT_ALLOC_ERROR;
        }
    }
    else if(env == &mon13_holocene) {
        //Every valid Gregorian (Year 0) date is a valid Holocene date
        if(mon13_addDays(&(kcd->d0), kcd->c0, offset, res)) {
            return THEFT_ALLOC_ERROR;
        }
    }
    else if(env == &mon13_cotsworth) {
        //Every valid Tranquility (Year 0) date is a valid Cotsworth date
        //except if month == 0.
        if(mon13_addDays(&(kcd->d1), kcd->c1, offset, res)) {
            return THEFT_ALLOC_ERROR;
        }
        if(res->month == 0) {
            bool armstrong = (res->day == 1);
            res->day = 29;
            res->month = armstrong ? 13 : 6;
            res->year = armstrong ? res->year : (res->year - 31);
        }
    }
    else if(env == &mon13_julian) {
        //Every valid Gregorian (No Year 0) date is a valid Julian date
        if(mon13_addDays(&(kcd->d0), &mon13_julian, offset, res)) {
            return THEFT_ALLOC_ERROR;
        }
    }
    else if(env == &mon13_positivist) {
        //Every valid Tranquility (Year 0) date is a valid Positivist date
        //except if month == 0
        if(mon13_addDays(&(kcd->d1), kcd->c1, offset, res)) {
            return THEFT_ALLOC_ERROR;
        }
        if(res->month == 0) {
            bool day_of_dead = (res->day == 1);
            res->year = day_of_dead ? res->year : (res->year + 181);
        }
    }
    else {
        return THEFT_ALLOC_ERROR;
    }

    if(!mon13_valid(res, env)) {
        return THEFT_ALLOC_ERROR;
    }

    *instance = (void*)res;
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_leap_day(struct theft* t, void* env, void** instance) {
    int32_t y = (theft_random_choice(t, INT32_MAX/4) - (INT32_MAX/8)) * 4;
    struct mon13_Cal* cal = env;
    struct mon13_Date* res = malloc(sizeof(struct mon13_Date));

    if(cal != &mon13_julian) {
        if((y % 100 == 0) && (y % 400 != 0)) {
            y = 400;
        }
    }

    if(cal == &mon13_gregorian_year_zero || cal == &mon13_holocene) {
        res->year = y;
        res->month = 2;
        res->day = 29;    
    }
    else if(cal == &mon13_gregorian || cal == &mon13_julian) {
        res->year = (y > 0) ? y : (y - 1);
        res->month = 2;
        res->day = 29;
    }
    else if(cal == &mon13_tranquility_year_zero) {
        res->year = y + 31;
        res->month = 0;
        res->day = 2;
    }
    else if(cal == &mon13_cotsworth) {
        res->year = y;
        res->month = 6;
        res->day = 29;
    }
    else if(cal == &mon13_positivist) {
        res->year = y + 212; //211
        res->month = 0;
        res->day = 2;
    }
    else {
        return THEFT_ALLOC_ERROR;
    }

    int64_t is_leap = 0;
    if(mon13_extractIsLeapYear(res, cal, &is_leap)) {
        return THEFT_ALLOC_ERROR;
    }
    if(!is_leap) {
        return THEFT_ALLOC_ERROR;
    }

    *instance = (void*)res;
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_strftime_fmt(struct theft* t, void* env, void** instance) {
    char fmt_list[] = "%ABdjmntuY";
    size_t fmt_len = (size_t) theft_random_choice(t, STRFTIME_BUF/10);
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

enum theft_alloc_res alloc_ascii_copy_fmt(struct theft* t, void* env, void** instance) {
    size_t fmt_len = (size_t) theft_random_choice(t, ASCII_COPY_BUF-1);
    fmt_len = (fmt_len > 0) ? fmt_len : 1;
    char* res = malloc(fmt_len * sizeof(char));
    if(res == NULL) {
        return THEFT_ALLOC_ERROR;
    }
    for(int i = 0; i < (fmt_len - 1); i++) { //Leave 1 more, for null character
        res[i] = theft_random_choice(t, 94) + 32; //avoid control codes
        if(res[i] < 32 || res[i] == '%') {
            res[i] = ' ';
        }
    }
    res[fmt_len - 1] = '\0';
    *instance = res;
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_utf8_copy_fmt(struct theft* t, void* env, void** instance) {
    //Generated from https://onlineunicodetools.com/convert-unicode-to-utf8
    char smiley[] = {240, 159, 152, 128, 0};
    char family0[] = {240, 159, 145, 168, 226, 128, 141, 240, 159, 145, 169, 226, 128, 141, 240, 159, 145, 167, 226, 128, 141, 240, 159, 145, 166, 0};
    char family1[] = {240, 159, 145, 169, 226, 128, 141, 240, 159, 145, 167, 226, 128, 141, 240, 159, 145, 167, 0};
    char lotus[] = {240, 159, 167, 152, 0};
    char cook[] = {240, 159, 145, 168, 226, 128, 141, 240, 159, 141, 179, 0};
    char whale[] = {240, 159, 144, 139, 0};
    char rice_ball[] = {240, 159, 141, 153, 0};
    char pawn[] = {226, 153, 159, 239, 184, 143, 0};
    char plane[] = {226, 156, 136, 239, 184, 143, 0};
    char swords[] = {226, 154, 148, 239, 184, 143, 0};

    char* choice = NULL;
    switch(theft_random_choice(t, 10)) {
        case  0: choice = smiley;    break;
        case  1: choice = family0;   break;
        case  2: choice = family1;   break;
        case  3: choice = lotus;     break;
        case  4: choice = cook;      break;
        case  5: choice = whale;     break;
        case  6: choice = rice_ball; break;
        case  7: choice = pawn;      break;
        case  8: choice = plane;     break;
        case  9: choice = swords;    break;
        default: choice = smiley;
    }

    int len = strlen(choice) + 1;
    char* res = malloc(len);
    if(res == NULL) {
        return THEFT_ALLOC_ERROR;
    }
    memcpy(res, choice, len);
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

//Theft trials: import
enum theft_trial_res import_mjd(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Cal* c = a1;
    int64_t mjd0 = ((int64_t)a2) % INT32_MAX;
    int32_t offset = (int32_t) ((int64_t)a3 % INT32_MAX);

    struct mon13_Date d0, d1;
    int status;
    status = mon13_importMjd(c, &mjd0, &d0);
    if(status) {
        if(status == MON13_ERROR_OVERFLOW) {
            return skip_import(mjd0) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        else {
            return THEFT_TRIAL_FAIL;
        }
    }
    status = mon13_addDays(&d0, c, offset, &d1);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int64_t mjd1;
    status = mon13_extractMjd(&d1, c, &mjd1);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }
    return ((mjd1 - mjd0) == offset) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res import_unix(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Cal* c = a1;
    int64_t u0 = ((int64_t)a2) % (((int64_t)INT32_MAX) * UNIX_DAY);
    int32_t offset = (int32_t) ((int64_t)a3 % INT32_MAX);

    int64_t u0_cut = u0 - (u0 % (UNIX_DAY));
    struct mon13_Date d0, d1;
    int status;
    status = mon13_importUnix(c, &u0_cut, &d0);
    if(status) {
        if(status == MON13_ERROR_OVERFLOW) {
            return skip_import(u0/(UNIX_DAY)) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        else {
            return THEFT_TRIAL_FAIL;
        }
    }
    status = mon13_addDays(&d0, c, offset, &d1);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int64_t u1;
    status = mon13_extractUnix(&d1, c, &u1);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }
    int64_t udiff = (u1 - u0_cut);
    int64_t udiff_days =  udiff / (UNIX_DAY);
    if(udiff_days == offset) {
        return THEFT_TRIAL_PASS;
    }

    return THEFT_TRIAL_FAIL;
}

enum theft_trial_res import_unix_epoch_start(struct theft* t, void* a1) {
    const struct mon13_Cal* c = &mon13_gregorian_year_zero;
    int64_t u0 = ((int64_t)a1) % (UNIX_DAY);

    if(u0 < 0) {
        u0 = -u0;
    }

    struct mon13_Date d0;
    if(mon13_importUnix(c, &u0, &d0)) {
        return THEFT_TRIAL_FAIL; //u0 is too small for error to be allowed
    }
    if(d0.year == 1970 && d0.month == 1 && d0.day == 1) {
        return THEFT_TRIAL_PASS;
    }
    else {
        return THEFT_TRIAL_FAIL;
    }
}

enum theft_trial_res import_unix_gmtime(struct theft* t, void* a1) {
    int64_t u0 = ((int64_t)a1) % (((int64_t)INT32_MAX) * UNIX_DAY);
    const struct mon13_Cal* c = &mon13_gregorian_year_zero;

    time_t unix0 = u0;
    const struct tm* gmt_u = gmtime(&unix0);
    struct mon13_Date d;
    if(mon13_importUnix(c, &u0, &d)) {
        return THEFT_TRIAL_SKIP;
    }

    if(d.day != gmt_u->tm_mday) {
        return THEFT_TRIAL_FAIL;
    }
    if((d.month - 1) != gmt_u->tm_mon) {
        return THEFT_TRIAL_FAIL;
    }
    if((d.year - 1900) != gmt_u->tm_year) {
        return THEFT_TRIAL_FAIL;
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res import_rd(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Cal* c = a1;
    int64_t rd0 = ((int64_t)a2) % INT32_MAX;
    int32_t offset = (int32_t) ((int64_t)a3 % INT32_MAX);

    struct mon13_Date d0, d1;
    int status;
    status = mon13_importRd(c, &rd0, &d0);
    if(status) {
        if(status == MON13_ERROR_OVERFLOW) {
            return skip_import(rd0) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        else {
            return THEFT_TRIAL_FAIL;
        }
    }
    status = mon13_addDays(&d0, c, offset, &d1);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int64_t rd1;
    status = mon13_extractRd(&d1, c, &rd1);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }
    return ((rd1 - rd0) == offset) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res import_c99_tm(struct theft* t, void* a1) {
    const struct mon13_Cal* c = &mon13_gregorian_year_zero;
    int64_t u0 = ((int64_t)a1) % (((int64_t)INT32_MAX) * UNIX_DAY);

    time_t unix0 = u0;
    const struct tm* local_u = localtime(&unix0);
    struct mon13_Date d;
    int status;
    status = mon13_importC99Tm(c, local_u, &d);
    if(status) {
        if(status == MON13_ERROR_OVERFLOW) {
            return skip_import(u0/(UNIX_DAY)) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        else {
            return THEFT_TRIAL_FAIL;
        }
    }
    if(d.day != local_u->tm_mday) {
        return THEFT_TRIAL_FAIL;
    }
    if(d.month != (local_u->tm_mon + 1)) {
        return THEFT_TRIAL_FAIL;
    }
    if(d.year != (local_u->tm_year + 1900)) {
        return THEFT_TRIAL_FAIL;
    }
    
    int64_t doy;
    status = mon13_extractDayOfYear(&d, c, &doy);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }
    if(doy != (local_u->tm_yday + 1)) {
        return THEFT_TRIAL_FAIL;
    }

    enum mon13_Weekday correct_weekday = MON13_NO_WEEKDAY;
    switch(local_u->tm_wday) {
        case 0: correct_weekday = MON13_SUNDAY; break;
        case 1: correct_weekday = MON13_MONDAY; break;
        case 2: correct_weekday = MON13_TUESDAY; break;
        case 3: correct_weekday = MON13_WEDNESDAY; break;
        case 4: correct_weekday = MON13_THURSDAY; break;
        case 5: correct_weekday = MON13_FRIDAY; break;
        case 6: correct_weekday = MON13_SATURDAY; break;
        default: return THEFT_TRIAL_ERROR;
    }
    int64_t res_weekday;
    status = mon13_extractDayOfWeek(&d, c, &res_weekday);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }
    if(res_weekday != correct_weekday && res_weekday != MON13_NO_WEEKDAY) {
        return THEFT_TRIAL_FAIL;
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res import_null(struct theft* t, void* a1, void* a2) {
    const struct mon13_Cal* c = a1;
    int64_t import_data = ((int64_t)a2) % INT32_MAX;

    struct mon13_Date d0;
    int status[4];
    status[0] = mon13_importMjd(NULL, &import_data, &d0);
    status[1] = mon13_importUnix(NULL, &import_data, &d0);
    status[2] = mon13_importRd(NULL, &import_data, &d0);
    status[3] = mon13_importC99Tm(NULL, &import_data, &d0);
    for(size_t si = 0; si < SIZEOF_ARR(status); si++) {
        if(status[si] != MON13_ERROR_NULL_CALENDAR) {
            return THEFT_TRIAL_FAIL;
        }
    }

    status[0] = mon13_importMjd(c, NULL, &d0);
    status[1] = mon13_importUnix(c, NULL, &d0);
    status[2] = mon13_importRd(c, NULL, &d0);
    status[3] = mon13_importC99Tm(c, NULL, &d0);
    for(size_t si = 0; si < SIZEOF_ARR(status); si++) {
        if(status[si] != MON13_ERROR_NULL_INPUT) {
            return THEFT_TRIAL_FAIL;
        }
    }

    status[0] = mon13_importMjd(c, &import_data, NULL);
    status[1] = mon13_importUnix(c, &import_data, NULL);
    status[2] = mon13_importRd(c, &import_data, NULL);
    status[3] = mon13_importC99Tm(c, &import_data, NULL);
    for(size_t si = 0; si < SIZEOF_ARR(status); si++) {
        if(status[si] != MON13_ERROR_NULL_RESULT) {
            return THEFT_TRIAL_FAIL;
        }
    }

    return THEFT_TRIAL_PASS;
}

//Theft trials: convert
enum theft_trial_res convert_known(struct theft* t, void* test_input) {
    const struct known_convert_date* kcd = test_input;
    struct mon13_Date res0, res1;
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

enum theft_trial_res convert_tq_year0(struct theft* t, void* test_input) {
    const struct mon13_Date* d = test_input;
    struct mon13_Date res;
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

enum theft_trial_res convert_gr_year0(struct theft* t, void* test_input) {
    const struct mon13_Date* d = test_input;
    struct mon13_Date res;
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

enum theft_trial_res convert_tq_year0_reverse(struct theft* t, void* test_input) {
    const struct mon13_Date* d = test_input;
    struct mon13_Date res;
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

enum theft_trial_res convert_gr_year0_reverse(struct theft* t, void* test_input) {
    const struct mon13_Date* d = test_input;
    struct mon13_Date res;
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

enum theft_trial_res convert_null(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* cal0 = a2;
    const struct mon13_Cal* cal1 = a3;
    struct mon13_Date res;
    int status;
    status = mon13_convert(NULL, cal0, cal1, &res);
    if(status != MON13_ERROR_NULL_DATE) {
        return THEFT_TRIAL_FAIL;
    }
    status = mon13_convert(d, NULL, cal1, &res);
    if(status != MON13_ERROR_NULL_CALENDAR) {
        return THEFT_TRIAL_FAIL;
    }
    status = mon13_convert(d, cal0, NULL, &res);
    if(status != MON13_ERROR_NULL_CALENDAR) {
        return THEFT_TRIAL_FAIL;
    }
    status = mon13_convert(d, cal0, cal1, NULL);
    if(status != MON13_ERROR_NULL_RESULT) {
        return THEFT_TRIAL_FAIL;
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res convert_same_cal(struct theft* t, void* a1, void* a2) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* cal0 = a2;
    struct mon13_Date res;
    int status = mon13_convert(d, cal0, cal0, &res);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }
    return equal_year_month_day(*d, res) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res convert_holocene(struct theft* t, void* a1) {
    const struct mon13_Date* d0 = a1;
    const struct mon13_Cal* cal_gr = &mon13_gregorian_year_zero;
    const struct mon13_Cal* cal_hl = &mon13_holocene;
    struct mon13_Date res_hl, res_gr;
    int status_hl = mon13_convert(d0, cal_gr, cal_hl, &res_hl);
    int status_gr = mon13_convert(d0, cal_hl, cal_gr, &res_gr);
    if(status_hl && status_gr) {
        if(status_hl != MON13_ERROR_OVERFLOW) {
            return THEFT_TRIAL_FAIL;
        }
        else if(status_gr != MON13_ERROR_OVERFLOW) {
            return THEFT_TRIAL_FAIL;
        }
        else {
            return THEFT_TRIAL_SKIP;    
        }
    }

    if(!status_hl) {
        if(res_hl.month != d0->month || res_hl.day != d0->day) {
            return THEFT_TRIAL_FAIL;
        }
        if(res_hl.year != (d0->year + 10000)) {
            return THEFT_TRIAL_FAIL;
        }
    }
    if(!status_gr) {
        if(res_gr.month != d0->month || res_gr.day != d0->day) {
            return THEFT_TRIAL_FAIL;
        }
        if(res_gr.year != (d0->year - 10000)) {
            return THEFT_TRIAL_FAIL;
        }
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res convert_same_year(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d0 = a1;
    const struct mon13_Cal* c0 = a2;
    const struct mon13_Cal* c1 = a3;
    struct mon13_Date res;
    int status = mon13_convert(d0, c0, c1, &res);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }
    return (res.year == d0->year) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res convert_same_doy(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d0 = a1;
    const struct mon13_Cal* c0 = a2;
    const struct mon13_Cal* c1 = a3;
    struct mon13_Date d1;
    int status = mon13_convert(d0, c0, c1, &d1);
    if(status) {
        return THEFT_TRIAL_SKIP;
    }
    int64_t doy0, doy1;
    status = mon13_extractDayOfYear(d0, c0, &doy0);
    if(status) {
        return THEFT_TRIAL_SKIP;
    }
    status = mon13_extractDayOfYear(&d1, c1, &doy1);
    if(status) {
        return THEFT_TRIAL_SKIP;
    }
    return (doy0 == doy1) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res convert_same_leap(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d0 = a1;
    const struct mon13_Cal* c0 = a2;
    const struct mon13_Cal* c1 = a3;
    struct mon13_Date d1;
    int status = mon13_convert(d0, c0, c1, &d1);
    if(status) {
        return THEFT_TRIAL_SKIP;
    }
    int64_t is_leap0, is_leap1;
    status = mon13_extractIsLeapYear(d0, c0, &is_leap0);
    if(status) {
        return THEFT_TRIAL_SKIP;
    }
    status = mon13_extractIsLeapYear(&d1, c1, &is_leap1);
    if(status) {
        return THEFT_TRIAL_SKIP;
    }

    bool correct_res = (is_leap0 && is_leap1) || ((!is_leap0) && (!is_leap1));
    return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

//Theft trials: add
enum theft_trial_res add_1day_gr(struct theft* t, void* test_input) {
    const struct mon13_Date* d = test_input;
    const struct mon13_Cal* c = &mon13_gregorian_year_zero;
    struct mon13_Date res;
    int status;
    status = mon13_addDays(d, c, 1, &res);
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

enum theft_trial_res add_1day_tq(struct theft* t, void* test_input) {
    const struct mon13_Date* d = test_input;
    const struct mon13_Cal* c = &mon13_tranquility_year_zero;
    struct mon13_Date res;
    int status;
    status = mon13_addDays(d, c, 1, &res);
    if(status) {
        return THEFT_TRIAL_FAIL; //Adding 1 is unlikely to be legitimate error.
    }
    if(!valid_tq(res)) {
        return THEFT_TRIAL_FAIL;
    }

    int64_t leap;
    status = mon13_extractIsLeapYear(&res, c, &leap);
    if(status) {
        leap = 0;
    }

    bool correct_res = false;
    bool stable_y = (res.year == d->year);

    if(!stable_y) {
        bool end_of_year = (d->month == 0) && (d->day == 1);
        correct_res = add_1day_year(*d, res, end_of_year);
    }
    else if(res.month == 0 && res.day == 1) {
        bool correct_start = (d->month == 13) && (d->day == 28);
        correct_res = correct_start && stable_y;
    }
    else if(res.month == 0 && res.day == 2) {
        bool correct_start = (d->month == 8) && (d->day == 27);    
        correct_res = (correct_start && leap && stable_y); 
    }
    else if(res.month == 8 && res.day == 28 && leap) {
        bool correct_start = (d->month == 0) && (d->day == 2);
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

enum theft_trial_res add_day_roundtrip(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d = a1;
    int32_t offset = (int32_t) ((int64_t)a2 % INT32_MAX);
    const struct mon13_Cal* c = a3;

    struct mon13_Date res0, res1;
    int status;
    status = mon13_addDays(d, c, offset, &res0);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    status = mon13_addDays(&res0, c, -offset, &res1);
    if(status) {
        return THEFT_TRIAL_FAIL; //Need to be able to return to origin.
    }

    return mon13_compare(d, &res1, c) ? THEFT_TRIAL_FAIL : THEFT_TRIAL_PASS;
}

enum theft_trial_res add_day_split(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d = a1;
    int32_t offset = (int32_t) ((uint64_t)a2 % INT32_MAX);
    const struct mon13_Cal* c = a3;

    offset = (offset > 0) ? offset : -offset;

    struct mon13_Date res0, res1, res2, res3;
    int status;
    status = mon13_addDays(d, c, offset, &res0);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    status = mon13_addDays(d, c, offset/2, &res1);
    if(status) {
        return THEFT_TRIAL_FAIL; //If adding offset is OK, adding offset/2 should be.
    }
    status = mon13_addDays(&res1, c, offset/2, &res2);
    if(status) {
        return THEFT_TRIAL_FAIL; //If adding offset is OK, adding offset/2 should be.
    }
    status = mon13_addDays(&res2, c, offset%2, &res3);
    if(status) {
        return THEFT_TRIAL_FAIL; //If adding offset is OK, adding offset%2 should be.
    }
    return mon13_compare(&res0, &res3, c) ? THEFT_TRIAL_FAIL : THEFT_TRIAL_PASS;
}

enum theft_trial_res add_1month_gr(struct theft* t, void* test_input) {
    const struct mon13_Date* d = test_input;
    const struct mon13_Cal* c = &mon13_gregorian_year_zero;
    struct mon13_Date res;
    int status;
    status = mon13_addMonths(d, c, 1, &res);
    if(status) {
        return THEFT_TRIAL_FAIL; //Adding 1 month is unlikely to be legitimate error.
    }
    if(!valid_gr(res)) {
        return THEFT_TRIAL_FAIL;
    }

    bool correct_res = false;
    if(res.day != d->day) {
        if(d->day == 29) {
            correct_res = (res.month == 3) && (res.day == 1);
        }
        else if(d->day == 30) {
            int64_t leap;
            if(mon13_extractIsLeapYear(d, c, &leap)) {
                leap = 0;
            }
            if(leap) {
                correct_res = (res.month == 3) && (res.day == 1);
            }
            else {
                correct_res = (res.month == 3) && (res.day == 2);
            }
        }
        else if(d->day == 31) {
            bool correct_month = (res.month == d->month + 2);
            bool correct_day = (res.day < 4);
            correct_res = correct_month && correct_day;
        }
        else {
            correct_res = false;
        }
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

enum theft_trial_res add_1month_tq(struct theft* t, void* test_input) {
    const struct mon13_Date* d = test_input;
    const struct mon13_Cal* c = &mon13_tranquility_year_zero;

    struct mon13_Date res;
    int status;
    status = mon13_addMonths(d, c, 1, &res);
    if(status) {
        return THEFT_TRIAL_FAIL; //Adding 1 month is unlikely to be legitimate error.
    }
    if(!valid_tq(res)) {
        return THEFT_TRIAL_FAIL;
    }

    bool correct_res = false;
    if(d->month == 0) {
        if(d->day == 1) {
            bool inc_year = (res.year == d->year + 1) && (res.month == 2);
            bool correct_day = (res.day == 1);
            correct_res = inc_year && correct_day;
        }
        else if(d->day == 2) {
            bool correct_year = (res.year == d->year);
            bool correct_month = (res.month == 9);
            bool correct_day = (res.day == 28);
            correct_res = correct_year && correct_month && correct_day;
        }
    }
    else if(res.day != d->day) {
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

enum theft_trial_res add_year(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d = a1;
    int32_t offset = (int32_t) ((int64_t)a2 % INT32_MAX);
    const struct mon13_Cal* c = a3;

    struct mon13_Date res;
    int status = mon13_addYears(d, c, offset, &res);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    if(res.year != (d->year + offset)) {
        return THEFT_TRIAL_FAIL;
    }
    if(!is_leap_day(*d, c) && (res.month != d->month || res.day != d->day)) {
        return THEFT_TRIAL_FAIL;
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res add_year_leap(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d = a1;
    int32_t offset = (int32_t) ((int64_t)a2 % INT32_MAX);
    const struct mon13_Cal* c = a3;

    struct mon13_Date res;
    int status = mon13_addYears(d, c, offset, &res);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int64_t leap;
    if(mon13_extractIsLeapYear(&res, c, &leap)) {
        leap = 0;
    }
    if(leap) {
        if(res.month != d->month || res.day != d->day) {
            return THEFT_TRIAL_FAIL;
        }
    }
    else {
        int64_t doy0;
        if(mon13_extractDayOfYear(d, c, &doy0)) {
            return THEFT_TRIAL_FAIL;
        }
        int64_t doy1;
        if(mon13_extractDayOfYear(&res, c, &doy1)) {
            return THEFT_TRIAL_FAIL;
        }
        if(doy0 != doy1) {
            return THEFT_TRIAL_FAIL;
        }
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res add_zero(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d = a1;
    enum mon13_AddMode m = (enum mon13_AddMode) ((uint64_t)a2);
    const struct mon13_Cal* c = a3;

    struct mon13_Date res0, res1;
    int status0, status1;

    if(m == MON13_ADD_DAYS) {
        status0 = mon13_addDays(d, c, 0, &res0);
        status1 = mon13_addDays(&res0, c, 0, &res1);
    }
    if(m == MON13_ADD_MONTHS) {
        status0 = mon13_addMonths(d, c, 0, &res0);
        status1 = mon13_addMonths(&res0, c, 0, &res1);
    }
    if(m == MON13_ADD_YEARS || m == MON13_ADD_NONE) {
        status0 = mon13_addYears(d, c, 0, &res0);
        status1 = mon13_addYears(&res0, c, 0, &res1);
    }
    
    if(status0 || status1) {
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
    const struct mon13_Date* d = a1;
    enum mon13_AddMode m = (enum mon13_AddMode) ((uint64_t) a2);
    const struct mon13_Cal* c_yz = a3;
    const struct mon13_Cal* c = a4;
    int32_t offset = (int32_t) ((uint64_t)a5 % INT32_MAX);

    struct mon13_Date d_yz, res_yz, res0, res1;
    int status, status0, status1;
    status = mon13_convert(d, c, c_yz, &d_yz);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }

    if(m == MON13_ADD_DAYS) {
        status0 = mon13_addDays(&d_yz, c_yz, offset, &res_yz);
        status1 = mon13_addDays(d, c, offset, &res0);
    }
    if(m == MON13_ADD_MONTHS) {
        status0 = mon13_addMonths(&d_yz, c_yz, offset, &res_yz);
        status1 = mon13_addMonths(d, c, offset, &res0);
    }
    if(m == MON13_ADD_YEARS || m == MON13_ADD_NONE) {
        status0 = mon13_addYears(&d_yz, c_yz, offset, &res_yz);
        status1 = mon13_addYears(d, c, offset, &res0);
    }

    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    status = mon13_convert(&res_yz, c_yz, c, &res1);
    if(status) {
        if(status == MON13_ERROR_OVERFLOW) {
            return skip_import(((int64_t)res_yz.year)*366) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        else {
            return THEFT_TRIAL_FAIL;
        }
    }

    if(mon13_compare(&res0, &res1, c)) {
        return THEFT_TRIAL_FAIL;
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res add_no_year_zero(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const struct mon13_Date* d = a1;
    enum mon13_AddMode m = (enum mon13_AddMode) ((uint64_t) a2);
    const struct mon13_Cal* c = a3;
    int32_t offset = (int32_t) ((uint64_t)a4 % INT32_MAX);

    struct mon13_Date res;
    int status;
    if(m == MON13_ADD_DAYS) {
        status = mon13_addDays(d, c, offset, &res);
    }
    if(m == MON13_ADD_MONTHS) {
        status = mon13_addMonths(d, c, offset, &res);
    }
    if(m == MON13_ADD_YEARS || m == MON13_ADD_NONE) {
        status = mon13_addYears(d, c, offset, &res);
    }
    
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    return (res.year == 0) ? THEFT_TRIAL_FAIL : THEFT_TRIAL_PASS;
}

enum theft_trial_res add_result_valid(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const struct mon13_Date* d = a1;
    enum mon13_AddMode m = (enum mon13_AddMode) ((uint64_t) a2);
    const struct mon13_Cal* c = a3;
    int32_t offset = (int32_t) ((uint64_t)a4 % INT32_MAX);

    struct mon13_Date res;
    int status;
    if(m == MON13_ADD_DAYS) {
        status = mon13_addDays(d, c, offset, &res);
    }
    if(m == MON13_ADD_MONTHS) {
        status = mon13_addMonths(d, c, offset, &res);
    }
    if(m == MON13_ADD_YEARS || m == MON13_ADD_NONE) {
        status = mon13_addYears(d, c, offset, &res);
    }

    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    return mon13_valid(&res, c) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_month_feb_29(struct theft* t, void* a1, void* a2) {
    struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;

    int32_t offset;
    if(d->month == 2) {
        offset = 1;
        d->month = 1;
        d->day = 29;
    }
    else {
        offset = 2 - d->month;
        d->day = 29;
    }


    struct mon13_Date res;
    int status;
    status = mon13_addMonths(d, c, offset, &res);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }

    if(is_leap_day(res, c)) {
        if (res.year != d->year || res.month != 2 || res.day != 29) {
            return THEFT_TRIAL_FAIL;
        }
    }
    else {
        if (res.year != d->year || res.month != 3 || res.day != 1) {
            return THEFT_TRIAL_FAIL;
        }
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res add_null(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const struct mon13_Date* d = a1;
    enum mon13_AddMode m = (enum mon13_AddMode) ((uint64_t) a2);
    const struct mon13_Cal* c = a3;
    int32_t offset = (int32_t) ((uint64_t)a4 % INT32_MAX);

    struct mon13_Date res0;
    int status[9];
    status[0] = mon13_addDays(NULL, c, offset, &res0);
    status[1] = mon13_addMonths(NULL, c, offset, &res0);
    status[2] = mon13_addYears(NULL, c, offset, &res0);

    status[3] = mon13_addDays(d, NULL, offset, &res0);
    status[4] = mon13_addMonths(d, NULL, offset, &res0);
    status[5] = mon13_addYears(d, NULL, offset, &res0);
    
    status[6] = mon13_addDays(d, c, offset, NULL);
    status[7] = mon13_addMonths(d, c, offset, NULL);
    status[8] = mon13_addYears(d, c, offset, NULL);
    
    for(size_t i = 0; i < SIZEOF_ARR(status); i++) {
        if(!status[i]) {
            return THEFT_TRIAL_FAIL;
        }
    }

    return THEFT_TRIAL_PASS;
}

//Theft trials: diff
enum theft_trial_res diff_days_roundtrip(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d0 = a1;
    const struct mon13_Date* d1 = a2;
    const struct mon13_Cal* c = a3;

    int status;
    int64_t diff;
    status = mon13_diffDays(d0, d1, c, &diff);
    if(status) {
        return THEFT_TRIAL_SKIP;
    }

    struct mon13_Date sum;
    status = mon13_addDays(d1, c, diff, &sum);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }

    return equal_year_month_day(*d0, sum) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res diff_months_roundtrip(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d0 = a1;
    const struct mon13_Date* d1 = a2;
    const struct mon13_Cal* c = a3;

    if(d0->month == 0 || d0->day > 28 || d1->month == 0 || d1->day > 28) {
        return THEFT_TRIAL_SKIP;
    }

    int status;
    int64_t diff;
    status = mon13_diffMonths(d0, d1, c, &diff);
    if(status) {
        return THEFT_TRIAL_SKIP;
    }

    struct mon13_Date sum;
    status = mon13_addMonths(d1, c, diff, &sum);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }

    return (d0->year == sum.year && d0->month == sum.month) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res diff_years_roundtrip(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d0 = a1;
    const struct mon13_Date* d1 = a2;
    const struct mon13_Cal* c = a3;

    if(d0->month == 0 || d0->day > 28 || d1->month == 0 || d1->day > 28) {
        return THEFT_TRIAL_SKIP;
    }

    int status;
    int64_t diff;
    status = mon13_diffYears(d0, d1, c, &diff);
    if(status) {
        return THEFT_TRIAL_SKIP;
    }

    struct mon13_Date sum;
    status = mon13_addYears(d1, c, diff, &sum);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }

    return (d0->year == sum.year) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

//Theft trials: compare

enum theft_trial_res compare_nearby(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const struct mon13_Date* d = a1;
    int32_t offset = (int32_t) ((int64_t)a2 % (INT32_MAX/2));
    enum mon13_AddMode m = (enum mon13_AddMode) ((uint64_t)a3);
    const struct mon13_Cal* c = a4;

    offset = offset >= 0 ? offset : -offset;
    
    struct mon13_Date sum[5];
    int status[5];
    if(m == MON13_ADD_DAYS) {
        status[0] = mon13_addDays(d, c, 2*(-offset), &(sum[0]));
        status[1] = mon13_addDays(d, c, 1*(-offset), &(sum[1]));
        status[2] = mon13_addDays(d, c, 0*( offset), &(sum[2]));
        status[3] = mon13_addDays(d, c, 1*( offset), &(sum[3]));
        status[4] = mon13_addDays(d, c, 2*( offset), &(sum[4]));
    }
    if(m == MON13_ADD_MONTHS) {
        status[0] = mon13_addMonths(d, c, 2*(-offset), &(sum[0]));
        status[1] = mon13_addMonths(d, c, 1*(-offset), &(sum[1]));
        status[2] = mon13_addMonths(d, c, 0*( offset), &(sum[2]));
        status[3] = mon13_addMonths(d, c, 1*( offset), &(sum[3]));
        status[4] = mon13_addMonths(d, c, 2*( offset), &(sum[4]));
    }
    if(m == MON13_ADD_YEARS || m == MON13_ADD_NONE)
    {
        status[0] = mon13_addYears(d, c, 2*(-offset), &(sum[0]));
        status[1] = mon13_addYears(d, c, 1*(-offset), &(sum[1]));
        status[2] = mon13_addYears(d, c, 0*( offset), &(sum[2]));
        status[3] = mon13_addYears(d, c, 1*( offset), &(sum[3]));
        status[4] = mon13_addYears(d, c, 2*( offset), &(sum[4]));
    }

    for(size_t i = 0; i < SIZEOF_ARR(status); i++) {
        if(status[i]) {
            return (status[i] == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
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

enum theft_trial_res compare_random(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d0 = a1;
    const struct mon13_Date* d1 = a2;
    const struct mon13_Cal* c = a3;

    int res = mon13_compare(d0, d1, c);
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

enum theft_trial_res compare_null_cal(struct theft* t, void* a1, void* a2) {
    const struct mon13_Date* d0 = a1;
    const struct mon13_Date* d1 = a2;

    if(d0->month == 0 || d1->month == 0) {
        return THEFT_TRIAL_SKIP;
    }

    int res = mon13_compare(d0, d1, NULL);
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

enum theft_trial_res compare_null_d(struct theft* t, void* a1, void* a2) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;

    int res0 = mon13_compare(d, NULL, c);
    int res1 = mon13_compare(NULL, d, c);
    int res2 = mon13_compare(NULL, NULL, c);
    return (!res0 && !res1 && !res2) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

//Theft trials: extract
enum theft_trial_res extract_is_leap(struct theft* t, void* a1, void* a2) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;

    int leap_count = 0;
    for(int i = 1; i < 9; i++) {
        struct mon13_Date sum;
        int status = mon13_addYears(d, c, i, &sum);
        if(status) {
            return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        int64_t is_leap;
        if(mon13_extractIsLeapYear(&sum, c, &is_leap)) {
            is_leap = 0;
        }
        if(is_leap) {
            leap_count++;
        }
    }
    bool res = (leap_count == 1) || (leap_count == 2);
    return res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res extract_day_of_week_gr(struct theft* t, void* a1, void* a2) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    
    struct mon13_Date sum0, sum1;
    int status;
    status = mon13_addDays(d, c, 0, &sum0);
    if(status) {
        //Adding zero to a valid date should not cause errors!
        return THEFT_TRIAL_FAIL;
    }
    status = mon13_addDays(d, c, 1, &sum1);
    if(status) {
        return THEFT_TRIAL_FAIL; //Adding 1 unlikely to cause errors.
    }

    int64_t dow0;
    if(mon13_extractDayOfWeek(&sum0, c, &dow0)) {
        return THEFT_TRIAL_FAIL;
    }
    int64_t dow1;
    if(mon13_extractDayOfWeek(&sum1, c, &dow1)) {
        return THEFT_TRIAL_FAIL;
    }
    if(dow0 == MON13_SUNDAY) {
        return dow1 == MON13_MONDAY ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
    else {
        return dow1 == (dow0 + 1) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
}

enum theft_trial_res extract_day_of_week_tq(struct theft* t, void* a1, void* a2) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    
    struct mon13_Date sum;
    int status = mon13_addDays(d, c, 0, &sum);
    if(status) {
        return THEFT_TRIAL_FAIL; //Adding 0 should never cause error!
    }

    int64_t dow;
    if(mon13_extractDayOfWeek(&sum, c, &dow)) {
        return THEFT_TRIAL_FAIL;
    }
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
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;

    struct mon13_Date sum;
    int status = mon13_addDays(d, c, 1, &sum);
    if(status) {
        return THEFT_TRIAL_FAIL; //Adding 1 unlikely to cause error.
    }

    int64_t doy0;
    if(mon13_extractDayOfYear(d, c, &doy0)) {
        return THEFT_TRIAL_FAIL;
    }
    int64_t doy1;
    if(mon13_extractDayOfYear(&sum, c, &doy1)) {
        return THEFT_TRIAL_FAIL;
    }
    int64_t leap;
    if(mon13_extractIsLeapYear(d, c, &leap)) {
        leap = 0;
    }
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

enum theft_trial_res extract_day_of_year_split(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Date* d = a1;
    int32_t offset = (int32_t) ((uint64_t)a2 % INT32_MAX);
    const struct mon13_Cal* c = a3;

    offset = (offset > 0) ? offset : -offset;

    struct mon13_Date res0, res1, res2, res3;
    int status;
    status = mon13_addDays(d, c, offset, &res0);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    status = mon13_addDays(d, c, offset/2, &res1);
    if(status) {
        return THEFT_TRIAL_FAIL; //Already added offset, so why error for offset/2?
    }
    status = mon13_addDays(&res1, c, offset/2, &res2);
    if(status) {
        return THEFT_TRIAL_FAIL; //Already added offset, so why error for offset/2?
    }
    status = mon13_addDays(&res2, c, offset%2, &res3);
    if(status) {
        return THEFT_TRIAL_FAIL; //Already added offset, so why error for offset%2?
    }

    int64_t doy0;
    if(mon13_extractDayOfYear(&res0, c, &doy0)) {
        return THEFT_TRIAL_FAIL;
    }
    int64_t doy3;
    if(mon13_extractDayOfYear(&res3, c, &doy3)) {
        return THEFT_TRIAL_FAIL;
    }

    return doy0 == doy3 ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}


enum theft_trial_res extract_null(struct theft* t, void* a1, void* a2) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;

    int64_t res;
    int err_res[24];
    err_res[0] = mon13_extractDayOfYear(d, NULL, &res);
    err_res[1] = mon13_extractDayOfYear(NULL, c, &res);
    err_res[2] = mon13_extractDayOfYear(NULL, NULL, &res);
    err_res[3] = mon13_extractDayOfYear(d, c, NULL);

    err_res[4] = mon13_extractDayOfWeek(d, NULL, &res);
    err_res[5] = mon13_extractDayOfWeek(NULL, c, &res);
    err_res[6] = mon13_extractDayOfWeek(NULL, NULL, &res);
    err_res[7] = mon13_extractDayOfWeek(d, c, NULL);

    err_res[8]  = mon13_extractIsLeapYear(d, NULL, &res);
    err_res[9]  = mon13_extractIsLeapYear(NULL, c, &res);
    err_res[10] = mon13_extractIsLeapYear(NULL, NULL, &res);
    err_res[11] = mon13_extractIsLeapYear(d, c, NULL);

    err_res[12] = mon13_extractMjd(d, NULL, &res);
    err_res[13] = mon13_extractMjd(NULL, c, &res);
    err_res[14] = mon13_extractMjd(NULL, NULL, &res);
    err_res[15] = mon13_extractMjd(d, c, NULL);

    err_res[16] = mon13_extractUnix(d, NULL, &res);
    err_res[17] = mon13_extractUnix(NULL, c, &res);
    err_res[18] = mon13_extractUnix(NULL, NULL, &res);
    err_res[19] = mon13_extractUnix(d, c, NULL);

    err_res[20] = mon13_extractRd(d, NULL, &res);
    err_res[21] = mon13_extractRd(NULL, c, &res);
    err_res[22] = mon13_extractRd(NULL, NULL, &res);
    err_res[23] = mon13_extractRd(d, c, NULL);

    for(size_t i = 0; i < SIZEOF_ARR(err_res); i++) {
        if(!err_res[i]) {
            return THEFT_TRIAL_FAIL;
        }
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_percent(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
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
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);


    int64_t day;
    if(mon13_extractDayOfWeek(d, c, &day)) {
        return THEFT_TRIAL_FAIL;
    }

    char buf[100];
    memset(buf, placeholder, 100);

    int res = mon13_format(d, c, n, "%A", buf, 100);
    if(day == MON13_NO_WEEKDAY) {
        const char* expected_ic = contained_ic(buf, n, 100, placeholder);
        return format_res_check_nonblank(res, expected_ic) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
    else {
        const char* expected = contained(buf, n->weekday_list, 100, placeholder);
        return format_res_check(res, expected) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
}

enum theft_trial_res format_month(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);

    char buf[100];
    memset(buf, placeholder, 100);

    int res = mon13_format(d, c, n, "%B", buf, 100);
    if(d->month == 0) {
        const char* expected_ic = contained_ic(buf, n, 100, placeholder);
        return format_res_check_nonblank(res, expected_ic) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
    else {
        const char* expected = contained(buf, n->month_list, 100, placeholder);
        return format_res_check(res, expected) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
}

enum theft_trial_res format_day_of_month(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
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
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);

    char buf[100];
    memset(buf, placeholder, 100);

    int res = mon13_format(d, c, n, "%f", buf, 100);
    if(res != strlen(n->calendar_name)) {
        return THEFT_TRIAL_FAIL;
    }
    if(strncmp(buf, n->calendar_name, 50)) {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[res] != '\0' && buf[res + 1] != placeholder) {
        return THEFT_TRIAL_FAIL;
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_doy(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);

    int64_t doy;
    if(mon13_extractDayOfYear(d, c, &doy)) {
        return THEFT_TRIAL_FAIL;
    }
    
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
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
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
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
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
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);

    if(d->month == 0) {
        return THEFT_TRIAL_SKIP;
    }

    const char* expected = (d->year < 0) ? n->era_list[0] : n->era_list[1];

    char buf[100];
    memset(buf, placeholder, 100);

    int res = mon13_format(d, c, n, "%q", buf, 100);
    if(res != strlen(expected)) {
        return THEFT_TRIAL_FAIL;
    }
    if(strncmp(buf, expected, 50)) {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[res] != '\0' && buf[res + 1] != placeholder) {
        return THEFT_TRIAL_FAIL;
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_tab(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
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
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
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
    const struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char placeholder = (char) (((uint64_t)a4) % CHAR_MAX);
    
    char buf[100];
    memset(buf, placeholder, 100);

    int res = mon13_format(d, c, n, "%Y", buf, 100);
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
    int64_t u0 = ((int64_t)a1) % (((int64_t)INT32_MAX) * UNIX_DAY);
    const char placeholder = (char) (((uint64_t)a2) % CHAR_MAX);
    const char* fmt = a3;
    const struct mon13_Cal* c = &mon13_gregorian_year_zero;
    const struct mon13_NameList* n = &mon13_gregorian_names_en_US;

    time_t unix0 = u0;
    const struct tm* gmt_u = gmtime(&unix0);
    struct mon13_Date d;
    if(mon13_importUnix(c, &u0, &d)) {
        return THEFT_TRIAL_SKIP;
    }

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
    struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char* fmt = a4;
    char placeholder = ' ';

    char flag = fmt[1];
    int min_width = (flag == '-') ? 0 : fmt[2] - '0'; //Assumes ASCII or UTF8.
    int64_t targ;
    int status;
    switch(fmt[3]) {
        case 'd': targ = d->day; break;
        case 'j': {
            status = mon13_extractDayOfYear(d, c, &targ);
            if(status) {
                return THEFT_TRIAL_FAIL;
            }
            break;
        }
        case 'm': targ = d->month; break;
        case 'u': {
            status = mon13_extractDayOfWeek(d, c, &targ);
            if(status) {
                return THEFT_TRIAL_FAIL;
            }
            break;
        }
        case 'Y': d->year = d->year % 9999; targ = d->year; break;
        default: {
            return THEFT_TRIAL_FAIL;
        }
    }

    double digits = (targ == 0) ? 1 : floor(log10(targ)) + 1;

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
    struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
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

enum theft_trial_res format_simple_copy(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char* fmt = a4;
    char placeholder = '\t';

    char buf[ASCII_COPY_BUF];
    memset(buf, placeholder, ASCII_COPY_BUF);
    mon13_format(d, c, n, fmt, buf, ASCII_COPY_BUF);
    return !strncmp(fmt, buf, ASCII_COPY_BUF) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res format_null(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char* fmt = a4;
    char placeholder = '\t';

    char buf[ASCII_COPY_BUF];
    int status;
    memset(buf, placeholder, ASCII_COPY_BUF);
    status = mon13_format(NULL, c, n, fmt, buf, ASCII_COPY_BUF);
    if(status >= 0 || buf[0] != placeholder) {
        return THEFT_TRIAL_FAIL;
    }
    status = mon13_format(d, NULL, n, fmt, buf, ASCII_COPY_BUF);
    if(status >= 0 || buf[0] != placeholder) {
        return THEFT_TRIAL_FAIL;
    }
    //NULL namelist is tested seperately.
    status = mon13_format(d, c, n, NULL, buf, ASCII_COPY_BUF);
    if(status >= 0 || buf[0] != placeholder) {
        return THEFT_TRIAL_FAIL;
    }
    //NULL buf is tested seperately.
    //0 length is tested seperately.
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_dry_run(struct theft* t,  void* a1, void* a2, void* a3, void* a4) {
    struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char* fmt = a4;

    int status_null_buf = mon13_format(d, c, n, fmt, NULL, STRFTIME_BUF);
    if(status_null_buf < 0) {
        return THEFT_TRIAL_FAIL;
    }


    char fake_buf = 0;
    int status_buf_len_0 = mon13_format(d, c, n, fmt, &fake_buf, 0);
    if(status_buf_len_0 != status_null_buf) {
        return THEFT_TRIAL_FAIL;
    }
    if(fake_buf != 0) {
        return THEFT_TRIAL_FAIL;
    }

    char* buf = calloc(status_null_buf + 1, sizeof(char));
    int status = mon13_format(d, c, n, fmt, buf, status_null_buf + 1);
    free(buf);

    return (status == status_null_buf) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res format_truncate(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    struct mon13_Date* d = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char* fmt = a4;
    char placeholder = '\t';

    int buflen_0 = mon13_format(d, c, n, fmt, NULL, STRFTIME_BUF);
    if(buflen_0 == 0) {
        return THEFT_TRIAL_SKIP;
    }
    if(buflen_0 < 0) {
        return THEFT_TRIAL_FAIL;
    }
    char* buf_0 = malloc((buflen_0 + 1) * sizeof(char));
    memset(buf_0, placeholder, (buflen_0 + 1) * sizeof(char));
    int status_0 = mon13_format(d, c, n, fmt, buf_0, buflen_0 + 1);
    if(status_0 != buflen_0) {
        free(buf_0);
        return THEFT_TRIAL_FAIL;
    }

    int buflen_1 = (buflen_0 > 1) ? (buflen_0/2) : 1;
    char* buf_1 = malloc((buflen_1 + 1) * sizeof(char));
    memset(buf_1, placeholder, (buflen_1 + 1) * sizeof(char));
    int status_1 = mon13_format(d, c, n, fmt, buf_1, buflen_1 + 1);
    if(status_1 != buflen_0) {
        free(buf_0);
        free(buf_1);
        return THEFT_TRIAL_FAIL;
    }

    unsigned char* ubuf_0 = (unsigned char*)buf_0;
    bool ended_inside_code_point = (ubuf_0[buflen_1] > 0x7F);

    if(buf_1[buflen_1] != 0 && !ended_inside_code_point) {
        return THEFT_TRIAL_FAIL;
    }

    int cmp = memcmp(buf_0, buf_1, buflen_1);
    if(cmp == 0 || ended_inside_code_point) {
        free(buf_0);
        free(buf_1);
        return THEFT_TRIAL_PASS;
    }

    free(buf_0);
    free(buf_1);
    return THEFT_TRIAL_FAIL;
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

struct theft_type_info gr2ct_wiki_info = {
    .alloc = select_gr2ct_wiki, //nothing to free
    .print = print_known
};

struct theft_type_info gr2jl_wiki_info = {
    .alloc = select_gr2jl_wiki, //nothing to free
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

struct theft_type_info hl_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_holocene
};

struct theft_type_info ct_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_cotsworth
};

struct theft_type_info jl_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_julian
};

struct theft_type_info ps_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_positivist
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

struct theft_type_info hl_date_info = {
    .alloc = alloc_date,
    .free = theft_generic_free_cb,
    .print = print_date,
    .env = (void*)&mon13_holocene
};

struct theft_type_info ct_date_info = {
    .alloc = alloc_date,
    .free = theft_generic_free_cb,
    .print = print_date,
    .env = (void*)&mon13_cotsworth
};

struct theft_type_info jl_date_info = {
    .alloc = alloc_date,
    .free = theft_generic_free_cb,
    .print = print_date,
    .env = (void*)&mon13_julian
};

struct theft_type_info ps_date_info = {
    .alloc = alloc_date,
    .free = theft_generic_free_cb,
    .print = print_date,
    .env = (void*)&mon13_positivist
};

struct theft_type_info gr_year0_leap_info = {
    .alloc = alloc_leap_day,
    .free = theft_generic_free_cb,
    .print = print_date,
    .env = (void*)&mon13_gregorian_year_zero
};

struct theft_type_info gr_leap_info = {
    .alloc = alloc_leap_day,
    .free = theft_generic_free_cb,
    .print = print_date,
    .env = (void*)&mon13_gregorian
};

struct theft_type_info tq_year0_leap_info = {
    .alloc = alloc_leap_day,
    .free = theft_generic_free_cb,
    .print = print_date,
    .env = (void*)&mon13_tranquility_year_zero
};

struct theft_type_info jl_leap_info = {
    .alloc = alloc_leap_day,
    .free = theft_generic_free_cb,
    .print = print_date,
    .env = (void*)&mon13_julian
};

struct theft_type_info ps_leap_info = {
    .alloc = alloc_leap_day,
    .free = theft_generic_free_cb,
    .print = print_date,
    .env = (void*)&mon13_positivist
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

struct theft_type_info ascii_copy_fmt_info = {
    .alloc = alloc_ascii_copy_fmt,
    .free = theft_generic_free_cb,
    .print = print_s
};

struct theft_type_info utf8_copy_fmt_info = {
    .alloc= alloc_utf8_copy_fmt,
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

struct theft_type_info ct_name_en_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_cotsworth_names_en_US
};

struct theft_type_info jl_name_en_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_julian_names_en_US
};

struct theft_type_info ps_name_en_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_positivist_names_en_US
};

struct theft_type_info ps_name_fr_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_positivist_names_fr_FR
};


int main(int argc, char** argv) {
    theft_seed seed;
    if(argc > 1) {
        seed = strtoul(argv[1], NULL, 10);
    }
    else {
        seed = theft_seed_of_time();
    }
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
            .name = "mon13_import: Julian<->MJD",
            .prop3 = import_mjd,
            .type_info = {
                &jl_cal_info,
                &random_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_import: Positivist<->MJD",
            .prop3 = import_mjd,
            .type_info = {
                &ps_cal_info,
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
            .name = "mon13_import: Julian<->Unix",
            .prop3 = import_unix,
            .type_info = {
                &jl_cal_info,
                &random_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_import: Positivist<->Unix",
            .prop3 = import_unix,
            .type_info = {
                &ps_cal_info,
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
            .name = "mon13_import: compare with gmtime",
            .prop1 = import_unix_gmtime,
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
            .name = "mon13_import: Julian<->RD",
            .prop3 = import_rd,
            .type_info = {
                &jl_cal_info,
                &random_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_import: Positivist<->RD",
            .prop3 = import_rd,
            .type_info = {
                &ps_cal_info,
                &random_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_import: struct tm, localtime",
            .prop1 = import_c99_tm,
            .type_info = {&random_info},
            .seed = seed
        },
        {
            .name = "mon13_import: NULL args, Gregorian Year 0",
            .prop2 = import_null,
            .type_info = {
                &gr_year0_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_import: NULL args, Tranquility Year 0",
            .prop2 = import_null,
            .type_info = {
                &tq_year0_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_import: NULL args, Gregorian",
            .prop2 = import_null,
            .type_info = {
                &gr_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_import: NULL args, Tranquility",
            .prop2 = import_null,
            .type_info = {
                &tq_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_import: NULL args, Julian",
            .prop2 = import_null,
            .type_info = {
                &jl_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_import: NULL args, Positivist",
            .prop2 = import_null,
            .type_info = {
                &ps_cal_info,
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
            .name = "mon13_convert: Gregorian<->Cotsworth (Wikipedia)",
            .prop1 = convert_known,
            .type_info = {&gr2ct_wiki_info},
            .seed = seed,
            .trials = SIZEOF_ARR(gr2ct_wiki)
        },
        {
            .name = "mon13_convert: Gregorian<->Julian (Wikipedia)",
            .prop1 = convert_known,
            .type_info = {&gr2jl_wiki_info},
            .seed = seed,
            .trials = SIZEOF_ARR(gr2jl_wiki)
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
            .type_info = {&gr_date_info},
            .seed = seed
        },
        {
            .name = "mon13_convert: Tranquility Year 0 (Reverse)",
            .prop1 = convert_tq_year0_reverse,
            .type_info = {&tq_date_info},
            .seed = seed
        },
        {
            .name = "mon13_convert: NULL args, Gregorian Year 0",
            .prop3 = convert_null,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &tq_year0_date_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: NULL args, Tranquility Year 0",
            .prop3 = convert_null,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_cal_info,
                &gr_year0_date_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Same Calendar, Gregorian Year 0",
            .prop2 = convert_same_cal,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Same Calendar, Tranquility Year 0",
            .prop2 = convert_same_cal,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Holocene <-> Gregorian Year 0",
            .prop1 = convert_holocene,
            .type_info = {&gr_year0_date_info},
            .seed = seed
        },
        {
            .name = "mon13_convert: Gregorian Year 0 -> Cotsworth, Same Year",
            .prop3 = convert_same_year,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &ct_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Cotsworth -> Gregorian Year 0, Same Year",
            .prop3 = convert_same_year,
            .type_info = {
                &ct_date_info,
                &ct_cal_info,
                &gr_year0_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Gregorian Year 0 -> Cotsworth, Same Day of Year",
            .prop3 = convert_same_doy,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &ct_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Cotsworth -> Gregorian Year 0, Same Day of Year",
            .prop3 = convert_same_doy,
            .type_info = {
                &ct_date_info,
                &ct_cal_info,
                &gr_year0_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Gregorian Year 0 -> Positivist, Same Day of Year",
            .prop3 = convert_same_doy,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &ps_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Positivist -> Gregorian Year 0, Same Day of Year",
            .prop3 = convert_same_doy,
            .type_info = {
                &ps_date_info,
                &ps_cal_info,
                &gr_year0_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Gregorian Year 0 -> Cotsworth, Same Leap Year",
            .prop3 = convert_same_leap,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &ct_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Cotsworth -> Gregorian Year 0, Same Leap Year",
            .prop3 = convert_same_leap,
            .type_info = {
                &ct_date_info,
                &ct_cal_info,
                &gr_year0_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Gregorian Year 0 -> Positivist, Same Leap Year",
            .prop3 = convert_same_leap,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &ps_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_convert: Positivist -> Gregorian Year 0, Same Leap Year",
            .prop3 = convert_same_leap,
            .type_info = {
                &ps_date_info,
                &ps_cal_info,
                &gr_year0_cal_info,
            },
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
            .name = "mon13_add: Day Round Trip, Julian",
            .prop3 = add_day_roundtrip,
            .type_info = {
                &jl_date_info,
                &random_info,
                &jl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Day Round Trip, Positivist",
            .prop3 = add_day_roundtrip,
            .type_info = {
                &ps_date_info,
                &random_info,
                &ps_cal_info
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
            .name = "mon13_add: Day Split, Positivist",
            .prop3 = add_day_split,
            .type_info = {
                &ps_date_info,
                &random_info,
                &ps_cal_info
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
            .name = "mon13_add: 1 Month On Leap Day, Gregorian Year 0",
            .prop1 = add_1month_gr,
            .type_info = {&gr_year0_leap_info},
            .seed = seed
        },
        {
            .name = "mon13_add: 1 Month On Leap Day, Tranquility Year 0",
            .prop1 = add_1month_tq,
            .type_info = {&tq_year0_leap_info},
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
            .name = "mon13_add: Year, Cotsworth",
            .prop3 = add_year,
            .type_info = {
                &ct_date_info,
                &random_info,
                &ct_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Year, Positivist",
            .prop3 = add_year,
            .type_info = {
                &ps_date_info,
                &random_info,
                &ps_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Year On Leap Day, Gregorian Year 0",
            .prop3 = add_year_leap,
            .type_info = {
                &gr_year0_leap_info,
                &random_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Year On Leap Day, Tranquility Year 0",
            .prop3 = add_year_leap,
            .type_info = {
                &tq_year0_leap_info,
                &random_info,
                &tq_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Year On Leap Day, Gregorian",
            .prop3 = add_year_leap,
            .type_info = {
                &gr_leap_info,
                &random_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Year On Leap Day, Julian",
            .prop3 = add_year_leap,
            .type_info = {
                &jl_leap_info,
                &random_info,
                &jl_cal_info
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
            .name = "mon13_add: Zero, Positivist",
            .prop3 = add_zero,
            .type_info = {
                &ps_date_info,
                &add_mode_info,
                &ps_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Zero On Leap Day, Gregorian Year 0",
            .prop3 = add_zero,
            .type_info = {
                &gr_year0_leap_info,
                &add_mode_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Zero On Leap Day, Tranquility Year 0",
            .prop3 = add_zero,
            .type_info = {
                &tq_year0_leap_info,
                &add_mode_info,
                &tq_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Zero On Leap Day, Positivist",
            .prop3 = add_zero,
            .type_info = {
                &ps_leap_info,
                &add_mode_info,
                &ps_cal_info
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
            .name = "mon13_add: Zero, Holocene",
            .prop3 = add_zero,
            .type_info = {
                &hl_date_info,
                &add_mode_info,
                &hl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Zero, Cotsworth",
            .prop3 = add_zero,
            .type_info = {
                &ct_date_info,
                &add_mode_info,
                &ct_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Zero, Julian",
            .prop3 = add_zero,
            .type_info = {
                &jl_date_info,
                &add_mode_info,
                &jl_cal_info
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
            .name = "mon13_add: Holocene like Gregorian Year 0",
            .prop5 = add_like_other_cal,
            .type_info = {
                &hl_date_info,
                &add_mode_info,
                &gr_year0_cal_info,
                &hl_cal_info,
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
            .name = "mon13_add: No Year 0 for Julian",
            .prop4 = add_no_year_zero,
            .type_info = {
                &jl_date_info,
                &add_mode_info,
                &jl_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Valid Result, Gregorian Year 0",
            .prop4 = add_result_valid,
            .type_info = {
                &gr_year0_date_info,
                &add_mode_info,
                &gr_year0_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Valid Result, Tranquility Year 0",
            .prop4 = add_result_valid,
            .type_info = {
                &tq_date_info,
                &add_mode_info,
                &tq_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Valid Result, Holocene",
            .prop4 = add_result_valid,
            .type_info = {
                &hl_date_info,
                &add_mode_info,
                &hl_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Valid Result, Cotsworth",
            .prop4 = add_result_valid,
            .type_info = {
                &ct_date_info,
                &add_mode_info,
                &ct_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Valid Result, Positivist",
            .prop4 = add_result_valid,
            .type_info = {
                &ps_date_info,
                &add_mode_info,
                &ps_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Valid Result On Leap Day, Gregorian Year 0",
            .prop4 = add_result_valid,
            .type_info = {
                &gr_year0_leap_info,
                &add_mode_info,
                &gr_year0_cal_info,
                &random_info
            },
            .seed = seed,
        },
        {
            .name = "mon13_add: Valid Result On Leap Day, Tranquility Year 0",
            .prop4 = add_result_valid,
            .type_info = {
                &tq_year0_leap_info,
                &add_mode_info,
                &tq_year0_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Month Onto Feb 29, Gregorian Year 0",
            .prop2 = add_month_feb_29,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Month Onto Feb 29, Gregorian",
            .prop2 = add_month_feb_29,
            .type_info = {
                &gr_date_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: Month Onto Feb 29, Julian",
            .prop2 = add_month_feb_29,
            .type_info = {
                &jl_date_info,
                &jl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: NULL args, Gregorian",
            .prop4 = add_null,
            .type_info = {
                &gr_date_info,
                &add_mode_info,
                &gr_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: NULL args, Tranquility",
            .prop4 = add_null,
            .type_info = {
                &tq_date_info,
                &add_mode_info,
                &tq_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: NULL args, Gregorian Year 0",
            .prop4 = add_null,
            .type_info = {
                &gr_year0_date_info,
                &add_mode_info,
                &gr_year0_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_add: NULL args, Tranquility Year 0",
            .prop4 = add_null,
            .type_info = {
                &tq_year0_date_info,
                &add_mode_info,
                &tq_year0_cal_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip day, Gregorian Year 0",
            .prop3 = diff_days_roundtrip,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_date_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip months, Gregorian Year 0",
            .prop3 = diff_months_roundtrip,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_date_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip years, Gregorian Year 0",
            .prop3 = diff_years_roundtrip,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_date_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip day, Gregorian",
            .prop3 = diff_days_roundtrip,
            .type_info = {
                &gr_date_info,
                &gr_date_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip months, Gregorian",
            .prop3 = diff_months_roundtrip,
            .type_info = {
                &gr_date_info,
                &gr_date_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip years, Gregorian",
            .prop3 = diff_years_roundtrip,
            .type_info = {
                &gr_date_info,
                &gr_date_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip day, Tranquility Year 0",
            .prop3 = diff_days_roundtrip,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_date_info,
                &tq_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip months, Tranquility Year 0",
            .prop3 = diff_months_roundtrip,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_date_info,
                &tq_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip years, Tranquility Year 0",
            .prop3 = diff_years_roundtrip,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_date_info,
                &tq_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip day, Tranquility",
            .prop3 = diff_days_roundtrip,
            .type_info = {
                &tq_date_info,
                &tq_date_info,
                &tq_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip months, Tranquility",
            .prop3 = diff_months_roundtrip,
            .type_info = {
                &tq_date_info,
                &tq_date_info,
                &tq_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip years, Tranquility",
            .prop3 = diff_years_roundtrip,
            .type_info = {
                &tq_date_info,
                &tq_date_info,
                &tq_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip day, Cotsworth",
            .prop3 = diff_days_roundtrip,
            .type_info = {
                &ct_date_info,
                &ct_date_info,
                &ct_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip months, Cotsworth",
            .prop3 = diff_months_roundtrip,
            .type_info = {
                &ct_date_info,
                &ct_date_info,
                &ct_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip years, Cotsworth",
            .prop3 = diff_years_roundtrip,
            .type_info = {
                &ct_date_info,
                &ct_date_info,
                &ct_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip day, Holocene",
            .prop3 = diff_days_roundtrip,
            .type_info = {
                &hl_date_info,
                &hl_date_info,
                &hl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip months, Holocene",
            .prop3 = diff_months_roundtrip,
            .type_info = {
                &hl_date_info,
                &hl_date_info,
                &hl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip years, Holocene",
            .prop3 = diff_years_roundtrip,
            .type_info = {
                &hl_date_info,
                &hl_date_info,
                &hl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip day, Julian",
            .prop3 = diff_days_roundtrip,
            .type_info = {
                &jl_date_info,
                &jl_date_info,
                &jl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip months, Julian",
            .prop3 = diff_months_roundtrip,
            .type_info = {
                &jl_date_info,
                &jl_date_info,
                &jl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip years, Julian",
            .prop3 = diff_years_roundtrip,
            .type_info = {
                &jl_date_info,
                &jl_date_info,
                &jl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip day, Positivist",
            .prop3 = diff_days_roundtrip,
            .type_info = {
                &ps_date_info,
                &ps_date_info,
                &ps_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip months, Positivist",
            .prop3 = diff_months_roundtrip,
            .type_info = {
                &ps_date_info,
                &ps_date_info,
                &ps_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_diff: Roundtrip years, Positivist",
            .prop3 = diff_years_roundtrip,
            .type_info = {
                &ps_date_info,
                &ps_date_info,
                &ps_cal_info
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
            .prop3 = compare_random,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_date_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_compare: Random, Gregorian",
            .prop3 = compare_random,
            .type_info = {
                &gr_date_info,
                &gr_date_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_compare: Random, Cotsworth",
            .prop3 = compare_random,
            .type_info = {
                &ct_date_info,
                &ct_date_info,
                &ct_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_compare: Random, Julian",
            .prop3 = compare_random,
            .type_info = {
                &jl_date_info,
                &jl_date_info,
                &jl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_compare: NULL cal, Gregorian Year 0",
            .prop2 = compare_null_cal,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_date_info
            },
            .seed = seed
        },
        {
            .name = "mon13_compare: NULL cal, Gregorian",
            .prop2 = compare_null_cal,
            .type_info = {
                &gr_date_info,
                &gr_date_info
            },
            .seed = seed
        },
        {
            .name = "mon13_compare: NULL cal, Tranquility Year 0",
            .prop2 = compare_null_cal,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_date_info
            },
            .seed = seed
        },
        {
            .name = "mon13_compare: NULL cal, Tranquility",
            .prop2 = compare_null_cal,
            .type_info = {
                &tq_date_info,
                &tq_date_info
            },
            .seed = seed
        },
        {
            .name = "mon13_compare: NULL d, Gregorian Year 0",
            .prop2 = compare_null_d,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_compare: NULL d, Gregorian",
            .prop2 = compare_null_d,
            .type_info = {
                &gr_date_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_compare: NULL d, Tranquility Year 0",
            .prop2 = compare_null_d,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_compare: NULL d, Tranquility",
            .prop2 = compare_null_d,
            .type_info = {
                &tq_date_info,
                &tq_cal_info
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
            .prop2 = extract_day_of_week_gr,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_extract: DAY_OF_WEEK, Tranquility Year 0",
            .prop2 = extract_day_of_week_tq,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_extract: DAY_OF_WEEK, Gregorian",
            .prop2 = extract_day_of_week_gr,
            .type_info = {
                &gr_date_info,
                &gr_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_extract: DAY_OF_WEEK, Tranquility",
            .prop2 = extract_day_of_week_tq,
            .type_info = {
                &tq_date_info,
                &tq_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_extract: DAY_OF_WEEK, Julian",
            .prop2 = extract_day_of_week_gr,
            .type_info = {
                &jl_date_info,
                &jl_cal_info,
            },
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
            .name = "mon13_extract: NULL args, Gregorian Year 0",
            .prop2 = extract_null,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_extract: NULL args, Tranquility Year 0",
            .prop2 = extract_null,
            .type_info = {
                &tq_year0_date_info,
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
            .name = "mon13_format: %A, Tranquility Year 0 (en_US) (Leap Day)",
            .prop4 = format_weekday,
            .type_info = {
                &tq_year0_leap_info,
                &tq_year0_cal_info,
                &tq_name_en_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: %A, Cotsworth (en_US)",
            .prop4 = format_weekday,
            .type_info = {
                &ct_date_info,
                &ct_cal_info,
                &ct_name_en_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: %A, Julian (en_US)",
            .prop4 = format_weekday,
            .type_info = {
                &jl_date_info,
                &jl_cal_info,
                &jl_name_en_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: %A, Positivist (en_US)",
            .prop4 = format_weekday,
            .type_info = {
                &ps_date_info,
                &ps_cal_info,
                &ps_name_en_info,
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
            .name = "mon13_format: %B, Gregorian Year 0 (en_US) (Leap Day)",
            .prop4 = format_month,
            .type_info = {
                &gr_year0_leap_info,
                &gr_year0_cal_info,
                &gr_name_en_info,
                &random_info
            }
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
            .name = "mon13_format: %B, Tranquility Year 0 (en_US) (Leap Day)",
            .prop4 = format_month,
            .type_info = {
                &tq_year0_leap_info,
                &tq_year0_cal_info,
                &tq_name_en_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: %B, Cotsworth (en_US)",
            .prop4 = format_month,
            .type_info = {
                &ct_date_info,
                &ct_cal_info,
                &ct_name_en_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: %B, Julian (en_US)",
            .prop4 = format_month,
            .type_info = {
                &jl_date_info,
                &jl_cal_info,
                &jl_name_en_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: %B, Positivist (en_US)",
            .prop4 = format_month,
            .type_info = {
                &ps_date_info,
                &ps_cal_info,
                &ps_name_en_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: %B, Positivist (fr_FR)",
            .prop4 = format_month,
            .type_info = {
                &ps_date_info,
                &ps_cal_info,
                &ps_name_en_info,
                &random_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: %d, Gregorian Year 0 (en_US)",
            .prop4 = format_day_of_month,
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
            .prop4 = format_day_of_month,
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
            .prop4 = format_day_of_month,
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
            .name = "mon13_format: NULL names, Gregorian Year 0 (en_US)",
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
            .name = "mon13_format: NULL names, Gregorian Year 0 (fr_FR)",
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
            .name = "mon13_format: NULL names, Tranquility Year 0 (en_US)",
            .prop4 = format_numeric_null,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_cal_info,
                &tq_name_en_info,
                &numeric_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: ASCII copy, Gregorian Year 0 (en_US)",
            .prop4 = format_simple_copy,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &gr_name_en_info,
                &ascii_copy_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: ASCII copy, Gregorian Year 0 (fr_FR)",
            .prop4 = format_simple_copy,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &gr_name_fr_info,
                &ascii_copy_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: ASCII copy, Tranquility Year 0 (en_US)",
            .prop4 = format_simple_copy,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_cal_info,
                &tq_name_en_info,
                &ascii_copy_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: UTF8 copy, Gregorian Year 0 (en_US)",
            .prop4 = format_simple_copy,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &gr_name_en_info,
                &utf8_copy_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: UTF8 copy, Gregorian Year 0 (fr_FR)",
            .prop4 = format_simple_copy,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &gr_name_fr_info,
                &utf8_copy_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: UTF8 copy, Tranquility Year 0 (en_US)",
            .prop4 = format_simple_copy,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_cal_info,
                &tq_name_en_info,
                &utf8_copy_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: NULL args, Gregorian Year 0 (en_US)",
            .prop4 = format_null,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &gr_name_en_info,
                &utf8_copy_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: NULL args, Gregorian Year 0 (fr_FR)",
            .prop4 = format_null,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &gr_name_fr_info,
                &utf8_copy_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: NULL args, Tranquility Year 0 (en_US)",
            .prop4 = format_null,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_cal_info,
                &tq_name_en_info,
                &utf8_copy_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: dry run, Gregorian Year 0 (en_US)",
            .prop4 = format_dry_run,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &gr_name_en_info,
                &strftime_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: dry run, Gregorian Year 0 (fr_FR)",
            .prop4 = format_dry_run,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &gr_name_fr_info,
                &strftime_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: dry run, Tranquility Year 0 (en_US)",
            .prop4 = format_dry_run,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_cal_info,
                &tq_name_en_info,
                &strftime_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: truncate, Gregorian Year 0 (en_US)",
            .prop4 = format_truncate,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &gr_name_en_info,
                &strftime_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: truncate, Gregorian Year 0 (fr_FR)",
            .prop4 = format_truncate,
            .type_info = {
                &gr_year0_date_info,
                &gr_year0_cal_info,
                &gr_name_fr_info,
                &strftime_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: truncate, Tranquility Year 0 (en_US)",
            .prop4 = format_truncate,
            .type_info = {
                &tq_year0_date_info,
                &tq_year0_cal_info,
                &tq_name_en_info,
                &strftime_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: truncate, Positivist (en_US)",
            .prop4 = format_truncate,
            .type_info = {
                &ps_date_info,
                &ps_cal_info,
                &ps_name_en_info,
                &strftime_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: truncate, Positivist (fr_FR)",
            .prop4 = format_truncate,
            .type_info = {
                &ps_date_info,
                &ps_cal_info,
                &ps_name_fr_info,
                &strftime_fmt_info
            },
            .seed = seed
        }
    };
    size_t prop_count = SIZEOF_ARR(config);
    size_t pass = 0;
    size_t fail = 0;
    size_t skip = 0;
    size_t error = 0;
    size_t other = 0;
    for(size_t i = 0; i < prop_count; i++) {
        enum theft_run_res test_res = theft_run(&(config[i]));
        switch(test_res) {
            case THEFT_RUN_PASS: pass++; break;
            case THEFT_RUN_FAIL: fail++; break;
            case THEFT_RUN_SKIP: skip++; break;
            case THEFT_RUN_ERROR: error++; return 1;
            default: other++;
        }
    }
    printf("\n");
    printf("SEED: %lu\n", seed);
    printf("\tPROP:\t%lu\n", prop_count);
    printf("\tPASS:\t%lu\n", pass);
    printf("\tFAIL:\t%lu\n", fail);
    printf("\tSKIP:\t%lu\n", skip);
    printf("\tERROR:\t%lu\n", error);
    printf("\tOTHER:\t%lu\n", other);
    return 0;
}