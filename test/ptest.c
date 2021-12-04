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

//Helpers
bool skip_import(int64_t x) {
    return (x > (INT32_MAX/2)) || (x < -(INT32_MAX/2));
}

bool add_with_overflow(int32_t a, int32_t b, int32_t* res) {
    int64_t raw = ((int64_t)a) + ((int64_t)b);
    if(raw < -((int64_t)INT32_MAX) || raw > ((int64_t)INT32_MAX - 1)) {
        return true;
    }
    if(res != NULL) {
        *res = (int32_t) (raw % INT32_MAX);
    }
    return false;
}

uint8_t last_day_of_month_gr(uint8_t month, int leap) {
    switch(month) {
        case 2: return (leap ? 29 : 28);
        case 4:
        case 6:
        case 9:
        case 11: return 30;
        default: return 31;
    }
}

bool add_1day_common(int32_t y0, int32_t y1, uint8_t m0, uint8_t m1, uint8_t d0, uint8_t d1) {
    if(d0 == 28 || d0 == 29) {
        bool good_d = (d1 == 1);
        bool good_m = (m1 == (m0 + 1));
        bool good_y = (y1 == y0);
        return good_d && good_m && good_y;
    }
    else {
        bool good_d = (d1 == (d0 + 1));
        bool good_ym = (y1 == y0 && m1 == m0);
        return good_d && good_ym;
    }
}

bool add_1day_yearend(int32_t y0, int32_t y1, uint8_t m0, uint8_t m1, uint8_t d0, uint8_t d1, bool no_yz) {
    bool good_md = (m1 == 1 && d1 == 1);
    bool good_y = (y1 == (y0 + 1));
    if(no_yz) {
        bool good_yz = (y1 == 1 && y0 == -1);
        return good_md && (good_y || good_yz);
    }
    else {
        return good_md && good_y;
    }
}

const char* contained(char* needle, char** haystack, size_t maxlen, char placeholder) {
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

const char* contained_ic(char* needle, struct mon13_NameList* nlist, size_t maxlen, char placeholder) {
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

size_t strlistlen(char** string_list) {
    if(string_list == NULL) {
        return 0;
    }
    size_t i = 0;
    while(string_list[i] != NULL) {
        i++;
    }
    return i;
}

bool valid_names_check(void* a1, void* a2, size_t w, size_t m, size_t e, size_t i, size_t a) {
    const struct mon13_Cal* c = a1;
    const struct mon13_NameList* n = a2;
    int res = mon13_validNameList(c, n);
    if(w != strlistlen(n->weekday_list)) {
        return !res;
    }
    if(m != strlistlen(n->month_list)) {
        return !res;
    }
    if(e != strlistlen(n->era_list)) {
        return !res;
    }
    if(i != strlistlen(n->intercalary_list)) {
        return !res;
    }
    if(a != strlistlen(n->alt_intercalary_list)) {
        return !res;
    }
    return res;
}

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
        fprintf(f, "%s", mon13_names_en_US_gregorian.calendar_name);
    }
    else if(c == &mon13_gregorian_year_zero) {
        fprintf(f, "%s (Year Zero)", mon13_names_en_US_gregorian.calendar_name);
    }
    else if(c == &mon13_tranquility) {
        fprintf(f, "%s", mon13_names_en_US_tranquility.calendar_name);
    }
    else if(c == &mon13_tranquility_year_zero) {
        fprintf(f, "%s (Year Zero)", mon13_names_en_US_tranquility.calendar_name);
    }
    else if(c == &mon13_holocene) {
        fprintf(f, "%s", mon13_names_en_US_holocene.calendar_name);
    }
    else if(c == &mon13_cotsworth) {
        fprintf(f, "%s", mon13_names_en_US_cotsworth.calendar_name);
    }
    else if(c == &mon13_julian) {
        fprintf(f, "%s", mon13_names_en_US_julian.calendar_name);
    }
    else if(c == &mon13_positivist) {
        fprintf(f, "%s", mon13_names_en_US_positivist.calendar_name);
    }
    else if(c == &mon13_symmetry454) {
        fprintf(f, "%s", mon13_names_en_US_symmetry454.calendar_name);
    }
    else if(c == &mon13_symmetry010) {
        fprintf(f, "%s", mon13_names_en_US_symmetry010.calendar_name);
    }
    else if(c == &mon13_ancient_egyptian) {
        fprintf(f, "%s", mon13_names_en_US_ancient_egyptian.calendar_name);
    }
    else if(c == &mon13_french_revolutionary_romme) {
        fprintf(f, "%s Romme", mon13_names_en_GB_french_revolutionary.calendar_name);
    }
    else if(c == &mon13_french_revolutionary_romme_sub1) {
        fprintf(f, "%s Romme-1", mon13_names_en_GB_french_revolutionary.calendar_name);
    }
    else {
        fprintf(f, "UNKNOWN");
    }
}

void print_name_cal(FILE* f, const void* instance, void* env) {
    const struct name_cal* nc = instance;
    print_cal(f, nc->c, NULL);
    fprintf(f, " ");
    if(nc->n == &mon13_names_fr_FR_gregorian) {
        fprintf(f, "fr_FR");
    }
    else if(nc->n == &mon13_names_fr_FR_julian) {
        fprintf(f, "fr_FR");
    }
    else if(nc->n == &mon13_names_fr_FR_positivist) {
        fprintf(f, "fr_FR");
    }
    else if(nc->n == &mon13_names_fr_FR_french_revolutionary) {
        fprintf(f, "fr_FR");
    }
    else if(nc->n == &mon13_names_en_GB_french_revolutionary) {
        fprintf(f, "en_GB");
    }
    else if(nc->n == &mon13_names_en_GB_french_revolutionary_joke) {
        fprintf(f, "en_GB (joke)");
    }
    else {
        fprintf(f, "en_US");
    }
}

void print_s(FILE* f, const void* instance, void* env) {
    const char* s = instance;
    fprintf(f, "%s", s);
}

void print_custom_namelist(FILE* f, const void* instance, void* env) {
    const struct mon13_NameList* n = instance;
    fprintf(
        f,
        "[m: %lu, w: %lu, e: %lu, i: %lu, a: %lu, c: %u]",
        strlistlen(n->month_list),
        strlistlen(n->weekday_list),
        strlistlen(n->era_list),
        strlistlen(n->intercalary_list),
        strlistlen(n->alt_intercalary_list),
        strlen(n->calendar_name)
        );
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

enum theft_alloc_res select_gr2ps_positivists_org(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2ps_positivists_org));
    *instance = (void*)&gr2ps_positivists_org[i];
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_gr2mjd_nasa(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2mjd_nasa));
    *instance = (void*)&gr2mjd_nasa[i];
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_eg2mjd_cctue(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(eg2mjd_cctue));
    *instance = (void*)&eg2mjd_cctue[i];
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_gr2sym454_irv(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2sym454_irv));
    *instance = (void*)&gr2sym454_irv[i];
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_gr2fr_brit(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2fr_brit));
    *instance = (void*)&gr2fr_brit[i];
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_gr2fr_wiki(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(gr2fr_wiki));
    *instance = (void*)&gr2fr_wiki[i];
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_env(struct theft* t, void* env, void** instance) {
    *instance = env;
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_random_cal(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(cal_list));
    *instance = (void*)cal_list[i];
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res select_random_name_cal(struct theft* t, void* env, void** instance) {
    uint64_t i = theft_random_choice(t, SIZEOF_ARR(name_cal_list));
    *instance = (void*)&name_cal_list[i];
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

enum theft_alloc_res alloc_ascii_string(struct theft* t, void* env, void** instance) {
    char* exclude = env;
    size_t fmt_len = (size_t) theft_random_choice(t, ASCII_COPY_BUF-1);
    fmt_len = (fmt_len > 0) ? fmt_len : 1;
    char* res = malloc(fmt_len * sizeof(char));
    if(res == NULL) {
        return THEFT_ALLOC_ERROR;
    }
    for(int i = 0; i < (fmt_len - 1); i++) { //Leave 1 more, for null character
        res[i] = theft_random_choice(t, 94) + 32; //avoid control codes
        if(res[i] < 32 || res[i] == *exclude) {
            res[i] = ' ';
        }
    }
    res[fmt_len - 1] = '\0';
    *instance = res;
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_ascii_copy_fmt(struct theft* t, void* env, void** instance) {
    char exclude = '%';
    return alloc_ascii_string(t, &exclude, instance);
}

enum theft_alloc_res alloc_ascii_string_list(struct theft* t, void* env, void** instance) {
    char exclude = '\0';
    size_t len = (size_t) theft_random_choice(t, STRING_LIST_LEN-1);
    len = (len > 0) ? len : 1;
    char** res = malloc(len * sizeof(char*));
    if(res == NULL) {
        return THEFT_ALLOC_ERROR;
    }
    for(int i = 0; i < (len - 1); i++) { //Leave 1 more, for null sentinel
        if(alloc_ascii_string(t, &exclude, (void**)&(res[i])) != THEFT_ALLOC_OK) {
            return THEFT_ALLOC_ERROR;
        }
    }
    res[len - 1] = NULL;
    *instance = res;
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_ascii_name_list(struct theft* t, void* env, void** instance) {
    struct mon13_NameList* res = malloc(sizeof(struct mon13_NameList));
    if(alloc_ascii_string_list(t, env, (void**) &(res->month_list)) != THEFT_ALLOC_OK) {
        return THEFT_ALLOC_ERROR;
    }
    if(alloc_ascii_string_list(t, env, (void**) &(res->weekday_list)) != THEFT_ALLOC_OK) {
        return THEFT_ALLOC_ERROR;
    }
    if(alloc_ascii_string_list(t, env, (void**) &(res->era_list)) != THEFT_ALLOC_OK) {
        return THEFT_ALLOC_ERROR;
    }

    if(theft_random_choice(t, 2)) {
        if(alloc_ascii_string_list(t, env, (void**) &(res->intercalary_list)) != THEFT_ALLOC_OK) {
            return THEFT_ALLOC_ERROR;
        }
        if(alloc_ascii_string_list(t, env, (void**) &(res->alt_intercalary_list)) != THEFT_ALLOC_OK) {
            return THEFT_ALLOC_ERROR;
        }
    } else {
        res->intercalary_list = NULL;
        res->alt_intercalary_list = NULL;
    }

    char exclude = '\0';
    if(alloc_ascii_string(t, &exclude, (void**) &(res->calendar_name)) != THEFT_ALLOC_OK) {
        return THEFT_ALLOC_ERROR;
    }

    *instance = res;
    return THEFT_ALLOC_OK;
}

enum theft_alloc_res alloc_emoji_copy_fmt(struct theft* t, void* env, void** instance) {
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

//Theft free
void free_string_list(void* instance, void* env) {
    char** string_list = instance;
    if(string_list != NULL) {
        for(int i = 0; string_list[i] != NULL; i++) {
            free(string_list[i]);
        }
        free(string_list);
    }
}

void free_name_list(void* instance, void* env) {
    struct mon13_NameList* names = instance;
    free_string_list(names->month_list, env);
    free_string_list(names->weekday_list, env);
    free_string_list(names->era_list, env);
    free_string_list(names->intercalary_list, env);
    free_string_list(names->alt_intercalary_list, env);
    free(names->calendar_name);
}

//Theft trials
enum theft_trial_res unix_basic(struct theft* t, void* a1, void* a2) {
    const int64_t* u0 = a1;
    const int32_t* offset = a2;

    int32_t mjd0;
    int status0 = mon13_mjdFromUnix(*u0, &mjd0);
    if(status0) {
        if(status0 == MON13_ERROR_OVERFLOW) {
            return skip_import(*u0/(UNIX_DAY)) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        else {
            return THEFT_TRIAL_FAIL;
        }
    }

    int32_t mjd1;
    if(add_with_overflow(mjd0, *offset, &mjd1)) {
        return THEFT_TRIAL_SKIP;
    }

    int64_t u1;
    if(mon13_mjdToUnix(mjd1, &u1)) {
        return THEFT_TRIAL_FAIL;
    }

    int64_t udiff = (u1 - *u0);
    int64_t udiff_days =  udiff / (UNIX_DAY);
    int64_t u0_mod = *u0 % UNIX_DAY;

    if(udiff_days == *offset) {
        return THEFT_TRIAL_PASS;
    }
    if(u0_mod != 0 && udiff_days == (*offset - 1)) {
        return THEFT_TRIAL_PASS;
    }
    return THEFT_TRIAL_FAIL;
}

enum theft_trial_res unix_gmtime(struct theft* t, void* a1) {
    const int64_t* u0 = a1;
    const struct mon13_Cal* c = &mon13_gregorian_year_zero;

    time_t unix0 = *u0;
    const struct tm* gmt_u = gmtime(&unix0);

    int32_t mjd0;
    if(mon13_mjdFromUnix(*u0, &mjd0)) {
        return THEFT_TRIAL_SKIP;
    }

    int32_t y;
    uint8_t m, d;
    if(mon13_mjdToYmd(mjd0, c, &y, &m, &d)) {
        return THEFT_TRIAL_SKIP;
    }

    if(d != gmt_u->tm_mday) {
        return THEFT_TRIAL_FAIL;
    }
    if((m - 1) != gmt_u->tm_mon) {
        return THEFT_TRIAL_FAIL;
    }
    if((y - 1900) != gmt_u->tm_year) {
        return THEFT_TRIAL_FAIL;
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res c99tm_basic(struct theft* t, void* a1) {
    const int64_t* u0 = a1;
    const struct mon13_Cal* c = &mon13_gregorian_year_zero;

    time_t unix0 = *u0;
    const struct tm* tm0 = gmtime(&unix0);

    int32_t mjd0;
    if(mon13_mjdFromUnix(*u0, &mjd0)) {
        return THEFT_TRIAL_SKIP;
    }

    struct tm tm1;
    if(mon13_mjdToC99Tm(mjd0, c, &tm1)) {
        return THEFT_TRIAL_SKIP;
    }
    if(tm0->tm_mday != tm1.tm_mday) {
        return THEFT_TRIAL_FAIL;
    }
    if(tm0->tm_mon != tm1.tm_mon) {
        return THEFT_TRIAL_FAIL;
    }
    if(tm0->tm_year != tm1.tm_year) {
        return THEFT_TRIAL_FAIL;
    }
    if(tm0->tm_wday != tm1.tm_wday) {
        return THEFT_TRIAL_FAIL;
    }
    if(tm0->tm_yday != tm1.tm_yday) {
        return THEFT_TRIAL_FAIL;
    }

    int32_t mjd1;
    if(mon13_mjdFromC99Tm(c, &tm1, &mjd1)) {
        return THEFT_TRIAL_FAIL;
    }
    return (mjd0 == mjd1) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res rd_basic(struct theft* t, void* a1, void* a2) {
    const int32_t* rd0 = a1;
    const int32_t* offset = a2;

    if(add_with_overflow(*rd0, *offset, NULL)) {
        return THEFT_TRIAL_SKIP;
    }

    int32_t mjd0;
    int status0 = mon13_mjdFromRd(*rd0, &mjd0);
    if(status0) {
        if(status0 == MON13_ERROR_OVERFLOW) {
            return skip_import(*rd0) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        else {
            return THEFT_TRIAL_FAIL;
        }
    }

    int32_t mjd1;
    if(add_with_overflow(mjd0, *offset, &mjd1)) {
        return THEFT_TRIAL_SKIP;
    }

    int32_t rd1;
    int status1 = mon13_mjdToRd(mjd1, &rd1);
    if(status1) {
        return THEFT_TRIAL_FAIL;
    }

    return ((rd1 - *rd0) == *offset) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res fromYmd_known(struct theft* t, void* test_input) {
    const struct known_convert_date* kcd = test_input;

    int32_t mjd0;
    int status0 = mon13_mjdFromYmd(
        kcd->c0, kcd->d0.year, kcd->d0.month, kcd->d0.day, &mjd0
    );

    int32_t mjd1;
    int status1 = mon13_mjdFromYmd(
        kcd->c1, kcd->d1.year, kcd->d1.month, kcd->d1.day, &mjd1
    );

    if(status0 || status1) {
        return THEFT_TRIAL_FAIL;
    }

    return (mjd0 == mjd1) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res fromYmd_known_mjd(struct theft* t, void* test_input) {
    const struct known_convert_mjd* kcm = test_input;

    int32_t mjd;
    int status0 = mon13_mjdFromYmd(
        kcm->c, kcm->d.year, kcm->d.month, kcm->d.day, &mjd
    );

    if(status0) {
        return THEFT_TRIAL_FAIL;
    }

    return (mjd == kcm->mjd) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res ymd_roundtrip(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;

    int32_t y;
    uint8_t m, d;

    int status = mon13_mjdToYmd(*mjd, c, &y, &m, &d);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int32_t mjd_res;
    if(mon13_mjdFromYmd(c, y, m, d, &mjd_res)) {
        return THEFT_TRIAL_FAIL; //Since we already did the opposite.
    }
    return (*mjd == mjd_res) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res ymd_gr_hl(struct theft* t, void* a1) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* cal_gr = &mon13_gregorian_year_zero;
    const struct mon13_Cal* cal_hl = &mon13_holocene;

    int32_t y_hl, y_gr;
    uint8_t m_hl, m_gr, d_hl, d_gr;

    int status0 = mon13_mjdToYmd(*mjd, cal_gr, &y_gr, &m_gr, &d_gr);
    int status1 = mon13_mjdToYmd(*mjd, cal_hl, &y_hl, &m_hl, &d_hl);

    if(status0 || status1) {
        if(status0 == MON13_ERROR_OVERFLOW || status1 == MON13_ERROR_OVERFLOW) {
            return THEFT_TRIAL_SKIP;
        }
        else {
            return THEFT_TRIAL_FAIL;
        }
    }

    bool md_check = (m_hl == m_gr) && (d_hl == d_gr);
    bool y_check = (y_hl == (y_gr + 10000));

    return (md_check && y_check) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res mjd_res_null(struct theft* t, void* a1, void* a2) {
    const int32_t* daycount = a1;
    const struct mon13_Cal* c = a2;
    struct tm tm0;

    if(mon13_mjdFromYmd(c, *daycount, 1, 1, NULL) != MON13_ERROR_NULL_RESULT) {
        return THEFT_TRIAL_FAIL;
    }
    if(mon13_mjdFromC99Tm(c, &tm0, NULL) != MON13_ERROR_NULL_RESULT) {
        return THEFT_TRIAL_FAIL;
    }
    if(mon13_mjdFromUnix(*daycount, NULL) != MON13_ERROR_NULL_RESULT) {
        return THEFT_TRIAL_FAIL;
    }
    if(mon13_mjdFromRd(*daycount, NULL) != MON13_ERROR_NULL_RESULT) {
        return THEFT_TRIAL_FAIL;
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res ymd_null(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;

    int32_t y0, y1;
    uint8_t m0, m1, d0, d1;

    if(mon13_mjdToYmd(*mjd, NULL, &y0, &m0, &d0) != MON13_ERROR_NULL_CALENDAR) {
        return THEFT_TRIAL_FAIL;
    }

    int status = mon13_mjdToYmd(*mjd, c, &y0, &m0, &d0);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    if(mon13_mjdToYmd(*mjd, c, &y1, NULL, NULL)) {
        return THEFT_TRIAL_FAIL; //If above works, this should work.
    }
    if(mon13_mjdToYmd(*mjd, c, NULL, &m1, NULL)) {
        return THEFT_TRIAL_FAIL;
    }
    if(mon13_mjdToYmd(*mjd, c, NULL, NULL, &d1)) {
        return THEFT_TRIAL_FAIL;
    }

    return ((y1 == y0) && (m1 == m0) && (d1 == d0)) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res ymd_yz(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c_yz = a2;
    const struct mon13_Cal* c_no_yz = a3;

    int32_t y_yz, y_no_yz;
    uint8_t m_yz, m_no_yz, d_yz, d_no_yz;
    int status = mon13_mjdToYmd(*mjd, c_yz, &y_yz, &m_yz, &d_yz);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    if(mon13_mjdToYmd(*mjd, c_no_yz, &y_no_yz, &m_no_yz, &d_no_yz)) {
        return THEFT_TRIAL_FAIL; //If above works, this should work.
    }

    if(y_no_yz == 0) {
        return THEFT_TRIAL_FAIL;
    }

    if(y_yz < 1 && y_no_yz == (y_yz - 1)) {
        return THEFT_TRIAL_PASS;
    }
    else if(y_yz >= 1 && y_no_yz == y_yz && m_no_yz == m_yz && d_no_yz == d_yz) {
        return THEFT_TRIAL_PASS;
    }

    return THEFT_TRIAL_FAIL;
}

enum theft_trial_res ymd_same_year(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c0 = a2;
    const struct mon13_Cal* c1 = a3;

    int32_t y0, y1;
    int status0 = mon13_mjdToYmd(*mjd, c0, &y0, NULL, NULL);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToYmd(*mjd, c1, &y1, NULL, NULL);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    return (y1 == y0) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res ymd_add_1day_gr(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t mjd1 = *mjd0 + 1;

    int32_t y0, y1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int leap;
    if(mon13_mjdToIsLeapYear(*mjd0, c, &leap)) {
        return THEFT_TRIAL_FAIL;
    }

    bool good = false;
    if(m0 == 12 && d0 == 31) {
        bool no_yz = (c == &mon13_gregorian || c == &mon13_julian);
        good = add_1day_yearend(y0, y1, m0, m1, d0, d1, no_yz);
    }
    else if(d0 == last_day_of_month_gr(m0, leap)) {
        bool good_y = (y1 == y0);
        bool good_m = (m1 == (m0 + 1));
        bool good_d = (d1 == 1);
        good = good_y && good_m && good_d;
    }
    else {
        bool good_ym = (y1 == y0 && m1 == m0);
        bool good_d = (d1 == d0 + 1);
        good = good_ym && good_d;
    }
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res ymd_add_1day_tq(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t mjd1 = *mjd0 + 1;

    int32_t y0, y1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int leap;
    if(mon13_mjdToIsLeapYear(*mjd0, c, &leap)) {
        return THEFT_TRIAL_FAIL;
    }

    bool good = false;
    if(m0 == 0 && d0 == 1) {
        bool no_yz = (c == &mon13_tranquility);
        good = add_1day_yearend(y0, y1, m0, m1, d0, d1, no_yz);
    }
    else if(m0 == 13 && d0 == 28) {
        bool good_md = (m1 == 0 && d1 == 1);
        bool good_y = (y1 == y0);
        good = good_md && good_y;
    }
    else if(m0 == 0 && d0 == 2 && leap) {
        bool good_md = (m1 == 8 && d1 == 28);
        bool good_y = (y1 == y0);
        good = good_md && good_y;
    }
    else if(m0 == 8 && d0 == 27 && leap) {
        bool good_md = (m1 == 0 && d1 == 2);
        bool good_y = (y1 == y0);
        good = good_md && good_y;
    }
    else {
        good = add_1day_common(y0, y1, m0, m1, d0, d1);
    }
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res ymd_add_1day_ct(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t mjd1 = *mjd0 + 1;

    int32_t y0, y1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int leap;
    if(mon13_mjdToIsLeapYear(*mjd0, c, &leap)) {
        return THEFT_TRIAL_FAIL;
    }

    bool good = false;
    if(m0 == 13 && d0 == 29) {
        good = add_1day_yearend(y0, y1, m0, m1, d0, d1, false);
    }
    else if((m0 == 13 && d0 == 28) || (m0 == 6 && d0 == 28 && leap)) {
        bool good_d = (d1 == 29);
        bool good_m = (m1 == m0);
        bool good_y = (y1 == y0);
        good = good_d && good_m && good_y;
    }
    else {
        good = add_1day_common(y0, y1, m0, m1, d0, d1);
    }
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res ymd_add_1day_ps(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t mjd1 = *mjd0 + 1;

    int32_t y0, y1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int leap;
    if(mon13_mjdToIsLeapYear(*mjd0, c, &leap)) {
        return THEFT_TRIAL_FAIL;
    }

    bool good = false;
    if((m0 == 0 && d0 == 1 && !leap) || (m0 == 0 && d0 == 2 && leap)) {
        good = add_1day_yearend(y0, y1, m0, m1, d0, d1, false);
    }
    else if(m0 == 0 && d0 == 1 && leap) {
        good = add_1day_common(y0, y1, m0, m1, d0, d1);
    }
    else if(m0 == 13 && d0 == 28) {
        bool good_md = (m1 == 0 && d1 == 1);
        bool good_y = (y1 == y0);
        good = good_md && good_y;
    }
    else {
        good = add_1day_common(y0, y1, m0, m1, d0, d1);
    }
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res ymd_add_1day_sym454(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t mjd1 = *mjd0 + 1;

    int32_t y0, y1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int leap;
    if(mon13_mjdToIsLeapYear(*mjd0, c, &leap)) {
        return THEFT_TRIAL_FAIL;
    }

    bool good = false;
    if((m0 == 12 && d0 == 28 && !leap) || (m0 == 12 && d0 == 35 && leap)) {
        good = add_1day_yearend(y0, y1, m0, m1, d0, d1, false);
    }
    else if(m0 != 12 && (((m0 % 3) != 2 && d0 == 28) || ((m0 % 3) == 2 && d0 == 35))) {
        bool good_d = (d1 == 1);
        bool good_m = (m1 == (m0 + 1));
        bool good_y = (y1 == y0);
        good = good_d && good_m && good_y;
    }
    else {
        bool good_d = (d1 == (d0 + 1));
        bool good_ym = (y1 == y0 && m1 == m0);
        good = good_d && good_ym;
    }
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res ymd_add_1day_sym010(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t mjd1 = *mjd0 + 1;

    int32_t y0, y1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int leap;
    if(mon13_mjdToIsLeapYear(*mjd0, c, &leap)) {
        return THEFT_TRIAL_FAIL;
    }

    bool good = false;
    if((m0 == 12 && d0 == 30 && !leap) || (m0 == 12 && d0 == 37 && leap)) {
        good = add_1day_yearend(y0, y1, m0, m1, d0, d1, false);
    }
    else if(m0 != 12 && (((m0 % 3) != 2 && d0 == 30) || ((m0 % 3) == 2 && d0 == 31))) {
        bool good_d = (d1 == 1);
        bool good_m = (m1 == (m0 + 1));
        bool good_y = (y1 == y0);
        good = good_d && good_m && good_y;
    }
    else {
        bool good_d = (d1 == (d0 + 1));
        bool good_ym = (y1 == y0 && m1 == m0);
        good = good_d && good_ym;
    }
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res ymd_add_1day_fr(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t mjd1 = *mjd0 + 1;

    int32_t y0, y1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int leap;
    if(mon13_mjdToIsLeapYear(*mjd0, c, &leap)) {
        return THEFT_TRIAL_FAIL;
    }

    bool good = false;
    if((m0 == 0 && d0 == 5 && !leap) || (m0 == 0 && d0 == 6 && leap)) {
        good = add_1day_yearend(y0, y1, m0, m1, d0, d1, false);
    }
    else if(m0 == 12 && d0 == 30) {
        bool good_md = (d1 == 1 && m1 == 0);
        bool good_y = (y1 == y0);
        good = good_md && good_y;
    }
    else if(d0 == 30) {
        bool good_d = (d1 == 1);
        bool good_m = (m1 == (m0 + 1));
        bool good_y = (y1 == y0);
        good = good_d && good_m && good_y;
    }
    else {
        bool good_ym = (y1 == y0 && m1 == m0);
        bool good_d = (d1 == (d0 + 1));
        good = good_d && good_ym;
    }
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_1month_gr(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t y0, y1, mjd1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_addMonths(*mjd0, c, 1, &mjd1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status2 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status2) {
        return (status2 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    bool correct_res = false;
    if(d1 != d0) {
        if(d0 == 29) {
            correct_res = (m1 == 3) && (d1 == 1);
        }
        else if(d0 == 30) {
            int leap;
            mon13_mjdToIsLeapYear(*mjd0, c, &leap);
            if(leap) {
                correct_res = (m1 == 3) && (d1 == 1);
            }
            else {
                correct_res = (m1 == 3) && (d1 == 2);
            }
        }
        else if(d0 == 31) {
            bool correct_month = (m1 == m0 + 2);
            bool correct_day = (d1 < 4);
            correct_res = correct_month && correct_day;
        }
        else {
            correct_res = false;
        }
    }
    else if(y1 == y0) {
        correct_res = (m1 == m0 + 1);
    }
    else {
        bool correct_start = (m0 == 12);
        bool inc_year = (y1 == y0 + 1) && (m1 == 1);
        if(c == &mon13_gregorian || c == &mon13_julian) {
            bool inc_yz = (y0 == -1) && (y1 == 1) && (m1 == 1);
            correct_res = correct_start && (inc_year || inc_yz);
        }
        else {
            correct_res = correct_start && inc_year;
        }
    }
    return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_1month_tq(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t y0, y1, mjd1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_addMonths(*mjd0, c, 1, &mjd1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status2 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status2) {
        return (status2 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    bool correct_res = false;
    if(m0 == 0) {
        if(d0 == 1) {
            bool inc_year = (y1 == y0 + 1) && (m1 == 2);
            bool correct_day = (d1 == 1);
            correct_res = inc_year && correct_day;
        }
        else if(d0 == 2) {
            bool correct_year = (y1 == y0);
            bool correct_month = (m1 == 9);
            bool correct_day = (d1 == 28);
            correct_res = correct_year && correct_month && correct_day;
        }
    }
    else if(d1 != d0) {
        correct_res = false;
    }
    else if(y1 == y0) {
        correct_res = (m1 == m0 + 1);
    }
    else {
        bool correct_start = (m0 == 13);
        bool inc_year = (y1 == y0 + 1) && (m1 == 1);
        if(c == &mon13_tranquility) {
            bool inc_yz = (y0 == -1) && (y1 == 1) && (m1 == 1);
            correct_res = correct_start && (inc_year || inc_yz);
        }
        else {
            correct_res = correct_start && inc_year;
        }
    }

    return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_1month_ct(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t y0, y1, mjd1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_addMonths(*mjd0, c, 1, &mjd1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status2 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status2) {
        return (status2 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    bool correct_res = false;
    if(m0 == 13 && d0 == 29) {
        bool correct_year = (y1 == (y0 + 1));
        bool correct_month = (m1 == 2);
        bool correct_day = (d1 == 1);
        correct_res = correct_year && correct_month && correct_day;
    }
    else if(m0 == 13) {
        bool correct_year = (y1 == (y0 + 1));
        bool correct_month = (m1 == 1);
        bool correct_day = (d1 == d0);
        correct_res = correct_year && correct_month && correct_day;
    }
    else if(m0 == 6 && d0 == 29) {
        bool correct_year = (y1 == y0);
        bool correct_month = (m1 == 8);
        bool correct_day = (d1 == 1);
        correct_res = correct_year && correct_month && correct_day;
    }
    else {
        bool correct_year = (y1 == y0);
        bool correct_month = (m1 == (m0 + 1));
        bool correct_day = (d1 == d0);
        correct_res = correct_year && correct_month && correct_day;
    }
    return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_1month_ps(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t y0, y1, mjd1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_addMonths(*mjd0, c, 1, &mjd1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status2 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status2) {
        return (status2 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    bool correct_res = false;
    if(m0 == 0) {
        bool correct_year = (y1 == (y0 + 1));
        bool correct_month = (m1 == 2);
        bool correct_day = (d1 == 1);
        correct_res = correct_year && correct_month && correct_day;
    }
    else if(m0 == 13) {
        bool correct_year = (y1 == (y0 + 1));
        bool correct_month = (m1 == 1);
        bool correct_day = (d1 == d0);
        correct_res = correct_year && correct_month && correct_day;
    }
    else {
        bool correct_year = (y1 == y0);
        bool correct_month = (m1 == (m0 + 1));
        bool correct_day = (d1 == d0);
        correct_res = correct_year && correct_month && correct_day;
    }
    return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_1month_sym454(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t y0, y1, mjd1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_addMonths(*mjd0, c, 1, &mjd1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status2 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status2) {
        return (status2 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int leap;
    mon13_mjdToIsLeapYear(*mjd0, c, &leap);


    bool correct_res = false;
    if(m0 == 12) {
        if(d0 > 28) {
            bool correct_year = (y1 == (y0 + 1));
            bool correct_month = (m1 == 2);
            bool correct_day = (d1 == (d0 - 28));
            correct_res = correct_year && correct_month && correct_day;
        }
        else {
            bool correct_year = (y1 == (y0 + 1));
            bool correct_month = (m1 == 1);
            bool correct_day = (d1 == d0);
            correct_res = correct_year && correct_month && correct_day;
        }
    }
    else if(m0 == 11 && d0 > 28 && !leap) {
        bool correct_year = (y1 == (y0 + 1));
        bool correct_month = (m1 == 1);
        bool correct_day = (d1 == (d0 - 28));
        correct_res = correct_year && correct_month && correct_day;
    }
    else if(m0 != 11 && d0 > 28) {
        bool correct_year = (y1 == y0);
        bool correct_month = (m1 == (m0 + 2));
        bool correct_day = (d1 == (d0 - 28));
        correct_res = correct_year && correct_month && correct_day;
    }
    else {
        bool correct_year = (y1 == y0);
        bool correct_month = (m1 == (m0 + 1));
        bool correct_day = (d1 == d0);
        correct_res = correct_year && correct_month && correct_day;

    }
    return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_1month_sym010(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t y0, y1, mjd1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_addMonths(*mjd0, c, 1, &mjd1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status2 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status2) {
        return (status2 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int leap;
    mon13_mjdToIsLeapYear(*mjd0, c, &leap);

    bool correct_res = false;
    if(m0 == 12) {
        if(d0 > 30) {
            bool correct_year = (y1 == (y0 + 1));
            bool correct_month = (m1 == 2);
            bool correct_day = (d1 == (d0 - 30));
            correct_res = correct_year && correct_month && correct_day;
        }
        else {
            bool correct_year = (y1 == (y0 + 1));
            bool correct_month = (m1 == 1);
            bool correct_day = (d1 == d0);
            correct_res = correct_year && correct_month && correct_day;
        }
    }
    else if(m0 == 11 && d0 > 30 && !leap) {
        bool correct_year = (y1 == (y0 + 1));
        bool correct_month = (m1 == 1);
        bool correct_day = (d1 == (d0 - 30));
        correct_res = correct_year && correct_month && correct_day;
    }
    else if(m0 != 11 && d0 > 30) {
        bool correct_year = (y1 == y0);
        bool correct_month = (m1 == (m0 + 2));
        bool correct_day = (d1 == (d0 - 30));
        correct_res = correct_year && correct_month && correct_day;
    }
    else {
        bool correct_year = (y1 == y0);
        bool correct_month = (m1 == (m0 + 1));
        bool correct_day = (d1 == d0);
        correct_res = correct_year && correct_month && correct_day;
    }
    return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_1month_fr(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t y0, y1, mjd1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_addMonths(*mjd0, c, 1, &mjd1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status2 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status2) {
        return (status2 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int leap;
    mon13_mjdToIsLeapYear(*mjd0, c, &leap);

    bool correct_res = false;
    if(m0 == 0) {
        bool correct_year = (y1 == (y0 + 1));
        bool correct_month = (m1 == 2);
        bool correct_day = (d1 == 1);
        correct_res = correct_year && correct_month && correct_day;
    }
    else if(m0 == 12) {
        bool correct_year = (y1 == (y0 + 1));
        bool correct_month = (m1 == 1);
        bool correct_day = (d1 == d0);
        correct_res = correct_year && correct_month && correct_day;
    }
    else {
        bool correct_year = (y1 == y0);
        bool correct_month = (m1 == (m0 + 1));
        bool correct_day = (d1 == d0);
        correct_res = correct_year && correct_month && correct_day;
    }
    return correct_res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res add_year(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd0 = a1;
    const int32_t* offset = a2;
    const struct mon13_Cal* c = a3;

    int32_t y0, y1, mjd1;
    uint8_t m0, m1, d0, d1;
    int status0 = mon13_mjdToYmd(*mjd0, c, &y0, &m0, &d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_addYears(*mjd0, c, *offset, &mjd1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status2 = mon13_mjdToYmd(mjd1, c, &y1, &m1, &d1);
    if(status2) {
        return (status2 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int leap;
    if(mon13_mjdToIsLeapYear(*mjd0, c, &leap)) {
        return THEFT_TRIAL_FAIL;
    }

    if(c == &mon13_positivist && m0 == 0 && d0 == 2) {
        if(y1 != ((y0 + *offset) + 1) && y1 != (y0 + *offset)) {
            return THEFT_TRIAL_FAIL;
        }
        if(!(m1 == 1 && d1 == 1) && !(m1 == 0 && d1 == 2)) {
            return THEFT_TRIAL_FAIL;
        }
    }
    else if(c == &mon13_symmetry454 && m0 == 12 && d0 > 28) {
        if(y1 != ((y0 + *offset) + 1) && y1 != (y0 + *offset)) {
            return THEFT_TRIAL_FAIL;
        }
        if(!(m1 == 1 && d1 == (d0 - 28)) && !(m1 == m0 && d1 == d0)) {
            return THEFT_TRIAL_FAIL;
        }
    }
    else if(c == &mon13_symmetry010 && m0 == 12 && d0 > 30) {
        if(y1 != ((y0 + *offset) + 1) && y1 != (y0 + *offset)) {
            return THEFT_TRIAL_FAIL;
        }
        if(!(m1 == 1 && d1 == (d0 - 30)) && !(m1 == m0 && d1 == d0)) {
            return THEFT_TRIAL_FAIL;
        }
    }
    else if(c == &mon13_french_revolutionary_romme && m0 == 0 && d0 == 6) {
        if(y1 != ((y0 + *offset) + 1) && y1 != (y0 + *offset)) {
            return THEFT_TRIAL_FAIL;
        }
        if(!(m1 == 1 && d1 == 1) && !(m1 == m0 && d1 == d0)) {
            return THEFT_TRIAL_FAIL;
        }
    }
    else if(c == &mon13_french_revolutionary_romme_sub1 && m0 == 0 && d0 == 6) {
        if(y1 != ((y0 + *offset) + 1) && y1 != (y0 + *offset)) {
            return THEFT_TRIAL_FAIL;
        }
        if(!(m1 == 1 && d1 == 1) && !(m1 == m0 && d1 == d0)) {
            return THEFT_TRIAL_FAIL;
        }
    }
    else {
        if(c == &mon13_gregorian || c == &mon13_tranquility || c == &mon13_julian) {
            if(y0 < 0 && y1 > 0) {
                if(y1 != (y0 + *offset + 1)) {
                    return THEFT_TRIAL_FAIL;
                }
            }
            else if(y0 > 0 && y1 < 0) {
                if(y1 != (y0 + *offset - 1)) {
                    return THEFT_TRIAL_FAIL;
                }
            }
            else if(y1 != (y0 + *offset)) {
                return THEFT_TRIAL_FAIL;
            }
        }
        else {
            if(y1 != (y0 + *offset)) {
                return THEFT_TRIAL_FAIL;
            }
        }
        
        if(!leap && (m1 != m0 || d1 != d0)) {
            return THEFT_TRIAL_FAIL;
        }
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res add_zero(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd0 = a1;
    const struct mon13_Cal* c = a2;

    int32_t mjd1, mjd2;
    if(mon13_addYears(*mjd0, c, 0, &mjd1)) {
        return THEFT_TRIAL_FAIL;
    }
    if(mon13_addMonths(*mjd0, c, 0, &mjd2)) {
        return THEFT_TRIAL_FAIL;
    }

    return ((*mjd0 == mjd1) && (*mjd0 == mjd2)) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res doy_same(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c0 = a2;
    const struct mon13_Cal* c1 = a3;

    uint16_t doy0, doy1;
    int status0 = mon13_mjdToDayOfYear(*mjd, c0, &doy0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToDayOfYear(*mjd, c1, &doy1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    return (doy1 == doy0) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res leap_same(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c0 = a2;
    const struct mon13_Cal* c1 = a3;

    int leap0, leap1;
    int status0 = mon13_mjdToIsLeapYear(*mjd, c0, &leap0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToIsLeapYear(*mjd, c1, &leap1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    return (leap1 == leap0) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res leap9(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;

    int leap_count = 0;
    for(int i = 1; i < 9; i++) {
        int32_t sum;
        int status0 = mon13_addYears(*mjd, c, i, &sum);
        if(status0) {
            return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        int is_leap;
        int status1 = mon13_mjdToIsLeapYear(sum, c, &is_leap);
        if(status1) {
            return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        if(is_leap) {
            leap_count++;
        }
    }
    bool res = (leap_count == 1) || (leap_count == 2);
    return res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res leap_fr(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;

    uint8_t m, d;
    if(!mon13_mjdToYmd(*mjd, c, NULL, &m, &d) && m == 0 && d == 6) {
        return THEFT_TRIAL_SKIP;
    }
    else {
        return leap9(t, a1, a2);
    }
}

enum theft_trial_res leap7(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;

    int leap_count = 0;
    for(int i = 1; i < 7; i++) {
        int32_t sum;
        int status0 = mon13_addYears(*mjd, c, i, &sum);
        if(status0) {
            return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        int is_leap;
        int status1 = mon13_mjdToIsLeapYear(sum, c, &is_leap);
        if(status1) {
            return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
        }
        if(is_leap) {
            leap_count++;
        }
    }
    bool res = (leap_count == 1) || (leap_count == 2);
    return res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res leap0(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;

    int is_leap;
    int status = mon13_mjdToIsLeapYear(*mjd, c, &is_leap);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    return is_leap ? THEFT_TRIAL_FAIL : THEFT_TRIAL_PASS;
}

enum theft_trial_res day_of_week_continuous(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd_raw = a1;
    const struct mon13_Cal* c = a2;
    if(*mjd_raw > (INT32_MAX - 1)) {
        return THEFT_TRIAL_SKIP;
    }

    int32_t sum0 = *mjd_raw;
    int32_t sum1 = sum0 + 1;

    uint8_t dow0;
    int status0 = mon13_mjdToDayOfWeek(sum0, c, &dow0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    uint8_t dow1;
    int status1 = mon13_mjdToDayOfWeek(sum1, c, &dow1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    if(dow0 == MON13_SUNDAY) {
        return dow1 == MON13_MONDAY ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
    else {
        return dow1 == (dow0 + 1) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
}

enum theft_trial_res day_of_week_tq(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;
    
    uint8_t m, d;
    if(mon13_mjdToYmd(*mjd, c, NULL, &m, &d)) {
        return THEFT_TRIAL_SKIP;
    }

    uint8_t dow;
    int status = mon13_mjdToDayOfWeek(*mjd, c, &dow);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int expected;
    if(m == 0) {
        expected = MON13_NO_WEEKDAY;
    }
    else {
        switch(d % 7) {
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

enum theft_trial_res day_of_week_fr(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;
    uint8_t m, d;
    if(mon13_mjdToYmd(*mjd, c, NULL, &m, &d)) {
        return THEFT_TRIAL_SKIP;
    }

    uint8_t dow;
    int status = mon13_mjdToDayOfWeek(*mjd, c, &dow);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int expected;
    if(m == 0) {
        expected = MON13_NO_WEEKDAY;
    }
    else {
        switch(d % 10) {
            case 1: expected = MON13_PRIMIDI; break;
            case 2: expected = MON13_DUODI; break;
            case 3: expected = MON13_TRIDI; break;
            case 4: expected = MON13_QUARTIDI; break;
            case 5: expected = MON13_QUINTIDI; break;
            case 6: expected = MON13_SEXTIDI; break;
            case 7: expected = MON13_SEPTIDI; break;
            case 8: expected = MON13_OCTIDI; break;
            case 9: expected = MON13_NONIDI; break;
            case 0: expected = MON13_DECADI; break;
            default: expected = -1;
        }
    }
    return dow == expected ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res day_of_week_same(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c0 = a2;
    const struct mon13_Cal* c1 = a2;

    uint8_t dow0, dow1;
    int status0 = mon13_mjdToDayOfWeek(*mjd, c0, &dow0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToDayOfWeek(*mjd, c1, &dow1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    return (dow0 == dow1) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res day_of_year_add_one(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;

    int32_t sum = *mjd + 1;

    uint16_t doy0, doy1;
    int status0 = mon13_mjdToDayOfYear(*mjd, c, &doy0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToDayOfYear(sum, c, &doy1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int leap;
    if(mon13_mjdToIsLeapYear(*mjd, c, &leap)) {
        return THEFT_TRIAL_FAIL;
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

enum theft_trial_res day_of_year_add_one_sym454(struct theft* t, void* a1, void* a2) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;

    int32_t sum = *mjd + 1;

    uint16_t doy0, doy1;
    int status0 = mon13_mjdToDayOfYear(*mjd, c, &doy0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToDayOfYear(sum, c, &doy1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int leap;
    if(mon13_mjdToIsLeapYear(*mjd, c, &leap)) {
        return THEFT_TRIAL_FAIL;
    }
    bool res = false;
    if(doy0 < 364 || (leap && doy0 < 371)) {
        res = (doy1 == (doy0 + 1));
    }
    else if((doy0 == 364 && !leap) || (doy0 == 371 && leap)) {
        res = (doy1 == 1);
    }
    return res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res diff_months_roundtrip(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd_raw0 = a1;
    const int32_t* mjd_raw1 = a2;
    const struct mon13_Cal* cal = a3;

    int32_t raw_y0, raw_y1;
    uint8_t raw_m0, raw_m1, raw_d0, raw_d1;
    int status0 = mon13_mjdToYmd(*mjd_raw0, cal, &raw_y0, &raw_m0, &raw_d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToYmd(*mjd_raw1, cal, &raw_y1, &raw_m1, &raw_d1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    if(raw_m0 == 0 || raw_d0 > 28 || raw_m1 == 0 || raw_d1 > 28) {
        return THEFT_TRIAL_SKIP;
    }

    int32_t mjd0 = *mjd_raw0;
    int32_t mjd1;
    int status2 = mon13_mjdFromYmd(cal, raw_y1, raw_m1, raw_d0, &mjd1);
    if(status2) {
        return (status2 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int status;
    int32_t diff;
    int status3 = mon13_diffMonths(mjd0, mjd1, cal, &diff);
    if(status3) {
        return (status3 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    int32_t mjd2;
    if(mon13_addMonths(mjd1, cal, diff, &mjd2)) {
        return THEFT_TRIAL_FAIL;
    }

    int32_t y_sum;
    uint8_t m_sum;
    if(mon13_mjdToYmd(mjd2, cal, &y_sum, &m_sum, NULL)) {
        return THEFT_TRIAL_FAIL;
    }

    return (raw_y0 == y_sum && raw_m0 == m_sum) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res diff_months_small_day(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;
    const uint8_t* offset = a3;

    if(*mjd > (INT32_MAX - *offset) || *mjd < ((-INT_MAX) + *offset)) {
        return THEFT_TRIAL_SKIP;
    }
    int32_t sum0 = *mjd + *offset;
    int32_t sum1 = *mjd - *offset;
    int32_t diff0, diff1;
    int status0 = mon13_diffMonths(*mjd, sum0, c, &diff0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_diffMonths(*mjd, sum1, c, &diff1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    return (diff0 == 0 && diff1 == 0) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res diff_years_roundtrip(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd_raw0 = a1;
    const int32_t* mjd_raw1 = a2;
    const struct mon13_Cal* cal = a3;

    int32_t raw_y0, raw_y1;
    uint8_t raw_m0, raw_m1, raw_d0, raw_d1;
    int status0 = mon13_mjdToYmd(*mjd_raw0, cal, &raw_y0, &raw_m0, &raw_d0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_mjdToYmd(*mjd_raw1, cal, &raw_y1, &raw_m1, &raw_d1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    if(raw_m0 == 0 || raw_d0 > 28 || raw_m1 == 0 || raw_d1 > 28) {
        return THEFT_TRIAL_SKIP;
    }

    int32_t mjd0 = *mjd_raw0;
    int32_t mjd1;
    if(mon13_mjdFromYmd(cal, raw_y1, raw_m0, raw_d0, &mjd1)) {
        return THEFT_TRIAL_SKIP;
    }

    int status;
    int32_t diff;
    status = mon13_diffYears(mjd0, mjd1, cal, &diff);
    if(status) {
        return THEFT_TRIAL_SKIP;
    }

    int32_t mjd2;
    status = mon13_addYears(mjd1, cal, diff, &mjd2);
    if(status) {
        return THEFT_TRIAL_FAIL;
    }

    int32_t y_sum;
    uint8_t m_sum, d_sum;
    if(mon13_mjdToYmd(mjd2, cal, &y_sum, &m_sum, &d_sum)) {
        return THEFT_TRIAL_FAIL;
    }

    return (raw_y0 == y_sum) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res diff_years_small_day(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;
    const uint8_t* offset = a3;

    if(*mjd > (INT32_MAX - *offset) || *mjd < ((-INT_MAX) + *offset)) {
        return THEFT_TRIAL_SKIP;
    }
    int32_t sum0 = *mjd + *offset;
    int32_t sum1 = *mjd - *offset;
    int32_t diff0, diff1;
    int status0 = mon13_diffYears(*mjd, sum0, c, &diff0);
    if(status0) {
        return (status0 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    int status1 = mon13_diffYears(*mjd, sum1, c, &diff1);
    if(status1) {
        return (status1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    return (diff0 == 0 && diff1 == 0) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_builtin(struct theft* t, void* a1) {
    const struct name_cal* nc = a1;
    int res = mon13_validNameList(nc->c, nc->n);
    return res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_null(struct theft* t, void* a1) {
    const struct name_cal* nc = a1;
    int res = mon13_validNameList(nc->c, nc->n);
    return res ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_no_month(struct theft* t, void* a1) {
    const struct name_cal* nc = a1;
    struct mon13_NameList bad_n = {
        .month_list = NULL,
        .weekday_list = nc->n->weekday_list,
        .era_list = nc->n->era_list,
        .intercalary_list = nc->n->intercalary_list,
        .alt_intercalary_list = nc->n->alt_intercalary_list,
        .calendar_name = nc->n->calendar_name
    };
    int res = mon13_validNameList(nc->c, &bad_n);
    return (!res) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_no_weekdays(struct theft* t, void* a1) {
    const struct name_cal* nc = a1;
    struct mon13_NameList bad_n = {
        .month_list = nc->n->month_list,
        .weekday_list = NULL,
        .era_list = nc->n->era_list,
        .intercalary_list = nc->n->intercalary_list,
        .alt_intercalary_list = nc->n->alt_intercalary_list,
        .calendar_name = nc->n->calendar_name
    };
    int res = mon13_validNameList(nc->c, &bad_n);
    return (!res) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_no_eras(struct theft* t, void* a1) {
    const struct name_cal* nc = a1;
    struct mon13_NameList bad_n = {
        .month_list = nc->n->month_list,
        .weekday_list = nc->n->weekday_list,
        .era_list = NULL,
        .intercalary_list = nc->n->intercalary_list,
        .alt_intercalary_list = nc->n->alt_intercalary_list,
        .calendar_name = nc->n->calendar_name
    };
    int res = mon13_validNameList(nc->c, &bad_n);
    return (!res) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_no_name(struct theft* t, void* a1) {
    const struct name_cal* nc = a1;
    struct mon13_NameList bad_n = {
        .month_list = nc->n->month_list,
        .weekday_list = nc->n->weekday_list,
        .era_list = nc->n->era_list,
        .intercalary_list = nc->n->intercalary_list,
        .alt_intercalary_list = nc->n->alt_intercalary_list,
        .calendar_name = NULL
    };
    int res = mon13_validNameList(nc->c, &bad_n);
    return (!res) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_gregorian(struct theft* t, void* a1, void* a2) {
    bool good = valid_names_check(a1, a2, 7, 12, 2, 0, 0);
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_tranquility(struct theft* t, void* a1, void* a2) {
    bool good = valid_names_check(a1, a2, 7, 13, 2, 2, 2);
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_cotsworth(struct theft* t, void* a1, void* a2) {
    bool good = valid_names_check(a1, a2, 7, 13, 2, 2, 0);
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_french_revolutionary(struct theft* t, void* a1, void* a2) {
    bool good = valid_names_check(a1, a2, 10, 12, 2, 6, 0);
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_ancient_egyptian(struct theft* t, void* a1, void* a2) {
    bool good = valid_names_check(a1, a2, 10, 12, 2, 5, 0);
    return good ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res valid_name_list_same(struct theft* t, void* a1, void* a2, void* a3) {
    const struct mon13_Cal* c0 = a1;
    const struct mon13_Cal* c1 = a2;
    const struct mon13_NameList* n = a3;

    bool same = (mon13_validNameList(c0, n) == mon13_validNameList(c1, n));
    return same ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res format_percent(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;
    
    char buf[3];
    memset(buf, *placeholder, 3);

    int res = mon13_format(*mjd, nc->c, nc->n, "%%", buf, 3);
    if(res < 0) {
        return THEFT_TRIAL_FAIL;
    }
    if(!format_res_check(res, "%")) {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[0] != '%') {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[1] != '\0') {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[2] != *placeholder) {
        return THEFT_TRIAL_FAIL;
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_weekday(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;

    uint8_t day;
    int status = mon13_mjdToDayOfWeek(*mjd, nc->c, &day);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    char buf[100];
    memset(buf, *placeholder, 100);

    int res = mon13_format(*mjd, nc->c, nc->n, "%A", buf, 100);
    if(res < 0) {
        return THEFT_TRIAL_FAIL;
    }
    if(day == MON13_NO_WEEKDAY) {
        const char* expected_ic = contained_ic(buf, nc->n, 100, *placeholder);
        return format_res_check_nonblank(res, expected_ic) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
    else {
        const char* expected = contained(buf, nc->n->weekday_list, 100, *placeholder);
        return format_res_check(res, expected) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
}

enum theft_trial_res format_month(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;

    uint8_t m;
    int status = mon13_mjdToYmd(*mjd, nc->c, NULL, &m, NULL);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    char buf[100];
    memset(buf, *placeholder, 100);

    int res = mon13_format(*mjd, nc->c, nc->n, "%B", buf, 100);
    if(res < 0) {
        return THEFT_TRIAL_FAIL;
    }
    if(m == 0) {
        const char* expected_ic = contained_ic(buf, nc->n, 100, *placeholder);
        return format_res_check_nonblank(res, expected_ic) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
    else {
        const char* expected = contained(buf, nc->n->month_list, 100, *placeholder);
        return format_res_check(res, expected) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
}

enum theft_trial_res format_day_of_month(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;

    uint8_t d;
    int status = mon13_mjdToYmd(*mjd, nc->c, NULL, NULL, &d);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    char buf[4];
    memset(buf, *placeholder, 4);

    int res = mon13_format(*mjd, nc->c, nc->n, "%d", buf, 4);
    if(res < 0) {
        return THEFT_TRIAL_FAIL;
    }
    char* endptr = buf;
    long parsed = strtol(buf, &endptr, 10);
    if(parsed != d || res < 1 || res > 2) {
        return THEFT_TRIAL_FAIL;
    }
    if(endptr[0] != '\0' || endptr[1] != *placeholder || &(endptr[0]) != &(buf[res])) {
        return THEFT_TRIAL_FAIL;
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_cal(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;

    char buf[100];
    memset(buf, *placeholder, 100);

    int res = mon13_format(*mjd, nc->c, nc->n, "%f", buf, 100);
    if(res < 0) {
        return THEFT_TRIAL_FAIL;
    }
    if(res != strlen(nc->n->calendar_name)) {
        return THEFT_TRIAL_FAIL;
    }
    if(strncmp(buf, nc->n->calendar_name, 50)) {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[res] != '\0' || buf[res + 1] != *placeholder) {
        return THEFT_TRIAL_FAIL;
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_doy(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;

    uint16_t doy;
    int status = mon13_mjdToDayOfYear(*mjd, nc->c, &doy);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    
    char buf[5];
    memset(buf, *placeholder, 5);

    int res = mon13_format(*mjd, nc->c, nc->n, "%j", buf, 5);
    if(res < 0) {
        return THEFT_TRIAL_FAIL;
    }
    char* endptr = buf;
    long parsed = strtol(buf, &endptr, 10);
    if(parsed != doy || res < 1 || res > 3) {
        return THEFT_TRIAL_FAIL;
    }
    if(endptr[0] != '\0' || endptr[1] != *placeholder || &(endptr[0]) != &(buf[res])) {
        return THEFT_TRIAL_FAIL;
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_month_number(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;

    uint8_t m;
    int status = mon13_mjdToYmd(*mjd, nc->c, NULL, &m, NULL);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    char buf[5];
    memset(buf, *placeholder, 5);

    int res = mon13_format(*mjd, nc->c, nc->n, "%m", buf, 5);
    if(res < 0) {
        return THEFT_TRIAL_FAIL;
    }
    char* endptr = buf;
    long parsed = strtol(buf, &endptr, 10);
    if(parsed != m || res < 1 || res > 2) {
        return THEFT_TRIAL_FAIL;
    }
    if(endptr[0] != '\0' || endptr[1] != *placeholder || &(endptr[0]) != &(buf[res])) {
        return THEFT_TRIAL_FAIL;
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_newline(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;

    char buf[3];
    memset(buf, *placeholder, 3);

    int res = mon13_format(*mjd, nc->c, nc->n, "%n", buf, 3);
    if(res < 0) {
        return THEFT_TRIAL_FAIL;
    }
    if(!format_res_check(res, "%")) {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[0] != '\n') {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[1] != '\0') {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[2] != *placeholder) {
        return THEFT_TRIAL_FAIL;
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_era(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;

    int32_t y;
    uint8_t m;
    int status = mon13_mjdToYmd(*mjd, nc->c, &y, &m, NULL);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    if(m == 0) {
        return THEFT_TRIAL_SKIP;
    }

    const char* expected = (y < 0) ? nc->n->era_list[0] : nc->n->era_list[1];

    char buf[100];
    memset(buf, *placeholder, 100);

    int res = mon13_format(*mjd, nc->c, nc->n, "%q", buf, 100);
    if(res != strlen(expected)) {
        return THEFT_TRIAL_FAIL;
    }
    if(strncmp(buf, expected, 50)) {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[res] != '\0' || buf[res + 1] != *placeholder) {
        return THEFT_TRIAL_FAIL;
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_tab(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;

    char buf[3];
    memset(buf, *placeholder, 3);

    int res = mon13_format(*mjd, nc->c, nc->n, "%t", buf, 3);
    if(res < 0) {
        return THEFT_TRIAL_FAIL;
    }
    if(!format_res_check(res, "%")) {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[0] != '\t') {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[1] != '\0') {
        return THEFT_TRIAL_FAIL;
    }
    if(buf[2] != *placeholder) {
        return THEFT_TRIAL_FAIL;
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_weekday_number(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;

    uint8_t weekday;
    int status = mon13_mjdToDayOfWeek(*mjd, nc->c, &weekday);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    char buf[5];
    memset(buf, *placeholder, 5);

    int res = mon13_format(*mjd, nc->c, nc->n, "%u", buf, 5);
    char* endptr = buf;
    long parsed = strtol(buf, &endptr, 10);
    if(parsed != weekday || (res != 1 && weekday < 10) || (res != 2 && weekday > 10)) {
        return THEFT_TRIAL_FAIL;
    }

    if(endptr[0] != '\0' || endptr[1] != *placeholder || &(endptr[0]) != &(buf[res])) {
        return THEFT_TRIAL_FAIL;
    }

    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_year(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;

    int32_t y;
    int status = mon13_mjdToYmd(*mjd, nc->c, &y, NULL, NULL);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    char buf[100];
    memset(buf, *placeholder, 100);

    int res = mon13_format(*mjd, nc->c, nc->n, "%Y", buf, 100);
    if(res < 0) {
        return THEFT_TRIAL_FAIL;
    }
    char* endptr = buf;
    long parsed = strtol(buf, &endptr, 10);
    if(parsed != y || res < 1) {
        return THEFT_TRIAL_FAIL;
    }
    if(endptr[0] != '\0' || endptr[1] != *placeholder) {
        return THEFT_TRIAL_FAIL;
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_strftime(struct theft* t, void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const char* placeholder = a2;
    const char* fmt = a3;

    const struct mon13_Cal* c = &mon13_gregorian_year_zero;
    const struct mon13_NameList* n = &mon13_names_en_US_gregorian;

    int64_t u0;
    int status = mon13_mjdToUnix(*mjd, &u0);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }
    time_t unix0 = u0;
    const struct tm* gmt_u = gmtime(&unix0);

    char buf0[STRFTIME_BUF];
    char buf1[STRFTIME_BUF];
    memset(buf0, *placeholder, STRFTIME_BUF);
    memset(buf1, *placeholder, STRFTIME_BUF);

    int res0 = strftime(buf0, STRFTIME_BUF, fmt, gmt_u);
    int res1 = mon13_format(*mjd, c, n, fmt, buf1, STRFTIME_BUF);

    if(res1 < 0) {
        return (res1 == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    if(res0 != res1) {
        return THEFT_TRIAL_FAIL;
    }
    if(strncmp(buf0, buf1, STRFTIME_BUF)) {
        return THEFT_TRIAL_FAIL;
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_numeric_padding(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;
    const char* fmt = a4;

    int32_t y;
    uint8_t m, d;
    int status = mon13_mjdToYmd(*mjd, nc->c, &y, &m, &d);
    if(status) {
        return (status == MON13_ERROR_OVERFLOW) ? THEFT_TRIAL_SKIP : THEFT_TRIAL_FAIL;
    }

    char flag = fmt[1];
    int min_width = (flag == '-') ? 0 : fmt[2] - '0'; //Assumes ASCII or UTF8.
    int targ;
    switch(fmt[3]) {
        case 'd': targ = d; break;
        case 'j': {
            uint16_t doy;
            if(mon13_mjdToDayOfYear(*mjd, nc->c, &doy)) {
                return THEFT_TRIAL_FAIL; //Getting ymd already worked
            }
            targ = doy;
            break;
        }
        case 'm': targ = m; break;
        case 'u': {
            uint8_t weekday;
            if(mon13_mjdToDayOfWeek(*mjd, nc->c, &weekday)) {
                return THEFT_TRIAL_FAIL; //Getting ymd already worked
            }
            targ = weekday;
            break;
        }
        case 'Y': {
            targ = y;
            if(y > 99999999) {
                return THEFT_TRIAL_SKIP;
            }
            break;
        }
        default: {
            return THEFT_TRIAL_FAIL;
        }
    }

    double digits = (targ == 0) ? 1 : floor(log10(targ)) + 1;

    char buf[20];
    memset(buf, *placeholder, 20);
    int res = mon13_format(*mjd, nc->c, nc->n, fmt, buf, 20);

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

enum theft_trial_res format_numeric_null(struct theft* t, void* a1, void* a2, void* a3, void* a4, void* a5) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder0 = a3;
    const char* placeholder1 = a4;
    const char* fmt = a5;

    char buf0[20];
    char buf1[20];
    memset(buf0, *placeholder0, 20);
    memset(buf1, *placeholder1, 20);
    int res0 = mon13_format(*mjd, nc->c, nc->n, fmt, buf0, 20);
    int res1 = mon13_format(*mjd, nc->c, NULL, fmt, buf1, 20);
    if(res0 < 0 || res1 < 0) {
        return (res0 == res1) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
    return (res0 == res1) && !strncmp(buf0, buf1, 20) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res format_simple_copy(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;
    const char* fmt = a4;

    char buf[ASCII_COPY_BUF];
    memset(buf, *placeholder, ASCII_COPY_BUF);
    int res = mon13_format(*mjd, nc->c, nc->n, fmt, buf, ASCII_COPY_BUF);
    if(res < 0) {
        return THEFT_TRIAL_FAIL;
    }
    return !strncmp(fmt, buf, ASCII_COPY_BUF) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res format_null(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;
    const char* fmt = a4;

    char buf[ASCII_COPY_BUF];
    int status;
    memset(buf, *placeholder, ASCII_COPY_BUF);
    status = mon13_format(*mjd, NULL, nc->n, fmt, buf, ASCII_COPY_BUF);
    if(status >= 0 || buf[0] != *placeholder) {
        return THEFT_TRIAL_FAIL;
    }
    //NULL namelist is tested seperately.
    status = mon13_format(*mjd, nc->c, nc->n, NULL, buf, ASCII_COPY_BUF);
    if(status >= 0 || buf[0] != *placeholder) {
        return THEFT_TRIAL_FAIL;
    }
    //NULL buf is tested seperately.
    //0 length is tested seperately.
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res format_dry_run(struct theft* t,  void* a1, void* a2, void* a3) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* fmt = a3;

    int status_null_buf = mon13_format(*mjd, nc->c, nc->n, fmt, NULL, STRFTIME_BUF);
    if(status_null_buf < 0) {
        return THEFT_TRIAL_SKIP;
    }

    char fake_buf = 0;
    int status_buf_len_0 = mon13_format(*mjd, nc->c, nc->n, fmt, &fake_buf, 0);
    if(status_buf_len_0 != status_null_buf) {
        return THEFT_TRIAL_FAIL;
    }
    if(fake_buf != 0) {
        return THEFT_TRIAL_FAIL;
    }

    char* buf = calloc(status_null_buf + 1, sizeof(char));
    int status = mon13_format(*mjd, nc->c, nc->n, fmt, buf, status_null_buf + 1);
    free(buf);

    return (status == status_null_buf) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
}

enum theft_trial_res format_truncate(struct theft* t, void* a1, void* a2, void* a3, void* a4) {
    const int32_t* mjd = a1;
    const struct name_cal* nc = a2;
    const char* placeholder = a3;
    const char* fmt = a4;

    int buflen_0 = mon13_format(*mjd, nc->c, nc->n, fmt, NULL, STRFTIME_BUF);
    if(buflen_0 == 0) {
        return THEFT_TRIAL_SKIP;
    }
    if(buflen_0 < 0) {
        return THEFT_TRIAL_SKIP;
    }
    char* buf_0 = malloc((buflen_0 + 1) * sizeof(char));
    memset(buf_0, *placeholder, (buflen_0 + 1) * sizeof(char));
    int status_0 = mon13_format(*mjd, nc->c, nc->n, fmt, buf_0, buflen_0 + 1);
    if(status_0 != buflen_0) {
        free(buf_0);
        return THEFT_TRIAL_FAIL;
    }

    int buflen_1 = (buflen_0 > 1) ? (buflen_0/2) : 1;
    char* buf_1 = malloc((buflen_1 + 1) * sizeof(char));
    memset(buf_1, *placeholder, (buflen_1 + 1) * sizeof(char));
    int status_1 = mon13_format(*mjd, nc->c, nc->n, fmt, buf_1, buflen_1 + 1);
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

enum theft_trial_res format_invalid_names(struct theft* t, void* a1, void* a2, void* a3, void* a4, void* a5) {
    const int32_t* mjd = a1;
    const struct mon13_Cal* c = a2;
    const struct mon13_NameList* n = a3;
    const char* placeholder = a4;
    const char* fmt = a5;

    if(mon13_validNameList(c, n)) {
        return THEFT_TRIAL_SKIP;
    }

    char buf[ASCII_COPY_BUF];
    memset(buf, *placeholder, ASCII_COPY_BUF);
    int res = mon13_format(*mjd, c, n, fmt, buf, ASCII_COPY_BUF);
    if(res >= 0) {
        return THEFT_TRIAL_FAIL;
    }
    for(size_t i = 0; i < ASCII_COPY_BUF; i++) {
        if(buf[i] != *placeholder) {
            return THEFT_TRIAL_FAIL;
        }
    }
    return THEFT_TRIAL_PASS;
}

enum theft_trial_res error_code_message(struct theft* t, void* a1) {
    const int8_t* error_code_p = a1;
    int error_code = *error_code_p;

    const char* res = mon13_errorMessage(error_code);
    if(error_code >= MON13_ERROR_UNKNOWN || error_code == -68) {
        const char* expected = "Unknown error";
        int expected_len = strlen(expected);
        int cmp = memcmp(res, expected, expected_len);
        return (cmp == 0) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
    else if(error_code >= MON13_ERROR_NULL_DATE) {
        const char* expected = "Null ";
        int cmp = memcmp(res, expected, 5);
        return (cmp == 0) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
    else if(error_code > MON13_ERROR_OVERFLOW) {
        const char* expected = "Unknown error";
        int expected_len = strlen(expected);
        int cmp = memcmp(res, expected, expected_len);
        return (cmp == 0) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
    else if(error_code >= MON13_ERROR_INVALID_NAME_LIST) {
        const char* expected = "Unknown error";
        int expected_len = strlen(expected);
        int cmp = memcmp(res, expected, expected_len);
        int res_len = strlen(res);
        return (cmp != 0) && (res_len > 5) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
    else {
        const char* expected = "Unknown error";
        int expected_len = strlen(expected);
        int cmp = memcmp(res, expected, expected_len);
        return (cmp == 0) ? THEFT_TRIAL_PASS : THEFT_TRIAL_FAIL;
    }
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

struct theft_type_info gr2ps_positivists_org_info = {
    .alloc = select_gr2ps_positivists_org, //nothing to free
    .print = print_known
};

struct theft_type_info gr2mjd_nasa_info = {
    .alloc = select_gr2mjd_nasa, //nothing to free
};

struct theft_type_info eg2mjd_cctue_info = {
    .alloc = select_eg2mjd_cctue, //nothing to free
};

struct theft_type_info gr2sym454_irv_info = {
    .alloc = select_gr2sym454_irv, //nothing to free
    .print = print_known
};

struct theft_type_info gr2fr_brit_info = {
    .alloc = select_gr2fr_brit, //nothing to free
    .print = print_known
};

struct theft_type_info gr2fr_wiki_info = {
    .alloc = select_gr2fr_wiki, //nothing to free
    .print = print_known
};

struct theft_type_info gr_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_gregorian,
    .print = print_cal
};

struct theft_type_info tq_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_tranquility,
    .print = print_cal
};

struct theft_type_info gr_year0_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_gregorian_year_zero,
    .print = print_cal
};

struct theft_type_info tq_year0_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_tranquility_year_zero,
    .print = print_cal
};

struct theft_type_info hl_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_holocene,
    .print = print_cal
};

struct theft_type_info ct_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_cotsworth,
    .print = print_cal
};

struct theft_type_info jl_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_julian,
    .print = print_cal
};

struct theft_type_info ps_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_positivist,
    .print = print_cal
};

struct theft_type_info sym454_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_symmetry454,
    .print = print_cal
};

struct theft_type_info sym010_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_symmetry010,
    .print = print_cal
};

struct theft_type_info eg_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_ancient_egyptian,
    .print = print_cal
};

struct theft_type_info fr0_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_french_revolutionary_romme,
    .print = print_cal
};

struct theft_type_info fr1_cal_info = {
    .alloc = select_env, //nothing to free
    .env = (void*)&mon13_french_revolutionary_romme_sub1,
    .print = print_cal
};

struct theft_type_info random_cal_info = {
    .alloc = select_random_cal, //nothing to free
    .print = print_cal
};

struct theft_type_info random_name_cal_info = {
    .alloc = select_random_name_cal, //nothing to free
    .print = print_name_cal
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

struct theft_type_info emoji_copy_fmt_info = {
    .alloc= alloc_emoji_copy_fmt,
    .free = theft_generic_free_cb,
    .print = print_s
};

struct theft_type_info ascii_namelist_info = {
    .alloc = alloc_ascii_name_list,
    .free = free_name_list,
    .print = print_custom_namelist
};

//Set fields at runtime
struct theft_type_info mjd_info;
struct theft_type_info day_offset_info;
struct theft_type_info unix_info;
struct theft_type_info year_offset_info;
struct theft_type_info placeholder_info;
struct theft_type_info small_offset_info;
struct theft_type_info positive_offset_info;
struct theft_type_info error_code_info;

int main(int argc, char** argv) {
    theft_seed seed;
    if(argc > 1) {
        seed = strtoul(argv[1], NULL, 10);
    }
    else {
        seed = theft_seed_of_time();
    }

    //Disable autoshrink to avoid long runtime in case of failure
    theft_copy_builtin_type_info(THEFT_BUILTIN_int32_t, &mjd_info);
    mjd_info.autoshrink_config.enable = false;

    theft_copy_builtin_type_info(THEFT_BUILTIN_int32_t, &day_offset_info);
    day_offset_info.autoshrink_config.enable = false;

    theft_copy_builtin_type_info(THEFT_BUILTIN_int64_t, &unix_info);
    int64_t unix_limit = ((int64_t)INT32_MAX) * UNIX_DAY;
    unix_info.env = &unix_limit;
    unix_info.autoshrink_config.enable = false;

    theft_copy_builtin_type_info(THEFT_BUILTIN_int32_t, &year_offset_info);
    int32_t year_limit = INT32_MAX / 366;
    year_offset_info.env = &year_limit;
    year_offset_info.autoshrink_config.enable = false;

    theft_copy_builtin_type_info(THEFT_BUILTIN_uint8_t, &placeholder_info);
    uint8_t placeholder_limit = 126;
    placeholder_info.env = &placeholder_limit;
    placeholder_info.autoshrink_config.enable = false;

    theft_copy_builtin_type_info(THEFT_BUILTIN_uint8_t, &small_offset_info);
    uint8_t small_offset_limit = 20;
    small_offset_info.env = &small_offset_limit;
    small_offset_info.autoshrink_config.enable = false;

    theft_copy_builtin_type_info(THEFT_BUILTIN_uint32_t, &positive_offset_info);
    uint32_t positive_offset_limit = INT32_MAX;
    positive_offset_info.env = &positive_offset_limit;
    positive_offset_info.autoshrink_config.enable = false;

    theft_copy_builtin_type_info(THEFT_BUILTIN_int8_t, &error_code_info);
    error_code_info.autoshrink_config.enable = false;

    struct theft_run_config config[] = {
        {
            .name = "mon13_mjd*Unix: basic",
            .prop2 = unix_basic,
            .type_info = {
                &unix_info,
                &day_offset_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjd*Unix: gmtime",
            .prop1 = unix_gmtime,
            .type_info = {
                &unix_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjd*C99Tm: basic",
            .prop1 = c99tm_basic,
            .type_info = {
                &unix_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjd*Rd: basic",
            .prop2 = rd_basic,
            .type_info = {
                &day_offset_info,
                &day_offset_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdFromYmd: Gregorian Year 0<->Tranquility (OA)",
            .prop1 = fromYmd_known,
            .type_info = {&gr2tq_oa_info},
            .seed = seed,
            .trials = SIZEOF_ARR(gr2tq_oa)
        },
        {
            .name = "mon13_mjdFromYmd: Gregorian Year 0<->Tranquility (Handy)",
            .prop1 = fromYmd_known,
            .type_info = {&gr2tq_handy_info},
            .seed = seed,
            .trials = SIZEOF_ARR(gr2tq_handy)
        },
        {
            .name = "mon13_mjdFromYmd: Gregorian Year 0<->Cotsworth (Wikipedia)",
            .prop1 = fromYmd_known,
            .type_info = {&gr2ct_wiki_info},
            .seed = seed,
            .trials = SIZEOF_ARR(gr2ct_wiki)
        },
        {
            .name = "mon13_mjdFromYmd: Gregorian Year 0<->Julian (Wikipedia)",
            .prop1 = fromYmd_known,
            .type_info = {&gr2jl_wiki_info},
            .seed = seed,
            .trials = SIZEOF_ARR(gr2jl_wiki)
        },
        {
            .name = "mon13_mjdFromYmd: Gregorian Year 0<->Positivist (positivists.org)",
            .prop1 = fromYmd_known,
            .type_info = {&gr2ps_positivists_org_info},
            .seed = seed,
            .trials = SIZEOF_ARR(gr2ps_positivists_org)
        },
        {
            .name = "mon13_mjdFromYmd: Gregorian Year 0<->MJD",
            .prop1 = fromYmd_known_mjd,
            .type_info = {&gr2mjd_nasa_info},
            .seed = seed,
            .trials = SIZEOF_ARR(gr2mjd_nasa)
        },
        {
            .name = "mon13_mjdFromYmd: Ancient Egyptian<->MJD",
            .prop1 = fromYmd_known_mjd,
            .type_info = {&eg2mjd_cctue_info},
            .seed = seed,
            .trials = SIZEOF_ARR(eg2mjd_cctue)
        },
        {
            .name = "mon13_mjdFromYmd: Gregorian Year 0<->Symmetry454",
            .prop1 = fromYmd_known,
            .type_info = {&gr2sym454_irv_info},
            .seed = seed,
            .trials = SIZEOF_ARR(gr2sym454_irv)
        },
        {
            .name = "mon13_mjdFromYmd: Gregorian Year 0<->French Revolutionary Romme-1 (Britannica)",
            .prop1 = fromYmd_known,
            .type_info = {&gr2fr_brit_info},
            .seed = seed,
            .trials = SIZEOF_ARR(gr2fr_brit)
        },
        {
            .name = "mon13_mjdFromYmd: Gregorian Year 0<->French Revolutionary Romme-1 (Wikipedia)",
            .prop1 = fromYmd_known,
            .type_info = {&gr2fr_wiki_info},
            .seed = seed,
            .trials = SIZEOF_ARR(gr2fr_wiki)
        },
        {
            .name = "mon13_mjd*Ymd: Roundtrip",
            .prop2 = ymd_roundtrip,
            .type_info = {
                &mjd_info,
                &random_cal_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(cal_list) * 100
        },
        {
            .name = "mon13_mjdToYmd: Gregorian Year 0<->Holocene",
            .prop1 = ymd_gr_hl,
            .type_info = {
                &mjd_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: null args",
            .prop2 = ymd_null,
            .type_info = {
                &mjd_info,
                &random_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdFrom*: null result",
            .prop2 = mjd_res_null,
            .type_info = {
                &day_offset_info,
                &random_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Gregorian<->Year 0",
            .prop3 = ymd_yz,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Tranquility<->Year 0",
            .prop3 = ymd_yz,
            .type_info = {
                &mjd_info,
                &tq_year0_cal_info,
                &tq_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Gregorian Year 0<->Cotsworth same year",
            .prop3 = ymd_same_year,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
                &ct_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Symmetry454<->Symmetry010",
            .prop3 = ymd_same_year,
            .type_info = {
                &mjd_info,
                &sym454_cal_info,
                &sym010_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, Gregorian Year 0",
            .prop2 = ymd_add_1day_gr,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, Gregorian",
            .prop2 = ymd_add_1day_gr,
            .type_info = {
                &mjd_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, Holocene",
            .prop2 = ymd_add_1day_gr,
            .type_info = {
                &mjd_info,
                &hl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, Julian",
            .prop2 = ymd_add_1day_gr,
            .type_info = {
                &mjd_info,
                &jl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, Tranquility Year 0",
            .prop2 = ymd_add_1day_tq,
            .type_info = {
                &mjd_info,
                &tq_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, Tranquility",
            .prop2 = ymd_add_1day_tq,
            .type_info = {
                &mjd_info,
                &tq_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, Cotsworth",
            .prop2 = ymd_add_1day_ct,
            .type_info = {
                &mjd_info,
                &ct_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, Positivist",
            .prop2 = ymd_add_1day_ps,
            .type_info = {
                &mjd_info,
                &ps_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, Symmetry454",
            .prop2 = ymd_add_1day_sym454,
            .type_info = {
                &mjd_info,
                &sym454_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, Symmetry010",
            .prop2 = ymd_add_1day_sym010,
            .type_info = {
                &mjd_info,
                &sym010_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, Ancient Egyptian",
            .prop2 = ymd_add_1day_fr,
            .type_info = {
                &mjd_info,
                &eg_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, French Revolutionary Romme",
            .prop2 = ymd_add_1day_fr,
            .type_info = {
                &mjd_info,
                &fr0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToYmd: Add 1 Day, French Revolutionary Romme-1",
            .prop2 = ymd_add_1day_fr,
            .type_info = {
                &mjd_info,
                &fr1_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfYear: Gregorian Year 0<->Positivist same doy",
            .prop3 = doy_same,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
                &ps_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfYear: Gregorian Year 0<->Cotsworth same doy",
            .prop3 = doy_same,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
                &ct_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfYear: Symmetry454<->Symmetry010 same doy",
            .prop3 = doy_same,
            .type_info = {
                &mjd_info,
                &sym454_cal_info,
                &sym010_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: Gregorian Year 0<->Cotsworth same leap",
            .prop3 = leap_same,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
                &ct_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: Gregorian Year 0<->Positivist same leap",
            .prop3 = leap_same,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
                &ps_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: Symmetry454<->Symmetry010",
            .prop3 = leap_same,
            .type_info = {
                &mjd_info,
                &sym454_cal_info,
                &sym010_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: Gregorian Year 0 leap count",
            .prop2 = leap9,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: Gregorian leap count",
            .prop2 = leap9,
            .type_info = {
                &mjd_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: Tranquility Year 0 leap count",
            .prop2 = leap9,
            .type_info = {
                &mjd_info,
                &tq_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: Tranquility leap count",
            .prop2 = leap9,
            .type_info = {
                &mjd_info,
                &tq_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: Julian leap count",
            .prop2 = leap9,
            .type_info = {
                &mjd_info,
                &jl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: Symmetry454 leap count",
            .prop2 = leap7,
            .type_info = {
                &mjd_info,
                &sym454_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: Ancient Egyptian never leaps",
            .prop2 = leap0,
            .type_info = {
                &mjd_info,
                &eg_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: French Revolutionary Romme leap count",
            .prop2 = leap_fr,
            .type_info = {
                &mjd_info,
                &fr0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToIsLeapYear: French Revolutionary Romme-1 leap count",
            .prop2 = leap_fr,
            .type_info = {
                &mjd_info,
                &fr0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfWeek: Gregorian Year 0",
            .prop2 = day_of_week_continuous,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfWeek: Tranquility Year 0",
            .prop2 = day_of_week_tq,
            .type_info = {
                &mjd_info,
                &tq_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfWeek: Ancient Egyptian",
            .prop2 = day_of_week_fr,
            .type_info = {
                &mjd_info,
                &eg_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfWeek: French Revolutionary Romme",
            .prop2 = day_of_week_fr,
            .type_info = {
                &mjd_info,
                &fr0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfWeek: French Revolutionary Romme-1",
            .prop2 = day_of_week_fr,
            .type_info = {
                &mjd_info,
                &fr1_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfWeek: Gregorian Year 0<->Gregorian same",
            .prop3 = day_of_week_same,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfWeek: Gregorian Year 0<->Holocene same",
            .prop3 = day_of_week_same,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
                &hl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfWeek: Gregorian Year 0<->Julian same",
            .prop3 = day_of_week_same,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
                &jl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfWeek: Gregorian Year 0<->Symmetry454 same",
            .prop3 = day_of_week_same,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
                &sym454_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfWeek: Gregorian Year 0<->Symmetry010 same",
            .prop3 = day_of_week_same,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
                &sym010_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfYear: Gregorian Year 0",
            .prop2 = day_of_year_add_one,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfYear: Tranquility Year 0",
            .prop2 = day_of_year_add_one,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfYear: Ancient Egyptian",
            .prop2 = day_of_year_add_one,
            .type_info = {
                &mjd_info,
                &eg_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfYear: French Revolutionary Romme",
            .prop2 = day_of_year_add_one,
            .type_info = {
                &mjd_info,
                &fr0_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfYear: French Revolutionary Romme-1",
            .prop2 = day_of_year_add_one,
            .type_info = {
                &mjd_info,
                &fr1_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_mjdToDayOfYear: Symmetry454",
            .prop2 = day_of_year_add_one_sym454,
            .type_info = {
                &mjd_info,
                &sym454_cal_info,
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, Gregorian Year 0",
            .prop2 = add_1month_gr,
            .type_info = {
                &mjd_info,
                &gr_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, Gregorian",
            .prop2 = add_1month_gr,
            .type_info = {
                &mjd_info,
                &gr_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, Holocene",
            .prop2 = add_1month_gr,
            .type_info = {
                &mjd_info,
                &hl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, Julian",
            .prop2 = add_1month_gr,
            .type_info = {
                &mjd_info,
                &jl_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, Tranquility Year 0",
            .prop2 = add_1month_tq,
            .type_info = {
                &mjd_info,
                &tq_year0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, Tranquility",
            .prop2 = add_1month_tq,
            .type_info = {
                &mjd_info,
                &tq_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, Cotsworth",
            .prop2 = add_1month_ct,
            .type_info = {
                &mjd_info,
                &ct_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, Positivist",
            .prop2 = add_1month_ps,
            .type_info = {
                &mjd_info,
                &ps_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, Symmetry454",
            .prop2 = add_1month_sym454,
            .type_info = {
                &mjd_info,
                &sym454_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, Symmetry010",
            .prop2 = add_1month_sym010,
            .type_info = {
                &mjd_info,
                &sym010_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, Ancient Egyptian",
            .prop2 = add_1month_fr,
            .type_info = {
                &mjd_info,
                &eg_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, French Revolutionary Romme",
            .prop2 = add_1month_fr,
            .type_info = {
                &mjd_info,
                &fr0_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addMonths: Add 1 Month, French Revolutionary Romme-1",
            .prop2 = add_1month_fr,
            .type_info = {
                &mjd_info,
                &fr1_cal_info
            },
            .seed = seed
        },
        {
            .name = "mon13_addYears: Add Years",
            .prop3 = add_year,
            .type_info = {
                &mjd_info,
                &year_offset_info,
                &random_cal_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(cal_list) * 100
        },
        {
            .name = "mon13_addYears: Add Zero",
            .prop2 = add_zero,
            .type_info = {
                &mjd_info,
                &random_cal_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(cal_list) * 100
        },
        {
            .name = "mon13_diffMonths: Roundtrip",
            .prop3 = diff_months_roundtrip,
            .type_info = {
                &mjd_info,
                &mjd_info,
                &random_cal_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(cal_list) * 100
        },
        {
            .name = "mon13_diffMonths: Small Days",
            .prop3 = diff_months_small_day,
            .type_info = {
                &mjd_info,
                &random_cal_info,
                &small_offset_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(cal_list) * 100
        },
        {
            .name = "mon13_diffYears: Roundtrip",
            .prop3 = diff_years_roundtrip,
            .type_info = {
                &mjd_info,
                &mjd_info,
                &random_cal_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(cal_list) * 100
        },
        {
            .name = "mon13_diffYears: Small Days",
            .prop3 = diff_years_small_day,
            .type_info = {
                &mjd_info,
                &random_cal_info,
                &small_offset_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(cal_list) * 100
        },
        {
            .name = "mon13_validNameList: Builtin",
            .prop1 = valid_name_list_builtin,
            .type_info = {
                &random_name_cal_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list)
        },
        {
            .name = "mon13_validNameList: NULL",
            .prop1 = valid_name_list_null,
            .type_info = {
                &random_name_cal_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list)
        },
        {
            .name = "mon13_validNameList: Missing months",
            .prop1 = valid_name_list_no_month,
            .type_info = {
                &random_name_cal_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list)
        },
        {
            .name = "mon13_validNameList: Missing weekdays",
            .prop1 = valid_name_list_no_weekdays,
            .type_info = {
                &random_name_cal_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list)
        },
        {
            .name = "mon13_validNameList: Missing eras",
            .prop1 = valid_name_list_no_eras,
            .type_info = {
                &random_name_cal_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list)
        },
        {
            .name = "mon13_validNameList: Missing name",
            .prop1 = valid_name_list_no_name,
            .type_info = {
                &random_name_cal_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list)
        },
        {
            .name = "mon13_validNameList: Gregorian Year 0",
            .prop2 = valid_name_list_gregorian,
            .type_info = {
                &gr_year0_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: Tranquility Year 0",
            .prop2 = valid_name_list_tranquility,
            .type_info = {
                &tq_year0_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: Cotsworth",
            .prop2 = valid_name_list_cotsworth,
            .type_info = {
                &ct_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: French Revolutionary Romme",
            .prop2 = valid_name_list_french_revolutionary,
            .type_info = {
                &fr0_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: Ancient Egyptian",
            .prop2 = valid_name_list_ancient_egyptian,
            .type_info = {
                &eg_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: Gregorian = Gregorian Year 0",
            .prop3 = valid_name_list_same,
            .type_info = {
                &gr_cal_info,
                &gr_year0_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: Holocene = Gregorian Year 0",
            .prop3 = valid_name_list_same,
            .type_info = {
                &hl_cal_info,
                &gr_year0_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: Julian = Gregorian Year 0",
            .prop3 = valid_name_list_same,
            .type_info = {
                &jl_cal_info,
                &gr_year0_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: Symmetry454 = Gregorian Year 0",
            .prop3 = valid_name_list_same,
            .type_info = {
                &sym454_cal_info,
                &gr_year0_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: Symmetry010 = Gregorian Year 0",
            .prop3 = valid_name_list_same,
            .type_info = {
                &sym010_cal_info,
                &gr_year0_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: Tranquility = Tranquility Year 0",
            .prop3 = valid_name_list_same,
            .type_info = {
                &tq_cal_info,
                &tq_year0_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: Positivist = Cotsworth",
            .prop3 = valid_name_list_same,
            .type_info = {
                &ps_cal_info,
                &ct_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_validNameList: French Revolutionary Romme = Romme - 1",
            .prop3 = valid_name_list_same,
            .type_info = {
                &fr0_cal_info,
                &fr1_cal_info,
                &ascii_namelist_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: %%",
            .prop3 = format_percent,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: %A",
            .prop3 = format_weekday,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: %B",
            .prop3 = format_month,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: %d",
            .prop3 = format_day_of_month,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: %f",
            .prop3 = format_cal,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: %j",
            .prop3 = format_doy,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: %m",
            .prop3 = format_month_number,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: %n",
            .prop3 = format_newline,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: %q",
            .prop3 = format_era,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: %t",
            .prop3 = format_tab,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: %u",
            .prop3 = format_weekday_number,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: %Y",
            .prop3 = format_year,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: strftime",
            .prop3 = format_strftime,
            .type_info = {
                &mjd_info,
                &placeholder_info,
                &strftime_fmt_info
            },
            .seed = seed
        },
        {
            .name = "mon13_format: numeric padding",
            .prop4 = format_numeric_padding,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info,
                &numeric_fmt_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: numeric null namelist",
            .prop5 = format_numeric_null,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info,
                &placeholder_info,
                &numeric_fmt_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: ASCII copy",
            .prop4 = format_simple_copy,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info,
                &ascii_copy_fmt_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: emoji copy",
            .prop4 = format_simple_copy,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info,
                &emoji_copy_fmt_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: null",
            .prop4 = format_null,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info,
                &strftime_fmt_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: dry run",
            .prop3 = format_dry_run,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &strftime_fmt_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: truncate",
            .prop4 = format_truncate,
            .type_info = {
                &mjd_info,
                &random_name_cal_info,
                &placeholder_info,
                &strftime_fmt_info
            },
            .seed = seed,
            .trials = SIZEOF_ARR(name_cal_list) * 100
        },
        {
            .name = "mon13_format: invalid name list",
            .prop5 = format_invalid_names,
            .type_info = {
                &mjd_info,
                &random_cal_info,
                &ascii_namelist_info,
                &placeholder_info,
                &strftime_fmt_info
            }
        },
        {
            .name = "mon13_errorMessage",
            .prop1 = error_code_message,
            .type_info = {
                &error_code_info
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
    printf("SEED:\t%lu\n", seed);
    printf("PROP:\t%lu\n", prop_count);
    printf("PASS:\t%lu\n", pass);
    printf("FAIL:\t%lu\n", fail);
    printf("SKIP:\t%lu\n", skip);
    printf("ERROR:\t%lu\n", error);
    printf("OTHER:\t%lu\n", other);
    return 0;
}