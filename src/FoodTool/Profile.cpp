#include "FoodTool.h"



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
	
	//CookedToRaw();
	
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
	
	const double walking_dist = conf->walking_dist + conf->tgt_walking_dist;
	const double jogging_dist = conf->tgt_jogging_dist;
	const double jogging_speed = 2.500; // m/s
	const double cals_in_kg_fat = 3500/0.453592;
	const double tgt_weight = GetTargetWeight(conf->height * 0.01);
	const double tgt_fat_perc = is_male ? 6 * 0.01 : 21 * 0.01;
	const double max_lean_gain = 226 / 7.0; // around 0.5 pounds lean in a week
	const double begin_weight = wl->weight;
	const double begin_fat_perc = wl->fat * 0.01;
	const double begin_lean_perc = wl->muscle * 0.01;
	
	double internal_perc = 0.15 + 0.25;
	double tgt_lean_perc = 1.0 - (internal_perc + tgt_fat_perc); // skeleton 15%, organs 25%, fat
	double tgt_lean_kgs = tgt_weight * tgt_lean_perc;
	double internal_kgs = internal_perc * begin_weight;
	double walking_speed = 0.916; // m/s
	double weight = wl->weight;
	double fat_perc = begin_fat_perc;
	double lean_perc = begin_lean_perc;
	double calorie_deficit_sum = 0.0;
	double max_calorie_deficit = 0.0;
	
	OnlineAverage1 lean_fat_loss_ratio_av;
	lean_fat_loss_ratio_av.Add(0.52);
	
	if (planned_nutrients.IsEmpty()) {
		planned_nutrients.FindAdd(KCAL);
		planned_nutrients.FindAdd(PROT);
		planned_nutrients.FindAdd(FAT);
		for(int i = 0; i < db.nutr_recom.GetCount(); i++)
			planned_nutrients.FindAdd(db.nutr_recom[i].nutr_no);
	}
	
	Date date = begin_date;
	planned_daily.SetCount(0);
	planned_daily.Reserve(2*365);
	
	Date end_date = GetSysTime();
	end_date += 30;
	
	int easy_day_counter = 0;
	
	int count = 0;
	int maintenance_day_count = 0;
	while (
		fabs(fat_perc - tgt_fat_perc) > 0.01 ||
		fabs(tgt_lean_perc - lean_perc) > 0.01 ||
		fabs(weight - tgt_weight) > 1.0 ||
		date < end_date ||
		maintenance_day_count < 30) {
		double prog = max(0.0, min(1.0, 1.0 - fabs(weight - tgt_weight) / fabs(begin_weight - tgt_weight)));
		//if (planned_daily.GetCount() && planned_daily.Top().prog > prog) break;
		//if (count > 365) break;
		count++;
		
		DailyPlan& d = planned_daily.Add();
		
		while (next_conf && date >= Date(next_conf->added)) {
			conf = next_conf;
			conf_i++;
			next_conf = (conf_i+1) < confs.GetCount() ? &confs[conf_i+1] : NULL;
		}
		
		double lean_fat_loss_ratio = lean_fat_loss_ratio_av.GetMean();
		double fat_kgs = weight * fat_perc;
		double lean_kgs = weight * lean_perc;

		d.mode = MODE_WEIGHTLOSS;
		d.date = date++;
		d.weight = weight;
		d.prog = prog;
		d.fat_perc = fat_perc;
		d.fat_kgs = fat_kgs;
		d.lean_kgs = lean_kgs;
		
		d.maintain_calories = conf->GetCaloriesMaintainWeight(weight);
		
		// Coffee
		double coffee_g = conf->daily_coffee;
		double caffeine_mg = 40 * (coffee_g / 100.0);
		double rmr_increase_100mg_caffeine = 0.04;
		double increase_length = 150.0 / (24*60);
		double rmr_increase = caffeine_mg / 100.0 * rmr_increase_100mg_caffeine;
		double coffee_maintain_calorie_increase = rmr_increase * d.maintain_calories;
		ASSERT(coffee_maintain_calorie_increase >= 0);
		d.maintain_calories += coffee_maintain_calorie_increase;
		
		double maintain_protein = weight * 0.8;
		double min_protein;
		if (fat_perc > tgt_fat_perc + 0.01) {
			min_protein = maintain_protein;
			d.variant_type = VARIANT_WEIGHTLOSS;
			maintenance_day_count = 0;
		}
		else if (fabs(lean_kgs - tgt_lean_kgs) < 0.5) {
			min_protein = maintain_protein;
			d.variant_type = VARIANT_MAINTENANCE;
			maintenance_day_count++;
		}
		else if (tgt_lean_kgs > lean_kgs) {
			min_protein = weight * 2.2;
			d.variant_type = VARIANT_MUSCLEGAIN;
			maintenance_day_count = 0;
		}
		else {
			min_protein = weight * 0.4;
			d.variant_type = VARIANT_FATTYACIDS;
			maintenance_day_count = 0;
		}
		
		double min_fat = 30.0 / 2000.0 * d.maintain_calories;
		double prot_cals, fat_cals, carb_cals;
		
		if (fat_perc > tgt_fat_perc + 0.01) {
			prot_cals = min_protein * 4.4;
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
		else if (fat_perc >= tgt_fat_perc) {
			d.allowed_calories = 0.6 * d.maintain_calories;
			prot_cals = min_protein * 4.4;
			fat_cals = (d.allowed_calories - prot_cals) * 0.33;
			carb_cals = (d.allowed_calories - prot_cals) * 0.67;
			d.is_easy_day = false;
		}
		else {
			d.allowed_calories = 1.4 * d.maintain_calories;
			prot_cals = min_protein * 4.4;
			fat_cals = (d.allowed_calories - prot_cals) * 0.33;
			carb_cals = (d.allowed_calories - prot_cals) * 0.67;
			d.is_easy_day = false;
		}
		
		if (d.is_easy_day)
			d.variant_type = VARIANT_SCORE;
		
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
		
		#if 1
		double burned_protein_grams = max(-max_lean_gain * 0.26, maintain_protein - (prot_cals / 4.4));
		double burned_lean_cals = burned_protein_grams / 26.0 * 250.0; // from beef... lol
		double burned_lean_kgs  = burned_protein_grams / 26.0 * 0.1; // prot 26g/100g --> kgs
		double burned_fat_cals = d.burned_calories - burned_lean_cals;
		#else
		double burned_lean_cals = lean_fat_loss_ratio * d.burned_calories;
		double burned_fat_cals = (1.0 - lean_fat_loss_ratio) * d.burned_calories;
		double burned_lean_kgs = burned_lean_cals / 250.0 * 0.1;
		#endif
		double burned_fat_kgs = burned_fat_cals / cals_in_kg_fat;
		d.burned_kgs = burned_lean_kgs + burned_fat_kgs;
		
		d.food.Reset();
		for(const NutritionRecommendation& r : db.nutr_recom) {
			if (r.per_kg)
				d.food.nutr[r.nutr_no] = weight * r.value;
			else
				d.food.nutr[r.nutr_no] = r.value;
		}
		d.food.grams = weight / 100.0 * 2000.0;
		d.food.nutr[KCAL] = d.allowed_calories;
		d.food.nutr[PROT] = max(min_protein, prot_cals / 4.4); // based on protein powder nutrients
		d.food.nutr[FAT] = max(min_fat, fat_cals / 9.0); // based on coconut oil nutrients
		d.food.nutr[CARB] = carb_cals / 10.0;
		ASSERT(db.nutr_recom.GetCount());
		
		double new_lean_kgs = lean_kgs - burned_lean_kgs;
		double new_fat_kgs = fat_kgs - burned_fat_kgs;
		ASSERT(new_lean_kgs <= lean_kgs + max_lean_gain);
		internal_kgs = (new_fat_kgs + new_lean_kgs) / (1.0 - internal_perc) * internal_perc;
		fat_perc = new_fat_kgs / (new_fat_kgs + new_lean_kgs + internal_kgs);
		lean_perc = new_lean_kgs / (new_fat_kgs + new_lean_kgs + internal_kgs);
		weight = new_fat_kgs + new_lean_kgs + internal_kgs;
		
		while (next_wl && date >= Date(next_wl->added)) {
			wl = next_wl;
			wl_i++;
			next_wl = (wl_i+1) < weights.GetCount() ? &weights[wl_i+1] : NULL;
			if (wl->weight > 0.0 && wl->fat > 0.0 && wl->muscle > 0.0) {
				weight = wl->weight;
				fat_perc = wl->fat * 0.01;
				lean_perc = wl->muscle * 0.01;
				internal_perc = 1.0 - lean_perc - fat_perc;
				tgt_lean_perc = 1.0 - (internal_perc + tgt_fat_perc); // skeleton 15%, organs 25%, fat
				tgt_lean_kgs = tgt_weight * tgt_lean_perc;
				internal_kgs = internal_perc * weight;
				
				double fat_loss = (begin_weight * begin_fat_perc) - (weight * fat_perc);
				double muscle_loss = (begin_weight * (1.0 - begin_lean_perc)) - (weight * (1.0 - lean_perc));
				if (fat_loss > 0.0 && muscle_loss > 0.0) {
					double lean_fat_loss_ratio = muscle_loss / (fat_loss + muscle_loss);
					lean_fat_loss_ratio_av.Add(lean_fat_loss_ratio);
				}
			}
		}
		
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
	
	if (conf.tgt_walking_dist > 0) {
		auto& walk = s.items.Add();
		walk.time = exercise_time;
		walk.type = ScheduleToday::WALKING;
		walk.msg = Format(t_("Go walk  %2n km!"), conf.tgt_walking_dist);
	}
	
	if (conf.tgt_jogging_dist > 0) {
		auto& jogging = s.items.Add();
		jogging.time = exercise_time;
		jogging.type = ScheduleToday::RUNNING;
		jogging.msg = Format(t_("Go jogging %2n km!"), conf.tgt_jogging_dist);
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



















int GetTargetWeight(double height_m) {
	double add = 4 + (15 - 4) * (height_m - 1.68) / (1.96 - 1.68);
	return 0.942 * ((height_m * 100) - 100 + add);
}

int GetBmiWeight(double height_m, int bmi) {
	return bmi * height_m * height_m;
}

double GetBMI(double height_m, double weight_kg) {
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
