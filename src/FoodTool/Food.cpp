#include "FoodTool.h"

void FoodDay::SetMealGrams(const Vector<double>& grams, bool check) {
	Panic("TODO");
	#if 0
	const Database& db = DB();
	int it = 0;
	total_sum.Reset();
	for(int i = 0; i < meals.GetCount(); i++) {
		Meal& m = meals[i];
		m.food_sum.Reset();
		for(int j = 0; j < m.food.GetCount(); j++) {
			int db_i = m.food.GetKey(j);
			const FoodDescription& d = db.food_descriptions[db_i];
			double g = grams[it++];
			
			int local_i = db.local_products.Find(db_i);
			if (local_i >= 0) {
				const LocalProduct& p = db.local_products[local_i];
				if (p.serving_grams > 0) {
					int count = (g + p.serving_grams * 0.5) / p.serving_grams;
					g = count * p.serving_grams;
				}
			}
			g = max(0.0, g);
			m.food[j] = g;
			
			if (g > 0.0) {
				m.food_sum.Add(g, d);
				if (check) {
					/*ASSERT(m.food_sum.fat >= 0 && m.food_sum.fat < 2000);
					ASSERT(m.food_sum.carbs >= 0 && m.food_sum.carbs < 2000);
					ASSERT(m.food_sum.protein >= 0 && m.food_sum.protein < 2000);*/
				}
			}
		}
		total_sum += m.food_sum;
	}
	ASSERT(it == grams.GetCount());
	#endif
}

double FoodDay::GetOptimizerEnergy() {
	// If weight loss mode
	if (mode == MODE_WEIGHTLOSS) {
		const Profile& prof = GetProfile();
		const Database& db = DB();
		double fabs_sum = 0.0;
		
		ASSERT(target_sum.grams > 0);
		fabs_sum += fabs((double)total_sum.grams / target_sum.grams - 1);
		
		for(int i = 0; i < prof.planned_nutrients.GetCount(); i++) {
			int j = prof.planned_nutrients[i];
			double total = total_sum.nutr[j];
			double target = target_sum.nutr[j];
			ASSERT(target > 0);
			double value = total / target - 1;
			fabs_sum += fabs(value);
		}
		
		double fabs_av = fabs_sum / (1 + prof.planned_nutrients.GetCount());
		ASSERT(IsFin(fabs_av));
		return -fabs_av;
	}
	else Panic("TODO");
}











void MealPreset::GetNutritions(Ingredient& dst) const {
	const Database& db = DB();
	dst.Reset();
	for(int i = 0; i < ingredients.GetCount(); i++) {
		const MealIngredient& src = ingredients[i];
		const FoodDescription& d = db.food_descriptions[src.db_food_no];
		if (!src.max_grams)
			continue;
		dst.grams += src.max_grams;
		double mul = src.max_grams / 100.0;
		for(const NutritionInfo& info : d.nutr)
			dst.nutr[info.nutr_no] += info.nutr_value * mul;
	}
}

double MealPreset::GetOptimizerEnergy(const Ingredient& target_sum, const Index<int>& nutr_idx, MealDebugger& dbg) {
	const Profile& prof = GetProfile();
	const Database& db = DB();
	double fabs_sum = 0.0;
	
	static thread_local Ingredient total_sum;
	GetNutritions(total_sum);
	
	int count = 0;
	
	if (!target_sum.grams)
		return -1000;
	ASSERT(target_sum.grams > 0);
	
	
	// Too many ingredients (doesn't work)
	if (0) {
		int ing_count = 0;
		for(int i = 0; i < ingredients.GetCount(); i++)
			if (ingredients[i].max_grams > 0.0)
				ing_count++;
		if (ing_count > 8) {
			fabs_sum += (ing_count - 7) * 10;
			count += 10;
		}
	}
	
	
	if (1) {
		fabs_sum += 10 * fabs((double)total_sum.grams / target_sum.grams - 1);
		count += 10;
	}
	
	// Penalty for too much for one ingredient
	if (1) {
		int penalties = 0;
		for(int i = 0; i < ingredients.GetCount(); i++)
			if (ingredients[i].max_grams > 250.0)
				penalties++;
		fabs_sum += 10 * penalties;
		count += 10;
	}
	
	for(int i = 0; i < nutr_idx.GetCount(); i++) {
		int j = nutr_idx[i];
		double total = total_sum.nutr[j];
		double target = target_sum.nutr[j];
		
		
		if (!target) {
			if (!total)
				continue;
			else
				return -10;
		}
		double value = total / target - 1;
		
		if (dbg.do_debug)
			dbg.nutr_fabs_av[j].Add(fabs(value));
		
		if (j  == KCAL) {
			value *= 100;
			count += 100;
		}
		/*else if (j == PROT) {
			//if (value > 0.0) value = 0.0;
			value *= 50;
			count += 50;
		}*/
		else {
			if (value > 0.0)
				value = max(0.0, value - 3.0);
			count++;
		}
		
		fabs_sum += fabs(value);
		
	}
	
	double fabs_av = fabs_sum / count;
	ASSERT(IsFin(fabs_av));
	return -fabs_av;
}

void GetFactors(Vector<MealIngredient>& ingredients, double& mass_factor, double& taste_factor, double& score) {
	const Database& db = DB();
	mass_factor = 0;
	taste_factor = 0;
	
	Ingredient ing;
	double av_kcals = 0, av_carb = 0, av_fat = 0, av_prot = 0, av_mass = 0;
	for(int i = 0; i < ingredients.GetCount(); i++) {
		const MealIngredient& in = ingredients[i];
		const FoodDescription& d = db.food_descriptions[in.db_food_no];
		double mass = (in.max_grams + in.min_grams) / 2.0;
		ing.Set(mass, d);
		av_kcals += ing.nutr[KCAL];
		av_carb += ing.nutr[CARB];
		av_fat += ing.nutr[FAT];
		av_prot += ing.nutr[PROT];
		av_mass += mass;
	}
	mass_factor = av_mass / av_kcals;
	
	av_carb = av_carb / av_mass;
	av_fat  = av_fat  / av_mass * 9.0;
	av_prot = av_prot / av_mass * 4.0;
	double av_macro = (av_carb + av_fat + av_prot) / 3.0;
	double macro_imbalance = fabs(av_carb / av_macro -1) + fabs(av_fat / av_macro -1) + fabs(av_prot / av_macro -1);
	
	double balance_score = 10.0 - max(0.0, min(10.0, macro_imbalance));
	taste_factor = balance_score * av_kcals / av_mass;
	
	score = mass_factor * taste_factor;
}

void MealPreset::UpdateFactors() {
	GetFactors(ingredients, mass_factor, taste_factor, score);
}

void MealPresetVariant::UpdateFactors() {
	GetFactors(ingredients, mass_factor, taste_factor, score);
}


/*
Variant ideas:
 - largest / smallest / average (= (largest + smallest) / 2)
	- mass
	- taste
	- score (mass * taste)
	- score without carbs
	- score without fat
	- score without protein
	- protein
	- fat
	- carbs
	- nutrient-score
	- fiber
	- essential amino acids
	- fatty acids
 - 1:5, 1:2, 1:1, 2:1, 5:1
	- largest mass vs best taste
	- largest protein vs largest fatty acids
 - most useful
	- easiest
		- best score, but punish by every new ingredient
	- for weight loss
		- largest mass with punishment for going lower than minimum taste factor
	- for muscle gain (2000kcal/day)
		- minimum carbs, 2.2g/weight-kg protein, essential amino acids, fatty acids (, and best score)
	- for maintenance (1800kcal/pv)
		- minimum carbs, 0.8g/weight-kg protein, minimum essential amino acids, maximum fatty acids
*/

template <int I>
double VariantScore(const MealPresetVariant& var) {
	thread_local static Ingredient ing;
	const Database& db = DB();
	
	ing.Reset();
	
	int ing_count = 0;
	for(const MealIngredient& mi : var.ingredients) {
		const FoodDescription& d = db.food_descriptions[mi.db_food_no];
		if (mi.max_grams > 0.0) {
			ing.Add(mi.max_grams, d);
			ing_count++;
		}
	}
	
	if (ing.grams == 0.0)
		return -10000000;
	
	double av_mass = ing.grams;
	double av_kcals = ing.nutr[KCAL];
	double mass_factor = av_mass / av_kcals;
	
	double av_carb = ing.nutr[CARB] / av_mass;
	double av_fat  = ing.nutr[FAT]  / av_mass * 9.0;
	double av_prot = ing.nutr[PROT] / av_mass * 4.0;
	double av_macro = (av_carb + av_fat + av_prot) / 3.0;
	double macro_imbalance = fabs(av_carb / av_macro -1) + fabs(av_fat / av_macro -1) + fabs(av_prot / av_macro -1);
	
	double balance_score = 10.0 - max(0.0, min(10.0, macro_imbalance));
	double taste_factor = balance_score * av_kcals / av_mass;
	
	double score = mass_factor * taste_factor;
	if (I > 0) {
		score /= I + ing_count;
	}
	else if (I == -1) {
		score = mass_factor;
		if (taste_factor < 5)
			score -= fabs(5 - taste_factor) / 2.0;
	}
	
	return score;
}

template <int I>
double VariantNutritionGroup(const MealPresetVariant& var) {
	thread_local static Ingredient ing;
	const Database& db = DB();
	
	ing.Reset();
	
	for(const MealIngredient& mi : var.ingredients) {
		const FoodDescription& d = db.food_descriptions[mi.db_food_no];
		ing.Add(mi.max_grams, d);
	}
	
	if (ing.grams == 0.0)
		return -10000000;
	
	double new_grams = 2000.0 / ing.nutr[KCAL] * ing.grams;
	ing.ChangeGrams(new_grams);
	
	double score = 0;
	for(const NutritionRecommendation& recom : db.nutr_recom) {
		if (recom.group == I) {
			double value = ing.nutr[recom.nutr_no];
			double target = recom.GetValue(100);
			double rel = value / target - 1.0;
			if (rel < 0)
				score += rel;
		}
	}
	
	return score;
}

template <int I, int J>
double VariantMaintenance(const MealPresetVariant& var) {
	thread_local static Ingredient ing;
	const Database& db = DB();
	
	ing.Reset();
	
	for(const MealIngredient& mi : var.ingredients) {
		const FoodDescription& d = db.food_descriptions[mi.db_food_no];
		ing.Add(mi.max_grams, d);
	}
	
	if (ing.grams == 0.0)
		return -10000000;
	
	double new_grams = (double)I / ing.nutr[KCAL] * ing.grams;
	ing.ChangeGrams(new_grams);
	
	double score = 0;
	for(const NutritionRecommendation& recom : db.nutr_recom) {
		if (recom.group == AMINOACID || recom.group == FATTYACID) {
			double value = ing.nutr[recom.nutr_no];
			double target = recom.GetValue(100);
			double rel = value / target - 1.0;
			if (rel < 0)
				score += rel;
		}
	}
	
	{
		double value = ing.nutr[CARB];
		double target = I * 0.2 / 4.0;
		double rel = value / target - 1.0;
		score -= fabs(rel) * 10;
	}
	
	{
		double value = ing.nutr[PROT];
		double target = J * 0.1 * 70; // TODO: use current weight
		double rel = value / target - 1.0;
		score -= fabs(rel) * 10;
	}
	
	return score;
}


void MealPreset::MakeVariants() {
	/*
	 - score
	 - easiest
	 - essential amino acids
	 - fatty acids
	 - weight loss
	 - muscle gain
	 - maintenance
	*/
	const Database& db = DB();
	
	typedef double (*VariantFn)(const MealPresetVariant& var);
	
	VariantFn eval;
	
	variants.SetCount(7);
	for(int i = 0; i < 7; i++) {
		MealPresetVariant& var = variants[i];
		
		var.ingredients <<= ingredients;
		
		double kcal_tgt = 1800;
		switch (i) {
			case 0: eval = &VariantScore<0>; var.name = "Score"; break;
			case 1: eval = &VariantScore<5>; var.name = "Easiest"; break;
			case 2: eval = &VariantNutritionGroup<AMINOACID>; var.name = "Amino Acids"; break;
			case 3: eval = &VariantNutritionGroup<FATTYACID>; var.name = "Fatty Acids"; break;
			case 4: eval = &VariantScore<-1>; var.name = "Weight Loss"; kcal_tgt = 600; break;
			case 5: eval = &VariantMaintenance<2000, 8>; var.name = "Muscle Gain"; kcal_tgt = 2000; break;
			case 6: eval = &VariantMaintenance<1800, 22>; var.name = "Maintenance"; break;
			default: Panic("Invalid variant code");
		}
		
		Optimizer opt;
		opt.Min().SetCount(var.ingredients.GetCount());
		opt.Max().SetCount(var.ingredients.GetCount());
		for(int i = 0; i < var.ingredients.GetCount(); i++) {
			const MealIngredient& mi = var.ingredients[i];
			opt.Min()[i] = mi.min_grams;
			opt.Max()[i] = mi.max_grams;
		}
		opt.SetMaxGenerations(100);
		opt.Init(var.ingredients.GetCount(), 100);
		
		while (!opt.IsEnd()) {
			opt.Start();
		
			const Vector<double>& trial = opt.GetTrialSolution();
			for(int i = 0; i < var.ingredients.GetCount(); i++) {
				const MealIngredient& main = ingredients[i];
				MealIngredient& ing = var.ingredients[i];
				double grams = max(main.min_grams, min(main.max_grams, trial[i]));
				ing.min_grams = grams;
				ing.max_grams = grams;
			}
			
			MealDebugger dbg;
			double energy = eval(var);
			
			//LOG(opt.GetRound() << ": " << energy);
			
			opt.Stop(energy);
		}
		
		Ingredient total;
		const Vector<double>& best = opt.GetBestSolution();
		for(int i = 0; i < var.ingredients.GetCount(); i++) {
			const MealIngredient& main = ingredients[i];
			MealIngredient& ing = var.ingredients[i];
			double grams = max(main.min_grams, min(main.max_grams, best[i]));
			ing.min_grams = grams;
			ing.max_grams = grams;
			const FoodDescription& d = db.food_descriptions[main.db_food_no];
			if (grams > 0)
				total.Add(grams, d);
		}
		double kcal_total = total.nutr[KCAL];
		double mul = kcal_tgt / kcal_total;
		for(auto& mi : var.ingredients) {
			mi.min_grams *= mul;
			mi.max_grams *= mul;
		}
		
		var.UpdateFactors();
	}
	
}

int MealPreset::FindIngredient(int food_no) const {
	for(int i = 0; i < ingredients.GetCount(); i++)
		if (ingredients[i].db_food_no == food_no)
			return i;
	return -1;
}
