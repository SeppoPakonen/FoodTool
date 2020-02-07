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
	int keto_cals = 0, keto_fats = 0, keto_carbs = 0, keto_prots = 0;
	int wl_start = 0, wl_loss = 0, wl_months = 0, wl_cals = 0;
	Time begin_time, end_time;
	
	Vector<IntakeExceptions> exceptions;
	Vector<Note> notes;
	Vector<ProgramUsageStat> usage;
	Vector<WeightLossStat> weight;
	
	FoodStorage storage;
	
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
	
	void LoadThis();
	void StoreThis();
	void Serialize(Stream& s) {
		s
			% keto_cals
			% keto_fats
			% keto_carbs
			% keto_prots
			% wl_start
			% wl_loss
			% wl_months
			% wl_cals
			% begin_time
			% end_time
			% exceptions
			% notes
			% usage
			% weight
			% storage
			% is_initialised
			;
	}
};

inline Profile& GetProfile() {return Single<Profile>();}



class ProfileCreator : public WithProfileCreatorLayout<TopWindow> {
	
	
public:
	typedef ProfileCreator CLASSNAME;
	ProfileCreator();
	
	void OpenCalculator1() {LaunchWebBrowser("https://www.ruled.me/keto-calculator");}
	void OpenCalculator2() {LaunchWebBrowser("https://www.calculators.org/health/weight-loss.php");}
	void Begin();
};

#endif
