#ifndef _FoodTool_FoodTool_h
#define _FoodTool_FoodTool_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define LAYOUTFILE <FoodTool/FoodTool.lay>
#include <CtrlCore/lay.h>

#define IMAGECLASS Images
#define IMAGEFILE <FoodTool/Images.iml>
#include <Draw/iml_header.h>


#include "Food.h"
#include "Profile.h"

struct StatusCtrl : public WithStatusLayout<ParentCtrl> {
	
	
	typedef StatusCtrl CLASSNAME;
	StatusCtrl();
	
	void Data();
};

struct ConfigurationCtrl : public WithConfigurationLayout<ParentCtrl> {
	
	
	typedef ConfigurationCtrl CLASSNAME;
	ConfigurationCtrl();
	
};

struct ExceptionsCtrl : public ParentCtrl {
	
	
	typedef ExceptionsCtrl CLASSNAME;
	ExceptionsCtrl();
	
};

struct NoteCtrl : public ParentCtrl {
	
	
	typedef NoteCtrl CLASSNAME;
	NoteCtrl();
	
};

struct UsageCtrl : public ParentCtrl {
	
	
	typedef UsageCtrl CLASSNAME;
	UsageCtrl();
	
};

struct WeightCtrl : public ParentCtrl {
	
	
	typedef WeightCtrl CLASSNAME;
	WeightCtrl();
	
};

class FoodTool : public WithFoodToolLayout<TopWindow> {
	TabCtrl tabs;
	
	StatusCtrl status;
	ConfigurationCtrl conf;
	ExceptionsCtrl exc;
	NoteCtrl notes;
	UsageCtrl usage;
	WeightCtrl weight;
	
	TimeCallback tc;
public:
	typedef FoodTool CLASSNAME;
	FoodTool();
	
	void Data();
};

#endif
