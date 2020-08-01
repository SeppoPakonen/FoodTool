#include "Common.h"

Time EndOfYesterday(Date d) {
	Time t(d.year, d.month, d.day, 23, 59, 59);
	t -= 24*60*60;
	return t;
}
