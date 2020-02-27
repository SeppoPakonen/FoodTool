#ifndef _FoodTool_Profile_h_
#define _FoodTool_Profile_h_


inline void RealizeImageFolder() {RealizeDirectory(ConfigFile("images"));}
inline String GetImageFile(String file) {return AppendFileName(ConfigFile("images"), file);}
inline int GetMonthDays() {return 30;}
inline int GetQuarterDays() {return 91;}


struct IntakeExceptions : Moveable<IntakeExceptions> {
	String reason;
	Date begin, end;
	int calorie_deficit;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			s
				%  reason
				%  begin
				%  end
				%  calorie_deficit
				;
		}
	}
};

struct Note : Moveable<Note> {
	String title, content;
	Time added;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			s
				%  title
				%  content
				%  added
				;
		}
	}
};

struct ProgramUsageStat : Moveable<ProgramUsageStat> {
	Time begin, end;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			s
				%  begin
				%  end
				;
		}
	}
};

enum {
	WORST = 0, LOWEST= 0,
	BAD = 1, LOW = 1,
	NORMAL = 2,
	GOOD = 3, HIGH = 3,
	BEST = 4, HIGHEST = 4,
};

struct WeightLossStat : Moveable<WeightLossStat> {
	Time added;
	float weight, fat, liquid, muscle, bmi, prog;
	bool is_dexa;
	
	byte sentiment = NORMAL, health = NORMAL, workload = NORMAL;
	float walking = 0, walking_mins = 0, excess = 0;
	float neck = 0, bicep = 0, forearm = 0, chest = 0, waist = 0, buttocks = 0, thigh = 0, leg = 0;
	
	void Serialize(Stream& s) {
		VER(4);
		FOR_VER(0) {
			s	% added
				% weight % fat % liquid % muscle % bmi % prog
				% is_dexa;
		}
		FOR_VER(1) {s	% sentiment % health % workload % walking;}
		FOR_VER(2) {s	% excess;}
		FOR_VER(3) {s	% neck % bicep % forearm % chest % waist % buttocks % thigh % leg;}
		FOR_VER(4) {s	% walking_mins;}
	}
	String GetGenericJpg() const {return Format("%d_%d_%d_%d_%d_%d.jpg", (int)added.year, (int)added.month, (int)added.day, (int)added.hour, (int)added.minute, (int)added.second);}
	String GetFrontFile() const {return GetImageFile("front_" + GetGenericJpg());}
	String GetRightFile() const {return GetImageFile("right_" + GetGenericJpg());}
	String GetBackFile() const {return GetImageFile("back_" + GetGenericJpg());}
	double GetLiquidKg() const {return weight * liquid * 0.01;}
};

struct Configuration : Moveable<Configuration> {
	Time added;
	Date end_date;
	double tgt_walking_dist, tgt_jogging_dist, walking_dist;
	double hours_between_meals, hours_between_making_meals = 8;
	int easy_day_interval;
	int waking_hour, waking_minute;
	int sleeping_hour, sleeping_minute;
	int height, age, removed0, activity;
	int removed1;
	int shop_interval;
	int daily_coffee = 0;
	
	void Serialize(Stream& s) {
		VER(2);
		FOR_VER(0) {
			s
				% added
				% end_date
				% tgt_walking_dist
				% tgt_jogging_dist
				% walking_dist
				% hours_between_meals
				% easy_day_interval
				% waking_hour
				% waking_minute
				% sleeping_hour
				% sleeping_minute
				% height
				% age
				% removed0
				% activity
				% removed1
				% shop_interval
				;
		}
		FOR_VER(1) {s % daily_coffee;}
		FOR_VER(2) {s % hours_between_making_meals;}
	}
	
	double GetBMR(double weight);
	double GetTDEE();
	double GetCaloriesMaintainWeight(double weight);
	double GetCaloriesWalking(double weight_kg, double speed, double hours);
	double GetCaloriesWalkingDist(double weight_kg, double distance_km, double hours);
	double GetCaloriesWalkingDistSpeed(double weight_kg, double distance_km, double speed);
	double GetCaloriesJogging(double weight_kg, double speed, double hours);
	double GetCaloriesJoggingDistSpeed(double weight_kg, double distance_km, double speed);

};

struct ScheduleToday {
	enum {
		WAKING,
		EATING,
		WALKING,
		RUNNING,
		SLEEPING,
		MUSCLETRAINING,
	};
	
	struct Item : Moveable<Item> {
		Time time;
		int type;
		String msg;
		bool done = false;
	};
	
	Array<Item> items;
	Date day;
	
	bool operator()(const Item& a, const Item& b) const {
		if (a.time == b.time)
			return a.type < b.type;
		else
			return a.time < b.time;
	}
};

int GetTargetWeight(double height_m);
int GetBmiWeight(double height_m, int bmi);
double GetBMI(double height_m, double weight_kg);
double GetNormalLiquidPercentage(double age, double height_cm, double weight_kg);

enum {
	SNAPSRC_USER,
	SNAPSRC_FOODLOG,
	SNAPSRC_SHOPLOG,
	SNAPSRC_RECEIPTLOG,
};

String GetSnapshotSourceString(int i);

struct FoodStorageSnapshot : Moveable<FoodStorageSnapshot> {
	Time time;
	FoodQuantityInt foods;
	byte adder = 0;
	
	Date removed0;
	
	FoodStorageSnapshot() {removed0.year = 0;}
	FoodStorageSnapshot(const FoodStorageSnapshot& s) {*this = s;}
	void operator=(const FoodStorageSnapshot& s) {
		time = s.time;
		foods <<= s.foods;
		adder = s.adder;
	}
	void Serialize(Stream& s) {
		VER(1);
		FOR_VER(0) {s % removed0 % foods;}
		FOR_VER(1) {s % time % adder;}
		if (s.IsLoading() && removed0.year != 0) time = Time(removed0.year, removed0.month, removed0.day, 0,0,0);
	}
	void GetNutritions(Ingredient& ing) const;
};

struct NutritionSupplement : Moveable<NutritionSupplement> {
	Index<int> used_food;
	bool is_weightloss = false, is_maintenance = false;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % used_food % is_weightloss % is_maintenance;}
	}
};

struct FoodPriceQuote : Moveable<FoodPriceQuote> {
	Time time;
	double price = 0;
	double grams = 0;
	int servings = 0, serving_batch = 0;
	String shop;
	
	FoodPriceQuote() {}
	FoodPriceQuote(const FoodPriceQuote& s) {*this = s;}
	void operator=(const FoodPriceQuote& s) {
		time = s.time;
		price = s.price;
		grams = s.grams;
		servings = s.servings;
		serving_batch = s.serving_batch;
		shop = s.shop;
	}
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % time % price % grams % servings % serving_batch % shop;}
	}
	String GetMassString() const;
	String GetPriceString() const;
	void Set(Time time, double grams, const FoodPriceQuote& prev);
	void SetPriceless(Time time, double grams);
};

struct FoodPriceHistory : Moveable<FoodPriceHistory> {
	VectorMap<int, Vector<FoodPriceQuote>> history;
	Time time;
	
	VectorMap<String, Vector<FoodPriceQuote>> removed0;
	
	void Serialize(Stream& s) {
		VER(2);
		FOR_VER(0) {s % removed0;}
		FOR_VER(1) {s % time;}
		FOR_VER(2) {s % history;}
		removed0.Clear();
	}
};

struct FoodPrice : Moveable<FoodPrice> {
	VectorMap<int, FoodPriceQuote> values;
	Time time;
	
	VectorMap<String, FoodPriceQuote> removed0;
	
	FoodPrice() {}
	FoodPrice(const FoodPrice& p) {*this = p;}
	void operator=(const FoodPrice& p) {
		values <<= p.values;
		time = p.time;
	}
	void Serialize(Stream& s) {
		VER(2);
		FOR_VER(0) {s % removed0;}
		FOR_VER(1) {s % time;}
		FOR_VER(2) {s % values;}
		removed0.Clear();
	}
};

struct ProductQueueHistory {
	Vector<FoodPrice> queue, history;
	
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % queue % history;}
	}
};

struct Profile {
	Vector<IntakeExceptions> exceptions;
	Vector<Note> notes;
	Vector<ProgramUsageStat> usage;
	Vector<WeightLossStat> weights;
	Vector<DailyPlan> planned_daily;
	Vector<Configuration> confs;
	Vector<NutrientDeficitProfile> defs;
	Vector<MealPreset> presets;
	Vector<FoodStorageSnapshot> storage_snaps;
	Vector<NutritionSupplement> supplements;
	Index<int> planned_nutrients;
	ProductQueueHistory foodlog, shoplog, receiptlog;
	
	FoodPriceHistory price;
	FoodStorage storage;
	Date begin_date;
	double av_calorie_deficit;
	int version = 0;
	bool is_male;
	bool is_initialised = false;
	
	
	VectorMap<String, int> removed0;
	Index<int> removed1;
	Ingredient removed2;
	
	Time tmp_usage_start;
	RunningFlag flag;
	
	
	typedef Profile CLASSNAME;
	Profile();
	~Profile() {
		Stop();
		ProgramUsageStat& u = usage.Add();
		u.begin = tmp_usage_start;
		u.end = GetSysTime();
		StoreThis();
	}
	void Serialize(Stream& s) {
		VER(6);
		FOR_VER(0) {
			s
				% exceptions
				% notes
				% usage
				% weights
				% planned_daily
				% confs
				% defs
				% presets
				% planned_nutrients
				% storage
				% begin_date
				% av_calorie_deficit
				% version
				% is_male
				% is_initialised
				;
		}
		FOR_VER(1) {s % storage_snaps;}
		FOR_VER(2) {s % removed0;}
		FOR_VER(3) {s % removed1 % removed2;}
		FOR_VER(4) {s % supplements;}
		FOR_VER(5) {s % price;}
		FOR_VER(6) {s % foodlog % shoplog % receiptlog;}
	}
	void MakeTodaySchedule(ScheduleToday& s);
	void AddWeightStat(int kgs);
	Date GetCurrentTotalBegin();
	Date GetCurrentTotalEnd();
	Date GetCurrentWeekBegin();
	Date GetCurrentMonthBegin();
	Date GetCurrentQuarterBegin();
	bool UpdatePlan();
	void Start(bool replan) {flag.Start(1); Thread::Start(THISBACK1(ProcessUpdate, replan));}
	void Stop() {flag.Stop();}
	void ProcessUpdate(bool replan);
	bool IsRunning() {return flag.running || flag.workers_running > 0;}
	void VLCD_Preset();
	void CookedToRaw();
	int FindMealPreset(String key) const;
	DailyPlan* GetTodayPlan();
	void SetMealPresetFoodsUsed();
	
	void LoadThis();
	void StoreThis();
	
	Callback WhenUpdateReady;
};

inline Profile& GetProfile() {return Single<Profile>();}



#endif
