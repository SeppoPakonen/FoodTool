#ifndef _Common_Time_h_
#define _Common_Time_h_


Time EndOfYesterday(Date d);

inline String GetTimeString(Time t) {
	return Format("%d.%d.%d %d:%02d", (int)t.day, (int)t.month, (int)t.year, (int)t.hour, (int)t.minute);
}

#endif
