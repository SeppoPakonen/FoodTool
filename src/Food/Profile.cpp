#include "Food.h"



Profile::Profile() {
	LoadThis();
	
	//if (weights.GetCount() && weights[0].fat == 0.0) weights.Remove(0);
	//weights[7].added = Time(2020,2,17,20,0,0);
	if (0) {
		#define FIX(x) \
		{ \
			double value = 0.0; \
			int i = 0;\
			for(; i < weights.GetCount(); i++) \
				if ((value = weights[i].x) > 0.0) \
					break;\
			for(; i >= 0; i--) \
				weights[i].x = value; \
		}
		FIX(neck);
		FIX(bicep);
		FIX(forearm);
		FIX(chest);
		FIX(waist);
		FIX(buttocks);
		FIX(thigh);
		FIX(leg);
	}
	
	tmp_usage_start = GetSysTime();
	
	RefreshAllVariants();
	//CookedToRaw();
	SetMealPresetFoodsUsed();
	
	if (is_initialised) {
		if (storage.days.IsEmpty())
			storage.Init(begin_date);
		Start(false);
	}
}

void Profile::ProcessUpdate(bool replan) {
	TimeStop ts;
	
	//planned_daily.Clear();
	if (planned_daily.IsEmpty()) replan = true;
	
	int prev_days = 0;
	if (planned_daily.GetCount())
		prev_days = planned_daily.Top().date.Get() - begin_date.Get();
	
	if (replan)
		UpdatePlan();
	
	storage.Update(replan, planned_daily);
	version++;
	
	StoreThis();
	
	int days = planned_daily.Top().date.Get() - begin_date.Get();
	int days_diff = days - prev_days;
	
	LOG("Calory-deficit: " << (av_calorie_deficit * 100) << "\%");
	LOG("Started " << Format("%", begin_date));
	LOG("Stopping " << Format("%", planned_daily.Top().date));
	//LOG("Days total " << days << (days_diff ? "(Diff " + (days_diff < 0 ? "-" : "+") + IntStr(days_diff) + ")" : "");
	LOG(Format("Days total %d (Diff %+d)", days, days_diff));
	
	WhenUpdateReady();
	
	flag.SetNotRunning();
	flag.IncreaseStopped();
}

void Profile::AddWeightStat(int kgs) {
	WeightLossStat& wl = weights.Add();
	wl.weight = kgs;
	wl.added = GetSysTime();
	wl.bmi = 0;
	wl.fat = 0;
	wl.liquid = 0;
	wl.muscle = 0;
	wl.is_dexa = 0;
	StoreThis();
}

Date Profile::GetCurrentTotalBegin() {
	return begin_date;
}

Date Profile::GetCurrentTotalEnd() {
	return planned_daily.Top().date + 1;
}

Date Profile::GetCurrentWeekBegin() {
	Date t = begin_date;
	Date now = GetSysTime();
	Date prev = t;
	while (1) {
		t += 7;
		if (t > now)
			break;
		prev = t;
	}
	return prev;
}

Date Profile::GetCurrentMonthBegin() {
	Date t = begin_date;
	Date now = GetSysTime();
	Date prev = t;
	while (1) {
		t += GetMonthDays();
		if (t > now)
			break;
		prev = t;
	}
	return prev;
}

Date Profile::GetCurrentQuarterBegin() {
	Date t = begin_date;
	Date now = GetSysTime();
	Date prev = t;
	while (1) {
		t += GetQuarterDays();
		if (t > now)
			break;
		prev = t;
	}
	return prev;
}

bool Profile::UpdatePlan() {
	const Database& db = DB();
	
	Configuration* conf = &confs[0];
	Configuration* next_conf = 1 < confs.GetCount() ? &confs[1] : NULL;
	int conf_i = 0;
	
	WeightLossStat* wl = &weights[0];
	WeightLossStat* next_wl = 1 < weights.GetCount() ? &weights[1] : NULL;
	int wl_i = 0;
	
	HairSampleDeficitProfile* def = defs.GetCount() ? &defs.Top() : NULL;
	
	PlanState s(is_male, *conf, *wl);
	
	if (planned_nutrients.IsEmpty()) {
		planned_nutrients.FindAdd(KCAL);
		planned_nutrients.FindAdd(PROT);
		planned_nutrients.FindAdd(FAT);
		for(int i = 0; i < db.nutr_recom.GetCount(); i++)
			planned_nutrients.FindAdd(db.nutr_recom[i].nutr_no);
	}
	
	Vector<int> def_nutr;
	def_nutr.SetCount(HSAMP_COUNT, -1);
	def_nutr[HSAMP_CALCIUM] = db.FindNutrition("CA");
	def_nutr[HSAMP_MAGNESIUM] = db.FindNutrition("MG");
	def_nutr[HSAMP_POTASSIUM] = db.FindNutrition("K");
	def_nutr[HSAMP_ZINC] = db.FindNutrition("ZN");
	def_nutr[HSAMP_MANGANESE] = db.FindNutrition("MN");
	def_nutr[HSAMP_PHOSPHORUS] = db.FindNutrition("P");
	def_nutr[HSAMP_SELENIUM] = db.FindNutrition("SE");
	def_nutr[HSAMP_IRON] = db.FindNutrition("FE");
	
	Date date = begin_date;
	planned_daily.SetCount(0);
	planned_daily.Reserve(2*365);
	
	Date end_date = GetSysTime();
	end_date += 30;
	
	int easy_day_counter = 0;
	
	int count = 0;
	int maintenance_day_count = 0;
	while (!s.IsReadyForMaintenance() || date < end_date || maintenance_day_count < 30) {
		double prog = s.GetProgress();
		//if (planned_daily.GetCount() && planned_daily.Top().prog > prog) break;
		
		if (count > 2*365 ||
			s.weight < 40 || s.weight > 400 ||
			s.fat_perc < 0.04 || s.fat_perc > 0.80 ||
			s.lean_perc < 0.1 || s.lean_perc > 0.80) {
			double weight_diff = s.weight - s.tgt_weight;
			double fat_perc_diff = s.fat_perc - s.tgt_fat_perc;
			double lean_perc_diff = s.lean_perc - s.tgt_lean_perc;
			LOG("Planning failed");
			DUMP(weight_diff);
			DUMP(fat_perc_diff);
			DUMP(lean_perc_diff);
			break;
		}
		count++;
		
		DailyPlan& d = planned_daily.Add();
		
		while (next_conf && date >= Date(next_conf->added)) {
			conf = next_conf;
			s.Set(*conf);
			conf_i++;
			next_conf = (conf_i+1) < confs.GetCount() ? &confs[conf_i+1] : NULL;
		}
		
		
		double fat_kgs = s.weight * s.fat_perc;
		double lean_kgs = s.weight * s.lean_perc;

		
		d.date = date++;
		d.weight = s.weight;
		d.prog = prog;
		d.fat_perc = s.fat_perc;
		d.fat_kgs = fat_kgs;
		d.lean_kgs = lean_kgs;
		
		d.maintain_calories = conf->GetCaloriesMaintainWeight(s.weight);
		
		// Coffee
		double coffee_g = conf->daily_coffee;
		double caffeine_mg = 40 * (coffee_g / 100.0);
		double rmr_increase_100mg_caffeine = 0.04;
		double increase_length = 150.0 / (24*60);
		double rmr_increase = caffeine_mg / 100.0 * rmr_increase_100mg_caffeine;
		double coffee_maintain_calorie_increase = rmr_increase * d.maintain_calories;
		ASSERT(coffee_maintain_calorie_increase >= 0);
		d.maintain_calories += coffee_maintain_calorie_increase;
		
		
		// Set protein levels, generic mode and food variant
		double maintain_protein = s.weight * (maintenance_protein_factor * 0.1);
		int protein_day_ival = 2;
		if (s.IsTooHighFatPercentage()) {
			d.mode = MODE_WEIGHTLOSS;
			d.variant_type = VARIANT_WEIGHTLOSS;
			protein_day_ival = 5;
			maintenance_day_count = 0;
		}
		/*else if (s.IsTooLowWeight()) {
			d.mode = MODE_MUSCLEGAIN;
			d.variant_type = VARIANT_MUSCLEGAIN;
			maintenance_day_count = 0;
		}
		else if (s.IsLowWeight()) {
			d.mode = MODE_MAINTAIN;
			d.variant_type = VARIANT_MAINTENANCE;
			maintenance_day_count++;
		}*/
		else {
			d.mode = MODE_MAINTAIN;
			d.variant_type = VARIANT_FATTYACIDS;
			maintenance_day_count++;
		}
		
		double min_protein, max_protein;
		if (!protein_day_ival || count % protein_day_ival == 0) {
			min_protein = maintain_protein;
			max_protein = maintain_protein * 2;
		}
		else {
			min_protein = s.weight * (fasting_protein_factor * 0.1);
			max_protein = min_protein * 1.7;
		}

		
		// Set calories and macros
		double min_fat = 10.0 / 2000.0 * d.maintain_calories;
		double prot_cals, fat_cals, carb_cals;
		prot_cals = min_protein * 4.4;
		if (s.IsTooHighFatPercentage()) {
			fat_cals = min_fat * 9.0;
			carb_cals = (prot_cals + fat_cals) / 0.95 * 0.05;
			
			d.allowed_calories = carb_cals + prot_cals + fat_cals;
			
			// We aren't going under minimum daily calories
			if (d.allowed_calories < MINIMUM_DAILY_KCAL) {
				double mul = (MINIMUM_DAILY_KCAL - prot_cals) / (carb_cals + fat_cals);
				d.allowed_calories = MINIMUM_DAILY_KCAL;
				fat_cals *= mul;
				carb_cals *= mul;
			}
			
			
			d.is_easy_day = false;
			if (easy_day_counter == conf->easy_day_interval-1 && d.allowed_calories < 1500.0) {
				d.is_easy_day = true;
				double mul = (1500.0 - prot_cals) / (carb_cals + fat_cals);
				d.allowed_calories = 1500.0;
				fat_cals *= mul;
				carb_cals *= mul;
			}
			easy_day_counter = (easy_day_counter + 1) % conf->easy_day_interval;
		}
		else if (s.IsHighFatPercentage()) {
			d.allowed_calories = 0.9 * d.maintain_calories;
			fat_cals = (d.allowed_calories - prot_cals) * 0.33;
			carb_cals = (d.allowed_calories - prot_cals) * 0.67;
			d.is_easy_day = false;
		}
		else {
			d.allowed_calories = 1.1 * d.maintain_calories;
			fat_cals = (d.allowed_calories - prot_cals) * 0.33;
			carb_cals = (d.allowed_calories - prot_cals) * 0.67;
			d.is_easy_day = false;
		}
		
		if (1) {
			String dbg_str;
			dbg_str << planned_daily.GetCount()-1 << ":\t";
			
			if (s.IsTooHighFatPercentage()) dbg_str << "++F";
			else if (s.IsHighFatPercentage()) dbg_str << "+F";
			else dbg_str << "-F";
			
			if (s.IsTooLowWeight()) dbg_str << "--W";
			else if (s.IsLowWeight()) dbg_str << "-W";
			else dbg_str << "+W";
			
			dbg_str << " min_prot: " << (min_protein / s.weight) << ", cals: " << (d.allowed_calories / d.maintain_calories);
			LOG(dbg_str);
		}
		
		double calorie_deficit = max(0.0, 1.0 - (d.allowed_calories / d.maintain_calories));
		s.AddCalorieDeficit(calorie_deficit);
		
		d.maintain_burned_calories = d.maintain_calories * calorie_deficit;
		
		d.walking_burned_calories = 0;
		d.jogging_burned_calories = 0;
		d.strength_burned_calories = 0;
		
		if (s.walking_dist && d.mode != MODE_WEIGHTLOSS)
			d.walking_burned_calories = conf->GetCaloriesWalkingDistSpeed(s.weight, s.walking_dist, s.walking_speed);
		
		if (s.jogging_dist && d.mode != MODE_WEIGHTLOSS)
			d.jogging_burned_calories = conf->GetCaloriesJoggingDistSpeed(s.weight, s.jogging_dist, s.jogging_speed);
		
		if (d.mode == MODE_WEIGHTLOSS || d.mode == MODE_MUSCLEGAIN || planned_daily.GetCount() % 2)
			d.strength_burned_calories = s.exercise_kcal;
		
		if (d.is_easy_day) {
			d.mode = MODE_MAINTAIN;
			d.variant_type = VARIANT_SCORE;
			d.jogging_burned_calories = 0;
			d.strength_burned_calories = 0;
		}
		
		d.burned_calories = -d.allowed_calories + d.maintain_calories + d.walking_burned_calories + d.jogging_burned_calories + d.strength_burned_calories;
		
		double max_protein_gain = s.max_lean_gain * 0.26;
		double protein_gram_diff = (prot_cals / 4.4) - maintain_protein;
		double gained_protein_grams = max(0.0, min(max_protein_gain, +protein_gram_diff));
		double burned_protein_grams = max(0.0, -protein_gram_diff);
		double gained_lean_kgs  = gained_protein_grams / 26.0 * 0.1; // prot 26g/100g --> kgs
		double burned_lean_kgs  = burned_protein_grams / 26.0 * 0.1; // prot 26g/100g --> kgs
		// NOTE: muscle gain comes from negative burned_protein_grams, which is too simplified
		double released_lean_cals = burned_protein_grams / 26.0 * 250.0; // from beef... lol
		double released_protein_cals = max(0.0, (protein_gram_diff - max_protein_gain) / 4.4);
		
		double fat_cals_diff = -d.burned_calories + released_lean_cals + released_protein_cals;
		double burned_fat_cals = max(0.0, -fat_cals_diff);
		double gained_fat_cals = max(0.0, +fat_cals_diff);
		double burned_fat_kgs = burned_fat_cals / s.cals_in_kg_fat;
		double gained_fat_kgs = gained_fat_cals / s.cals_in_kg_fat;
		
		
		if (!s.IsHighFatPercentage() && burned_fat_cals > 0.0) {
			double new_allowed_calories = d.allowed_calories + burned_fat_cals;
			double diff = new_allowed_calories - d.allowed_calories;
			ASSERT(diff > 0);
			d.allowed_calories += diff;
			fat_cals += diff * 0.33;
			carb_cals += diff * 0.67;
			burned_fat_cals = 0;
			burned_fat_kgs = 0;
		}
		
		
		d.burned_kgs = burned_lean_kgs + burned_fat_kgs - gained_lean_kgs - gained_fat_kgs;
		
		d.food.Reset();
		for(const NutritionRecommendation& r : db.nutr_recom) {
			if (r.per_kg)
				d.food.nutr[r.nutr_no] = s.weight * r.value;
			else
				d.food.nutr[r.nutr_no] = r.value;
		}
		if (def) for(int i = 0; i < HSAMP_COUNT; i++) {
			int nutr_i = def_nutr[i];
			if (nutr_i >= 0) {
				double value = def->elements[i];
				if (value < 0.5) {
					double mul = 1 + (0.5 - value) * 2;
					d.food.nutr[nutr_i] *= mul;
				}
			}
		}
		double fat_grams = max(min_fat, fat_cals / 9.0);
		d.food.grams = s.weight / 100.0 * 2000.0;
		d.food.nutr[KCAL] = d.allowed_calories;
		d.food.nutr[PROT] = max(min_protein, min(max_protein, prot_cals / 4.4)); // based on protein powder nutrients
		d.food.nutr[FAT] = fat_grams; // based on coconut oil nutrients
		d.food.nutr[CARB] = carb_cals / 10.0;
		ASSERT(db.nutr_recom.GetCount());
		
		double new_lean_kgs = lean_kgs - burned_lean_kgs + gained_lean_kgs;
		double new_fat_kgs = fat_kgs - burned_fat_kgs + gained_fat_kgs;
		ASSERT(new_lean_kgs <= lean_kgs + s.max_lean_gain);
		s.SetLeanAndFatMass(new_lean_kgs, new_fat_kgs);
		
		while (next_wl && date >= Date(next_wl->added)) {
			wl = next_wl;
			s.Set(*wl);
			wl_i++;
			next_wl = (wl_i+1) < weights.GetCount() ? &weights[wl_i+1] : NULL;
		}
		
	}
	
	double lean_fat_loss_ratio = s.lean_fat_loss_ratio_av.GetMean();
	
	av_calorie_deficit = s.calorie_deficit_sum / planned_daily.GetCount();
	
	confs.Top().end_date = begin_date + planned_daily.GetCount();
	
	return true;
}





void Profile::LoadThis() {
	String file = ConfigFile("latest.bin");
	LoadFromFile(*this, file);
}

void Profile::StoreThis() {
	String backup_dir = ConfigFile("backups");
	RealizeDirectory(backup_dir);
	
	Date d = GetSysTime();
	String file = Format("backup_%d_%d_%d.bin", (int)d.year, (int)d.month, (int)d.day);
	String path = AppendFileName(backup_dir, file);
	StoreToFile(*this, path);
	
	path = ConfigFile("latest.bin");
	StoreToFile(*this, path);
}

void Profile::MakeTodaySchedule(ScheduleToday& s) {
	const Configuration& conf = confs.Top();
	Time now = GetSysTime();
	s.day = now;
	s.items.SetCount(0);
	
	int day_i = -1;
	for(int i = 0; i < storage.days.GetCount(); i++)
		if (storage.days[i].date == s.day)
			{day_i = i; break;}
	if (day_i < 0)
		return;
	const FoodDay& day = storage.days[day_i];
	const DailyPlan& plan = planned_daily[day_i];
	
	auto& wake = s.items.Add();
	wake.time = Time(s.day.year, s.day.month, s.day.day, conf.waking_hour, conf.waking_minute, 0);
	wake.type = ScheduleToday::WAKING;
	wake.msg = Format(t_("Good morning. Have %d calories today!"), (int)day.total_sum.nutr[KCAL]);
	
	auto& sleep = s.items.Add();
	sleep.time = Time(s.day.year, s.day.month, s.day.day, conf.sleeping_hour, conf.sleeping_minute, 0);
	sleep.type = ScheduleToday::SLEEPING;
	sleep.msg = t_("Good night!");
	if (sleep.time < wake.time)
		sleep.time += 24*60*60;
	
	for(int i = 0; i < day.meals.GetCount(); i++) {
		const Meal& m = day.meals[i];
		auto& meal = s.items.Add();
		meal.time = m.time;
		meal.type = ScheduleToday::EATING;
		int j = storage.meal_types.Find(m.key);
		if (j >= 0)
			meal.msg = storage.meal_types.Get(m.key).name;
		else
			meal.msg = Format(t_("%d grams"), m.grams);
	}
	
	int day_len = sleep.time.Get() - wake.time.Get();
	Time exercise_time = wake.time + day_len * 3 / 8;
	
	// Cardio decreases muscle more than strength training during weight loss diet
	if (conf.tgt_walking_dist > 0 && plan.mode != MODE_WEIGHTLOSS) {
		auto& walk = s.items.Add();
		walk.time = exercise_time;
		walk.type = ScheduleToday::WALKING;
		walk.msg = Format(t_("Go walk %2n km!"), conf.tgt_walking_dist);
	}
	
	if (conf.tgt_jogging_dist > 0 && plan.mode != MODE_WEIGHTLOSS) {
		auto& jogging = s.items.Add();
		jogging.time = exercise_time;
		jogging.type = ScheduleToday::RUNNING;
		jogging.msg = Format(t_("Go jogging %2n km!"), conf.tgt_jogging_dist);
	}
	
	if (plan.mode == MODE_WEIGHTLOSS || plan.mode == MODE_MUSCLEGAIN) {
		int seconds_per_exer = conf.tgt_exercise_min * 60 / conf.tgt_exercise_count;
		int between_exer_seconds = (sleep.time.Get() - wake.time.Get() - seconds_per_exer - 15*60) / (conf.tgt_exercise_count-1);
		Time t = wake.time + 15*60;
		for(int i = 0; i < conf.tgt_exercise_count; i++) {
			auto& jogging = s.items.Add();
			jogging.time = t;
			jogging.type = ScheduleToday::MUSCLETRAINING;
			jogging.msg = t_("Do muscle resistance exercises!");
			t += between_exer_seconds;
		}
	}
	
	for(auto& it : s.items)
		it.done = now > it.time;
	
	Sort(s.items, ScheduleToday());
}

void Profile::VLCD_Preset() {
	
	
	
	
}

void Profile::CookedToRaw() {
	Database& db = DB();
	
	for(int i = 0; i < presets.GetCount(); i++) {
		MealPreset& mp = presets[i];
		bool has_changes = false;
		
		for(MealIngredient& ing : mp.ingredients) {
			const FoodDescription& d = db.food_descriptions[ing.db_food_no];
			int a = d.long_desc.Find(" cooked");
			if (a >= 0) {
				String search_str = d.long_desc.Left(a) + " raw";
				int j = db.FindFoodLeft(search_str);
				if (j >= 0) {
					ing.db_food_no = j;
					has_changes = true;
				}
			}
		
		}
		
		if (has_changes)
			mp.MakeVariants();
	}
}

void Profile::RefreshAllVariants() {
	for(int i = 0; i < presets.GetCount(); i++) {
		MealPreset& mp = presets[i];
		mp.MakeVariants();
	}
	StoreThis();
}

int Profile::FindMealPreset(String key) const {
	for(int i = 0; i < presets.GetCount(); i++)
		if (presets[i].key == key)
			return i;
	return -1;
}

int Profile::FindExercise(String key) const {
	for(int i = 0; i < exercises.GetCount(); i++)
		if (exercises[i].name == key)
			return i;
	return -1;
}

DailyPlan* Profile::GetTodayPlan() {
	Date today = GetSysTime();
	for(DailyPlan& plan : planned_daily)
		if (plan.date == today)
			return &plan;
	return NULL;
}

void Profile::SetMealPresetFoodsUsed() {
	Database& db = DB();
	for(const MealPreset& preset : presets) {
		for(const MealIngredient& mi : preset.ingredients) {
			db.used_foods.FindAdd(mi.db_food_no);
		}
	}
}















int GetTargetWeight(double height_cm) {
	double height_m = height_cm * 0.01;
	double add = 4 + (15 - 4) * (height_m - 1.68) / (1.96 - 1.68);
	return 0.942 * ((height_m * 100) - 100 + add);
}

int GetBmiWeight(double height_cm, int bmi) {
	double height_m = height_cm * 0.01;
	return bmi * height_m * height_m;
}

double GetBMI(double height_cm, double weight_kg) {
	double height_m = height_cm * 0.01;
	return weight_kg / (height_m * height_m);
}

double GetNormalLiquidPercentage(double age, double height_cm, double weight_kg) {
	// 2.447 – (0.09145 x age) + (0.1074 x height in centimeters) + (0.3362 x weight in kilograms) = total body weight (TBW) in liters
	double liquid_kg = 2.447 - 0.09145 * age + 0.1074 * height_cm + 0.3362 * weight_kg;
	double liquid_perc = 100 * liquid_kg / weight_kg;
	return liquid_perc;
}

double Configuration::GetBMR(double weight) {
	// Male: BMR = (10 × weight in kg) + (6.25 × height in cm) - (5 × age in years) + 5
	// Female: BMR = (10 × weight in kg) + (6.25 × height in cm) - (5 × age in years) - 161
	double bmr = (10.0 * weight) + (6.25 * height) - (5.0 * age);
	if (GetProfile().is_male)
		bmr += 5;
	else
		bmr -= 161;
	
	return bmr;
}

double GetHeartrateCalories(bool is_male, double weight, double age, double heartrate_bpm, double duration_s) {
	if (is_male)
		return (0.2017 * age + 0.1988 * weight + 0.6309 * heartrate_bpm - 55.0969) * duration_s / 60.0 / 4.184;
	else
		return (0.074 * age - 0.1263 * weight + 0.4472 * heartrate_bpm - 20.4022) * duration_s / 60.0 / 4.184;
}

double Configuration::GetTDEE() {
	switch (activity) {
		case 0: return 1.200;
		case 1: return 1.375;
		case 2: return 1.550;
		case 3: return 1.725;
		case 4: return 1.900;
		case 5: return 2.250;
		default: Panic("Invalid activity");
	}
	return 1.0;
}

double Configuration::GetCaloriesMaintainWeight(double weight) {
	double bmr = GetBMR(weight);
	double tdee = GetTDEE();
	return bmr * tdee;
}

double Configuration::GetCaloriesWalking(double weight_kg, double speed, double hours) {
	double bmr = GetBMR(weight_kg);
	
	// 0.76m/s = 2.3 MET
	// 1.34m/s = 3.3 MET
	double met = 2.3 + ((speed - 0.76) / (1.34 - 0.76)) * (3.3 - 2.3);
	
	// BMR (1,885.2) x METs (3.3) ÷ 24 x duration of activity in hours (1) = 259.2 calories
	double calories = bmr * met * hours / 24.0;
	return calories;
}

double Configuration::GetCaloriesWalkingDist(double weight_kg, double distance_km, double hours) {
	double speed = distance_km * 1000.0 / (hours * 60.0 * 60.0);
	ASSERT(speed >= 0.0);
	return GetCaloriesWalking(weight_kg, speed, hours);
}

double Configuration::GetCaloriesWalkingDistSpeed(double weight_kg, double distance_km, double speed) {
	double speed_kmh = speed * 60*60 / 1000.0;
	double hours = distance_km / speed_kmh;
	ASSERT(hours >= 0.0);
	return GetCaloriesWalking(weight_kg, speed, hours);
}

double Configuration::GetCaloriesJogging(double weight_kg, double speed, double hours) {
	double bmr = GetBMR(weight_kg);
	
	// 2.50m/s = 8.8 MET
	// 3.03m/s = 11.2 MET
	double met = 8.8 + ((speed - 2.50) / (3.03 - 2.50)) * (11.2 - 8.8);
	
	// BMR (1,885.2) x METs (3.3) ÷ 24 x duration of activity in hours (1) = 259.2 calories
	double calories = bmr * met * hours / 24.0;
	return calories;
}

double Configuration::GetCaloriesJoggingDistSpeed(double weight_kg, double distance_km, double speed) {
	double speed_kmh = speed * 60*60 / 1000.0;
	double hours = distance_km / speed_kmh;
	ASSERT(hours >= 0.0);
	return GetCaloriesJogging(weight_kg, speed, hours);
}




void FoodStorageSnapshot::GetNutritions(Ingredient& dst) const {
	const Database& db = DB();
	dst.Reset();
	for(int i = 0; i < foods.GetCount(); i++) {
		int db_food_no = foods.GetKey(i);
		int grams = foods[i];
		const FoodDescription& d = db.food_descriptions[db_food_no];
		if (!grams)
			continue;
		dst.grams += grams;
		double mul = grams / 100.0;
		for(const NutritionInfo& info : d.nutr)
			dst.nutr[info.nutr_no] += info.nutr_value * mul;
	}
}

String FoodPriceQuote::GetMassString() const {
	if (servings == 1 && serving_batch == 1) {
		if (grams >= 800)
			return Format(t_("%2n`kg"), grams*0.001);
		else
			return Format(t_("%2n`g"), grams);
	}
	else if (servings > 1 && serving_batch == 1) {
		double serving_g = grams / servings;
		double serving_kg = serving_g * 0.001;
		if (serving_kg >= 1)
			return Format(t_("%d`pc. ( * ~%2n`kg = ~%2n`kg)"), servings, serving_kg, servings*serving_kg);
		else
			return Format(t_("%d`pc. ( * ~%2n`g = ~%2n`g)"), servings, serving_g, servings*serving_g);
	}
	else if (servings > 1 && serving_batch > 1) {
		int batches = servings / serving_batch + (servings % serving_batch ? 1 : 0);
		double serving_g = grams / servings;
		double serving_kg = serving_g * 0.001;
		int servings = batches * serving_batch;
		if (serving_kg >= 1)
			return Format(t_("%d`*%d`pc. (%d * ~%2n`kg = ~%2n`kg)"), batches, serving_batch, servings, serving_kg, servings*serving_kg);
		else
			return Format(t_("%d`*%d`pc. (%d * ~%2n`g = ~%2n`g)"), batches, serving_batch, servings, serving_g, servings*serving_g);
	}
	else return t_("Invalid mass");
}

String FoodPriceQuote::GetPriceString() const {
	if (servings == 1 && serving_batch == 1) {
		double g_price = price / grams;
		double kg_price = g_price * 1000;
		if (kg_price >= 0.1)
			return Format(t_("%2n`EUR/kg"), kg_price);
		else
			return Format(t_("%2n`EUR/g"), g_price);
	}
	else if (servings > 1 && serving_batch == 1) {
		double serving_price = price / servings;
		double serving_g = grams / servings;
		double serving_kg = serving_g * 0.001;
		if (serving_kg >= 1)
			return Format(t_("%2n`EUR (~%2n`kg)"), serving_price, serving_kg);
		else
			return Format(t_("%2n`EUR (~%2n`g)"), serving_price, serving_g);
	}
	else if (servings > 1 && serving_batch > 1) {
		int batches = servings / serving_batch + (servings % serving_batch ? 1 : 0);
		double serving_price = price / servings;
		double batch_price = price / servings * serving_batch;
		double serving_g = grams / servings;
		double serving_kg = serving_g * 0.001;
		double total_price = batches * serving_batch * serving_price;
		if (batches > 1) {
			if (serving_kg >= 1)
				return Format(t_("%2n`EUR/%d`pc. (%2n`EUR/%d`pc., %2n`EUR/pc., ~%2n`kg/pc.)"), total_price, batches * serving_batch, batch_price, serving_batch, serving_price, serving_kg);
			else
				return Format(t_("%2n`EUR/%d`pc. (%2n`EUR/%d`pc., %2n`EUR/pc., ~%2n`g/pc.)"), total_price, batches * serving_batch, batch_price, serving_batch, serving_price, serving_g);
		}
		else {
			if (serving_kg >= 1)
				return Format(t_("%2n`EUR/%d`pc. (%2n`EUR/pc., ~%2n`kg/pc.)"), total_price, batches * serving_batch, serving_price, serving_kg);
			else
				return Format(t_("%2n`EUR/%d`pc. (%2n`EUR/pc., ~%2n`g/pc.)"), total_price, batches * serving_batch, serving_price, serving_g);
		}
	}
	else return t_("Invalid price");
}

void FoodPriceQuote::Set(Time time, double min_grams, const FoodPriceQuote& prev) {
	this->time = time;
	double gram_price = prev.servings ? prev.price / prev.grams : 0;
	double serving_grams = prev.servings ? prev.grams / prev.servings : 10;
	serving_batch = max(1, prev.serving_batch);
	double fservings = min_grams / serving_grams;
	servings = 0;
	while (servings < fservings) servings += serving_batch;
	grams = servings * serving_grams;
	price = grams * gram_price;
	shop = prev.shop;
	/*double fservings = grams / serving_grams;
	servings = fservings;
	if (fservings > servings) servings++;
	*/
}

void FoodPriceQuote::SetPriceless(Time time, double grams) {
	this->time = time;
	this->grams = grams;
	price = 0;
	servings = 1;
	serving_batch = 1;
	shop = "";
}







String GetSnapshotSourceString(int i) {
	switch(i) {
		case SNAPSRC_USER: return t_("User");
		case SNAPSRC_FOODLOG: return t_("Food Log");
		case SNAPSRC_SHOPLOG: return t_("Shop Log");
		case SNAPSRC_RECEIPTLOG: return t_("Receipt Log");
		default: return t_("Invalid Source");
	}
}
















PlanState::PlanState(bool is_male, Configuration& conf, WeightLossStat& wl) : is_male(is_male) {
	cals_in_kg_fat = 3500/0.453592;
	tgt_fat_perc = DEFAULT_FAT_PERC(is_male);
	max_lean_gain = 226 / 7.0; // around 0.5 pounds (=~226 grams) lean in a week (TODO shouldn't the weight affect this too? how much lean gain per weight kg?)
	walking_speed = 0.916; // m/s
	internal_perc = 0.15 + 0.25;
	internal_kgs = internal_perc * wl.weight;
	
	worst_weight = wl.weight;
	worst_fat_perc = wl.fat * 0.01;
	worst_lean_perc = wl.muscle * 0.01;
	
	tgt_weight = wl.weight - (wl.weight * worst_fat_perc) + (wl.weight * tgt_fat_perc);
	
	Set(conf);
	Set(wl);
	
	lean_fat_loss_ratio_av.Add(0.52);
}

bool PlanState::IsReadyForMaintenance() const {
	/*return	tgt_fat_perc + 0.01 > fat_perc &&
			tgt_weight - 1.0 < weight;*/
	return	tgt_fat_perc + 0.01 > fat_perc;
}

double PlanState::GetProgress() const {
	double weight_prog = 1.0 - fabs(weight - tgt_weight) / fabs(worst_weight - tgt_weight);
	double fat_perc_prog = 1.0 - fabs(fat_perc - tgt_fat_perc) / fabs(worst_fat_perc - tgt_fat_perc);
	//double lean_perc_prog = 1.0 - fabs(lean_perc - tgt_lean_perc) / fabs(worst_lean_perc - tgt_lean_perc);
	//double prog = (weight_prog + fat_perc_prog + lean_perc_prog) / 3.0;
	
	//double prog = (weight_prog + fat_perc_prog) / 2.0;
	//return prog;
	
	return fat_perc_prog;
}

void PlanState::Set(Configuration& conf) {
	exercise_kcal = conf.tgt_exercise_kcal;
	walking_dist = conf.walking_dist + conf.tgt_walking_dist;
	jogging_dist = conf.tgt_jogging_dist;
}

void PlanState::Set(WeightLossStat& wl) {
	tgt_lean_perc = 1.0 - (internal_perc + tgt_fat_perc); // skeleton 15%, organs 25%, fat
	jogging_speed = 2.500; // m/s (TODO set in gui and read from wl)
	if (wl.walking_mins > 0 && wl.walking > 0.0) {
		double seconds = wl.walking_mins * 60;
		double meters = wl.walking * 1000;
		walking_speed = meters / seconds; // m/s
	}
	weight = wl.weight;
	fat_perc = wl.fat * 0.01;
	lean_perc = wl.muscle * 0.01;
	
	if (wl.weight > 0.0 && wl.fat > 0.0 && wl.muscle > 0.0) {
		weight = wl.weight;
		fat_perc = wl.fat * 0.01;
		lean_perc = wl.muscle * 0.01;
		internal_perc = 1.0 - lean_perc - fat_perc;
		tgt_lean_perc = 1.0 - (internal_perc + tgt_fat_perc); // skeleton 15%, organs 25%, fat
		internal_kgs = internal_perc * weight;
		
		double fat_loss = (worst_weight * worst_fat_perc) - (weight * fat_perc);
		double muscle_loss = (worst_weight * (1.0 - worst_lean_perc)) - (weight * (1.0 - lean_perc));
		if (fat_loss > 0.0 && muscle_loss > 0.0) {
			double lean_fat_loss_ratio = muscle_loss / (fat_loss + muscle_loss);
			lean_fat_loss_ratio_av.Add(lean_fat_loss_ratio);
		}
	}
	
	CheckWorst();
}

void PlanState::SetLeanAndFatMass(double new_lean_kgs, double new_fat_kgs) {
	//internal_kgs = (new_fat_kgs + new_lean_kgs) / (1.0 - internal_perc) * internal_perc;
	internal_perc = internal_kgs / (new_lean_kgs + new_fat_kgs + internal_kgs);
	fat_perc = new_fat_kgs / (new_fat_kgs + new_lean_kgs + internal_kgs);
	lean_perc = new_lean_kgs / (new_fat_kgs + new_lean_kgs + internal_kgs);
	weight = new_fat_kgs + new_lean_kgs + internal_kgs;
}

void PlanState::CheckWorst() {
	if (fabs(weight - tgt_weight) > fabs(worst_weight - tgt_weight))
		worst_weight = weight;
	if (fabs(fat_perc - tgt_fat_perc) > fabs(worst_fat_perc - tgt_fat_perc))
		worst_fat_perc = fat_perc;
	if (fabs(lean_perc - tgt_lean_perc) > fabs(worst_lean_perc - tgt_lean_perc))
		worst_lean_perc = lean_perc;
}

void PlanState::AddCalorieDeficit(double calorie_deficit) {
	max_calorie_deficit = max(calorie_deficit, max_calorie_deficit);
	calorie_deficit_sum += calorie_deficit;
}


















String ActivityItem::GetTypeString() const {
	switch (type) {
		case ACT_UNKNOWN: return t_("Unknown");
		case ACT_INTERVAL: return t_("Interval");
		case ACT_EXERCISE: return t_("Exercise");
		default: return t_("Invalid");
	}
}
