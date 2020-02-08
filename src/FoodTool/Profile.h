#ifndef _FoodTool_Profile_h_
#define _FoodTool_Profile_h_


struct IntakeExceptions : Moveable<IntakeExceptions> {
	String reason;
	Date begin, end;
	int cals;
	
	void Serialize(Stream& s) {
		s
			%  reason
			%  begin
			%  end
			%  cals
			;
	}
};

struct Note : Moveable<Note> {
	String msg;
	Time added;
	
	void Serialize(Stream& s) {
		s
			%  msg
			%  added
			;
	}
};

struct ProgramUsageStat : Moveable<ProgramUsageStat> {
	Time begin, end;
	
	void Serialize(Stream& s) {
		s
			%  begin
			%  end
			;
	}
};

struct WeightLossStat : Moveable<WeightLossStat> {
	Date time;
	int weight;
	String jpg_file;
	
	void Serialize(Stream& s) {
		s
			%  time
			%  weight
			%  jpg_file
			;
	}
};

inline void RealizeImageFolder() {RealizeDirectory(ConfigFile("images"));}
inline String GetImageFile(String file) {return AppendFileName(ConfigFile("images"), file);}
inline int GetWeekSeconds() {return 7 * 24 * 60 * 60;}
inline int GetMonthSeconds() {return 2629743;}

struct Profile {
	Vector<IntakeExceptions> exceptions;
	Vector<Note> notes;
	Vector<ProgramUsageStat> usage;
	Vector<WeightLossStat> weight;
	Vector<DailyPlan> planned_daily;
	FoodStorage storage;
	Time begin_time, end_time;
	double calory_deficit;
	double tgt_walking_dist, tgt_jogging_dist, walking_dist;
	int height, begin_weight, age, bodyfat, activity;
	int tgt_weight, months;
	bool is_male;
	bool is_initialised = false;
	
	
	Time tmp_usage_start;
	
	
	Profile();
	~Profile() {
		ProgramUsageStat& u = usage.Add();
		u.begin = tmp_usage_start;
		u.end = GetSysTime();
		StoreThis();
	}
	void AddWeightStat(int kgs, String jpg_file_path);
	Time GetCurrentTotalBegin();
	Time GetCurrentTotalEnd();
	Time GetCurrentWeekBegin();
	Time GetCurrentMonthBegin();
	Time GetCurrentQuarterBegin();
	double GetBMR(double weight);
	double GetTDEE();
	double GetCaloriesMaintainWeight(double weight);
	double GetCaloriesWalking(double weight_kg, double speed, double hours);
	double GetCaloriesWalkingDist(double weight_kg, double distance_km, double hours);
	double GetCaloriesWalkingDistSpeed(double weight_kg, double distance_km, double speed);
	double GetCaloriesJogging(double weight_kg, double speed, double hours);
	double GetCaloriesJoggingDistSpeed(double weight_kg, double distance_km, double speed);
	bool UpdatePlan(int min_days);
	
	void LoadThis();
	void StoreThis();
	void Serialize(Stream& s) {
		s
			% exceptions
			% notes
			% usage
			% weight
			% planned_daily
			% storage
			% begin_time
			% end_time
			% calory_deficit
			% tgt_walking_dist
			% tgt_jogging_dist
			% walking_dist
			% height
			% begin_weight
			% age
			% bodyfat
			% activity
			% tgt_weight
			% months
			% is_male
			
			% is_initialised
			;
	}
};

inline Profile& GetProfile() {return Single<Profile>();}



class ProfileCreator : public WithProfileCreatorLayout<TopWindow> {
	VectorMap<int, double> height_bmis;
	
public:
	typedef ProfileCreator CLASSNAME;
	ProfileCreator();
	
	void Next();
	void UpdateTargetWeight();
};

#endif
