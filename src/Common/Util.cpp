#include "Common.h"

void FillVector(Vector<double>& v) {
	int first = -1;
	for(int i = 0; i < v.GetCount(); i++)
		if (v[i] != 0.0)
			{first = i; break;}
	if (first < 0)
		return;
	double left_pad = v[first];
	for(int i = first-1; i >= 0; i--)
		v[i] = left_pad;
	double prev = left_pad;
	for(int i = first+1; i < v.GetCount(); i++) {
		double& d = v[i];
		if (!d)
			d = prev;
		else
			prev = d;
	}
}

String UpperBegin(String s) {
	String o;
	int is_upper = true;
	for(int i = 0; i < s.GetCount(); i++) {
		int chr = s[i];
		if (is_upper) {
			o.Cat(ToUpper(chr));
			is_upper = false;
		}
		else
			o.Cat(ToLower(chr));
		if (chr == ' ')
			is_upper = true;
	}
	return o;
}

String GetTimeDurationString(Time begin, Time end) {
	String out;
	int64 total = end.Get() - begin.Get();
	bool neg = false;
	if (total < 0) {
		neg = true;
		total *= -1;
	}
	int sec = total % 60;
	total /= 60;
	int min = total % 60;
	total /= 60;
	int hours = total % 24;
	total /= 24;
	int days = total % 30;
	total /= 30;
	int months = total % 12;
	total /= 12;
	int64 years = total;
	if (years)		{if (out.GetCount()) out << ", ";	out << Format(t_("%d years"), years);}
	if (months)		{if (out.GetCount()) out << ", ";	out << Format(t_("%d months"), months);}
	if (days)		{if (out.GetCount()) out << ", ";	out << Format(t_("%d days"), days);}
	if (hours)		{if (out.GetCount()) out << ", ";	out << Format(t_("%d hours"), hours);}
	if (min)		{if (out.GetCount()) out << ", ";	out << Format(t_("%d minutes"), min);}
	if (sec)		{if (out.GetCount()) out << ", ";	out << Format(t_("%d seconds"), sec);}
	
	if (neg) out = "-" + out;
	return out;
}
