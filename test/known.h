//This is supposed to be a private header, not exposed as part of the mon13 API.

#ifndef MON13_KNOWN_H
#define MON13_KNOWN_H

#include "mon13.h"

struct known_convert_date {
	const struct mon13_cal* c0;
	const struct mon13_cal* c1;
	const struct mon13_date d0;
	const struct mon13_date d1;
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

#endif //MON13_KNOWN_H