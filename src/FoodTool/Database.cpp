#include "FoodTool.h"

Database::Database() {
	LoadThis();
}

void LoadLine(String line, Vector<String>& items) {
	items.SetCount(0);
	
	enum {
		EXPECTING_ITEM,
		READING_ITEM,
		SIMPLE_READ,
	};
	
	int mode = EXPECTING_ITEM;
	String* it = NULL;
	for(int i = 0; i < line.GetCount(); i++) {
		int chr = line[i];
		int peek_chr = i+1 < line.GetCount() ? line[i+1] : 0;
		
		if (mode == EXPECTING_ITEM) {
			if (chr == '~') {
				mode = READING_ITEM;
				it = &items.Add();
			}
			else if (chr == '^')
				items.Add(); // empty item
			else {
				mode = SIMPLE_READ;
				it = &items.Add();
				it->Cat(chr);
			}
		}
		else if (mode == READING_ITEM) {
			if (chr == '~' && peek_chr == '^') {
				i++;
				mode = EXPECTING_ITEM;
			}
			else if (chr == '~' && peek_chr == 0) {
				break;
			}
			else {
				it->Cat(chr);
			}
		}
		else if (mode == SIMPLE_READ) {
			if (chr == '^')
				mode = EXPECTING_ITEM;
			else
				it->Cat(chr);
		}
	}
	if (mode == EXPECTING_ITEM)
		items.Add();
}




bool Database::Init() {
	String dir = ConfigFile("db");
	
	//String data_src = LoadFile(AppendFileName(dir, "DATA_SRC.txt"));
	//String datsrcln = LoadFile(AppendFileName(dir, "DATSRCLN.txt"));
	//String deriv_cd = LoadFile(AppendFileName(dir, "DERIV_CD.txt"));
	String fd_group = LoadFile(AppendFileName(dir, "FD_GROUP.txt"));
	String food_des = LoadFile(AppendFileName(dir, "FOOD_DES.txt"));
	//String footnote = LoadFile(AppendFileName(dir, "FOOTNOTE.txt"));
	//String langdesc = LoadFile(AppendFileName(dir, "LANGDESC.txt"));
	//String langual = LoadFile(AppendFileName(dir, "LANGUAL.txt"));
	String nut_data = LoadFile(AppendFileName(dir, "NUT_DATA.txt"));
	String nutr_def = LoadFile(AppendFileName(dir, "NUTR_DEF.txt"));
	//String src_cd = LoadFile(AppendFileName(dir, "SRC_CD.txt"));
	//String weight = LoadFile(AppendFileName(dir, "WEIGHT.txt"));
	
	Vector<String> items;
	
	
	
	{
		Vector<String> lines = Split(fd_group, "\r\n");
		for(String line : lines) {
			LoadLine(line, items);
			
			FoodGroup& g = food_groups.Add(items[0]);
			g.name = items[1];
		}
	}
	
	Vector<int> nutr_no_trans;
	nutr_no_trans.SetCount(1000, -1);
	{
		Vector<String> lines = Split(nutr_def, "\r\n");
		for(String line : lines) {
			LoadLine(line, items);
			
			int no = StrInt(items[0]);
			nutr_no_trans[no] = nutr_types.GetCount();
			
			NutritionType& t = nutr_types.Add();
			t.is_valid = true;
			t.units = items[1];
			t.tagname = items[2];
			t.nutr_desc = items[3];
		}
	}
	
	{
		Vector<String> lines = Split(food_des, "\r\n");
		for(String line : lines) {
			LoadLine(line, items);
			//DUMPC(items);
			
			FoodDescription& s = food_descriptions.Add(items[0]);
			s.food_group_key = items[1];
			s.long_desc = items[2];
			s.short_desc = items[3];
			s.company_name = items[4];
			s.manufacturer_name = items[5];
			s.survey = items[6];
			s.ref_desc = items[7];
			s.refuse = StrInt(items[8]);
			s.sci_name = items[9];
			s.n_factor = StrDbl(items[10]);
			s.pro_factor = StrDbl(items[11]);
			s.fat_factor = StrDbl(items[12]);
			s.cho_factor = StrDbl(items[13]);
		}
	}
	
	{
		Vector<String> lines = Split(nut_data, "\r\n");
		for(String line : lines) {
			LoadLine(line, items);
			
			FoodDescription& s = food_descriptions.Get(items[0]);
			NutritionInfo& n = s.nutr.Add();
			
			n.nutr_no = nutr_no_trans[StrInt(items[1])];
			n.nutr_value = StrDbl(items[2]);
			n.std_error = StrDbl(items[4]);
			ASSERT(n.nutr_no < nutr_types.GetCount());
		}
	}
	
	
	#if 0
	
	
	// https://wholefoodcatalog.info/nutrient/vitamin_b7(biotin)/vegetables/
	int b7 = AddNutrition("B7-Vitamin Biotin", "mg", "");
	// broccoli 13.5ug / 290g
	// cauli 28.1ug / 660g
	// pea 0.3ug / 5g
	// spinach 7.6ug / 250g
	// lettuce 7ug / 257g
	// carrot 3.6ug / 130g
	// eggplant 3.3ug / 160g
	// tomato 3.9ug / 220g
	// garlic 1ug / 55g
	// asparagus 1.4ug / 95g
	// cabbage 23.ug / 1700g
	// chinese cabbage 27.6ug /2100g
	// cucumber 1.6ug / 120g
	// onion 1ug / 200g
	
	int chrom = AddNutrition("Chromium", "mg", "");
	// Broccoli	1 cup	54.6	18.55ug
	// Green Beans	1 cup	43.8	2.04ug
	// Tomatoes	1 cup	32.4	1ug
	
	int moly = AddNutrition("Molybdenum", "mg", "");
	// Lentils	1 cup	229.7	148.50ug
	// Kidney Beans	1 cup	224.8	132.75ug
	// Soybeans	1 cup	297.6	129.00ug
	// Black Beans	1 cup	227.0	129.00ug
	// Lima Beans	1 cup	216.2	141.00ug
	// Tomatoes	1 cup	32.4	9.00ug
	// Cucumber	1 cup	15.6	5.20ug
	// Carrots	1 cup	50.0	6.10ug
	// Bell Peppers	1 cup	28.5	4.60ug
	// Peanuts	0.25 cup	206.9	10.77ug
	// Sesame Seeds	0.25 cup	206.3	10.62ug
	// Walnuts	0.25 cup	196.2	8.85ug
	// Green Peas	1 cup	115.7	6.89ug
	// Almonds	0.25 cup	132.2	6.78ug
	
	// https://www.tandfonline.com/doi/pdf/10.1080/10942910701584252
	int sili = AddNutrition("Silicon", "mg", "");
	// beans 5577ug / 100g
	// spinach 1782ug / 100g
	// banana 6195ug / 100g
	
	// http://cookcal.info/en/foodstuffs/content/348
	int brom = AddNutrition("Bromine", "mg", "");
	// Ginger 0.0468mg / 100g
	// Lentils 0.0033mg / 100g
	
	// https://www.torrinomedica.it/approfondimenti/dietologia/nickel-in-foods/
	int nick = AddNutrition("Nickel", "mg", "");
	// Almonds	130.0ug /100g
	// Apple	2.0ug /100g
	// Apricots, dried	120.0ug /100g
	// Avocado	100.0ug /100g
	// Banana	7.0ug /100g
	// Beets	5.0ug /100g
	// Blackcurrant	10.0ug /100g
	// Broccoli	10.0ug /100g
	// Brussels Sprouts	3.0ug /100g
	// Cabbages	4.0ug /100g
	// Carrots	5.0ug /100g
	// Cauliflower	4.0ug /100g
	// Chickpeas, dried	130.0ug /100g
	// Cocoa powder	855.0ug /100g
	// Hazelnuts	170.0ug /100g
	// Lentils, dried	248.0ug /100g
	// Olives	9.0ug /100g
	// Onions	3.0ug /100g
	// Orange	4.0ug /100g
	// Peanuts	390.0ug /100g
	// Peas	22.0ug /100g
	// Pineapple	50.0ug /100g
	// Red lentils, dried	292.0ug /100g
	// Soybean oil	6.0ug /100g
	// Spinach	3.0ug /100g
	// Sunflower seeds	356.0ug /100g
	// Tomatoes	10.0ug /100g
	// Walnut	320.0ug /100g
	
	// https://www.greenfacts.org/en/boron/toolboxes/2.htm
	int boron = AddNutrition("Boron", "mg", "");
	// mg/ kg <-- !!!
	// Apple, red with peel, raw 	2.73
	// Banana, raw 	- 	3.72 
	// Dates 	9.2
	// Beans, green 		1.56
	// Broccoli, flowers 	1.85 
	// Lettuce, iceberg 	<0.015
	// Carrots, canned 	0.75 
	// Almonds 	23
	// Hazelnuts 	16
	// Peanuts 	18
	
	// http://cookcal.info/en/foodstuffs/content/427?group=30
	int lith = AddNutrition("Lithium", "mg", "");
	// Potatoes 0.077mg/100g
	// Tomatoes 0.029mg/100g
	// Carrot  0.006mg/100g
	// Ginger 0.00079mg/100g
	
	#endif
	
	
	is_init = true;
	
	StoreThis();
	
	return true;
}

int Database::AddNutrition(String name, String unit, String desc) {
	int i = nutr_types.GetCount();
	NutritionType& t = nutr_types.Add();
	t.is_valid = true;
	t.is_user_added = true;
	t.tagname = name;
	t.units = unit;
	t.nutr_desc = desc;
	return i;
}

void Database::VLCD_Preset() {
	used_foods.Add(2862);
	used_foods.Add(2870);
	used_foods.Add(2878);
	used_foods.Add(2887);
	used_foods.Add(2982);
	used_foods.Add(3001);
	used_foods.Add(3002);
	used_foods.Add(3578);
	used_foods.Add(3051);
	used_foods.Add(3166);
	used_foods.Add(3238);
	used_foods.Add(2155);
	used_foods.Add(3437);
	used_foods.Add(2957);
	used_foods.Add(2961);
	used_foods.Add(2280);
	used_foods.Add(3222);
	used_foods.Add(2907);
	used_foods.Add(4794);
	used_foods.Add(4806);
	used_foods.Add(4875);
	used_foods.Add(2823);
	used_foods.Add(2967);
	used_foods.Add(3029);
	used_foods.Add(2900);
	used_foods.Add(2968);
	used_foods.Add(3094);
	used_foods.Add(3150);
	used_foods.Add(291);
	used_foods.Add(3490);
	used_foods.Add(4753);
	used_foods.Add(4755);
	used_foods.Add(4809);
	used_foods.Add(4993);
	used_foods.Add(4966);
	used_foods.Add(4979);
	used_foods.Add(4973);
	used_foods.Add(3187);
	used_foods.Add(2122);
	used_foods.Add(2325);
	used_foods.Add(2150);
	used_foods.Add(2423);
	used_foods.Add(2270);
	used_foods.Add(2310);
	used_foods.Add(2352);
	used_foods.Add(2299);
	used_foods.Add(2222);
	used_foods.Add(2254);
	used_foods.Add(2283);
	used_foods.Add(2194);
	used_foods.Add(2165);
	used_foods.Add(2190);
	used_foods.Add(2240);
	used_foods.Add(2252);
	used_foods.Add(2373);
	used_foods.Add(2384);
	used_foods.Add(2392);
	used_foods.Add(2275);
	used_foods.Add(2158);
	used_foods.Add(2304);
	used_foods.Add(2307);
	used_foods.Add(2339);
	used_foods.Add(2358);
	used_foods.Add(2376);
	used_foods.Add(3008);
	used_foods.Add(3634);
	used_foods.Add(3644);
	used_foods.Add(3665);
	used_foods.Add(4824);
	used_foods.Add(3619);
	used_foods.Add(3625);
	used_foods.Add(3609);
	used_foods.Add(3715);
	used_foods.Add(8604);
	used_foods.Add(3689);
	used_foods.Add(657);
	used_foods.Add(4858);
	used_foods.Add(658);
	used_foods.Add(3663);
	used_foods.Add(725);
	used_foods.Add(4853);
	used_foods.Add(6452);
	used_foods.Add(6194);
	used_foods.Add(3435);
	used_foods.Add(659);
	used_foods.Add(662);
	used_foods.Add(661);
	used_foods.Add(685);
	used_foods.Add(716);

	
	SortIndex(used_foods, StdLess<int>());
}













