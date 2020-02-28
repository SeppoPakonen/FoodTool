#ifndef _Food_Database_h_
#define _Food_Database_h_



struct NutritionType : Moveable<NutritionType> {
	bool is_valid = false;
	bool is_user_added = false;
	String units;
	String tagname;
	String nutr_desc;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			s	% is_valid
				% is_user_added
				% units
				% tagname
				% nutr_desc;
		}
	}
};

enum {
	OTHER,
	VITAMIN,
	MINERAL,
	AMINOACID,
	FATTYACID,
};

struct NutritionInfo : Moveable<NutritionInfo> {
	uint16 nutr_no = 0;
	double nutr_value = 0;
	//int num_data_pts;
	double std_error = 0;
	bool is_user_added = false;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			s	% nutr_no
				% nutr_value
				% std_error
				% is_user_added;
		}
	}
};

struct FoodGroup : Moveable<FoodGroup> {
	String name;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			s	% name;
		}
	}
};

struct FoodDescription : Moveable<FoodDescription> {
	Vector<NutritionInfo> nutr;
	String food_group_key, long_desc, short_desc, company_name;
	String manufacturer_name, survey, ref_desc;
	int refuse = 0;
	String sci_name;
	double n_factor = 0, pro_factor = 0, fat_factor = 0, cho_factor = 0;
	bool require_soaking = false;
	bool is_user_added = false;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			s	% nutr
				% food_group_key % long_desc % short_desc % company_name
				% manufacturer_name % survey % ref_desc
				% refuse
				% sci_name
				% n_factor % pro_factor % fat_factor % cho_factor
				% require_soaking
				% is_user_added
				;
		}
	}
	FoodDescription& AddNutrition(int nutr_no, double value, double error);
	FoodDescription& RequireSoaking() {require_soaking = true; return *this;}
};

struct NutritionRecommendation : Moveable<NutritionRecommendation> {
	int nutr_no;
	double value;
	bool per_kg;
	byte group = 0;
	
	void Serialize(Stream& s) {
		VER(1);
		FOR_VER(0) {
			s	% nutr_no
				% value
				% per_kg;
		}
		FOR_VER(1) {s % group;}
	}
	double GetValue(double weight) const {
		if (per_kg)
			return value * weight;
		else
			return value;
	}
};

struct Removed__LocalProduct : Moveable<Removed__LocalProduct> {
	int food_no = 0;
	int grams = 0;
	int serving_grams = 0;
	String title;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			s	% food_no
				% grams
				% serving_grams
				% title;
		}
	}
};

extern int KCAL, PROT, FAT, CARB, SODIUM, PUFA;

struct Database {
	VectorMap<String, FoodGroup> food_groups;
	VectorMap<String, FoodDescription> food_descriptions;
	Vector<NutritionType> nutr_types;
	Vector<NutritionRecommendation> nutr_recom;
	Index<int> used_foods;
	bool is_init = false;
	
	VectorMap<int, Removed__LocalProduct> removed0;
	
	typedef Database CLASSNAME;
	Database();
	~Database() {StoreThis();}
	
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			s	% food_groups
				% food_descriptions
				% removed0
				% nutr_types
				% nutr_recom
				% used_foods
				% is_init;
		}
	}
	void LoadThis() {LoadFromFile(*this, ConfigFile("latest.db"));}
	void StoreThis() {StoreToFile(*this, ConfigFile("latest.db"));}
	bool Init();
	FoodDescription& AddFood(String fg, String l, String s, String c, String m, String su, String r, int re, String sci, double nf, double pf, double ff, double cf);
	int AddNutrition(String name, String unit, String desc);
	void StartStoreThis() {Thread::Start(THISBACK(StoreThis));}
	int FindFood(String long_desc) const;
	int FindFoodLeft(String long_desc) const;
	int FindNutrition(String key) const;
	int FindNutritionRecommendation(String key) const;
	void AddRecommendation(String nutr, double value, bool per_kg, int group) {auto& n = nutr_recom.Add(); n.nutr_no = FindNutrition(nutr); n.value = value; ASSERT(n.nutr_no >= 0); n.per_kg = per_kg; n.group = group;}
	void SetCommonNutrs();
	void RemoveDuplicates();
	void GetBestMacroFoods();
	
	void VLCD_Preset();
};

inline Database& GetDatabase() {return Single<Database>();}
inline Database& DB() {return GetDatabase();}

#endif
