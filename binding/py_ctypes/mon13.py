from ctypes import *
from ctypes.util import find_library
from collections import namedtuple
from enum import Enum

_libmon13 = cdll.LoadLibrary(find_library("mon13"))

class _CAL(Structure):
	pass

class _NAMELIST(Structure):
	_fields_ = [("month_list", POINTER(c_char_p)),
				("weekday_list", POINTER(c_char_p)),
				("era_list", POINTER(c_char_p)),
				("intercalary_list", POINTER(c_char_p)),
				("alt_intercalary_list", POINTER(c_char_p)),
				("calendar_name", c_char_p)]

def _getCal(name):
	return pointer(_CAL.in_dll(_libmon13, name))

def _getNameList(name):
	return _NAMELIST.in_dll(_libmon13, name)

gregorian = _getCal("mon13_gregorian")
gregorian_year_zero = _getCal("mon13_gregorian_year_zero")
tranquility = _getCal("mon13_tranquility")
tranquility_year_zero = _getCal("mon13_tranquility_year_zero")
holocene = _getCal("mon13_holocene")
cotsworth = _getCal("mon13_cotsworth")
julian = _getCal("mon13_julian")
positivist = _getCal("mon13_positivist")
symmetry454 = _getCal("mon13_symmetry454")
symmetry010 = _getCal("mon13_symmetry010")
ancient_egyptian = _getCal("mon13_ancient_egyptian")
french_revolutionary_romme = _getCal("mon13_french_revolutionary_romme")
french_revolutionary_romme_sub1 = _getCal("mon13_french_revolutionary_romme_sub1")

gregorian_names_en_US = _getNameList("mon13_gregorian_names_en_US")
tranquility_names_en_US = _getNameList("mon13_tranquility_names_en_US")
holocene_names_en_US = _getNameList("mon13_holocene_names_en_US")
cotsworth_names_en_US = _getNameList("mon13_cotsworth_names_en_US")
julian_names_en_US = _getNameList("mon13_julian_names_en_US")
positivist_names_en_US = _getNameList("mon13_positivist_names_en_US")
symmetry454_names_en_US = _getNameList("mon13_symmetry454_names_en_US")
symmetry010_names_en_US = _getNameList("mon13_symmetry010_names_en_US")
ancient_egyptian_names_en_US = _getNameList("mon13_ancient_egyptian_names_en_US")

french_revolutionary_names_en_GB = _getNameList("mon13_french_revolutionary_names_en_GB")
french_revolutionary_names_en_GB_joke = _getNameList("mon13_french_revolutionary_names_en_GB_joke")

gregorian_names_fr_FR = _getNameList("mon13_gregorian_names_fr_FR")
julian_names_fr_FR = _getNameList("mon13_julian_names_fr_FR")
positivist_names_fr_FR = _getNameList("mon13_positivist_names_fr_FR")
french_revolutionary_names_fr_FR = _getNameList("mon13_french_revolutionary_names_fr_FR")


Ymd = namedtuple('Ymd', ['year', 'month', 'day'])
NameList = namedtuple('NameList',[
	'month_list',
	'weekday_list',
	'era_list',
	'intercalary_list',
	'alt_intercalary_list',
	'calendar_name'
])

class Weekday7(Enum):
	NO_WEEKDAY = 0
	MONDAY = 1
	TUESDAY = 2
	WEDNESDAY = 3
	THURSDAY = 4
	FRIDAY = 5
	SATURDAY = 6
	SUNDAY = 7

class Weekday10(Enum):
	PRIMIDI = 1
	DUODI = 2
	TRIDI = 3
	QUARTIDI = 4
	QUINTIDI = 5
	SEXTIDI = 6
	SEPTIDI = 7
	OCTIDI = 8
	NONIDI = 9
	DECADI = 10

def _tail(res, status):
	if status == 0:
		return res.value, status
	else:
		return None, status


def validYmd(cal, ymd):
	return _libmon13.mon13_validYmd(cal, ymd.year, ymd.month, ymd.day) != 0


def mjdFromYmd(cal, ymd):
	res = c_int32()
	status = _libmon13.mon13_mjdFromYmd(cal, ymd.year, ymd.month, ymd.day, byref(res))
	return _tail(res, status)

def mjdFromUnix(unix_time):
	res = c_int32()
	status = _libmon13.mon13_mjdFromUnix(c_int64(unix_time), byref(res))
	return _tail(res, status)

def mjdFromRd(rd):
	res = c_int32()
	status = _libmon13.mon13_mjdFromRd(rd, byref(res))
	return _tail(res, status)


def mjdToYmd(mjd, cal):
	y = c_int32()
	m = c_uint8()
	d = c_uint8()
	status = _libmon13.mon13_mjdToYmd(mjd, cal, byref(y), byref(m), byref(d))
	if status == 0:
		return Ymd(y.value, m.value, d.value), status
	else:
		return None, status

def mjdToUnix(mjd):
	res = c_int64()
	status = _libmon13.mon13_mjdToUnix(c_int32(mjd), byref(res))
	return _tail(res, status)

def mjdToRd(mjd):
	res = c_int32()
	status = _libmon13.mon13_mjdToRd(mjd, byref(res))
	return _tail(res, status)

def mjdToIsLeapYear(mjd, cal):
	res = c_bool()
	status = _libmon13.mon13_mjdToIsLeapYear(mjd, cal, byref(res))
	return _tail(res, status)

def mjdToDayOfWeek(mjd, cal):
	res = c_uint8()
	status = _libmon13.mon13_mjdToDayOfWeek(mjd, cal, byref(res))
	return _tail(res, status)

def mjdToDayOfYear(mjd, cal):
	res = c_uint16()
	status = _libmon13.mon13_mjdToDayOfYear(mjd, cal, byref(res))
	return _tail(res, status)


def addMonths(mjd, cal, offset):
	res = c_int32()
	status = _libmon13.mon13_addMonths(mjd, cal, offset, byref(res))
	return _tail(res, status)

def addYears(mjd, cal, offset):
	res = c_int32()
	status = _libmon13.mon13_addYears(mjd, cal, offset, byref(res))
	return _tail(res, status)


def diffMonths(mjd0, mjd1, cal):
	res = c_int32()
	status = _libmon13.mon13_diffMonths(mjd0, mjd1, cal, byref(res))
	return _tail(res, status)

def diffYears(mjd0, mjd1, cal):
	res = c_int32()
	status = _libmon13.mon13_diffYears(mjd0, mjd1, cal, byref(res))
	return _tail(res, status)

def _makeStringList(input_list):
	if type(input_list) is list:
		res = (c_char_p * (len(input_list) + 1))()
		res_buf = {}
		for i,v in enumerate(input_list):
			bv = bytes(v, "UTF-8")
			buf = create_string_buffer(bv, len(bv) + 1)
			res[i] = c_char_p(addressof(buf))
			res_buf[i] = buf
		res[len(input_list)] = None
		return res, res_buf
	else:
		return input_list, None

def format(mjd, cal, nlist=None, fmt=""):
	bm = None
	bw = None
	be = None
	bi = None
	ba = None
	bn = None
	if type(nlist) is NameList:
		old_nlist = nlist
		nlist = _NAMELIST()
		nlist.month_list, bm = _makeStringList(old_nlist.month_list)
		nlist.weekday_list, bw = _makeStringList(old_nlist.weekday_list)
		nlist.era_list, be = _makeStringList(old_nlist.era_list)
		nlist.intercalary_list, bi = _makeStringList(old_nlist.intercalary_list)
		nlist.alt_intercalary_list, ba = _makeStringList(old_nlist.alt_intercalary_list)
		if type(old_nlist.calendar_name) is str:
			bname = bytes(old_nlist.calendar_name, "UTF-8")
			bn = create_string_buffer(bname, len(bname) + 1)
			nlist.calendar_name = c_char_p(addressof(bn))
		else:
			nlist.calendar_name = old_nlist.calendar_name

	nlist_arg = None
	if nlist is not None:
		nlist_arg = byref(nlist)

	b_fmt = bytes(fmt, "UTF-8")
	c_fmt = create_string_buffer(b_fmt, len(b_fmt) + 1)
	buflen = _libmon13.mon13_format(mjd, cal, nlist_arg, c_fmt, None, 0)
	if buflen < 0:
		return None, buflen

	buf = create_string_buffer(buflen)
	status = _libmon13.mon13_format(mjd, cal, nlist_arg, c_fmt, buf, buflen + 1)
	if status < 0:
		return None, status
	else:
		return buf.value.decode("UTF-8"), status