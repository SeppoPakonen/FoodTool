#ifndef _FoodTool_Database_h_
#define _FoodTool_Database_h_


struct NutritionType : Moveable<NutritionType> {
	bool is_valid = false;
	bool is_user_added = false;
	String units;
	String tagname;
	String nutr_desc;
	
	void Serialize(Stream& s) {
		s	% is_valid
			% is_user_added
			% units
			% tagname
			% nutr_desc;
	}
};

struct NutritionInfo : Moveable<NutritionInfo> {
	uint16 nutr_no = 0;
	float nutr_value = 0;
	//int num_data_pts;
	float std_error = 0;
	bool is_user_added = false;
	
	void Serialize(Stream& s) {
		s	% nutr_no
			% nutr_value
			% std_error
			% is_user_added;
	}
};

struct FoodGroup : Moveable<FoodGroup> {
	String name;
	
	void Serialize(Stream& s) {
		s	% name;
	}
};

struct FoodDescription : Moveable<FoodDescription> {
	Vector<NutritionInfo> nutr;
	String food_group_key, long_desc, short_desc, company_name;
	String manufacturer_name, survey, ref_desc;
	int refuse = 0;
	String sci_name;
	float n_factor = 0, pro_factor = 0, fat_factor = 0, cho_factor = 0;
	bool is_user_added = false;
	
	void Serialize(Stream& s) {
		s	% nutr
			% food_group_key % long_desc % short_desc % company_name
			% manufacturer_name % survey % ref_desc
			% refuse
			% sci_name
			% n_factor % pro_factor % fat_factor % cho_factor
			% is_user_added
			;
	}
};

struct Database {
	VectorMap<String, FoodGroup> food_groups;
	VectorMap<String, FoodDescription> food_descriptions;
	Vector<NutritionType> nutr_types;
	Index<int> used_foods;
	bool is_init = false;
	
	typedef Database CLASSNAME;
	Database();
	~Database() {StoreThis();}
	
	
	void Serialize(Stream& s) {
		s	% food_groups
			% food_descriptions
			% nutr_types
			% used_foods
			% is_init;
	}
	void LoadThis() {LoadFromFile(*this, ConfigFile("latest.db"));}
	void StoreThis() {StoreToFile(*this, ConfigFile("latest.db"));}
	bool Init();
	int AddNutrition(String name, String unit, String desc);
	void StartStoreThis() {Thread::Start(THISBACK(StoreThis));}
	
	void VLCD_Preset();
};

inline Database& GetDatabase() {return Single<Database>();}
inline Database& DB() {return GetDatabase();}

#endif
