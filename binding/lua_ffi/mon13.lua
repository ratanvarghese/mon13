ffi = require("ffi")
ffi.cdef[[
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
	bool* res_isLeap
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

mon13.Weekday7 = {
	NO_WEEKDAY = 0,
	MONDAY = 1,
	TUESDAY = 2,
	WEDNESDAY = 3,
	THURSDAY = 4,
	FRIDAY = 5,
	SATURDAY = 6,
	SUNDAY = 7
}

mon13.Weekday10 = {
	PRIMIDI = 1,
	DUODI = 2,
	TRIDI = 3,
	QUARTIDI = 4,
	QUINTIDI = 5,
	SEXTIDI = 6,
	SEPTIDI = 7,
	OCTIDI = 8,
	NONIDI = 9,
	DECADI = 10,
}

mon13.Error = {
	NONE = 0,
	UNKNOWN = -1,
	NULL_CALENDAR = -2,
	NULL_NAME_LIST = -3,
	NULL_FORMAT = -4,
	NULL_INPUT = -5,
	NULL_RESULT = -6,
	NULL_DATE = -7,
	OVERFLOW = -64,
	BAD_CALENDAR = -65,
	DATE_NOT_FOUND = -66,
	DAY_OF_YEAR_NOT_FOUND = -67,
	INVALID_UTF8 = -69,
	INVALID_STATE = -70,
	INVALID_SEQUENCE = -71,
	FAILED_TO_INSERT_NULLCHAR = -72,
	INVALID_DATE = -73,
}

local function tail(res, status)
	if status == mon13.Error.NONE then
		return tonumber(res[0])
	else
		return nil, status
	end
end

local c_bool = ffi.typeof("bool[1]")
local c_int64 = ffi.typeof("int64_t[1]")
local c_int32 = ffi.typeof("int32_t[1]")
local c_uint8 = ffi.typeof("uint8_t[1]")
local c_uint16 = ffi.typeof("uint16_t[1]")
local c_str = ffi.typeof("char[?]")

function mon13.validYmd(cal, ymd)
	return raw_lib.mon13_validYmd(cal, ymd.year, ymd.month, ymd.day) ~= 0
end


function mon13.mjdFromYmd(cal, ymd)
	local res = c_int32()
	local status = raw_lib.mon13_mjdFromYmd(cal, ymd.year, ymd.month, ymd.day, res)
	return tail(res, status)
end

function mon13.mjdFromUnix(unix_time)
	local res = c_int32()
	local status = raw_lib.mon13_mjdFromUnix(unix_time, res)
	return tail(res, status)
end

function mon13.mjdFromRd(rd)
	local res = c_int32()
	local status = raw_lib.mon13_mjdFromRd(rd, res)
	return tail(res, status)
end


function mon13.mjdToYmd(mjd, cal)
	local res_y = c_int32()
	local res_m = c_uint8()
	local res_d = c_uint8()
	local status = raw_lib.mon13_mjdToYmd(mjd, cal, res_y, res_m, res_d)
	if status == mon13.Error.NONE then
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
	local res = c_int64()
	local status = raw_lib.mon13_mjdToUnix(mjd, res)
	return tail(res, status)
end

function mon13.mjdToRd(mjd)
	local res = c_int32()
	local status = raw_lib.mon13_mjdToRd(mjd, res)
	return tail(res, status)
end

function mon13.mjdToIsLeapYear(mjd, cal)
	local res = c_bool()
	local status = raw_lib.mon13_mjdToIsLeapYear(mjd, cal, res)
	if status == mon13.Error.NONE then
		return res[0]
	else
		return nil, status
	end
end

function mon13.mjdToDayOfWeek(mjd, cal)
	local res = c_uint8()
	local status = raw_lib.mon13_mjdToDayOfWeek(mjd, cal, res)
	return tail(res, status)
end

function mon13.mjdToDayOfYear(mjd, cal)
	local res = c_uint16()
	local status = raw_lib.mon13_mjdToDayOfYear(mjd, cal, res)
	return tail(res, status)
end


function mon13.addMonths(mjd, cal, offset)
	local res = c_int32()
	local status = raw_lib.mon13_addMonths(mjd, cal, offset, res)
	return tail(res, status)
end

function mon13.addYears(mjd, cal, offset)
	local res = c_int32()
	local status = raw_lib.mon13_addYears(mjd, cal, offset, res)
	return tail(res, status)
end


function mon13.diffMonths(mjd0, mjd1, cal)
	local res = c_int32()
	local status = raw_lib.mon13_diffMonths(mjd0, mjd1, cal, res)
	return tail(res, status)
end

function mon13.diffYears(mjd0, mjd1, cal)
	local res = c_int32()
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

	local buf = c_str(buflen + 1)
	local status = raw_lib.mon13_format(mjd, cal, nlist, fmt, buf, buflen + 1)
	if status < 0 then
		return nil, status
	else
		return ffi.string(buf)
	end
end

return mon13