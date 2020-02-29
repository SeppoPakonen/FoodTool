#ifndef _ActivityCtrl_ActivityHistoryCtrl_h_
#define _ActivityCtrl_ActivityHistoryCtrl_h_

struct ActivityHistoryCtrl : public ParentCtrl {
	Splitter hsplit;
	ArrayCtrl list, items;
	
	typedef ActivityHistoryCtrl CLASSNAME;
	ActivityHistoryCtrl();
	void Data();
	void SelectGroup();
	
};

#endif
