#include "FoodCtrl.h"


String MultipurposeGraph::GetLineTitle(int s, int l) {return src[s].lines[l].title;}
int MultipurposeGraph::GetLineCount(int s) {return src[s].lines.GetCount();}
Color MultipurposeGraph::GetLineColor(int s, int l) {return src[s].lines[l].color;}
int MultipurposeGraph::GetLineWidth(int s, int l) {return src[s].lines[l].width;}

#define MEASURE_FORECAST 10

int MultipurposeGraph::GetCount(int s) {
	if (s < FOOD_BEGIN)
		return GetProfile().planned_daily.GetCount();
	else if (s < MEASURES_BEGIN)
		return GetProfile().storage.days.GetCount();
	else if (s < GRAPH_COUNT)
		return GetProfile().weights.GetCount() + MEASURE_FORECAST;
	Panic("Invalid source");
	return -1;
}

const Vector<double>& MultipurposeGraph::GetValue(int src, int l) {
	if (tmp.GetCount() <= l)
		tmp.SetCount(l+1);
	
	const Profile& prof = GetProfile();
	const Configuration& conf = prof.confs.Top();
	const Database& db = DB();
	
	Vector<double>& v = tmp[l];
	if (v.IsEmpty()) {
		v.SetCount(GetCount(src));
		int src_i = 0;
		
		if (src == src_i++) {
			if (l == 0) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].lean_kgs;
			}
			else if (l == 1) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].fat_kgs;
			}
			else if (l == 2) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].weight;
			}
		}
		else if (src == src_i++) {
			if (l == 0) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].maintain_calories;
			}
			else if (l == 1) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].burned_calories;
			}
			else if (l == 2) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].allowed_calories;
			}
		}
		else if (src == src_i++) {
			if (l == 0) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].burned_kgs;
			}
		}
		else if (src == src_i++) {
			int n;
			if (l == 0) {
				n = FAT;
			}
			else if (l == 1) {
				n = CARB;
			}
			else if (l == 2) {
				n = PROT;
			}
			for(int i = 0; i < prof.planned_daily.GetCount(); i++)
				v[i] = prof.planned_daily[i].food.nutr[n];
		}
		
		else if (src == src_i++) {
			int n = KCAL;
			if (l == 0) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].target_sum.nutr[n];
			}
			else if (l == 1) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].total_sum.nutr[n];
			}
		}
		else if (src == src_i++) {
			if (l == 0) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].target_sum.grams;
			}
			else if (l == 1) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].total_sum.grams;
			}
		}
		else if (src == src_i++) {
			int n = -1;
			if (l == 0 || l == 3) {
				n = FAT;
			}
			else if (l == 1 || l == 4) {
				n = CARB;
			}
			else if (l == 2 || l == 5) {
				n = PROT;
			}
			if (l < 3) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].target_sum.nutr[n];
			}
			else {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].total_sum.nutr[n];
			}
		}
		else if (src == src_i++) {
			int n = SODIUM;
			if (l == 0) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].target_sum.nutr[n];
			}
			else if (l == 1) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].total_sum.nutr[n];
			}
		}
		
		else if (src == src_i++) {
			String key;
			switch (l) {
				case 0: key = ("VITA_RAE"); break;
				case 1: key = ("THIA"); break;
				case 2: key = ("RIBF"); break;
				case 3: key = ("NIA"); break;
				case 4: key = ("PANTAC"); break;
				case 5: key = ("VITB6A"); break;
				case 6: key = ("FOL"); break;
				case 7: key = ("VITB12"); break;
				case 8: key = ("VITC"); break;
				case 9: key = ("VITDMCR"); break;
				case 10: key = ("TOCPHA"); break;
				case 11: key = ("VITK1"); break;
				case 12: key = ("CARTB"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		else if (src == src_i++) {
			String key;
			switch (l) {
				case 0: key = ("HISTN_G"); break;
				case 1: key = ("ILE_G"); break;
				case 2: key = ("LEU_G"); break;
				case 3: key = ("LYS_G"); break;
				case 4: key = ("MET_G"); break;
				case 5: key = ("PHE_G"); break;
				case 6: key = ("THR_G"); break;
				case 7: key = ("TRP_G"); break;
				case 8: key = ("VAL_G"); break;
				case 9: key = ("ARG_G"); break;
				case 10: key = ("GLU_G"); break;
				case 11: key = ("GLY_G"); break;
				case 12: key = ("PRO_G"); break;
				case 13: key = ("TYR_G"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		else if (src == src_i++) {
			String key;
			switch (l) {
				case 0: key = ("K"); break;
				case 1: key = ("NA"); break;
				case 2: key = ("CA"); break;
				case 3: key = ("P"); break;
				case 4: key = ("MG"); break;
				case 5: key = ("FE"); break;
				case 6: key = ("ZN"); break;
				case 7: key = ("MN"); break;
				case 8: key = ("CU"); break;
				case 9: key = ("SE"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		else if (src == src_i++) {
			String key;
			switch (l) {
				case 0: key = ("CHOLE"); break;
				case 1: key = ("FATRN"); break;
				case 2: key = ("FASAT"); break;
				case 3: key = ("FAMS"); break;
				case 4: key = ("FAPU"); break;
				case 5: key = ("FATRNM"); break;
				case 6: key = ("FATRNP"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		else if (src == src_i++) {
			String key;
			switch (l) {
				case 0: key = ("CHOLN"); break;
				case 1: key = ("FIBTG"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		else if (src == src_i++) {
			String key;
			switch (l) {
				case 0: key = ("CA"); break;
				case 1: key = ("MG"); break;
				case 2: key = ("P"); break;
				case 3: key = ("K"); break;
				case 4: key = ("NA"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		
		else if (src == src_i++) {
			if (l == 0) {
				int i = 0;
				for(; i < prof.weights.GetCount(); i++) {
					const WeightLossStat& s = prof.weights[i];
					v[i] = s.weight;
				}
				
				const DailyPlan* p = prof.planned_daily.Begin();
				const DailyPlan* end = prof.planned_daily.End();
				
				Date begin = Date(prof.weights.Top().added) + 1;
				while (p != end) {
					if (p->date >= begin)
						break;
					p++;
				}
				int count = 0;
				while (p != end) {
					v[i + count++] = p->weight;
					v[i + count++] = p->weight;
					if (count >= MEASURE_FORECAST)
						break;
					p++;
				}
			}
		}
		else if (src == src_i++) {
			if (l == 0) {
				int i = 0;
				for(; i < prof.weights.GetCount(); i++) {
					const WeightLossStat& s = prof.weights[i];
					v[i] = s.weight * s.fat * 0.01;
				}
				
				const DailyPlan* p = prof.planned_daily.Begin();
				const DailyPlan* end = prof.planned_daily.End();
				
				Date begin = Date(prof.weights.Top().added) + 1;
				while (p != end) {
					if (p->date >= begin)
						break;
					p++;
				}
				int count = 0;
				while (p != end) {
					v[i + count++] = p->fat_kgs;
					v[i + count++] = p->fat_kgs;
					if (count >= MEASURE_FORECAST)
						break;
					p++;
				}
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				#define MEAS(var) \
				for(int i = 0; i < prof.weights.GetCount(); i++) \
					v[i] = prof.weights[i].var; \
				double diff = (prof.weights.Top().var - prof.weights[0].var) / (prof.weights.GetCount()-1); \
				for(int i = prof.weights.GetCount(); i < v.GetCount(); i++) \
					v[i] = v[i-1] + diff;
				MEAS(GetLiquidKg());
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				int i = 0;
				for(; i < prof.weights.GetCount(); i++) {
					const WeightLossStat& s = prof.weights[i];
					v[i] = s.weight * s.muscle * 0.01;
				}
				
				const DailyPlan* p = prof.planned_daily.Begin();
				const DailyPlan* end = prof.planned_daily.End();
				
				Date begin = Date(prof.weights.Top().added) + 1;
				while (p != end) {
					if (p->date >= begin)
						break;
					p++;
				}
				int count = 0;
				while (p != end) {
					v[i + count++] = p->lean_kgs;
					v[i + count++] = p->lean_kgs;
					if (count >= MEASURE_FORECAST)
						break;
					p++;
				}
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(GetInternalKg());
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			#define MEAS_STEP_CHANGE(var) \
				v[0] = 0.0;\
				for(int i = 1; i < prof.weights.GetCount(); i++) \
					v[i] = prof.weights[i].var / prof.weights[i-1].var * 100 - 100; \
				double diff = (prof.weights.Top().var - prof.weights[0].var) / (prof.weights.GetCount()-1); \
				for(int i = prof.weights.GetCount(); i < v.GetCount(); i++) \
					v[i] = v[i-1] + diff;
			if (l == 0) {
				MEAS_STEP_CHANGE(GetFatKg());
			}
			else if (l == 1) {
				MEAS_STEP_CHANGE(GetMuscleKg());
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			#define MEAS_TOTAL_CHANGE(var) \
				v[0] = 0.0;\
				for(int i = 1; i < prof.weights.GetCount(); i++) \
					v[i] = prof.weights[i].var / prof.weights[0].var * 100 - 100; \
				double diff = (prof.weights.Top().var - prof.weights[0].var) / (prof.weights.GetCount()-1); \
				for(int i = prof.weights.GetCount(); i < v.GetCount(); i++) \
					v[i] = v[i-1] + diff;
			if (l == 0) {
				MEAS_TOTAL_CHANGE(GetFatKg());
			}
			else if (l == 1) {
				MEAS_TOTAL_CHANGE(GetMuscleKg());
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(fat);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(liquid);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(muscle);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(GetInternalPerc());
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				int count = prof.weights.GetCount();
				for(int i = 0; i < count; i++) {
					const WeightLossStat& wl = prof.weights[i];
					double normal = GetNormalLiquidPercentage(conf.age, conf.height, wl.weight);
					v[i] = wl.liquid / normal * 100;
				}
				double diff = (v[count-1] - v[0]) / (count-1); \
				for(int i = count; i < v.GetCount(); i++) \
					v[i] = v[i-1] + diff;
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS_STEP_CHANGE(fat);
			}
			else if (l == 1) {
				MEAS_STEP_CHANGE(muscle);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS_TOTAL_CHANGE(fat);
			}
			else if (l == 1) {
				MEAS_TOTAL_CHANGE(muscle);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				int i = 0;
				for(; i < prof.weights.GetCount(); i++) {
					const WeightLossStat& s = prof.weights[i];
					v[i] = GetBMI(conf.height, s.weight);
				}
				
				const DailyPlan* p = prof.planned_daily.Begin();
				const DailyPlan* end = prof.planned_daily.End();
				
				Date begin = Date(prof.weights.Top().added) + 1;
				while (p != end) {
					if (p->date >= begin)
						break;
					p++;
				}
				int count = 0;
				while (p != end) {
					double bmi = GetBMI(conf.height, p->weight);
					v[i + count++] = bmi;
					v[i + count++] = bmi;
					if (count >= MEASURE_FORECAST)
						break;
					p++;
				}
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(neck);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(bicep);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(forearm);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(chest);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(waist);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(buttocks);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(thigh);
			}
			FillVector(v);
		}
		else if (src == src_i++) {
			if (l == 0) {
				MEAS(leg);
			}
			FillVector(v);
		}
	}
	return v;
}

int MultipurposeGraph::GetVertLine(int s) {
	Src& ss = src[s];
	return ss.vert_line;
}

int MultipurposeGraph::GetHorzLine(int s) {
	const Profile& prof = GetProfile();
	if (s < FOOD_BEGIN) {
		Date today = GetSysTime();
		for(int i = 0; i < prof.planned_daily.GetCount(); i++)
			if (prof.planned_daily[i].date == today)
				return i;
		return 0;
	}
	else if (s < MEASURES_BEGIN) {
		Date today = GetSysTime();
		for(int i = 0; i < prof.storage.days.GetCount(); i++)
			if (prof.storage.days[i].date == today)
				return i;
		return 0;
	}
	else if (s < GRAPH_COUNT)
		return prof.weights.GetCount()-1;
	Panic("Invalid source");
	return -1;
}

void MultipurposeGraph::Paint(Draw& d) {
	int src = new_src;
	for(auto& a : tmp)
		a.SetCount(0);
	
	USize sz(GetSize());
	d.DrawRect(sz, White());
	
	int vert = GetVertLine(src);
	
	bool get_min = vert || src >= MEASURES_BEGIN;
	
	double min = get_min ? +DBL_MAX : 0;
	double max = -DBL_MAX;
	double last = 0.0;
	
	
	int count = GetCount(src);
	int line_count = GetLineCount(src);
	for(int l = 0; l < line_count; l++) {
		const auto& values = GetValue(src, l);
		for(int j = 0; j < count; j++) {
			double d = values[j];
			if (d > max)
				max = d;
			if (get_min && d < min)
				min = d;
		}
	}
	
	Font fnt = Monospace(10);
	
	if (count > 1 && max >= min) {
		double diff = max - min;
		double xstep = (double)sz.cx / (count - 1);
		
		int xoff = 0; // 16
		int text_y = 0;
		
		for(int l = 0; l < line_count; l++) {
			double line_max = -DBL_MAX;
			double line_min = +DBL_MAX;
			const auto& values = GetValue(src, l);
			
			polyline.SetCount(0);
			for(int j = 0; j < count; j++) {
				double v = values[j];
				last = v;
				int x = (int)(j * xstep);
				int y = (int)(sz.cy - (v - min) / diff * sz.cy);
				polyline.Add(Point(x, y));
				if (v > line_max) line_max = v;
				if (v < line_min) line_min = v;
			}
			if (polyline.GetCount() >= 2) {
				Color clr = GetLineColor(src, l);
				d.DrawPolyline(polyline, GetLineWidth(src, l), clr);
				
				String txt = GetLineTitle(src, l) + " - min " + Format("%2n", line_min) + " - max " + Format("%2n", line_max);
				USize str_sz = GetTextSize(txt, fnt);
				d.DrawRect(xoff, text_y, str_sz.cx, str_sz.cy, White());
				d.DrawText(xoff, text_y, txt, fnt.Bold(), Blend(clr, Black()));
				text_y += 10;
			}
		}
		
		if (vert) {
			int y = (int)(sz.cy - (vert - min) / diff * sz.cy);
			d.DrawLine(0, y, sz.cx, y, 1, Black());
		}
		
		int horz = GetHorzLine(src);
		if (horz) {
			int x = (int)(horz * xstep);
			d.DrawLine(x, 0, x, sz.cy, 1, Black());
		}
	}
	else {
		String txt = t_("Waiting for data");
		USize txt_sz = GetTextSize(txt, fnt);
		d.DrawText((sz.cx - txt_sz.cx) / 2, (sz.cy - txt_sz.cy) / 2, txt, fnt);
	}
}
