#ifndef _FoodTool_NoteCtrl_h_
#define _FoodTool_NoteCtrl_h_


struct NoteCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	WithNoteLayout<ParentCtrl> note;
	
	
	typedef NoteCtrl CLASSNAME;
	NoteCtrl();
	
	void Data();
	void AddNote();
	void SelectNote();
};


#endif
