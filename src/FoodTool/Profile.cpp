#include "FoodTool.h"



Profile::Profile() {
	LoadThis();
	tmp_usage_start = GetSysTime();
	
	if (is_initialised) {
		if (storage.days.IsEmpty())
			storage.Init(begin_date);
		storage.Update(planned_daily);
	}
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
	
	Date date = begin_date;
	planned_daily.SetCount(0);
	planned_daily.Reserve(2*365);
	while (weight > conf->tgt_weight) {
		DailyPlan& d = planned_daily.Add();
		
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
		// If weight loss mode
		if (1) {
			prot_cals = min_protein * 4.4;
			fat_cals = min_fat * 9.0;
			carb_cals = (prot_cals + fat_cals) / 0.95 * 0.05;
			
			d.allowed_calories = carb_cals + prot_cals + fat_cals;
		}
		
		// We aren't going under 600kcals
		if (d.allowed_calories < 600) {
			double mul = 600 / d.allowed_calories;
			d.allowed_calories = 600;
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
		
		d.food.grams = weight / 100.0 * 1000.0;
		d.food.kcals = d.allowed_calories;
		d.food.protein = max(min_protein, prot_cals / 4.4); // based on protein powder nutrients
		d.food.fat = max(min_fat, fat_cals / 9.0); // based on coconut oil nutrients
		d.food.carbs = carb_cals / 10.0;
		d.food.salt = 1.5;
		
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
	String tstr = Format("%", GetSysTime());
	tstr.Replace("/", "_");
	tstr.Replace(":", "_");
	tstr.Replace(" ", "_");
	String file = ConfigFile(tstr + ".bin");
	StoreToFile(*this, file);
	
	file = ConfigFile("latest.bin");
	StoreToFile(*this, file);
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
