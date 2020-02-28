#include "FoodCtrl.h"


HairSampleDeficitCtrl::HairSampleDeficitCtrl() {
	Add(split.SizePos());
	split.Horz() << list << right;
	split.SetPos(2500);
	
	right.Add(edit.HSizePos().VSizePos(0,30));
	right.Add(add.BottomPos(0,30).RightPos(0, 200));
	
	add.SetLabel(t_("Add"));
	add <<= THISBACK(AddItem);
	
	list.AddColumn(t_("Added"));
	list.WhenAction << THISBACK(SelectItem);
	list.WhenLeftClick << THISBACK(SelectItem);
}

void HairSampleDeficitCtrl::Data() {
	Profile& prof = GetProfile();
	
	SplitterHotfix(split, 2500);
	
	if (prof.defs.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.defs.GetCount(); i++) {
			int row = prof.defs.GetCount() - 1 - i;
			const HairSampleDeficitProfile& n = prof.defs[i];
			list.Set(row, 0, n.added);
		}
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectItem();
	}
}

void HairSampleDeficitCtrl::SelectItem() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int note_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	const HairSampleDeficitProfile& n = prof.defs[note_i];
	edit.prof = n;
	edit.Refresh();
}

void HairSampleDeficitCtrl::AddItem() {
	Profile& prof = GetProfile();
	HairSampleDeficitProfile& n = prof.defs.Add();
	n = edit.prof;
	n.added = GetSysTime();
	
	Data();
}

















HairSampleDeficitEdit::HairSampleDeficitEdit() {
	prof.added = GetSysTime();
	prof.elements[HSAMP_CALCIUM] = 0.14;
	prof.elements[HSAMP_MAGNESIUM] = 0.01;
	prof.elements[HSAMP_SODIUM] = 0.25;
	prof.elements[HSAMP_POTASSIUM] = 0.60;
	prof.elements[HSAMP_COPPER] = 0.07;
	prof.elements[HSAMP_ZINC] = 0.32;
	prof.elements[HSAMP_MANGANESE] = 0.10;
	prof.elements[HSAMP_CHROMIUM] = 0.04;
	prof.elements[HSAMP_VANADIUM] = 0.005;
	prof.elements[HSAMP_MOLYBDENUM] = 0.15;
	prof.elements[HSAMP_BORON] = 0.012;
	prof.elements[HSAMP_IODINE] = 0.04;
	prof.elements[HSAMP_LITHIUM] = 0.001;
	prof.elements[HSAMP_PHOSPHORUS] = 0.20;
	prof.elements[HSAMP_SELENIUM] = 0.02;
	prof.elements[HSAMP_STRONTIUM] = 0.005;
	prof.elements[HSAMP_SULFUR] = 0.75;
	prof.elements[HSAMP_COBALT] = 0.001;
	prof.elements[HSAMP_IRON] = 0.03;
	prof.elements[HSAMP_GERMANIUM] = 0.20;
	prof.elements[HSAMP_RUBIDIUM] = 0.50;
	prof.elements[HSAMP_ZIRCONIUM] = 0.18;
	
	green = Color(178, 255, 197);
	yellow = Color(255, 255, 169);
	red = Color(255, 177, 170);
	
	x_to_percentile.SetCount(1001);
	
	double sum = 0;
	for(int i = 0; i < x_to_percentile.GetCount(); i++) {
		double f = (double)i / (x_to_percentile.GetCount() - 1);
		double pdf = NormalPDF(-3+6*f, 0, 1);;
		x_to_percentile[i] = pdf;
		sum += pdf;
	}
	
	// x25, x160, x500, x840, x975;
	int mode = 0;
	double sum2 = 0;
	for(int i = 0; i < x_to_percentile.GetCount(); i++) {
		double& d = x_to_percentile[i];
		if      (mode == 0 && sum2 >= 0.025) {x25 = (double)i / (x_to_percentile.GetCount() - 1); mode++;}
		else if (mode == 1 && sum2 >= 0.160) {x160 = (double)i / (x_to_percentile.GetCount() - 1); mode++;}
		else if (mode == 2 && sum2 >= 0.450) {x450 = (double)i / (x_to_percentile.GetCount() - 1); mode++;}
		else if (mode == 3 && sum2 >= 0.500) {x500 = (double)i / (x_to_percentile.GetCount() - 1); mode++;}
		else if (mode == 4 && sum2 >= 0.550) {x550 = (double)i / (x_to_percentile.GetCount() - 1); mode++;}
		else if (mode == 5 && sum2 >= 0.840) {x840 = (double)i / (x_to_percentile.GetCount() - 1); mode++;}
		else if (mode == 6 && sum2 >= 0.975) {x975 = (double)i / (x_to_percentile.GetCount() - 1); mode++;}
		
		double a = d / sum;
		d = sum2;
		sum2 += a;
	}
}

void HairSampleDeficitEdit::LeftDown(Point p, dword keyflags) {
	Size sz(GetSize());
	
	int rows = 1 + HSAMP_COUNT;
	double ystep = (double)sz.cy / rows;
	
	double htitle_part = 0.333;
	double value_part = 1.0 - htitle_part;
	int valuex = sz.cx * htitle_part;
	int valuew = sz.cx * value_part;
	
	if (p.x >= valuex && p.y >= ystep) {
		int row = (p.y / ystep) - 1;
		double f = (double)(p.x - valuex) / valuew;
		int i = f * x_to_percentile.GetCount();
		if (row >= 0 && row < HSAMP_COUNT && i >= 0 && i < x_to_percentile.GetCount()) {
			prof.elements[row] = x_to_percentile[i];
			Refresh();
		}
	}
}

void HairSampleDeficitEdit::Paint(Draw& d) {
	Size sz(GetSize());
	d.DrawRect(sz, White());
	
	int rows = 1 + HSAMP_COUNT;
	double ystep = (double)sz.cy / rows;
	
	double htitle_part = 0.333;
	double value_part = 1.0 - htitle_part;
	int valuex = sz.cx * htitle_part;
	int valuew = sz.cx * value_part;
	
	int x25 = valuex + this->x25 * valuew;
	int x160 = valuex + this->x160 * valuew;
	int x450 = valuex + this->x450 * valuew;
	int x500 = valuex + this->x500 * valuew;
	int x550 = valuex + this->x550 * valuew;
	int x840 = valuex + this->x840 * valuew;
	int x975 = valuex + this->x975 * valuew;
	int ry = ystep;
	int rh = sz.cy - ry;
	d.DrawRect(valuex, ry, x25 - valuex, rh, red);
	d.DrawRect(x25, ry, x160 - x25, rh, yellow);
	d.DrawRect(x160, ry, x450 - x160, rh, green);
	d.DrawRect(x550, ry, x840 - x550, rh, green);
	d.DrawRect(x840, ry, x975 - x840, rh, yellow);
	d.DrawRect(x975, ry, sz.cx - x975, rh, red);
	
	d.DrawLine(valuex, ry, valuex, sz.cy, 1, GrayColor());
	d.DrawLine(x25, ry, x25, sz.cy, 1, GrayColor());
	d.DrawLine(x160, ry, x160, sz.cy, 1, GrayColor());
	d.DrawLine(x450, ry, x450, sz.cy, 1, GrayColor());
	d.DrawLine(x550, ry, x550, sz.cy, 1, GrayColor());
	d.DrawLine(x840, ry, x840, sz.cy, 1, GrayColor());
	d.DrawLine(x975, ry, x975, sz.cy, 1, GrayColor());
	
	Font fnt = SansSerif(ystep * 0.80);
	d.DrawText(5, 0, t_("Your mineral levels"), fnt, Black());
	DrawTextCentered(d, x25, 0, t_("2.5th"), fnt, Black());
	DrawTextCentered(d, x160, 0, t_("16th"), fnt, Black());
	DrawTextCentered(d, x500, 0, t_("50th"), fnt, Black());
	DrawTextCentered(d, x840, 0, t_("84th"), fnt, Black());
	DrawTextCentered(d, x975, 0, t_("97.5th"), fnt, Black());
	
	for(int i = 1; i < rows; i++) {
		int y = i * ystep;
		
		if (i)
			d.DrawLine(0, y, sz.cx, y, 1, GrayColor());
		
		d.DrawText(5, y, GetHairSampleDeficitElement(i-1), fnt, Black());
		
		double f = 0.5;
		double perc = prof.elements[i - 1];
		for(int j = 0; j < x_to_percentile.GetCount(); j++) {
			double value = x_to_percentile[j];
			if (value >= perc) {
				f = (double)j / (x_to_percentile.GetCount() - 1);
				break;
			}
		}
		
		int bary = y + 0.3 * ystep;
		int barh = 0.4 * ystep;
		
		if (f < 0.5) {
			int barx = valuex + f * valuew;
			int barw = x500 - barx;
			d.DrawRect(RectC(barx, bary, barw, barh), Black());
		}
		else {
			int barx = x500;
			int barw = valuex + f * valuew - x500;
			d.DrawRect(RectC(barx, bary, barw, barh), Black());
		}
	}
}

