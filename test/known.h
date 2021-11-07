//This is supposed to be a private header, not exposed as part of the mon13 API.

#ifndef MON13_KNOWN_H
#define MON13_KNOWN_H

#include "mon13.h"

#define SIZEOF_ARR(arr) sizeof(arr)/sizeof(*arr)
#define UNIX_DAY (24 * 60 * 60)
#define STRFTIME_BUF 500
#define ASCII_COPY_BUF 500

struct test_Date {
	int32_t year;
	uint8_t month;
	uint8_t day;
};

struct known_convert_date {
	const struct mon13_Cal* c0;
	const struct mon13_Cal* c1;
	const struct test_Date d0;
	const struct test_Date d1;
};

struct known_convert_mjd {
	const struct mon13_Cal* c;
	const struct test_Date d;
	int32_t mjd;
};

struct name_cal {
	const struct mon13_Cal* c;
	const struct mon13_NameList* n;
};

const struct name_cal name_cal_list[] = {
	{
		.c = &mon13_gregorian,
		.n = &mon13_gregorian_names_en_US,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.n = &mon13_gregorian_names_en_US,
	},
	{
		.c = &mon13_gregorian,
		.n = &mon13_gregorian_names_fr_FR,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.n = &mon13_gregorian_names_fr_FR,
	},
	{
		.c = &mon13_tranquility,
		.n = &mon13_tranquility_names_en_US,
	},
	{
		.c = &mon13_tranquility_year_zero,
		.n = &mon13_tranquility_names_en_US,
	},
	{
		.c = &mon13_holocene,
		.n = &mon13_holocene_names_en_US,
	},
	{
		.c = &mon13_cotsworth,
		.n = &mon13_cotsworth_names_en_US,
	},
	{
		.c = &mon13_julian,
		.n = &mon13_julian_names_en_US,
	},
	{
		.c = &mon13_julian,
		.n = &mon13_julian_names_fr_FR,
	},
	{
		.c = &mon13_positivist,
		.n = &mon13_positivist_names_en_US,
	},
	{
		.c = &mon13_positivist,
		.n = &mon13_positivist_names_fr_FR,
	},
	{
		.c = &mon13_symmetry454,
		.n = &mon13_symmetry454_names_en_US,
	},
	{
		.c = &mon13_symmetry010,
		.n = &mon13_symmetry010_names_en_US,
	},
	{
		.c = &mon13_ancient_egyptian,
		.n = &mon13_ancient_egyptian_names_en_US,
	},
	{
		.c = &mon13_french_revolutionary_romme,
		.n = &mon13_french_revolutionary_names_en_GB,
	},
	{
		.c = &mon13_french_revolutionary_romme,
		.n = &mon13_french_revolutionary_names_en_GB_joke,
	},
	{
		.c = &mon13_french_revolutionary_romme,
		.n = &mon13_french_revolutionary_names_fr_FR,
	},
	{
		.c = &mon13_french_revolutionary_romme_sub1,
		.n = &mon13_french_revolutionary_names_en_GB,
	},
	{
		.c = &mon13_french_revolutionary_romme_sub1,
		.n = &mon13_french_revolutionary_names_en_GB_joke,
	},
	{
		.c = &mon13_french_revolutionary_romme_sub1,
		.n = &mon13_french_revolutionary_names_fr_FR,
	},
};

const struct mon13_Cal* cal_list[] = {
    &mon13_gregorian,
    &mon13_gregorian_year_zero,
    &mon13_tranquility,
    &mon13_tranquility_year_zero,
    &mon13_holocene,
    &mon13_cotsworth,
    &mon13_julian,
    &mon13_positivist,
    &mon13_symmetry454,
    &mon13_symmetry010,
    &mon13_ancient_egyptian,
    &mon13_french_revolutionary_romme,
    &mon13_french_revolutionary_romme_sub1,
};

//Tranquility & Gregorian conversions
//Gregorian dates are from Wikipedia.
//Tranquility years are from the Orion's Arm (OA) Encyclopaedia Galactica.
//https://www.orionsarm.com/eg-topic/45b2a375249bb
//OA's years had off-by-one errors on dates after Armstrong Day, corrected here.
//Tranquility month/day are manually calculated.
//Where month/day are unknown in Gregorian, end of Tranquility year is used.
const struct known_convert_date gr2tq_oa[] = {
	//Alexander the Great's birth and death
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = -356, .month = 7, .day = 20}, //Not a fake date!
		.d1 = {.year = -2325, .month = 0, .day = 1}
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = -323, .month = 6, .day = 10},
		.d1 = {.year = -2292, .month = 12, .day = 17} //end L: 28 - (21 - 10)
	},
	//Aristotle's birth and death
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = -384, .month = 7, .day = 20}, //Unknown month/day
		.d1 = {.year = -2353, .month = 0, .day = 1}
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = -322, .month = 7, .day = 20}, //Unknown month/day
		.d1 = {.year = -2291, .month = 0, .day = 1}
	},
	//Augustine's birth and death
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 354, .month = 11, .day = 13}, //Past Armstrong Day
		.d1 = {.year = -1614, .month = 5, .day = 4} //start E: 1 + (13 - 10)
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 430, .month = 8, .day = 28}, //Past Armstrong Day
		.d1 = {.year = -1538, .month = 2, .day = 11} //start B: 1 + (28 - 18)
	},
	/*
	Siddhartha Gautama founds Buddhism
	Problems:
	+ no idea where the Gregorian year is from, it's not on Wikipedia
	+ exact day unknown
	+ -535 - 1969 = -2504
	+ -529 - 1969 = -2498
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = -535, .month = 7, .day = 20},
		.d1 = {.year = -2498, .month = 0, .day = 1}
	},
	*/
	//Subrahmanyan Chandrasekhar's birth and death
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1910, .month = 10, .day = 19}, //Past Armstrong Day
		.d1 = {.year = -58, .month = 4, .day = 7} //start D: 1 + (19 - 13)
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1995, .month = 8, .day = 21}, //Past Armstrong Day
		.d1 = {.year = 27, .month = 2, .day = 4} //start B: 1 + (21 - 18)
	},
	/*Publication of *De Revolutionibus Orbium Coelestium* by Copernicus
	Problems:
	+ exact day unknown
	+ 1543 - 1969 = -426
	+ 1539 - 1969 = -430
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1543, .month = 7, .day = 20},
		.d1 = {.year = -430, .month = 0, .day = 1}
	},
	*/
	//Democritus of Abdera's birth and death
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = -460, .month = 7, .day = 20}, //Unknown month/day
		.d1 = {.year = -2429, .month = 0, .day = 1}
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = -370, .month = 7, .day = 20}, //Unknown month/day
		.d1 = {.year = -2339, .month = 0, .day = 1}
	},
	//Albert Einstein's birth and death
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1879, .month = 3, .day = 14}, //Born on Pi Day? Really?
		.d1 = {.year = -90, .month = 9, .day = 13} //start I: 1 + (14 - 2)
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1955, .month = 4, .day = 18},
		.d1 = {.year = -14, .month = 10, .day = 20} //end J: 28 - (26 - 18)
	},
	//Galileo's birth and death
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1564, .month = 2, .day = 15},
		.d1 = {.year = -405, .month = 8, .day = 14} //start H: 1 + (15 - 2)
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1642, .month = 1, .day = 8},
		.d1 = {.year = -327, .month = 7, .day = 4} //start G (nice?): 1 + (8 - 5)
	},
	//Kepler's birth and death
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1671, .month = 12, .day = 27}, //Past Armstrong Day
		.d1 = {.year = -297, .month = 6, .day = 20} //start F: 1 + (27 - 8)
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1630, .month = 11, .day = 15}, //Past Armstrong Day
		.d1 = {.year = -338, .month = 5, .day = 6} //start E: 1 + (15 - 10)
	},
	//Newton's birth and death
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1642, .month = 12, .day = 25}, //Past Armstrong Day
		.d1 = {.year = -326, .month = 6, .day = 18} //start F: 1 + (25 - 8)
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1727, .month = 3, .day = 20},
		.d1 = {.year = -242, .month = 9, .day = 19} //start I: 1 + (20 - 2)
	},
	//Plato's birth and death
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = -428, .month = 7, .day = 20}, //Unknown month/day
		.d1 = {.year = -2397, .month = 0, .day = 1}
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = -348, .month = 7, .day = 20}, //Unknown month/day
		.d1 = {.year = -2317, .month = 0, .day = 1}
	},
	//Alan Turing's birth and death
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1912, .month = 6, .day = 23},
		.d1 = {.year = -57, .month = 13, .day = 2} //start M: 1 + (23 - 22)
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1954, .month = 6, .day = 7},
		.d1 = {.year = -15, .month = 12, .day = 14} //end L: 28 - (21 - 7)
	},
};

//Tranquility and Gregorian conversions
//All dates are from https://everything2.com/title/Tranquility+Calendar
//Year shouldn't matter.
const struct known_convert_date gr2tq_handy[] = {
	//New Year's Day
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1969, .month = 1, .day = 1},
		.d1 = {.year = 0, .month = 6, .day = 25}
	},
	//Valentine's Day
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1969, .month = 2, .day = 14},
		.d1 = {.year = 0, .month = 8, .day = 13}
	},
	//April Fool's Day
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1969, .month = 4, .day = 1},
		.d1 = {.year = 0, .month = 10, .day = 3}
	},
	//US Independence Day
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1969, .month = 7, .day = 4},
		.d1 = {.year = 0, .month = 13, .day = 13}
	},
	//Bastille Day
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1969, .month = 7, .day = 14},
		.d1 = {.year = 0, .month = 13, .day = 23}
	},
	//Halloween
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1969, .month = 10, .day = 31},
		.d1 = {.year = 1, .month = 4, .day = 19}	
	},
	//Armistice Day
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1969, .month = 11, .day = 11},
		.d1 = {.year = 1, .month = 5, .day = 2}	
	},
	//Christmas Day
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_tranquility_year_zero,
		.d0 = {.year = 1969, .month = 12, .day = 25},
		.d1 = {.year = 1, .month = 6, .day = 18}	
	}
};

//Gregorian and Cotsworth conversions
//All dates are from Wikipedia
//https://en.wikipedia.org/wiki/International_Fixed_Calendar
const struct known_convert_date gr2ct_wiki[] = {
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 1, .day = 1},
		.d1 = {.year = 1902, .month = 1, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 1, .day = 28},
		.d1 = {.year = 1902, .month = 1, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 1, .day = 29},
		.d1 = {.year = 1902, .month = 2, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 2, .day = 25},
		.d1 = {.year = 1902, .month = 2, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 2, .day = 26},
		.d1 = {.year = 1902, .month = 3, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 3, .day = 25},
		.d1 = {.year = 1902, .month = 3, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1928, .month = 3, .day = 24},
		.d1 = {.year = 1928, .month = 3, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 3, .day = 26},
		.d1 = {.year = 1902, .month = 4, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1928, .month = 3, .day = 25},
		.d1 = {.year = 1928, .month = 4, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 4, .day = 22},
		.d1 = {.year = 1902, .month = 4, .day = 28},
	},

	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1928, .month = 4, .day = 21},
		.d1 = {.year = 1928, .month = 4, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 4, .day = 23},
		.d1 = {.year = 1902, .month = 5, .day = 1},
	},

	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1928, .month = 4, .day = 22},
		.d1 = {.year = 1928, .month = 5, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 5, .day = 20},
		.d1 = {.year = 1902, .month = 5, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1928, .month = 5, .day = 19},
		.d1 = {.year = 1928, .month = 5, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 5, .day = 21},
		.d1 = {.year = 1902, .month = 6, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1928, .month = 5, .day = 20},
		.d1 = {.year = 1928, .month = 6, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 6, .day = 17},
		.d1 = {.year = 1902, .month = 6, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1928, .month = 6, .day = 16},
		.d1 = {.year = 1928, .month = 6, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1928, .month = 6, .day = 17},
		.d1 = {.year = 1928, .month = 6, .day = 29},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 6, .day = 18},
		.d1 = {.year = 1902, .month = 7, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 7, .day = 15},
		.d1 = {.year = 1902, .month = 7, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 7, .day = 16},
		.d1 = {.year = 1902, .month = 8, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 8, .day = 12},
		.d1 = {.year = 1902, .month = 8, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 8, .day = 13},
		.d1 = {.year = 1902, .month = 9, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 9, .day = 9},
		.d1 = {.year = 1902, .month = 9, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 9, .day = 10},
		.d1 = {.year = 1902, .month = 10, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 10, .day = 7},
		.d1 = {.year = 1902, .month = 10, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 10, .day = 8},
		.d1 = {.year = 1902, .month = 11, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 11, .day = 4},
		.d1 = {.year = 1902, .month = 11, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 11, .day = 5},
		.d1 = {.year = 1902, .month = 12, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 12, .day = 2},
		.d1 = {.year = 1902, .month = 12, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 12, .day = 3},
		.d1 = {.year = 1902, .month = 13, .day = 1},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 12, .day = 30},
		.d1 = {.year = 1902, .month = 13, .day = 28},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_cotsworth,
		.d0 = {.year = 1902, .month = 12, .day = 31},
		.d1 = {.year = 1902, .month = 13, .day = 29},
	},
};


//Gregorian and Julian conversions
//All dates are from Wikipedia, Julian years before 0 are shifted by 1.
//https://en.wikipedia.org/wiki/Conversion_between_Julian_and_Gregorian_calendars
const struct known_convert_date gr2jl_wiki[] = {
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year = -500, .month = 2, .day = 28},
		.d1 = {.year = -501, .month = 3, .day = 5},
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  -500 , .month =  3 , .day =  1 },
		.d1 = {.year =  -501 , .month =  3 , .day =  6 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  -300 , .month =  2 , .day =  27 },
		.d1 = {.year =  -301 , .month =  3 , .day =  3 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  -300 , .month =  2 , .day =  28 },
		.d1 = {.year =  -301 , .month =  3 , .day =  4 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  -300 , .month =  3 , .day =  1 },
		.d1 = {.year =  -301 , .month =  3 , .day =  5 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  -200 , .month =  2 , .day =  27 },
		.d1 = {.year =  -201 , .month =  3 , .day =  2 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  -200 , .month =  2 , .day =  28 },
		.d1 = {.year =  -201 , .month =  3 , .day =  3 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  -200 , .month =  3 , .day =  1 },
		.d1 = {.year =  -201 , .month =  3 , .day =  4 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  -100 , .month =  2 , .day =  27 },
		.d1 = {.year =  -101 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  -100 , .month =  2 , .day =  28 },
		.d1 = {.year =  -101 , .month =  3 , .day =  2 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  -100 , .month =  3 , .day =  1 },
		.d1 = {.year =  -101 , .month =  3 , .day =  3 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  100 , .month =  2 , .day =  27 },
		.d1 = {.year =  100 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  100 , .month =  2 , .day =  28 },
		.d1 = {.year =  100 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  100 , .month =  3 , .day =  1 },
		.d1 = {.year =  100 , .month =  3 , .day =  2 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  200 , .month =  2 , .day =  27 },
		.d1 = {.year =  200 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  200 , .month =  2 , .day =  28 },
		.d1 = {.year =  200 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  200 , .month =  3 , .day =  1 },
		.d1 = {.year =  200 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  300 , .month =  2 , .day =  28 },
		.d1 = {.year =  300 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  300 , .month =  3 , .day =  1 },
		.d1 = {.year =  300 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  300 , .month =  3 , .day =  2 },
		.d1 = {.year =  300 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  500 , .month =  3 , .day =  1 },
		.d1 = {.year =  500 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  500 , .month =  3 , .day =  2 },
		.d1 = {.year =  500 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  500 , .month =  3 , .day =  3 },
		.d1 = {.year =  500 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  600 , .month =  3 , .day =  2 },
		.d1 = {.year =  600 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  600 , .month =  3 , .day =  3 },
		.d1 = {.year =  600 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  600 , .month =  3 , .day =  4 },
		.d1 = {.year =  600 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  700 , .month =  3 , .day =  3 },
		.d1 = {.year =  700 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  700 , .month =  3 , .day =  4 },
		.d1 = {.year =  700 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  700 , .month =  3 , .day =  5 },
		.d1 = {.year =  700 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  900 , .month =  3 , .day =  4 },
		.d1 = {.year =  900 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  900 , .month =  3 , .day =  5 },
		.d1 = {.year =  900 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  900 , .month =  3 , .day =  6 },
		.d1 = {.year =  900 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1000 , .month =  3 , .day =  5 },
		.d1 = {.year =  1000 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1000 , .month =  3 , .day =  6 },
		.d1 = {.year =  1000 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1000 , .month =  3 , .day =  7 },
		.d1 = {.year =  1000 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1100 , .month =  3 , .day =  6 },
		.d1 = {.year =  1100 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1100 , .month =  3 , .day =  7 },
		.d1 = {.year =  1100 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1100 , .month =  3 , .day =  8 },
		.d1 = {.year =  1100 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1300 , .month =  3 , .day =  7 },
		.d1 = {.year =  1300 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1300 , .month =  3 , .day =  8 },
		.d1 = {.year =  1300 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1300 , .month =  3 , .day =  9 },
		.d1 = {.year =  1300 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1400 , .month =  3 , .day =  8 },
		.d1 = {.year =  1400 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1400 , .month =  3 , .day =  9 },
		.d1 = {.year =  1400 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1400 , .month =  3 , .day =  10 },
		.d1 = {.year =  1400 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1500 , .month =  3 , .day =  9 },
		.d1 = {.year =  1500 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1500 , .month =  3 , .day =  10 },
		.d1 = {.year =  1500 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1500 , .month =  3 , .day =  11 },
		.d1 = {.year =  1500 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1582 , .month =  10 , .day =  14 },
		.d1 = {.year =  1582 , .month =  10 , .day =  4 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1582 , .month =  10 , .day =  15 },
		.d1 = {.year =  1582 , .month =  10 , .day =  5 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1582 , .month =  10 , .day =  16 },
		.d1 = {.year =  1582 , .month =  10 , .day =  6 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1700 , .month =  2 , .day =  28 },
		.d1 = {.year =  1700 , .month =  2 , .day =  18 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1700 , .month =  3 , .day =  1 },
		.d1 = {.year =  1700 , .month =  2 , .day =  19 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1700 , .month =  3 , .day =  10 },
		.d1 = {.year =  1700 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1700 , .month =  3 , .day =  11 },
		.d1 = {.year =  1700 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1700 , .month =  3 , .day =  12 },
		.d1 = {.year =  1700 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1800 , .month =  2 , .day =  28 },
		.d1 = {.year =  1800 , .month =  2 , .day =  17 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1800 , .month =  3 , .day =  1 },
		.d1 = {.year =  1800 , .month =  2 , .day =  18 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1800 , .month =  3 , .day =  11 },
		.d1 = {.year =  1800 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1800 , .month =  3 , .day =  12 },
		.d1 = {.year =  1800 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1800 , .month =  3 , .day =  13 },
		.d1 = {.year =  1800 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1900 , .month =  2 , .day =  28 },
		.d1 = {.year =  1900 , .month =  2 , .day =  16 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1900 , .month =  3 , .day =  1 },
		.d1 = {.year =  1900 , .month =  2 , .day =  17 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1900 , .month =  3 , .day =  12 },
		.d1 = {.year =  1900 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1900 , .month =  3 , .day =  13 },
		.d1 = {.year =  1900 , .month =  2 , .day =  29 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  1900 , .month =  3 , .day =  14 },
		.d1 = {.year =  1900 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  2100 , .month =  2 , .day =  28 },
		.d1 = {.year =  2100 , .month =  2 , .day =  15 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  2100 , .month =  3 , .day =  1 },
		.d1 = {.year =  2100 , .month =  2 , .day =  16 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  2100 , .month =  3 , .day =  13 },
		.d1 = {.year =  2100 , .month =  2 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_julian,
		.d0 = {.year =  2100 , .month =  3 , .day =  14 },
		.d1 = {.year =  2100 , .month =  2 , .day =  29 },
	}
};

//From these pages:
// + http://positivists.org/calendar.html
// + http://positivists.org/c/p-date.js
const struct known_convert_date gr2ps_positivists_org[] = {
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_positivist,
		.d0 = {.year =  1789 , .month =  1 , .day =  1 },
		.d1 = {.year =  1 , .month =  1 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_positivist,
		.d0 = {.year =  2021 , .month =  10 , .day =  25 },
		.d1 = {.year =  233 , .month =  11 , .day =  18 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_positivist,
		.d0 = {.year =  2020 , .month =  12 , .day =  31 },
		.d1 = {.year =  232 , .month =  0 , .day =  2 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_positivist,
		.d0 = {.year =  1969 , .month =  7 , .day =  20 },
		.d1 = {.year =  181 , .month =  8 , .day =  5 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_positivist,
		.d0 = {.year =  1798 , .month =  1 , .day =  19 },
		.d1 = {.year =  10 , .month =  1 , .day =  19 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_positivist,
		.d0 = {.year =  1857 , .month =  9 , .day =  5 },
		.d1 = {.year =  69 , .month =  9 , .day =  24 },
	},
};

//From https://core2.gsfc.nasa.gov/time/
const struct known_convert_mjd gr2mjd_nasa[] = {
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  1 , .day =  1 },
		.mjd =  51544 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  1 , .day =  1 },
		.mjd =  51910 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  1 , .day =  1 },
		.mjd =  52275 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  1 , .day =  1 },
		.mjd =  52640 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  1 , .day =  1 },
		.mjd =  53005 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  1 , .day =  1 },
		.mjd =  53371 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  1 , .day =  1 },
		.mjd =  53736 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  1 , .day =  1 },
		.mjd =  54101 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  1 , .day =  1 },
		.mjd =  54466 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  1 , .day =  1 },
		.mjd =  54832 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  2 , .day =  1 },
		.mjd =  51575 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  2 , .day =  1 },
		.mjd =  51941 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  2 , .day =  1 },
		.mjd =  52306 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  2 , .day =  1 },
		.mjd =  52671 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  2 , .day =  1 },
		.mjd =  53036 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  2 , .day =  1 },
		.mjd =  53402 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  2 , .day =  1 },
		.mjd =  53767 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  2 , .day =  1 },
		.mjd =  54132 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  2 , .day =  1 },
		.mjd =  54497 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  2 , .day =  1 },
		.mjd =  54863 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  3 , .day =  1 },
		.mjd =  51604 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  3 , .day =  1 },
		.mjd =  51969 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  3 , .day =  1 },
		.mjd =  52334 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  3 , .day =  1 },
		.mjd =  52699 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  3 , .day =  1 },
		.mjd =  53065 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  3 , .day =  1 },
		.mjd =  53430 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  3 , .day =  1 },
		.mjd =  53795 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  3 , .day =  1 },
		.mjd =  54160 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  3 , .day =  1 },
		.mjd =  54526 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  3 , .day =  1 },
		.mjd =  54891 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  4 , .day =  1 },
		.mjd =  51635 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  4 , .day =  1 },
		.mjd =  52000 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  4 , .day =  1 },
		.mjd =  52365 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  4 , .day =  1 },
		.mjd =  52730 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  4 , .day =  1 },
		.mjd =  53096 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  4 , .day =  1 },
		.mjd =  53461 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  4 , .day =  1 },
		.mjd =  53826 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  4 , .day =  1 },
		.mjd =  54191 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  4 , .day =  1 },
		.mjd =  54557 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  4 , .day =  1 },
		.mjd =  54922 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  5 , .day =  1 },
		.mjd =  51665 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  5 , .day =  1 },
		.mjd =  52030 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  5 , .day =  1 },
		.mjd =  52395 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  5 , .day =  1 },
		.mjd =  52760 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  5 , .day =  1 },
		.mjd =  53126 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  5 , .day =  1 },
		.mjd =  53491 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  5 , .day =  1 },
		.mjd =  53856 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  5 , .day =  1 },
		.mjd =  54221 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  5 , .day =  1 },
		.mjd =  54587 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  5 , .day =  1 },
		.mjd =  54952 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  6 , .day =  1 },
		.mjd =  51696 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  6 , .day =  1 },
		.mjd =  52061 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  6 , .day =  1 },
		.mjd =  52426 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  6 , .day =  1 },
		.mjd =  52791 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  6 , .day =  1 },
		.mjd =  53157 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  6 , .day =  1 },
		.mjd =  53522 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  6 , .day =  1 },
		.mjd =  53887 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  6 , .day =  1 },
		.mjd =  54252 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  6 , .day =  1 },
		.mjd =  54618 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  6 , .day =  1 },
		.mjd =  54983 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  7 , .day =  1 },
		.mjd =  51726 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  7 , .day =  1 },
		.mjd =  52091 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  7 , .day =  1 },
		.mjd =  52456 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  7 , .day =  1 },
		.mjd =  52821 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  7 , .day =  1 },
		.mjd =  53187 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  7 , .day =  1 },
		.mjd =  53552 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  7 , .day =  1 },
		.mjd =  53917 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  7 , .day =  1 },
		.mjd =  54282 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  7 , .day =  1 },
		.mjd =  54648 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  7 , .day =  1 },
		.mjd =  55013 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  8 , .day =  1 },
		.mjd =  51757 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  8 , .day =  1 },
		.mjd =  52122 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  8 , .day =  1 },
		.mjd =  52487 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  8 , .day =  1 },
		.mjd =  52852 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  8 , .day =  1 },
		.mjd =  53218 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  8 , .day =  1 },
		.mjd =  53583 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  8 , .day =  1 },
		.mjd =  53948 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  8 , .day =  1 },
		.mjd =  54313 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  8 , .day =  1 },
		.mjd =  54679 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  8 , .day =  1 },
		.mjd =  55044 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  9 , .day =  1 },
		.mjd =  51788 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  9 , .day =  1 },
		.mjd =  52153 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  9 , .day =  1 },
		.mjd =  52518 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  9 , .day =  1 },
		.mjd =  52883 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  9 , .day =  1 },
		.mjd =  53249 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  9 , .day =  1 },
		.mjd =  53614 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  9 , .day =  1 },
		.mjd =  53979 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  9 , .day =  1 },
		.mjd =  54344 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  9 , .day =  1 },
		.mjd =  54710 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  9 , .day =  1 },
		.mjd =  55075 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  10 , .day =  1 },
		.mjd =  51818 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  10 , .day =  1 },
		.mjd =  52183 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  10 , .day =  1 },
		.mjd =  52548 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  10 , .day =  1 },
		.mjd =  52913 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  10 , .day =  1 },
		.mjd =  53279 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  10 , .day =  1 },
		.mjd =  53644 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  10 , .day =  1 },
		.mjd =  54009 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  10 , .day =  1 },
		.mjd =  54374 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  10 , .day =  1 },
		.mjd =  54740 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  10 , .day =  1 },
		.mjd =  55105 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  11 , .day =  1 },
		.mjd =  51849 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  11 , .day =  1 },
		.mjd =  52214 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  11 , .day =  1 },
		.mjd =  52579 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  11 , .day =  1 },
		.mjd =  52944 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  11 , .day =  1 },
		.mjd =  53310 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  11 , .day =  1 },
		.mjd =  53675 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  11 , .day =  1 },
		.mjd =  54040 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  11 , .day =  1 },
		.mjd =  54405 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  11 , .day =  1 },
		.mjd =  54771 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  11 , .day =  1 },
		.mjd =  55136 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2000 , .month =  12 , .day =  1 },
		.mjd =  51879 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2001 , .month =  12 , .day =  1 },
		.mjd =  52244 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2002 , .month =  12 , .day =  1 },
		.mjd =  52609 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2003 , .month =  12 , .day =  1 },
		.mjd =  52974 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2004 , .month =  12 , .day =  1 },
		.mjd =  53340 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2005 , .month =  12 , .day =  1 },
		.mjd =  53705 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2006 , .month =  12 , .day =  1 },
		.mjd =  54070 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2007 , .month =  12 , .day =  1 },
		.mjd =  54435 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2008 , .month =  12 , .day =  1 },
		.mjd =  54801 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  2009 , .month =  12 , .day =  1 },
		.mjd =  55166 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  1 , .day =  1 },
		.mjd =  47892 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  1 , .day =  1 },
		.mjd =  48257 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  1 , .day =  1 },
		.mjd =  48622 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  1 , .day =  1 },
		.mjd =  48988 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  1 , .day =  1 },
		.mjd =  49353 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  1 , .day =  1 },
		.mjd =  49718 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  1 , .day =  1 },
		.mjd =  50083 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  1 , .day =  1 },
		.mjd =  50449 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  1 , .day =  1 },
		.mjd =  50814 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  1 , .day =  1 },
		.mjd =  51179 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  2 , .day =  1 },
		.mjd =  47923 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  2 , .day =  1 },
		.mjd =  48288 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  2 , .day =  1 },
		.mjd =  48653 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  2 , .day =  1 },
		.mjd =  49019 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  2 , .day =  1 },
		.mjd =  49384 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  2 , .day =  1 },
		.mjd =  49749 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  2 , .day =  1 },
		.mjd =  50114 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  2 , .day =  1 },
		.mjd =  50480 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  2 , .day =  1 },
		.mjd =  50845 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  2 , .day =  1 },
		.mjd =  51210 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  3 , .day =  1 },
		.mjd =  47951 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  3 , .day =  1 },
		.mjd =  48316 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  3 , .day =  1 },
		.mjd =  48682 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  3 , .day =  1 },
		.mjd =  49047 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  3 , .day =  1 },
		.mjd =  49412 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  3 , .day =  1 },
		.mjd =  49777 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  3 , .day =  1 },
		.mjd =  50143 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  3 , .day =  1 },
		.mjd =  50508 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  3 , .day =  1 },
		.mjd =  50873 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  3 , .day =  1 },
		.mjd =  51238 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  4 , .day =  1 },
		.mjd =  47982 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  4 , .day =  1 },
		.mjd =  48347 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  4 , .day =  1 },
		.mjd =  48713 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  4 , .day =  1 },
		.mjd =  49078 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  4 , .day =  1 },
		.mjd =  49443 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  4 , .day =  1 },
		.mjd =  49808 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  4 , .day =  1 },
		.mjd =  50174 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  4 , .day =  1 },
		.mjd =  50539 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  4 , .day =  1 },
		.mjd =  50904 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  4 , .day =  1 },
		.mjd =  51269 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  5 , .day =  1 },
		.mjd =  48012 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  5 , .day =  1 },
		.mjd =  48377 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  5 , .day =  1 },
		.mjd =  48743 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  5 , .day =  1 },
		.mjd =  49108 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  5 , .day =  1 },
		.mjd =  49473 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  5 , .day =  1 },
		.mjd =  49838 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  5 , .day =  1 },
		.mjd =  50204 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  5 , .day =  1 },
		.mjd =  50569 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  5 , .day =  1 },
		.mjd =  50934 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  5 , .day =  1 },
		.mjd =  51299 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  6 , .day =  1 },
		.mjd =  48043 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  6 , .day =  1 },
		.mjd =  48408 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  6 , .day =  1 },
		.mjd =  48774 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  6 , .day =  1 },
		.mjd =  49139 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  6 , .day =  1 },
		.mjd =  49504 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  6 , .day =  1 },
		.mjd =  49869 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  6 , .day =  1 },
		.mjd =  50235 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  6 , .day =  1 },
		.mjd =  50600 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  6 , .day =  1 },
		.mjd =  50965 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  6 , .day =  1 },
		.mjd =  51330 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  7 , .day =  1 },
		.mjd =  48073 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  7 , .day =  1 },
		.mjd =  48438 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  7 , .day =  1 },
		.mjd =  48804 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  7 , .day =  1 },
		.mjd =  49169 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  7 , .day =  1 },
		.mjd =  49534 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  7 , .day =  1 },
		.mjd =  49899 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  7 , .day =  1 },
		.mjd =  50265 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  7 , .day =  1 },
		.mjd =  50630 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  7 , .day =  1 },
		.mjd =  50995 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  7 , .day =  1 },
		.mjd =  51360 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  8 , .day =  1 },
		.mjd =  48104 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  8 , .day =  1 },
		.mjd =  48469 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  8 , .day =  1 },
		.mjd =  48835 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  8 , .day =  1 },
		.mjd =  49200 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  8 , .day =  1 },
		.mjd =  49565 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  8 , .day =  1 },
		.mjd =  49930 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  8 , .day =  1 },
		.mjd =  50296 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  8 , .day =  1 },
		.mjd =  50661 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  8 , .day =  1 },
		.mjd =  51026 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  8 , .day =  1 },
		.mjd =  51391 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  9 , .day =  1 },
		.mjd =  48135 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  9 , .day =  1 },
		.mjd =  48500 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  9 , .day =  1 },
		.mjd =  48866 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  9 , .day =  1 },
		.mjd =  49231 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  9 , .day =  1 },
		.mjd =  49596 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  9 , .day =  1 },
		.mjd =  49961 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  9 , .day =  1 },
		.mjd =  50327 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  9 , .day =  1 },
		.mjd =  50692 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  9 , .day =  1 },
		.mjd =  51057 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  9 , .day =  1 },
		.mjd =  51422 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  10 , .day =  1 },
		.mjd =  48165 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  10 , .day =  1 },
		.mjd =  48530 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  10 , .day =  1 },
		.mjd =  48896 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  10 , .day =  1 },
		.mjd =  49261 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  10 , .day =  1 },
		.mjd =  49626 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  10 , .day =  1 },
		.mjd =  49991 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  10 , .day =  1 },
		.mjd =  50357 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  10 , .day =  1 },
		.mjd =  50722 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  10 , .day =  1 },
		.mjd =  51087 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  10 , .day =  1 },
		.mjd =  51452 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  11 , .day =  1 },
		.mjd =  48196 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  11 , .day =  1 },
		.mjd =  48561 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  11 , .day =  1 },
		.mjd =  48927 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  11 , .day =  1 },
		.mjd =  49292 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  11 , .day =  1 },
		.mjd =  49657 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  11 , .day =  1 },
		.mjd =  50022 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  11 , .day =  1 },
		.mjd =  50388 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  11 , .day =  1 },
		.mjd =  50753 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  11 , .day =  1 },
		.mjd =  51118 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  11 , .day =  1 },
		.mjd =  51483 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1990 , .month =  12 , .day =  1 },
		.mjd =  48226 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1991 , .month =  12 , .day =  1 },
		.mjd =  48591 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1992 , .month =  12 , .day =  1 },
		.mjd =  48957 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1993 , .month =  12 , .day =  1 },
		.mjd =  49322 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1994 , .month =  12 , .day =  1 },
		.mjd =  49687 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1995 , .month =  12 , .day =  1 },
		.mjd =  50052 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1996 , .month =  12 , .day =  1 },
		.mjd =  50418 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1997 , .month =  12 , .day =  1 },
		.mjd =  50783 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1998 , .month =  12 , .day =  1 },
		.mjd =  51148 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1999 , .month =  12 , .day =  1 },
		.mjd =  51513 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  1 , .day =  1 },
		.mjd =  44239 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  1 , .day =  1 },
		.mjd =  44605 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  1 , .day =  1 },
		.mjd =  44970 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  1 , .day =  1 },
		.mjd =  45335 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  1 , .day =  1 },
		.mjd =  45700 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  1 , .day =  1 },
		.mjd =  46066 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  1 , .day =  1 },
		.mjd =  46431 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  1 , .day =  1 },
		.mjd =  46796 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  1 , .day =  1 },
		.mjd =  47161 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  1 , .day =  1 },
		.mjd =  47527 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  2 , .day =  1 },
		.mjd =  44270 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  2 , .day =  1 },
		.mjd =  44636 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  2 , .day =  1 },
		.mjd =  45001 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  2 , .day =  1 },
		.mjd =  45366 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  2 , .day =  1 },
		.mjd =  45731 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  2 , .day =  1 },
		.mjd =  46097 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  2 , .day =  1 },
		.mjd =  46462 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  2 , .day =  1 },
		.mjd =  46827 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  2 , .day =  1 },
		.mjd =  47192 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  2 , .day =  1 },
		.mjd =  47558 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  3 , .day =  1 },
		.mjd =  44299 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  3 , .day =  1 },
		.mjd =  44664 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  3 , .day =  1 },
		.mjd =  45029 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  3 , .day =  1 },
		.mjd =  45394 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  3 , .day =  1 },
		.mjd =  45760 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  3 , .day =  1 },
		.mjd =  46125 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  3 , .day =  1 },
		.mjd =  46490 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  3 , .day =  1 },
		.mjd =  46855 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  3 , .day =  1 },
		.mjd =  47221 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  3 , .day =  1 },
		.mjd =  47586 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  4 , .day =  1 },
		.mjd =  44330 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  4 , .day =  1 },
		.mjd =  44695 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  4 , .day =  1 },
		.mjd =  45060 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  4 , .day =  1 },
		.mjd =  45425 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  4 , .day =  1 },
		.mjd =  45791 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  4 , .day =  1 },
		.mjd =  46156 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  4 , .day =  1 },
		.mjd =  46521 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  4 , .day =  1 },
		.mjd =  46886 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  4 , .day =  1 },
		.mjd =  47252 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  4 , .day =  1 },
		.mjd =  47617 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  5 , .day =  1 },
		.mjd =  44360 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  5 , .day =  1 },
		.mjd =  44725 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  5 , .day =  1 },
		.mjd =  45090 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  5 , .day =  1 },
		.mjd =  45455 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  5 , .day =  1 },
		.mjd =  45821 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  5 , .day =  1 },
		.mjd =  46186 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  5 , .day =  1 },
		.mjd =  46551 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  5 , .day =  1 },
		.mjd =  46916 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  5 , .day =  1 },
		.mjd =  47282 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  5 , .day =  1 },
		.mjd =  47647 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  6 , .day =  1 },
		.mjd =  44391 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  6 , .day =  1 },
		.mjd =  44756 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  6 , .day =  1 },
		.mjd =  45121 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  6 , .day =  1 },
		.mjd =  45486 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  6 , .day =  1 },
		.mjd =  45852 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  6 , .day =  1 },
		.mjd =  46217 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  6 , .day =  1 },
		.mjd =  46582 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  6 , .day =  1 },
		.mjd =  46947 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  6 , .day =  1 },
		.mjd =  47313 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  6 , .day =  1 },
		.mjd =  47678 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  7 , .day =  1 },
		.mjd =  44421 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  7 , .day =  1 },
		.mjd =  44786 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  7 , .day =  1 },
		.mjd =  45151 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  7 , .day =  1 },
		.mjd =  45516 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  7 , .day =  1 },
		.mjd =  45882 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  7 , .day =  1 },
		.mjd =  46247 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  7 , .day =  1 },
		.mjd =  46612 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  7 , .day =  1 },
		.mjd =  46977 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  7 , .day =  1 },
		.mjd =  47343 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  7 , .day =  1 },
		.mjd =  47708 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  8 , .day =  1 },
		.mjd =  44452 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  8 , .day =  1 },
		.mjd =  44817 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  8 , .day =  1 },
		.mjd =  45182 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  8 , .day =  1 },
		.mjd =  45547 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  8 , .day =  1 },
		.mjd =  45913 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  8 , .day =  1 },
		.mjd =  46278 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  8 , .day =  1 },
		.mjd =  46643 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  8 , .day =  1 },
		.mjd =  47008 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  8 , .day =  1 },
		.mjd =  47374 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  8 , .day =  1 },
		.mjd =  47739 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  9 , .day =  1 },
		.mjd =  44483 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  9 , .day =  1 },
		.mjd =  44848 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  9 , .day =  1 },
		.mjd =  45213 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  9 , .day =  1 },
		.mjd =  45578 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  9 , .day =  1 },
		.mjd =  45944 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  9 , .day =  1 },
		.mjd =  46309 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  9 , .day =  1 },
		.mjd =  46674 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  9 , .day =  1 },
		.mjd =  47039 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  9 , .day =  1 },
		.mjd =  47405 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  9 , .day =  1 },
		.mjd =  47770 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  10 , .day =  1 },
		.mjd =  44513 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  10 , .day =  1 },
		.mjd =  44878 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  10 , .day =  1 },
		.mjd =  45243 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  10 , .day =  1 },
		.mjd =  45608 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  10 , .day =  1 },
		.mjd =  45974 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  10 , .day =  1 },
		.mjd =  46339 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  10 , .day =  1 },
		.mjd =  46704 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  10 , .day =  1 },
		.mjd =  47069 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  10 , .day =  1 },
		.mjd =  47435 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  10 , .day =  1 },
		.mjd =  47800 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  11 , .day =  1 },
		.mjd =  44544 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  11 , .day =  1 },
		.mjd =  44909 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  11 , .day =  1 },
		.mjd =  45274 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  11 , .day =  1 },
		.mjd =  45639 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  11 , .day =  1 },
		.mjd =  46005 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  11 , .day =  1 },
		.mjd =  46370 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  11 , .day =  1 },
		.mjd =  46735 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  11 , .day =  1 },
		.mjd =  47100 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  11 , .day =  1 },
		.mjd =  47466 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  11 , .day =  1 },
		.mjd =  47831 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1980 , .month =  12 , .day =  1 },
		.mjd =  44574 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1981 , .month =  12 , .day =  1 },
		.mjd =  44939 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1982 , .month =  12 , .day =  1 },
		.mjd =  45304 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1983 , .month =  12 , .day =  1 },
		.mjd =  45669 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1984 , .month =  12 , .day =  1 },
		.mjd =  46035 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1985 , .month =  12 , .day =  1 },
		.mjd =  46400 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1986 , .month =  12 , .day =  1 },
		.mjd =  46765 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1987 , .month =  12 , .day =  1 },
		.mjd =  47130 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1988 , .month =  12 , .day =  1 },
		.mjd =  47496 ,
	},
	{
		.c = &mon13_gregorian_year_zero,
		.d = {.year =  1989 , .month =  12 , .day =  1 },
		.mjd =  47861 ,
	},
};

//Gregorian and Symmetry454 conversions
//All dates are from Dr. Irv Bromberg's Kalendis site
//http://individual.utoronto.ca/kalendis/convert/index.html
const struct known_convert_date gr2sym454_irv[] = {
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  1 , .day =  1 },
		.d1 = {.year =  2018 , .month =  1 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  1 , .day =  28 },
		.d1 = {.year =  2018 , .month =  1 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  1 , .day =  29 },
		.d1 = {.year =  2018 , .month =  2 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  3 , .day =  4 },
		.d1 = {.year =  2018 , .month =  2 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  3 , .day =  5 },
		.d1 = {.year =  2018 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  4 , .day =  1 },
		.d1 = {.year =  2018 , .month =  3 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  4 , .day =  2 },
		.d1 = {.year =  2018 , .month =  4 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  4 , .day =  29 },
		.d1 = {.year =  2018 , .month =  4 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  4 , .day =  30 },
		.d1 = {.year =  2018 , .month =  5 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  6 , .day =  3 },
		.d1 = {.year =  2018 , .month =  5 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  6 , .day =  4 },
		.d1 = {.year =  2018 , .month =  6 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  7 , .day =  1 },
		.d1 = {.year =  2018 , .month =  6 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  7 , .day =  2 },
		.d1 = {.year =  2018 , .month =  7 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  7 , .day =  29 },
		.d1 = {.year =  2018 , .month =  7 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  7 , .day =  30 },
		.d1 = {.year =  2018 , .month =  8 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  9 , .day =  2 },
		.d1 = {.year =  2018 , .month =  8 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  9 , .day =  3 },
		.d1 = {.year =  2018 , .month =  9 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  9 , .day =  30 },
		.d1 = {.year =  2018 , .month =  9 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  10 , .day =  1 },
		.d1 = {.year =  2018 , .month =  10 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  10 , .day =  28 },
		.d1 = {.year =  2018 , .month =  10 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  10 , .day =  29 },
		.d1 = {.year =  2018 , .month =  11 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  12 , .day =  2 },
		.d1 = {.year =  2018 , .month =  11 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  12 , .day =  3 },
		.d1 = {.year =  2018 , .month =  12 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  12 , .day =  30 },
		.d1 = {.year =  2018 , .month =  12 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2018 , .month =  12 , .day =  31 },
		.d1 = {.year =  2019 , .month =  1 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  1 , .day =  27 },
		.d1 = {.year =  2019 , .month =  1 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  1 , .day =  28 },
		.d1 = {.year =  2019 , .month =  2 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  3 , .day =  3 },
		.d1 = {.year =  2019 , .month =  2 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  3 , .day =  4 },
		.d1 = {.year =  2019 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  3 , .day =  31 },
		.d1 = {.year =  2019 , .month =  3 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  4 , .day =  1 },
		.d1 = {.year =  2019 , .month =  4 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  4 , .day =  28 },
		.d1 = {.year =  2019 , .month =  4 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  4 , .day =  29 },
		.d1 = {.year =  2019 , .month =  5 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  6 , .day =  2 },
		.d1 = {.year =  2019 , .month =  5 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  6 , .day =  3 },
		.d1 = {.year =  2019 , .month =  6 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  6 , .day =  30 },
		.d1 = {.year =  2019 , .month =  6 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  7 , .day =  1 },
		.d1 = {.year =  2019 , .month =  7 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  7 , .day =  28 },
		.d1 = {.year =  2019 , .month =  7 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  7 , .day =  29 },
		.d1 = {.year =  2019 , .month =  8 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  9 , .day =  1 },
		.d1 = {.year =  2019 , .month =  8 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  9 , .day =  2 },
		.d1 = {.year =  2019 , .month =  9 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  9 , .day =  29 },
		.d1 = {.year =  2019 , .month =  9 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  9 , .day =  30 },
		.d1 = {.year =  2019 , .month =  10 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  10 , .day =  27 },
		.d1 = {.year =  2019 , .month =  10 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  10 , .day =  28 },
		.d1 = {.year =  2019 , .month =  11 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  12 , .day =  1 },
		.d1 = {.year =  2019 , .month =  11 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  12 , .day =  2 },
		.d1 = {.year =  2019 , .month =  12 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  12 , .day =  29 },
		.d1 = {.year =  2019 , .month =  12 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2019 , .month =  12 , .day =  30 },
		.d1 = {.year =  2020 , .month =  1 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  1 , .day =  26 },
		.d1 = {.year =  2020 , .month =  1 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  1 , .day =  27 },
		.d1 = {.year =  2020 , .month =  2 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  3 , .day =  1 },
		.d1 = {.year =  2020 , .month =  2 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  3 , .day =  2 },
		.d1 = {.year =  2020 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  3 , .day =  29 },
		.d1 = {.year =  2020 , .month =  3 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  3 , .day =  30 },
		.d1 = {.year =  2020 , .month =  4 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  4 , .day =  26 },
		.d1 = {.year =  2020 , .month =  4 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  4 , .day =  27 },
		.d1 = {.year =  2020 , .month =  5 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  5 , .day =  31 },
		.d1 = {.year =  2020 , .month =  5 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  6 , .day =  1 },
		.d1 = {.year =  2020 , .month =  6 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  6 , .day =  28 },
		.d1 = {.year =  2020 , .month =  6 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  6 , .day =  29 },
		.d1 = {.year =  2020 , .month =  7 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  7 , .day =  26 },
		.d1 = {.year =  2020 , .month =  7 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  7 , .day =  27 },
		.d1 = {.year =  2020 , .month =  8 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  8 , .day =  30 },
		.d1 = {.year =  2020 , .month =  8 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  8 , .day =  31 },
		.d1 = {.year =  2020 , .month =  9 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  9 , .day =  27 },
		.d1 = {.year =  2020 , .month =  9 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  9 , .day =  28 },
		.d1 = {.year =  2020 , .month =  10 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  10 , .day =  25 },
		.d1 = {.year =  2020 , .month =  10 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  10 , .day =  26 },
		.d1 = {.year =  2020 , .month =  11 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  11 , .day =  29 },
		.d1 = {.year =  2020 , .month =  11 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  11 , .day =  30 },
		.d1 = {.year =  2020 , .month =  12 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  12 , .day =  27 },
		.d1 = {.year =  2020 , .month =  12 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2020 , .month =  12 , .day =  28 },
		.d1 = {.year =  2021 , .month =  1 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  1 , .day =  24 },
		.d1 = {.year =  2021 , .month =  1 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  1 , .day =  25 },
		.d1 = {.year =  2021 , .month =  2 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  2 , .day =  28 },
		.d1 = {.year =  2021 , .month =  2 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  3 , .day =  1 },
		.d1 = {.year =  2021 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  3 , .day =  28 },
		.d1 = {.year =  2021 , .month =  3 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  3 , .day =  29 },
		.d1 = {.year =  2021 , .month =  4 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  4 , .day =  25 },
		.d1 = {.year =  2021 , .month =  4 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  4 , .day =  26 },
		.d1 = {.year =  2021 , .month =  5 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  5 , .day =  30 },
		.d1 = {.year =  2021 , .month =  5 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  5 , .day =  31 },
		.d1 = {.year =  2021 , .month =  6 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  6 , .day =  27 },
		.d1 = {.year =  2021 , .month =  6 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  6 , .day =  28 },
		.d1 = {.year =  2021 , .month =  7 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  7 , .day =  25 },
		.d1 = {.year =  2021 , .month =  7 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  7 , .day =  26 },
		.d1 = {.year =  2021 , .month =  8 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  8 , .day =  29 },
		.d1 = {.year =  2021 , .month =  8 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  8 , .day =  30 },
		.d1 = {.year =  2021 , .month =  9 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  9 , .day =  26 },
		.d1 = {.year =  2021 , .month =  9 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  9 , .day =  27 },
		.d1 = {.year =  2021 , .month =  10 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  10 , .day =  24 },
		.d1 = {.year =  2021 , .month =  10 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  10 , .day =  25 },
		.d1 = {.year =  2021 , .month =  11 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  11 , .day =  28 },
		.d1 = {.year =  2021 , .month =  11 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2021 , .month =  11 , .day =  29 },
		.d1 = {.year =  2021 , .month =  12 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  1 , .day =  2 },
		.d1 = {.year =  2021 , .month =  12 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  1 , .day =  3 },
		.d1 = {.year =  2022 , .month =  1 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  1 , .day =  30 },
		.d1 = {.year =  2022 , .month =  1 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  1 , .day =  31 },
		.d1 = {.year =  2022 , .month =  2 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  3 , .day =  6 },
		.d1 = {.year =  2022 , .month =  2 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  3 , .day =  7 },
		.d1 = {.year =  2022 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  4 , .day =  3 },
		.d1 = {.year =  2022 , .month =  3 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  4 , .day =  4 },
		.d1 = {.year =  2022 , .month =  4 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  5 , .day =  1 },
		.d1 = {.year =  2022 , .month =  4 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  5 , .day =  2 },
		.d1 = {.year =  2022 , .month =  5 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  6 , .day =  5 },
		.d1 = {.year =  2022 , .month =  5 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  6 , .day =  6 },
		.d1 = {.year =  2022 , .month =  6 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  7 , .day =  3 },
		.d1 = {.year =  2022 , .month =  6 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  7 , .day =  4 },
		.d1 = {.year =  2022 , .month =  7 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  7 , .day =  31 },
		.d1 = {.year =  2022 , .month =  7 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  8 , .day =  1 },
		.d1 = {.year =  2022 , .month =  8 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  9 , .day =  4 },
		.d1 = {.year =  2022 , .month =  8 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  9 , .day =  5 },
		.d1 = {.year =  2022 , .month =  9 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  10 , .day =  2 },
		.d1 = {.year =  2022 , .month =  9 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  10 , .day =  3 },
		.d1 = {.year =  2022 , .month =  10 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  10 , .day =  30 },
		.d1 = {.year =  2022 , .month =  10 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  10 , .day =  31 },
		.d1 = {.year =  2022 , .month =  11 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  12 , .day =  4 },
		.d1 = {.year =  2022 , .month =  11 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2022 , .month =  12 , .day =  5 },
		.d1 = {.year =  2022 , .month =  12 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  1 , .day =  1 },
		.d1 = {.year =  2022 , .month =  12 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  1 , .day =  2 },
		.d1 = {.year =  2023 , .month =  1 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  1 , .day =  29 },
		.d1 = {.year =  2023 , .month =  1 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  1 , .day =  30 },
		.d1 = {.year =  2023 , .month =  2 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  3 , .day =  5 },
		.d1 = {.year =  2023 , .month =  2 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  3 , .day =  6 },
		.d1 = {.year =  2023 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  4 , .day =  2 },
		.d1 = {.year =  2023 , .month =  3 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  4 , .day =  3 },
		.d1 = {.year =  2023 , .month =  4 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  4 , .day =  30 },
		.d1 = {.year =  2023 , .month =  4 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  5 , .day =  1 },
		.d1 = {.year =  2023 , .month =  5 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  6 , .day =  4 },
		.d1 = {.year =  2023 , .month =  5 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  6 , .day =  5 },
		.d1 = {.year =  2023 , .month =  6 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  7 , .day =  2 },
		.d1 = {.year =  2023 , .month =  6 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  7 , .day =  3 },
		.d1 = {.year =  2023 , .month =  7 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  7 , .day =  30 },
		.d1 = {.year =  2023 , .month =  7 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  7 , .day =  31 },
		.d1 = {.year =  2023 , .month =  8 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  9 , .day =  3 },
		.d1 = {.year =  2023 , .month =  8 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  9 , .day =  4 },
		.d1 = {.year =  2023 , .month =  9 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  10 , .day =  1 },
		.d1 = {.year =  2023 , .month =  9 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  10 , .day =  2 },
		.d1 = {.year =  2023 , .month =  10 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  10 , .day =  29 },
		.d1 = {.year =  2023 , .month =  10 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  10 , .day =  30 },
		.d1 = {.year =  2023 , .month =  11 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  12 , .day =  3 },
		.d1 = {.year =  2023 , .month =  11 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  12 , .day =  4 },
		.d1 = {.year =  2023 , .month =  12 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2023 , .month =  12 , .day =  31 },
		.d1 = {.year =  2023 , .month =  12 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  1 , .day =  1 },
		.d1 = {.year =  2024 , .month =  1 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  1 , .day =  28 },
		.d1 = {.year =  2024 , .month =  1 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  1 , .day =  29 },
		.d1 = {.year =  2024 , .month =  2 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  3 , .day =  3 },
		.d1 = {.year =  2024 , .month =  2 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  3 , .day =  4 },
		.d1 = {.year =  2024 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  3 , .day =  31 },
		.d1 = {.year =  2024 , .month =  3 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  4 , .day =  1 },
		.d1 = {.year =  2024 , .month =  4 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  4 , .day =  28 },
		.d1 = {.year =  2024 , .month =  4 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  4 , .day =  29 },
		.d1 = {.year =  2024 , .month =  5 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  6 , .day =  2 },
		.d1 = {.year =  2024 , .month =  5 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  6 , .day =  3 },
		.d1 = {.year =  2024 , .month =  6 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  6 , .day =  30 },
		.d1 = {.year =  2024 , .month =  6 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  7 , .day =  1 },
		.d1 = {.year =  2024 , .month =  7 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  7 , .day =  28 },
		.d1 = {.year =  2024 , .month =  7 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  7 , .day =  29 },
		.d1 = {.year =  2024 , .month =  8 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  9 , .day =  1 },
		.d1 = {.year =  2024 , .month =  8 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  9 , .day =  2 },
		.d1 = {.year =  2024 , .month =  9 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  9 , .day =  29 },
		.d1 = {.year =  2024 , .month =  9 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  9 , .day =  30 },
		.d1 = {.year =  2024 , .month =  10 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  10 , .day =  27 },
		.d1 = {.year =  2024 , .month =  10 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  10 , .day =  28 },
		.d1 = {.year =  2024 , .month =  11 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  12 , .day =  1 },
		.d1 = {.year =  2024 , .month =  11 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  12 , .day =  2 },
		.d1 = {.year =  2024 , .month =  12 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  12 , .day =  29 },
		.d1 = {.year =  2024 , .month =  12 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2024 , .month =  12 , .day =  30 },
		.d1 = {.year =  2025 , .month =  1 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  1 , .day =  26 },
		.d1 = {.year =  2025 , .month =  1 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  1 , .day =  27 },
		.d1 = {.year =  2025 , .month =  2 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  3 , .day =  2 },
		.d1 = {.year =  2025 , .month =  2 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  3 , .day =  3 },
		.d1 = {.year =  2025 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  3 , .day =  30 },
		.d1 = {.year =  2025 , .month =  3 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  3 , .day =  31 },
		.d1 = {.year =  2025 , .month =  4 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  4 , .day =  27 },
		.d1 = {.year =  2025 , .month =  4 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  4 , .day =  28 },
		.d1 = {.year =  2025 , .month =  5 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  6 , .day =  1 },
		.d1 = {.year =  2025 , .month =  5 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  6 , .day =  2 },
		.d1 = {.year =  2025 , .month =  6 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  6 , .day =  29 },
		.d1 = {.year =  2025 , .month =  6 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  6 , .day =  30 },
		.d1 = {.year =  2025 , .month =  7 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  7 , .day =  27 },
		.d1 = {.year =  2025 , .month =  7 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  7 , .day =  28 },
		.d1 = {.year =  2025 , .month =  8 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  8 , .day =  31 },
		.d1 = {.year =  2025 , .month =  8 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  9 , .day =  1 },
		.d1 = {.year =  2025 , .month =  9 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  9 , .day =  28 },
		.d1 = {.year =  2025 , .month =  9 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  9 , .day =  29 },
		.d1 = {.year =  2025 , .month =  10 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  10 , .day =  26 },
		.d1 = {.year =  2025 , .month =  10 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  10 , .day =  27 },
		.d1 = {.year =  2025 , .month =  11 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  11 , .day =  30 },
		.d1 = {.year =  2025 , .month =  11 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  12 , .day =  1 },
		.d1 = {.year =  2025 , .month =  12 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  12 , .day =  28 },
		.d1 = {.year =  2025 , .month =  12 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2025 , .month =  12 , .day =  29 },
		.d1 = {.year =  2026 , .month =  1 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  1 , .day =  25 },
		.d1 = {.year =  2026 , .month =  1 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  1 , .day =  26 },
		.d1 = {.year =  2026 , .month =  2 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  3 , .day =  1 },
		.d1 = {.year =  2026 , .month =  2 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  3 , .day =  2 },
		.d1 = {.year =  2026 , .month =  3 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  3 , .day =  29 },
		.d1 = {.year =  2026 , .month =  3 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  3 , .day =  30 },
		.d1 = {.year =  2026 , .month =  4 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  4 , .day =  26 },
		.d1 = {.year =  2026 , .month =  4 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  4 , .day =  27 },
		.d1 = {.year =  2026 , .month =  5 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  5 , .day =  31 },
		.d1 = {.year =  2026 , .month =  5 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  6 , .day =  1 },
		.d1 = {.year =  2026 , .month =  6 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  6 , .day =  28 },
		.d1 = {.year =  2026 , .month =  6 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  6 , .day =  29 },
		.d1 = {.year =  2026 , .month =  7 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  7 , .day =  26 },
		.d1 = {.year =  2026 , .month =  7 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  7 , .day =  27 },
		.d1 = {.year =  2026 , .month =  8 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  8 , .day =  30 },
		.d1 = {.year =  2026 , .month =  8 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  8 , .day =  31 },
		.d1 = {.year =  2026 , .month =  9 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  9 , .day =  27 },
		.d1 = {.year =  2026 , .month =  9 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  9 , .day =  28 },
		.d1 = {.year =  2026 , .month =  10 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  10 , .day =  25 },
		.d1 = {.year =  2026 , .month =  10 , .day =  28 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  10 , .day =  26 },
		.d1 = {.year =  2026 , .month =  11 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  11 , .day =  29 },
		.d1 = {.year =  2026 , .month =  11 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2026 , .month =  11 , .day =  30 },
		.d1 = {.year =  2026 , .month =  12 , .day =  1 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2027 , .month =  1 , .day =  3 },
		.d1 = {.year =  2026 , .month =  12 , .day =  35 },
	},
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_symmetry454,
		.d0 = {.year =  2027 , .month =  1 , .day =  4 },
		.d1 = {.year =  2027 , .month =  1 , .day =  1 },
	},
};

//From Calendrical Calculations: The Ultimate Edition
//Appendix C: Sample Date
const struct known_convert_mjd eg2mjd_cctue[] = {
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  161 , .month =  7 , .day =  15 },
		.mjd = -892769,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  580 , .month =  3 , .day =  6 },
		.mjd = -739963,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  818 , .month =  2 , .day =  22 },
		.mjd = -653107,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  883 , .month =  3 , .day =  15 },
		.mjd = -629359,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  1217 , .month =  9 , .day =  15 },
		.mjd = -507269,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  1324 , .month =  2 , .day =  18 },
		.mjd = -468421,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  1442 , .month =  9 , .day =  10 },
		.mjd = -425149,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  1761 , .month =  5 , .day =  8 },
		.mjd = -308836,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  1844 , .month =  6 , .day =  28 },
		.mjd = -278491,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  1938 , .month =  5 , .day =  18 },
		.mjd = -244221,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  1988 , .month =  5 , .day =  18 },
		.mjd = -225971,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2036 , .month =  6 , .day =  23 },
		.mjd = -208416,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2046 , .month =  7 , .day =  20 },
		.mjd = -204739,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2139 , .month =  9 , .day =  28 },
		.mjd = -170726,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2184 , .month =  5 , .day =  29 },
		.mjd = -154420,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2240 , .month =  8 , .day =  19 },
		.mjd = -133900,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2302 , .month =  2 , .day =  11 },
		.mjd = -111458,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2308 , .month =  7 , .day =  30 },
		.mjd = -109099,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2396 , .month =  11 , .day =  29 },
		.mjd = -76860,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2428 , .month =  12 , .day =  27 },
		.mjd = -65152,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2465 , .month =  1 , .day =  24 },
		.mjd = -51980,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2517 , .month =  1 , .day =  2 },
		.mjd = -33022,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2568 , .month =  2 , .day =  27 },
		.mjd = -14352,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2587 , .month =  10 , .day =  29 },
		.mjd = -7175,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2651 , .month =  12 , .day =  7 },
		.mjd = 16223,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2678 , .month =  4 , .day =  17 },
		.mjd = 25848,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2690 , .month =  5 , .day =  25 },
		.mjd = 30266,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2691 , .month =  12 , .day =  17 },
		.mjd = 30833,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2692 , .month =  6 , .day =  3 },
		.mjd = 31004,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2740 , .month =  11 , .day =  27 },
		.mjd = 48698,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2744 , .month =  11 , .day =  7 },
		.mjd = 50138,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2787 , .month =  8 , .day =  1 },
		.mjd = 65737,
	},
	{
		.c = &mon13_ancient_egyptian,
		.d = {.year =  2843 , .month =  4 , .day =  20 },
		.mjd = 86076,
	},
};

//Gregorian and French Revolutionary conversions
//Most dates are from Encyclopedia Britannica
//https://www.britannica.com/science/French-republican-calendar
//https://www.britannica.com/place/France/The-Directory#ref465247
//Some are from Wikipedia
//https://en.wikipedia.org/wiki/Glossary_of_the_French_Revolution#Events_commonly_known_by_their_Revolutionary_dates
const struct known_convert_date gr2fr_brit[] = {
	//National Convention proclaims France a republic
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1792 , .month =  9 , .day =  22 },
		.d1 = {.year =  1 , .month =  1 , .day =  1 },
	},
	//Consolidation of Revolutionary government
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  12 , .day =  4 },
		.d1 = {.year =  2 , .month =  3 , .day =  14 },
	},
	//Law of 22 Prairial (accelerating Reign of Terror)
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1794 , .month =  6 , .day =  10 },
		.d1 = {.year =  2 , .month =  9 , .day =  22 },
	},
	//Thermidor Reaction
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1794 , .month =  7 , .day =  27 },
		.d1 = {.year =  2 , .month =  11 , .day =  9 },
	},
	//Insurrection of sanscullotes
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1795 , .month =  5 , .day =  20 },
		.d1 = {.year =  3 , .month =  9 , .day =  1 },
	},
	//Napoleon's "Whiff of Grapeshot" (Wikipedia)
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1795 , .month =  10 , .day =  5 },
		.d1 = {.year =  4 , .month =  1 , .day =  13 },
	},
	//Coup of 18 Fructidor, against royalists
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1797 , .month =  9 , .day =  4 },
		.d1 = {.year =  5 , .month =  12 , .day =  18 },
	},
	//Law of 22 Floreal, depriving left-wing deputies of seats (Wikipedia)
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1798 , .month =  5 , .day =  11 },
		.d1 = {.year =  6 , .month =  8 , .day =  22 },
	},
	//Coup of 30 Prairial, bringing Emmanuel-Joseph Sieyès to power
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1799 , .month =  6 , .day =  18 },
		.d1 = {.year =  7 , .month =  9 , .day =  30 },
	},
	//Coup of 18 Brumaire, bringing Napoleon to power
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1799 , .month =  11 , .day =  9 },
		.d1 = {.year =  8 , .month =  2 , .day =  18 },
	},
};

//Gregorian and French Revolutionary conversions
//Begin and end of months
//https://en.wikipedia.org/wiki/French_Republican_calendar#Rural_calendar
const struct known_convert_date gr2fr_wiki[] = {
	//1 Vendémiaire <-> 22 September
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1792 , .month =  9 , .day =  22 },
		.d1 = {.year =  1 , .month =  1 , .day =  1 },
	},
	//30 Vendémiaire <-> 21 October
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1792 , .month =  10 , .day =  21 },
		.d1 = {.year =  1 , .month =  1 , .day =  30 },
	},
	//1 Brumaire <-> 22 October
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1792 , .month =  10 , .day =  22 },
		.d1 = {.year =  1 , .month =  2 , .day =  1 },
	},
	//30 Brumaire <-> 20 November
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1792 , .month =  11 , .day =  20 },
		.d1 = {.year =  1 , .month =  2 , .day =  30 },
	},
	//1 Frimaire <-> 21 November
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1792 , .month =  11 , .day =  21 },
		.d1 = {.year =  1 , .month =  3 , .day =  1 },
	},
	//30 Frimaire <-> 20 December
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1792 , .month =  12 , .day =  20 },
		.d1 = {.year =  1 , .month =  3 , .day =  30 },
	},
	//1 Nivôse <-> 22 December
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1792 , .month =  12 , .day =  21 },
		.d1 = {.year =  1 , .month =  4 , .day =  1 },
	},
	//30 Nivôse <-> 20 January
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  1 , .day =  19 },
		.d1 = {.year =  1 , .month =  4 , .day =  30 },
	},
	//1 Pluviôse <-> 21 January
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  1 , .day =  20 },
		.d1 = {.year =  1 , .month =  5 , .day =  1 },
	},
	//30 Pluviôse <-> 18 February
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  2 , .day =  18 },
		.d1 = {.year =  1 , .month =  5 , .day =  30 },
	},
	//1 Ventôse <-> 19 February
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  2 , .day =  19 },
		.d1 = {.year =  1 , .month =  6 , .day =  1 },
	},
	//30 Ventôse <-> 20 March
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  3 , .day =  20 },
		.d1 = {.year =  1 , .month =  6 , .day =  30 },
	},
	//1 Germinal <-> 21 March
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  3 , .day =  21 },
		.d1 = {.year =  1 , .month =  7 , .day =  1 },
	},
	//30 Germinal <-> 19 April
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  4 , .day =  19 },
		.d1 = {.year =  1 , .month =  7 , .day =  30 },
	},
	//1 Floréal <-> 20 April
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  4 , .day =  20 },
		.d1 = {.year =  1 , .month =  8 , .day =  1 },
	},
	//30 Floréal <-> 19 May
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  5 , .day =  19 },
		.d1 = {.year =  1 , .month =  8 , .day =  30 },
	},
	//1 Prairial <-> 20 May
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  5 , .day =  20 },
		.d1 = {.year =  1 , .month =  9 , .day =  1 },
	},
	//30 Prairial <-> 18 June
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  6 , .day =  18 },
		.d1 = {.year =  1 , .month =  9 , .day =  30 },
	},
	//1 Messidor <-> 19 June
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  6 , .day =  19 },
		.d1 = {.year =  1 , .month =  10 , .day =  1 },
	},
	//30 Messidor <-> 18 July
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  7 , .day =  18 },
		.d1 = {.year =  1 , .month =  10 , .day =  30 },
	},
	//1 Thermidor <-> 19 July
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  7 , .day =  19 },
		.d1 = {.year =  1 , .month =  11 , .day =  1 },
	},
	//30 Prairial <-> 17 August
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  8 , .day =  17 },
		.d1 = {.year =  1 , .month =  11 , .day =  30 },
	},
	//1 Fructidor <-> 18 August July
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  8 , .day =  18 },
		.d1 = {.year =  1 , .month =  12 , .day =  1 },
	},
	//30 Fructidor <-> 16 September
	{
		.c0 = &mon13_gregorian_year_zero,
		.c1 = &mon13_french_revolutionary_romme_sub1,
		.d0 = {.year =  1793 , .month =  9 , .day =  16 },
		.d1 = {.year =  1 , .month =  12 , .day =  30 },
	},
};

#endif //MON13_KNOWN_H