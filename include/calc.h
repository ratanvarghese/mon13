//This is supposed to be a private header, not exposed as part of the mon13 API.

#ifndef MON13_CALC_H
#define MON13_CALC_H

#include "mon13.h"

struct mon13_date mon13_normalize(struct mon13_date d, const struct mon13_cal* cal);

#endif //MON13_CALC_H