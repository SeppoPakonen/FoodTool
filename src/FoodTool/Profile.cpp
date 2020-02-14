#include "FoodTool.h"



Profile::Profile() {
	LoadThis();
	tmp_usage_start = GetSysTime();
	
	if (is_initialised) {
		if (storage.days.IsEmpty())
			storage.Init(begin_date);
		Start(false);
	}
}

void Profile::ProcessUpdate(bool replan) {
	TimeStop ts;
	
	int prev_days = 0;
	if (planned_daily.GetCount())
		prev_days = planned_daily.Top().date.Get() - begin_date.Get();
	
	if (replan)
		UpdatePlan();
	
	storage.Update(planned_daily);
	version++;
	
	StoreThis();
	
	int days = planned_daily.Top().date.Get() - begin_date.Get();
	int days_diff = days - prev_days;
	
	LOG("Calory-deficit: " << (av_calorie_deficit * 100) << "\%");
	LOG("Started " << Format("%", begin_date));
	LOG("Stopping " << Format("%", planned_daily.Top().date));
	//LOG("Days total " << days << (days_diff ? "(Diff " + (days_diff < 0 ? "-" : "+") + IntStr(days_diff) + ")" : "");
	LOG(Format("Days total %d (Diff %+d)", days, days_diff));
	
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
	WeightLossStat* wl = &weights[0];
	
	double begin_weight = wl->weight;
	double begin_bodyfat = conf->bodyfat;
	
	double weight = wl->weight;
	double walking_dist = conf->walking_dist + conf->tgt_walking_dist;
	double jogging_dist = conf->tgt_jogging_dist;
	double walking_speed = 0.916; // m/s
	double jogging_speed = 2.500; // m/s
	double cals_in_kg_fat = 3500/0.453592;
	double tgt_fat_perc = 13;
	
	double calorie_deficit_sum = 0.0;
	double max_calorie_deficit = 0.0;
	
	if (planned_nutrients.IsEmpty()) {
		planned_nutrients.FindAdd(KCAL);
		planned_nutrients.FindAdd(PROT);
		planned_nutrients.FindAdd(FAT);
		for(int i = 0; i < db.nutr_recom.GetCount(); i++)
			planned_nutrients.FindAdd(db.nutr_recom[i].nutr_no);
	}
	
	int easy_day_counter = 0;
	Date date = begin_date;
	planned_daily.SetCount(0);
	planned_daily.Reserve(2*365);
	while (weight > conf->tgt_weight) {
		DailyPlan& d = planned_daily.Add();
		
		d.mode = MODE_WEIGHTLOSS;
		d.date = date++;
		d.weight = weight;
		d.prog = 1.0 - (weight - conf->tgt_weight) / (begin_weight - conf->tgt_weight);
		ASSERT(d.prog >= 0.0 && d.prog <= 1.0);
		d.fat_perc = d.prog * tgt_fat_perc * 0.01 + (1.0 - d.prog) * begin_bodyfat * 0.01;
		ASSERT(d.fat_perc >= 0.0 && d.fat_perc <= 1.0);
		d.fat_kgs = weight * d.fat_perc;
		d.lean_body_kgs = weight * (1.0 - d.fat_perc);
		
		d.maintain_calories = conf->GetCaloriesMaintainWeight(weight);
		
		double min_protein = d.lean_body_kgs * 0.8;
		double min_fat = 30.0 / 2000.0 * d.maintain_calories;
		double prot_cals, fat_cals, carb_cals;
		
		if (d.mode == MODE_WEIGHTLOSS) {
			prot_cals = min_protein * 4.4;
			fat_cals = min_fat * 9.0;
			carb_cals = (prot_cals + fat_cals) / 0.95 * 0.05;
			
			d.allowed_calories = carb_cals + prot_cals + fat_cals;
		}
		else Panic("TODO");
			
		// We aren't going under minimum daily calories
		if (d.allowed_calories < MINIMUM_DAILY_KCAL) {
			double mul = MINIMUM_DAILY_KCAL / d.allowed_calories;
			d.allowed_calories = MINIMUM_DAILY_KCAL;
			fat_cals *= mul;
			carb_cals *= mul;
			prot_cals *= mul;
		}
		
		easy_day_counter++;
		d.is_easy_day = false;
		if (easy_day_counter == conf->easy_day_interval) {
			easy_day_counter = 0;
			d.is_easy_day = true;
			double mul = 1500.0 / d.allowed_calories;
			d.allowed_calories = max(1500.0, d.allowed_calories);
			fat_cals *= mul;
			carb_cals *= mul;
			prot_cals *= mul;
		}
		
		double calorie_deficit = 1.0 - (d.allowed_calories / d.maintain_calories);
		max_calorie_deficit = max(calorie_deficit, max_calorie_deficit);
		calorie_deficit_sum += calorie_deficit;
		
		d.maintain_burned_calories = d.maintain_calories * calorie_deficit;
		if (walking_dist)
			d.walking_burned_calories = conf->GetCaloriesWalkingDistSpeed(weight, walking_dist, walking_speed);
		else
			d.walking_burned_calories = 0;
		if (jogging_dist)
			d.jogging_burned_calories = conf->GetCaloriesJoggingDistSpeed(weight, jogging_dist, jogging_speed);
		else
			d.jogging_burned_calories = 0;
		d.burned_calories = d.maintain_burned_calories + d.walking_burned_calories + d.jogging_burned_calories;
		d.burned_kgs = d.burned_calories / cals_in_kg_fat;
		
		d.food.grams = weight / 100.0 * 2300.0;
		d.food.nutr[KCAL] = d.allowed_calories;
		d.food.nutr[PROT] = max(min_protein, prot_cals / 4.4); // based on protein powder nutrients
		d.food.nutr[FAT] = max(min_fat, fat_cals / 9.0); // based on coconut oil nutrients
		d.food.nutr[CARB] = carb_cals / 10.0;
		ASSERT(db.nutr_recom.GetCount());
		for(const NutritionRecommendation& r : db.nutr_recom) {
			if (r.per_kg)
				d.food.nutr[r.nutr_no] = weight * r.value;
			else
				d.food.nutr[r.nutr_no] = r.value;
		}
		
		weight -= d.burned_kgs;
	}
	
	av_calorie_deficit = calorie_deficit_sum / planned_daily.GetCount();
	
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
	int days_left_tomorrow = planned_daily.GetCount() - day_i - 1;
	
	auto& wake = s.items.Add();
	wake.time = Time(s.day.year, s.day.month, s.day.day, conf.waking_hour, conf.waking_minute, 0);
	wake.type = ScheduleToday::WAKING;
	wake.msg = Format("Good morning. Have %d calories today!", (int)day.total_sum.nutr[KCAL]);
	
	auto& sleep = s.items.Add();
	sleep.time = Time(s.day.year, s.day.month, s.day.day, conf.sleeping_hour, conf.sleeping_minute, 0);
	sleep.type = ScheduleToday::SLEEPING;
	sleep.msg = Format("Good night. Only %d days left tomorrow!", days_left_tomorrow);
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
			meal.msg = Format("%d grams", m.grams);
	}
	
	int day_len = sleep.time.Get() - wake.time.Get();
	Time exercise_time = wake.time + day_len * 3 / 4;
	
	if (conf.tgt_walking_dist > 0) {
		auto& walk = s.items.Add();
		walk.time = exercise_time;
		walk.type = ScheduleToday::WALKING;
		walk.msg = "Go walk " + Format("%2n", conf.tgt_walking_dist) + "km!";
	}
	
	if (conf.tgt_jogging_dist > 0) {
		auto& jogging = s.items.Add();
		jogging.time = exercise_time;
		jogging.type = ScheduleToday::RUNNING;
		jogging.msg = "Go jogging " + Format("%2n", conf.tgt_jogging_dist) + "km!";
	}
	
	for(auto& it : s.items)
		it.done = now > it.time;
	
	Sort(s.items, ScheduleToday());
}























int GetTargetWeight(double height_m, int bmi) {
	return bmi * height_m * height_m;
}

int GetBMI(double height_m, double weight_kg) {
	return weight_kg / (height_m * height_m);
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



