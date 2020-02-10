#ifndef _FoodTool_ScheduleCtrl_h_
#define _FoodTool_ScheduleCtrl_h_


void DrawGradientVert(Draw& d, Rect r, Color top_clr, Color btm_clr);
Size GetTextSizeWithNewlines(String txt, Font fnt);
void DrawTextWithNewlines(Draw& d, int x, int y, String txt, Font fnt, Color c);

struct ScheduleCtrl : public Ctrl {
	Color menu_clr_top, menu_clr_btm;
	Color prep_clr_top, prep_clr_btm;
	Color shop_clr_top, shop_clr_btm;
	Color nutr_clr_top, nutr_clr_btm;
	int shift = 0;
	
	typedef ScheduleCtrl CLASSNAME;
	ScheduleCtrl();
	
	void Paint(Draw& d);
	virtual void MouseWheel(Point p, int zdelta, dword keyflags);
	virtual bool Key(dword key, int count);
	virtual void LeftDown(Point p, dword keyflags);
};



#endif
