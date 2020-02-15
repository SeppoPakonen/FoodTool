#ifndef _TimelineCtrl_TimelineCtrl_h
#define _TimelineCtrl_TimelineCtrl_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define IMAGECLASS TimelineImg
#define IMAGEFILE <TimelineCtrl/TimelineCtrl.iml>
#include <Draw/iml_header.h>

#include "Common.h"
#include "Timeline.h"

class TimelineCtrl : public ParentCtrl {
public:
	typedef TimelineCtrl CLASSNAME;
	TimelineCtrl();
	
	
};

#endif
