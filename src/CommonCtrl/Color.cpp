#include "CommonCtrl.h"


Color CoolBlue(double progress) {
	double alpha = progress - (int)progress;
	Color a = Color(28, 170, 255);
	Color b = Color(28, 42, 200);
	return Blend(a, b, alpha * 0x100);
}

Color Rainbow(double progress) {
    double div = (fabs(progress - (int)progress) * 6);
    int ascending = (int) ((div - (int)div) * 255);
    int descending = 255 - ascending;

    switch ((int) div)
    {
        case 0:
            return Color(255, ascending, 0);
        case 1:
            return Color(descending, 255, 0);
        case 2:
            return Color(0, 255, ascending);
        case 3:
            return Color(0, descending, 255);
        case 4:
            return Color(ascending, 0, 255);
        default: // case 5:
            return Color(255, 0, descending);
    }
}
