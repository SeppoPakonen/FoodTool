#include "Common.h"


double nround (double n, unsigned int c)
{
    double marge = pow (10, c);
    double up    = n * marge;
    double ret   = round (up) / marge;

    return ret;
}
double NormalPDF(double x, double mean, double dev) {
	double a = dev * sqrt(2 * M_PI);
	double b = 1.0 / a;
	double c = x  - mean;
	double d = -(c * c) / (2 * dev * dev);
	double e = exp(d);
	return b * e;
}

