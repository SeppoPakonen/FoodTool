#include "FoodTool.h"

void DrawGradientVert(Draw& d, Rect r, Color top_clr, Color btm_clr) {
	double menu_blend_step = 255.0 / (r.GetHeight()-1);
	for(int y = r.top, i = 0; y < r.bottom; y++, i++) {
		d.DrawLine(r.left, y, r.right, y, 1, Blend(top_clr, btm_clr, i * menu_blend_step));
	}
}

ScheduleCtrl::ScheduleCtrl() {
	WantFocus();
	
	menu_clr_top = Color(185, 213, 255);
	menu_clr_btm = Color(189, 255, 219);
	prep_clr_top = Color(255, 255, 95);
	prep_clr_btm = Color(255, 255, 203);
	shop_clr_top = Color(170, 42, 0);
	shop_clr_btm = Color(255, 42, 0);
	nutr_clr_top = Color(207, 255, 204);
	nutr_clr_btm = Color(255, 226, 209);
}

void ScheduleCtrl::Paint(Draw& d) {
	Size sz(GetSize());
	int gw = 500;
	int per_screen = sz.cx / gw + (sz.cx % gw ? 1 : 0);
	
	const Profile& prof = GetProfile();
	Date today = GetSysTime();
	int today_i = -1;
	for(int i = prof.storage.days.GetCount()-1; i >= 0; i--) {
		if (prof.storage.days[i].date == today) {
			today_i = i;
			break;
		}
	}
	if (today_i < 0) return;
	
	if (showed_nutr.IsEmpty()) {
		const Database& db = DB();
		showed_nutr.FindAdd(KCAL);
		showed_nutr.FindAdd(FAT);
		showed_nutr.FindAdd(CARB);
		showed_nutr.FindAdd(PROT);
		for(int i = 0; i < db.nutr_recom.GetCount(); i++)
			showed_nutr.FindAdd(db.nutr_recom[i].nutr_no);
	}
	
	int min_shift = -today_i;
	int max_shift = prof.storage.days.GetCount() - today_i - per_screen;
	if (shift < min_shift) shift = min_shift;
	if (shift > max_shift) shift = max_shift;
	
	
	
	for(int i = 0; i < per_screen; i++) {
		int x = i * gw;
		int pos = today_i + shift + i;
		const FoodDay& day = prof.storage.days[pos];
		const DailyPlan& plan = prof.planned_daily[pos];
		
		for(int txt_h = 15; txt_h >= 6; txt_h--) {
			Font txt_fnt = SansSerif(txt_h);
			Size menu_sz = GetTextSizeWithNewlines(day.menu, txt_fnt);
			Size prep_sz = GetTextSizeWithNewlines(day.preparation, txt_fnt);
			int xpad = 4;
			int txtsum_h = menu_sz.cy + prep_sz.cy;
			
			if (menu_sz.cx + xpad > gw || prep_sz.cx + xpad > gw) continue;
			
			double date_part = 0.1;
			double menu_part = txtsum_h ? (double)menu_sz.cy / txtsum_h : 1;
			double prep_part = txtsum_h ? (double)prep_sz.cy / txtsum_h : 0;
			double shop_part = day.is_shopping ? 0.1 : 0.0;
			double nutr_part = 0.5;
			double part_sum = date_part + menu_part + prep_part + shop_part + nutr_part;
			
			int date_h = date_part / part_sum * sz.cy;
			int menu_h = menu_part / part_sum * sz.cy;
			int prep_h = prep_part / part_sum * sz.cy;
			int shop_h = shop_part / part_sum * sz.cy;
			int nutr_h = nutr_part / part_sum * sz.cy;
			int y = 0;
			
			if (menu_sz.cy > menu_h || prep_sz.cy > prep_h) continue;
			
			DrawGradientVert(d, RectC(x, y, gw, date_h), GrayColor(256-128), GrayColor(256-32));
			Font date_fnt = SansSerif(date_h*0.8);
			String date_str = Format("%d.%d. %Day", (int)day.date.day, (int)day.date.month, DayOfWeek(day.date));
			d.DrawText(x+2, y+2, date_str, date_fnt, Black());
			d.DrawText(x, y, date_str, date_fnt, White());
			y += date_h;
			
			if (day.date < today)
				DrawGradientVert(d, RectC(x, y, gw, menu_h), GrayColor(256-32), GrayColor(256-64));
			else
				DrawGradientVert(d, RectC(x, y, gw, menu_h), menu_clr_top, menu_clr_btm);
			DrawTextWithNewlines(d, x+xpad+1, y+1, day.menu, txt_fnt, White());
			DrawTextWithNewlines(d, x+xpad, y, day.menu, txt_fnt, Black());
			y += menu_h;
			
			DrawGradientVert(d, RectC(x, y, gw, prep_h), prep_clr_top, prep_clr_btm);
			DrawTextWithNewlines(d, x+xpad+1, y+1, day.preparation, txt_fnt, White());
			DrawTextWithNewlines(d, x+xpad, y, day.preparation, txt_fnt, Black());
			y += prep_h;
			
			if (day.is_shopping) {
				Font fnt = SansSerif(shop_h * 0.8);
				DrawGradientVert(d, RectC(x, y, gw, shop_h), shop_clr_top, shop_clr_btm);
				d.DrawText(x+xpad+2, y+2, "Groceries", fnt, Black());
				d.DrawText(x+xpad, y, "Groceries", fnt, White());
				y += shop_h;
			}
			
			const Database& db = DB();
			
			DrawGradientVert(d, RectC(x, y, gw, nutr_h), nutr_clr_top, nutr_clr_btm);
			double min = -1;
			double max = +1;
			double range = max - min;
			int zero_y = (0 - min) * nutr_h / range;
			double xstep = (double)gw / (4 * 3 + showed_nutr.GetCount());
			int xi = 0;
			for(int j = 0; j < showed_nutr.GetCount(); j++) {
				int nutr_i = showed_nutr[j];
				int w = 0;
				Color c;
				double value = 0;
				int nx = xi * xstep;
				
				if (j < 4) {
					w = (xi + 4) * xstep - nx;
					xi += 4;
					switch (j) {
						case 0: c = GrayColor(); break;
						case 1: c = Color(198, 127, 0); break;
						case 2: c = Color(28, 170, 150); break;
						case 3: c = Color(170, 42, 0); break;
					}
				}
				else {
					w = (xi + 1) * xstep - nx;
					xi += 1;
					//c = Rainbow((j - 4) * 1.333 / (showed_nutr.GetCount()-4));
					c = CoolBlue((double)(j - 4) / (showed_nutr.GetCount()-4));
				}
				
				value = day.total_sum.nutr[nutr_i] / day.target_sum.nutr[nutr_i] - 1;
				if (value < min) value = min;
				if (value > max) value = max;
				
				int ny = (value - min) * nutr_h / range;
				if (value >= 0) {
					int h = ny - zero_y;
					d.DrawRect(x + nx, y + nutr_h-ny, w, h, c);
				}
				else {
					int h = zero_y - ny;
					d.DrawRect(x + nx, y + nutr_h-zero_y, w, h, c);
				}
			}
			int ly = y + nutr_h-zero_y;
			d.DrawLine(x, ly, x + gw, ly, 1, GrayColor(128-32));
			y += nutr_h;
			
			d.DrawLine(x, 0, x, sz.cy, 1, GrayColor(128+32));
			d.DrawLine(x + gw - 1, 0, x + gw - 1, sz.cy, 1, GrayColor(128-32));
			
			break;
		}
	}
	
}

Size GetTextSizeWithNewlines(String txt, Font fnt) {
	if (txt.IsEmpty()) return Size(0,0);
	Vector<String> lines = Split(txt, "\n", false);
	Size sz(0,0);
	for(int i = 0; i < lines.GetCount(); i++) {
		String line = lines[i];
		if (line.IsEmpty())
			sz.cy += fnt.GetHeight();
		else {
			Size l = GetTextSize(line, fnt);
			sz.cy += max(fnt.GetHeight(), l.cy);
			sz.cx = max(sz.cx, l.cx);
		}
	}
	return sz;
}

void DrawTextWithNewlines(Draw& d, int x, int y, String txt, Font fnt, Color c) {
	if (txt.IsEmpty()) return;
	Vector<String> lines = Split(txt, "\n", false);
	int yoff = 0;
	for(int i = 0; i < lines.GetCount(); i++) {
		String line = lines[i];
		if (line.IsEmpty())
			y += fnt.GetHeight();
		else {
			d.DrawText(x, y + yoff, line, fnt, c);
			Size l = GetTextSize(line, fnt);
			yoff += max(fnt.GetHeight(), l.cy);
		}
	}
}


void ScheduleCtrl::MouseWheel(Point p, int zdelta, dword keyflags) {
	if (zdelta < 0)
		shift += 3;
	else if (zdelta > 0)
		shift -= 3;
		
	Refresh();
}

bool ScheduleCtrl::Key(dword key, int count) {
	
	if (key == K_HOME) {
		shift = 0;
		Refresh();
		return true;
	}
	
	if (key == K_END) {
		shift = INT_MAX;
		Refresh();
		return true;
	}
	
	if (key == K_PAGEUP) {
		shift += 3;
		Refresh();
		return true;
	}
	
	if (key == K_PAGEDOWN) {
		shift -= 3;
		Refresh();
		return true;
	}
	
	if (key == K_LEFT) {
		shift--;
		Refresh();
		return true;
	}
	
	if (key == K_RIGHT) {
		shift++;
		Refresh();
		return true;
	}
	
	return false;
}

void ScheduleCtrl::LeftDown(Point p, dword keyflags) {
	SetFocus();
}
