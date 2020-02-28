#ifndef _CommonCtrl_Draw_h_
#define _CommonCtrl_Draw_h_


class TableDrawer {
	int cols = 0, rows = 0;
	Vector<String> items;
	Vector<int> max_col_w;
	
public:
	
	void SetSize(int rows, int cols) {this->cols = cols; this->rows = rows; items.SetCount(cols * rows);}
	void Reset() {for(auto& s : items) s.Clear();}
	void Set(int r, int c, String s) {ASSERT(c>=0&&c<cols&&r>=0&&r<rows); items[r * cols + c] = s;}
	String Get(int r, int c) const {ASSERT(c>=0&&c<cols&&r>=0&&r<rows); return items[r * cols + c];}
	void Paint(Rect rect, Draw& d);
};

void DrawTextCentered(Draw& d, int x, int y, String txt, Font fnt, Color c);

#endif
