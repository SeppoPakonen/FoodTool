#include "CommonCtrl.h"


void DrawTextCentered(Draw& d, int x, int y, String txt, Font fnt, Color c) {
	Size sz = GetTextSize(txt, fnt);
	x -= sz.cx / 2;
	d.DrawText(x, y, txt, fnt, c);
}


void TableDrawer::Paint(Rect rect, Draw& d) {
	int pad = 5;
	Size sz(rect.GetSize());
	if (sz.cx < 3*pad || sz.cy < 3*pad)
		return;
	int fnt_h = min(20, sz.cy / rows - 2*pad);
	Font fnt = SansSerif(fnt_h);
	max_col_w.SetCount(cols);
	
	Color color = Black();
	
	for(int t = 0; t < 2; t++) {
		for(auto& d : max_col_w) d = 0;
		
		for(int c = 0; c < cols; c++) {
			for(int r = 0; r < rows; r++) {
				String s = Get(r, c);
				Size sz = GetTextSize(s, fnt);
				sz.cx += 2*pad;
				sz.cy += 2*pad;
				max_col_w[c] = max(sz.cx, max_col_w[c]);
			}
		}
		
		int w = 0;
		for(int c = 0; c < cols; c++)
			w += max_col_w[c];
		if (!w) break;
		
		if (t == 0) {
			double mul = (double)sz.cx / w;
			if (mul < 1.0) {
				fnt_h *= mul;
				fnt = SansSerif(fnt_h);
				continue;
			}
		}
		
		int x = rect.left;
		for(int c = 0; c < cols; c++) {
			int y = rect.top;
			d.DrawLine(x, rect.top, x, rect.bottom, 1, color);
			for(int r = 0; r < rows; r++) {
				String s = Get(r, c);
				d.DrawText(x+pad, y+pad, s, fnt, color);
				y += fnt_h + 2*pad;
			}
			x += max_col_w[c];
		}
		d.DrawLine(x, rect.top, x, rect.bottom, 1, color);
		
		int y = rect.top;
		for(int r = 0; r <= rows; r++) {
			d.DrawLine(rect.left, y, rect.right, y, 1, color);
			y += fnt_h + 2*pad;
		}
		
		break;
	}
}
