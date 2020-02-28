#ifndef _CommonCtrl_CommonCtrl_h
#define _CommonCtrl_CommonCtrl_h

#include <CtrlLib/CtrlLib.h>
#include <RichEdit/RichEdit.h>
#include <plugin/jpg/jpg.h>
using namespace Upp;

#include <Common/Common.h>

#include "Color.h"
#include "Draw.h"
#include "Images.h"





inline void SplitterHotfix(Splitter& split, int pos) {
	if (split.GetPos() <= 100)
		split.SetPos(pos);
}


#endif
