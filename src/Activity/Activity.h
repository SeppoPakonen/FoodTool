#ifndef _Activity_Activity_h
#define _Activity_Activity_h

#include <Food/Food.h>

#include "Body.h"

struct Activity {
	Body body;
	
	
	typedef Activity CLASSNAME;
	Activity();
	
	void AddMuscleGroup(String name, double relative_mass);
	
	
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {}
	}
};


#endif
