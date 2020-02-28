#ifndef _FoodTool_StatusCtrl_h_
#define _FoodTool_StatusCtrl_h_

struct StatusCtrl : public WithStatusLayout<ParentCtrl> {
	Date cur_date;
	
	typedef StatusCtrl CLASSNAME;
	StatusCtrl();
	
	void Data();
};

#endif
