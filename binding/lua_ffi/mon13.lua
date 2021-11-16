ffi = require("ffi")
ffi.cdef[[
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

enum Weekday10 {
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
    MON13_ERROR_INVALID_UTF8 = -69,
    MON13_ERROR_INVALID_STATE = -70,
    MON13_ERROR_INVALID_SEQUENCE = -71,
    MON13_ERROR_FAILED_TO_INSERT_NULLCHAR = -72,
    MON13_ERROR_INVALID_DATE = -73,
};

//Structures
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
    int* res_weekday
);
int mon13_mjdToDayOfYear(
    int32_t mjd,
    const struct mon13_Cal* cal,
    int* res_yearday
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

int mon13_format(
    int32_t mjd,
    const struct mon13_Cal* cal,
    const struct mon13_NameList* nlist,
    const char* fmt,
    char* buf,
    int32_t buflen
);

//Predefined name lists
extern const struct mon13_NameList mon13_gregorian_names_en_US;
extern const struct mon13_NameList mon13_tranquility_names_en_US;
extern const struct mon13_NameList mon13_holocene_names_en_US;
extern const struct mon13_NameList mon13_cotsworth_names_en_US;
extern const struct mon13_NameList mon13_julian_names_en_US;
extern const struct mon13_NameList mon13_positivist_names_en_US;
extern const struct mon13_NameList mon13_symmetry454_names_en_US;
extern const struct mon13_NameList mon13_symmetry010_names_en_US;
extern const struct mon13_NameList mon13_ancient_egyptian_names_en_US;

extern const struct mon13_NameList mon13_french_revolutionary_names_en_GB;
extern const struct mon13_NameList mon13_french_revolutionary_names_en_GB_joke;

extern const struct mon13_NameList mon13_gregorian_names_fr_FR;
extern const struct mon13_NameList mon13_julian_names_fr_FR;
extern const struct mon13_NameList mon13_positivist_names_fr_FR;
extern const struct mon13_NameList mon13_french_revolutionary_names_fr_FR;

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
]]
local raw_lib = ffi.load("mon13")

local mon13 = {
	gregorian_names_en_US = raw_lib.mon13_gregorian_names_en_US,
	tranquility_names_en_US = raw_lib.mon13_tranquility_names_en_US,
	holocene_names_en_US = raw_lib.mon13_holocene_names_en_US,
	cotsworth_names_en_US = raw_lib.mon13_cotsworth_names_en_US,
	julian_names_en_US = raw_lib.mon13_julian_names_en_US,
	positivist_names_en_US = raw_lib.mon13_positivist_names_en_US,
	symmetry454_names_en_US = raw_lib.mon13_symmetry454_names_en_US,
	symmetry010_names_en_US = raw_lib.mon13_symmetry010_names_en_US,
	ancient_egyptian_names_en_US = raw_lib.mon13_ancient_egyptian_names_en_US,

	french_revolutionary_names_en_GB = raw_lib.mon13_french_revolutionary_names_en_GB,
	french_revolutionary_names_en_GB_joke = raw_lib.mon13_french_revolutionary_names_en_GB_joke,

	gregorian_names_fr_FR = raw_lib.mon13_gregorian_names_fr_FR,
	julian_names_fr_FR = raw_lib.mon13_julian_names_fr_FR,
	positivist_names_fr_FR = raw_lib.mon13_positivist_names_fr_FR,
	french_revolutionary_names_fr_FR = raw_lib.mon13_french_revolutionary_names_fr_FR,

	gregorian = raw_lib.mon13_gregorian,
	gregorian_year_zero = raw_lib.mon13_gregorian_year_zero,
	tranquility = raw_lib.mon13_tranquility,
	tranquility_year_zero = raw_lib.mon13_tranquility_year_zero,
	holocene = raw_lib.mon13_holocene,
	cotsworth = raw_lib.mon13_cotsworth,
	julian = raw_lib.mon13_julian,
	positivist = raw_lib.mon13_positivist,
	symmetry454 = raw_lib.mon13_symmetry454,
	symmetry010 = raw_lib.mon13_symmetry010,
	ancient_egyptian = raw_lib.mon13_ancient_egyptian,
	french_revolutionary_romme = raw_lib.mon13_french_revolutionary_romme,
	french_revolutionary_romme_sub1 = raw_lib.mon13_french_revolutionary_romme_sub1,
}

local function tail(res, status)
	if status == raw_lib.MON13_ERROR_NONE then
		return tonumber(res[0])
	else
		return nil, status
	end
end

function mon13.validYmd(cal, ymd)
	return raw_lib.validYmd(cal, ymd.year, ymd.month, ymd.day) ~= 0
end


function mon13.mjdFromYmd(cal, ymd)
	local res = ffi.new("int32_t[1]")
	local status = raw_lib.mon13_mjdFromYmd(cal, ymd.year, ymd.month, ymd.day, res)
	return tail(res, status)
end

function mon13.mjdFromUnix(unix_time)
	local res = ffi.new("int32_t[1]")
	local status = raw_lib.mon13_mjdFromUnix(unix_time, res)
	return tail(res, status)
end

function mon13.mjdFromRd(rd)
	local res = ffi.new("int32_t[1]")
	local status = raw_lib.mon13_mjdFromRd(rd, res)
	return tail(res, status)
end


function mon13.mjdToYmd(mjd, cal)
	local res_y = ffi.new("int32_t[1]")
	local res_m = ffi.new("uint8_t[1]")
	local res_d = ffi.new("uint8_t[1]")
	local status = raw_lib.mon13_mjdToYmd(mjd, cal, res_y, res_m, res_d)
	if status == raw_lib.MON13_ERROR_NONE then
		return {
			year = tonumber(res_y[0]),
			month = tonumber(res_m[0]),
			day = tonumber(res_d[0])
		}
	else
		return nil, status
	end
end

function mon13.mjdToUnix(mjd)
	local res = ffi.new("int64_t[1]")
	local status = raw_lib.mon13_mjdToUnix(mjd, res)
	return tail(res, status)
end

function mon13.mjdToRd(mjd)
	local res = ffi.new("int32_t[1]")
	local status = raw_lib.mon13_mjdToRd(mjd, res)
	return tail(res, status)
end

function mon13.mjdToIsLeapYear(mjd, cal)
	local res = ffi.new("int[1]")
	local status = raw_lib.mon13_mjdToIsLeapYear(mjd, cal, res)
	if status == raw_lib.MON13_ERROR_NONE then
		return tonumber(res[0]) ~= 0
	else
		return nil, status
	end
end

function mon13.mjdToDayOfWeek(mjd, cal)
	local res = ffi.new("int[1]")
	local status = raw_lib.mon13_mjdToDayOfWeek(mjd, cal, res)
	return tail(res, status)
end

function mon13.mjdToDayOfYear(mjd, cal)
	local res = ffi.new("int[1]")
	local status = raw_lib.mon13_mjdToDayOfYear(mjd, cal, res)
	return tail(res, status)
end


function mon13.addMonths(mjd, cal, offset)
	local res = ffi.new("int32_t[1]")
	local status = raw_lib.mon13_addMonths(mjd, cal, offset, res)
	return tail(res, status)
end

function mon13.addYears(mjd, cal, offset)
	local res = ffi.new("int32_t[1]")
	local status = raw_lib.mon13_addYears(mjd, cal, offset, res)
	return tail(res, status)
end


function mon13.diffMonths(mjd0, mjd1, cal)
	local res = ffi.new("int32_t[1]")
	local status = raw_lib.mon13_diffMonths(mjd0, mjd1, cal, res)
	return tail(res, status)
end

function mon13.diffYears(mjd0, mjd1, cal)
	local res = ffi.new("int32_t[1]")
	local status = raw_lib.mon13_diffYears(mjd0, mjd1, cal, res)
	return tail(res, status)
end


function mon13.format(mjd, cal, arg3, arg4)
	local nlist = arg3
	local fmt = arg4
	if fmt == nil then
		nlist = nil
		fmt = arg3
	end

	local buflen = raw_lib.mon13_format(mjd, cal, nlist, fmt, nil, 0)
	if buflen < 0 then
		return nil, buflen
	end

	local buf = ffi.new("char[?]", buflen + 1)
	local status = raw_lib.mon13_format(mjd, cal, nlist, fmt, buf, buflen + 1)
	if status < 0 then
		return nil, status
	else
		return ffi.string(buf)
	end
end

return mon13