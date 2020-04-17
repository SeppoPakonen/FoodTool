#ifndef _FoodTool_Profile_h_
#define _FoodTool_Profile_h_

#define DEFAULT_FAT_PERC(is_male) (is_male ? 6 * 0.01 : 21 * 0.01)

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
	double weight, fat, liquid, muscle, bmi, prog;
	bool is_dexa;
	
	byte sentiment = NORMAL, health = NORMAL, workload = NORMAL;
	double walking = 0, walking_mins = 0, excess = 0;
	double neck = 0, bicep = 0, forearm = 0, chest = 0, waist = 0, buttocks = 0, thigh = 0, leg = 0;
	double user_defined0 = 0;
	
	void Serialize(Stream& s) {
		VER(5);
		FOR_VER(0) {
			s	% added
				% weight % fat % liquid % muscle % bmi % prog
				% is_dexa;
		}
		FOR_VER(1) {s	% sentiment % health % workload % walking;}
		FOR_VER(2) {s	% excess;}
		FOR_VER(3) {s	% neck % bicep % forearm % chest % waist % buttocks % thigh % leg;}
		FOR_VER(4) {s	% walking_mins;}
		FOR_VER(5) {s	% user_defined0;}
	}
	String GetGenericJpg() const {return Format("%d_%d_%d_%d_%d_%d.jpg", (int)added.year, (int)added.month, (int)added.day, (int)added.hour, (int)added.minute, (int)added.second);}
	String GetFrontFile() const {return GetImageFile("front_" + GetGenericJpg());}
	String GetRightFile() const {return GetImageFile("right_" + GetGenericJpg());}
	String GetBackFile() const {return GetImageFile("back_" + GetGenericJpg());}
	double GetLiquidKg() const {return weight * liquid * 0.01;}
	double GetInternalPerc() const {return 100 - fat - muscle;}
	double GetInternalKg() const {return weight * (100 - fat - muscle) * 0.01;}
	double GetFatKg() const {return weight * fat * 0.01;}
	double GetMuscleKg() const {return weight * muscle * 0.01;}
};

struct Configuration : Moveable<Configuration> {
	Time added;
	Date end_date;
	double tgt_exercise_kcal = 0, tgt_walking_dist, tgt_jogging_dist, walking_dist;
	double hours_between_meals, hours_between_making_meals = 8;
	int tgt_exercise_count = 0, tgt_exercise_min = 0;
	int easy_day_interval;
	int waking_hour, waking_minute;
	int sleeping_hour, sleeping_minute;
	int height, age, removed0, activity;
	int removed1;
	int shop_interval;
	int daily_coffee = 0;
	
	void Serialize(Stream& s) {
		VER(4);
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
		FOR_VER(3) {s % tgt_exercise_kcal;}
		FOR_VER(4) {s % tgt_exercise_count % tgt_exercise_min;}
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

int GetTargetWeight(double height_cm);
int GetBmiWeight(double height_cm, int bmi);
double GetBMI(double height_cm, double weight_kg);
double GetNormalLiquidPercentage(double age, double height_cm, double weight_kg);
double GetHeartrateCalories(bool is_male, double weight, double age, double heartrate_bpm, double duration_s);
	
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

struct PlanState {
	bool is_male;
	double tgt_fat_perc;
	
	double cals_in_kg_fat;
	double exercise_kcal;
	double walking_dist;
	double jogging_dist;
	double jogging_speed;
	double tgt_weight;
	double max_lean_gain;
	double worst_weight;
	double worst_fat_perc;
	double worst_lean_perc;
	
	double internal_perc;
	double tgt_lean_perc;
	double internal_kgs;
	double walking_speed;
	double weight;
	double fat_perc;
	double lean_perc;
	
	OnlineAverage lean_fat_loss_ratio_av;
	double calorie_deficit_sum = 0.0;
	double max_calorie_deficit = 0.0;
	
	
	
	PlanState(bool is_male, Configuration& conf, WeightLossStat& wl);
	bool IsReadyForMaintenance() const;
	double GetProgress() const;
	bool IsTooHighFatPercentage() const {return fat_perc > tgt_fat_perc + 0.01;}
	bool IsHighFatPercentage() const {return fat_perc > tgt_fat_perc;}
	bool IsTooLowWeight() const {return weight < tgt_weight - 1;}
	bool IsLowWeight() const {return weight < tgt_weight;}
	void Set(Configuration& conf);
	void Set(WeightLossStat& wl);
	void SetLeanAndFatMass(double new_lean_kgs, double new_fat_kgs);
	void CheckWorst();
	void AddCalorieDeficit(double calorie_deficit);
};

struct Dummy__Removed {
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {}
	}
};

enum {
	ACT_UNKNOWN,
	ACT_INTERVAL,
	ACT_EXERCISE,
	
	ACT_COUNT,
};

struct ActivityItem : Moveable<ActivityItem> {
	byte type = 0;
	Time begin, end;
	String what, msg;
	double kcal = 0;
	double heartrate = 0;
	bool real_values = false;
	
	int removed0 = 0;
	int removed1 = 0;
	
	void Serialize(Stream& s) {
		VER(3);
		FOR_VER(0) {s % type % begin % end % what % msg % removed0 % removed1;}
		FOR_VER(1) {s % real_values;}
		FOR_VER(2) {s % kcal;}
		FOR_VER(3) {s % heartrate;}
	}
	String GetTypeString() const;
	int GetSeconds() const {return end.Get() - begin.Get();}
	String ToString() const {return GetTypeString() + ": " + what + " " + IntStr(GetSeconds()) + " seconds";}
};

struct ActivityGroupItem : Moveable<ActivityGroupItem> {
	Vector<ActivityItem> items;
	ActivityItem main;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % items % main;}
	}
};

struct ExerciseType : Moveable<ExerciseType> {
	Time added;
	String name, instructions, primary_trained;
	OnlineAverage av_kcal, av_heartrate;
	VectorMap<String, byte> muscle_areas, muscles;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % added % name % instructions % primary_trained % av_kcal % av_heartrate % muscle_areas % muscles;}
	}
};

struct Profile {
	Vector<IntakeExceptions> exceptions;
	Vector<Note> notes;
	Vector<ProgramUsageStat> usage;
	Vector<WeightLossStat> weights;
	Vector<DailyPlan> planned_daily;
	Vector<Configuration> confs;
	Vector<HairSampleDeficitProfile> defs;
	Vector<MealPreset> presets;
	Vector<FoodStorageSnapshot> storage_snaps;
	Vector<NutritionSupplement> supplements;
	Vector<ActivityGroupItem> activity, planned_exercises;
	Vector<ExerciseType> exercises;
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
	Dummy__Removed removed3;
	
	Time tmp_usage_start;
	RunningFlag flag;
	static const int maintenance_protein_factor = 8; // 0.8g per weight kg
	static const int muscle_gain_protein_factor = 10;
	static const int fasting_protein_factor = 1;
	static const int easy_day_calories = 1500;
	
	
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
		VER(8);
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
		FOR_VER(7) {s % removed3;}
		FOR_VER(8) {s % activity % planned_exercises % exercises;}
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
	void RefreshAllVariants();
	int FindMealPreset(String key) const;
	int FindExercise(String key) const;
	DailyPlan* GetTodayPlan();
	void SetMealPresetFoodsUsed();
	
	void LoadThis();
	void StoreThis();
	
	Callback WhenUpdateReady;
};

inline Profile& GetProfile() {return Single<Profile>();}



#endif
