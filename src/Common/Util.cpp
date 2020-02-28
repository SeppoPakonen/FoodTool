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
