#ifndef _FoodTool_Activity_h_
#define _FoodTool_Activity_h_

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
