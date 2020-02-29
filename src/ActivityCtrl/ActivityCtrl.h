#ifndef _ActivityCtrl_ActivityCtrl_h_
#define _ActivityCtrl_ActivityCtrl_h_

#include <Activity/Activity.h>
#include <FoodCtrl/FoodCtrl.h>

#define LAYOUTFILE <ActivityCtrl/Layouts.lay>
#include <CtrlCore/lay.h>

#include "ExerciseEditorCtrl.h"
#include "ActivityHistoryCtrl.h"
#include "ExercisePlayerCtrl.h"

struct ActivityCtrl : public ParentCtrl {
	
	
	typedef ActivityCtrl CLASSNAME;
	ActivityCtrl();
	void Data();
	void SelectActivity();
	
};


#endif
