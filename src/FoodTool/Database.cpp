#include "FoodTool.h"

int KCAL, PROT, FAT, CARB, SODIUM;



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
		FoodGroup& other = food_groups.Add("OTHER");
		other.name = "Other";
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
	
	AddRecommendation("PROCNT", 0.8, true);
	AddRecommendation("FAT", 60.5 / 65.0, true);
	AddRecommendation("CA", 1000 / 65.0, true);
	AddRecommendation("FE", 19.8 / 65.0, true);
	AddRecommendation("MG", 420 / 65.0, true);
	AddRecommendation("P", 700 / 65.0, true);
	AddRecommendation("K", 4100 / 65.0, true);
	AddRecommendation("NA", 1900 / 65.0, true);
	AddRecommendation("ZN", 11 / 65.0, true);
	AddRecommendation("CU", 1 / 65.0, true);
	AddRecommendation("FLD", 50, true);
	AddRecommendation("MN", 2.3, true);
	AddRecommendation("SE", 100 / 65.0, true);
	AddRecommendation("VITA_IU", 3000, true);
	AddRecommendation("RETOL", 900 / 65.0, true);
	AddRecommendation("VITA_RAE", 900 / 65.0, true);
	AddRecommendation("CARTB", 15000 / 65.0, true);
	AddRecommendation("CARTA", 600 / 65.0, true);
	AddRecommendation("TOCPHA", 15 / 65.0, true);
	AddRecommendation("VITD", 800 / 65.0, true);
	AddRecommendation("VITDMCR", 100 / 65.0, true);
	AddRecommendation("LYCPN", 21000 / 65.0, true);
	AddRecommendation("VITC", 90 / 65.0, true);
	AddRecommendation("THIA", 1.2 / 65.0, true);
	AddRecommendation("RIBF", 3 / 65.0, true);
	AddRecommendation("NIA", 16 / 65.0, true);
	AddRecommendation("PANTAC", 5 / 65.0, true);
	AddRecommendation("VITB6A", 1.3 / 65.0, true);
	AddRecommendation("FOL", 400 / 65.0, true);
	AddRecommendation("VITB12", 1.8 / 65.0, true);
	AddRecommendation("CHOLN", 500 / 65.0, true);
	AddRecommendation("MK4", 92.5 / 65.0, true);
	AddRecommendation("VITK1D", 120 / 65.0, true);
	AddRecommendation("FOLAC", 400 / 65.0, true);
	AddRecommendation("BETN", 550 / 65.0, true);
	AddRecommendation("TRP_G", 0.006, true);
	AddRecommendation("THR_G", 0.015, true);
	AddRecommendation("ILE_G", 1.4 / 65.0, true);
	AddRecommendation("LEU_G", 2.7 / 65.0, true);
	AddRecommendation("LYS_G", 1 / 65.0, true);
	AddRecommendation("MET_G", 0.019, true);
	AddRecommendation("PHE_G", 0.026, true);
	AddRecommendation("TYR_G", 6 / 65.0, true);
	AddRecommendation("VAL_G", 1.8 / 65.0, true);
	AddRecommendation("ARG_G", 6 / 65.0, true);
	AddRecommendation("HISTN_G", 1 / 65.0, true);
	AddRecommendation("ALA_G", 3.5 / 65.0, true);
	AddRecommendation("ASP_G", 3 / 65.0, true);
	AddRecommendation("GLU_G", 15 / 65.0, true);
	//AddRecommendation("GLY_G", 5 / 65.0, true);
	AddRecommendation("GLY_G", 5 / 65.0, true);
	
	
	
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

FoodDescription& Database::AddFood(String fg, String l, String s, String c, String m, String su, String r, int re, String sci, float nf, float pf, float ff, float cf) {
	String key;
	for(int i = 0; i < 8; i++)
		key.Cat('A' + Random('Z' - 'A' + 1));
	
	FoodDescription& d = food_descriptions.Add(key);
	d.food_group_key = fg;
	d.long_desc = l;
	d.short_desc = s;
	d.company_name = c;
	d.manufacturer_name = m;
	d.survey = su;
	d.ref_desc = r;
	d.refuse = re;
	d.sci_name = sci;
	d.n_factor = nf;
	d.pro_factor = pf;
	d.fat_factor = ff;
	d.cho_factor = cf;
	d.is_user_added = true;
	return d;
}

FoodDescription& FoodDescription::AddNutrition(int nutr_no, double value, double error) {
	NutritionInfo& i = nutr.Add();
	i.nutr_no = nutr_no;
	i.nutr_value = value;
	i.std_error = error;
	return *this;
}

void Database::VLCD_Preset() {
	AddFood("OTHER", "Salt, iodized", "", "Meira", "", "", "", 0, "", 0, 0, 0, 0)
		.AddNutrition(25, 38758, 0);
	AddFood("0100", "Magnex sitraatti + B6- vitamiini", "", "", "", "", "", 0, "", 0, 0, 0, 0)
		.AddNutrition(29, 12254, 0)
		.AddNutrition(56, 98, 0);
	AddFood("0100", "Reformi 55g B-vitamiini", "", "", "", "", "", 0, "", 0, 0, 0, 0)
		.AddNutrition(52, 6363, 0)
		.AddNutrition(53, 6363, 0)
		.AddNutrition(54, 6363, 0)
		.AddNutrition(55, 6363, 0)
		.AddNutrition(56, 4545, 0)
		.AddNutrition(58, 1818, 0)
		.AddNutrition(63, 109090, 0);
	AddFood("0100", "Calcia 800 Plus kalkki-monivitamiini-monimineraalitabletti", "", "", "", "", "", 0, "", 0, 0, 0, 0)
		.AddNutrition(56, 2.20000004768372, 0)
		.AddNutrition(51, 60, 0)
		.AddNutrition(40, 10, 0)
		.AddNutrition(62, 70, 0)
		.AddNutrition(20, 800, 0)
		.AddNutrition(63, 300, 0)
		.AddNutrition(22, 350, 0)
		.AddNutrition(29, 2.5, 0)
		.AddNutrition(51, 60, 0)
		.AddNutrition(26, 15, 0);
	AddFood("0100", "Rainbow Vahva C-vitamiini 500 mg 90 tablettia", "", "", "", "", "", 0, "", 0, 0, 0, 0)
		.AddNutrition(51, 64935, 0);
	AddFood("0100", "Rainbow 72g monivitamiini-hivenainetabletti", "", "", "", "", "", 0, "", 0, 0, 0, 0)
		.AddNutrition(53, 1.60000002384186, 0)
		.AddNutrition(52, 1.39999997615814, 0)
		.AddNutrition(58, 2.5, 0)
		.AddNutrition(56, 2.20000004768372, 0)
		.AddNutrition(51, 80, 0)
		.AddNutrition(40, 10, 0)
		.AddNutrition(44, 0.0120000001043081, 0)
		.AddNutrition(22, 120, 0)
		.AddNutrition(29, 2, 0)
		.AddNutrition(54, 16, 0)
		.AddNutrition(55, 6, 0)
		.AddNutrition(30, 55, 0)
		.AddNutrition(51, 80, 0)
		.AddNutrition(40, 10, 0)
		.AddNutrition(36, 12, 0)
		.AddNutrition(26, 10, 0);
	AddFood("0100", "Makrobios B-12 vitamiini+folaatti ", "", "", "", "", "", 0, "", 0, 0, 0, 0)
		.AddNutrition(58, 285000, 0)
		.AddNutrition(63, 57142, 0);
	AddFood("0100", "Reformi Multivitamin 100tabl ", "", "", "", "", "", 0, "", 0, 0, 0, 0)
		.AddNutrition(40, 510, 0)
		.AddNutrition(36, 1020, 0)
		.AddNutrition(51, 6122, 0)
		.AddNutrition(52, 142, 0)
		.AddNutrition(53, 163, 0)
		.AddNutrition(54, 1836, 0)
		.AddNutrition(56, 224, 0)
		.AddNutrition(63, 30612, 0)
		.AddNutrition(58, 306, 0)
		.AddNutrition(55, 408, 0)
		.AddNutrition(20, 20408, 0)
		.AddNutrition(22, 9183, 0)
		.AddNutrition(26, 1530, 0)
		.AddNutrition(30, 5102, 0);
	used_foods.Add(291);
	used_foods.Add(657);
	used_foods.Add(658);
	used_foods.Add(659);
	used_foods.Add(661);
	used_foods.Add(662);
	used_foods.Add(685);
	used_foods.Add(716);
	used_foods.Add(725);
	used_foods.Add(2122);
	used_foods.Add(2150);
	used_foods.Add(2155);
	used_foods.Add(2158);
	used_foods.Add(2165);
	used_foods.Add(2190);
	used_foods.Add(2194);
	used_foods.Add(2222);
	used_foods.Add(2240);
	used_foods.Add(2252);
	used_foods.Add(2254);
	used_foods.Add(2270);
	used_foods.Add(2275);
	used_foods.Add(2280);
	used_foods.Add(2283);
	used_foods.Add(2299);
	used_foods.Add(2304);
	used_foods.Add(2307);
	used_foods.Add(2310);
	used_foods.Add(2325);
	used_foods.Add(2339);
	used_foods.Add(2352);
	used_foods.Add(2358);
	used_foods.Add(2373);
	used_foods.Add(2376);
	used_foods.Add(2384);
	used_foods.Add(2392);
	used_foods.Add(2423);
	used_foods.Add(2823);
	used_foods.Add(2862);
	used_foods.Add(2870);
	used_foods.Add(2878);
	used_foods.Add(2887);
	used_foods.Add(2900);
	used_foods.Add(2907);
	used_foods.Add(2957);
	used_foods.Add(2961);
	used_foods.Add(2967);
	used_foods.Add(2968);
	used_foods.Add(2982);
	used_foods.Add(3001);
	used_foods.Add(3002);
	used_foods.Add(3008);
	used_foods.Add(3029);
	used_foods.Add(3051);
	used_foods.Add(3094);
	used_foods.Add(3150);
	used_foods.Add(3166);
	used_foods.Add(3187);
	used_foods.Add(3222);
	used_foods.Add(3238);
	used_foods.Add(3435);
	used_foods.Add(3437);
	used_foods.Add(3490);
	used_foods.Add(3578);
	used_foods.Add(3609);
	used_foods.Add(3619);
	used_foods.Add(3625);
	used_foods.Add(3634);
	used_foods.Add(3644);
	used_foods.Add(3663);
	used_foods.Add(3665);
	used_foods.Add(3689);
	used_foods.Add(3715);
	used_foods.Add(4753);
	used_foods.Add(4755);
	used_foods.Add(4794);
	used_foods.Add(4806);
	used_foods.Add(4809);
	used_foods.Add(4824);
	used_foods.Add(4853);
	used_foods.Add(4858);
	used_foods.Add(4875);
	used_foods.Add(4966);
	used_foods.Add(4973);
	used_foods.Add(4979);
	used_foods.Add(4993);
	used_foods.Add(6194);
	used_foods.Add(6452);
	used_foods.Add(8604);
	used_foods.Add(8789);
	used_foods.Add(8790);
	used_foods.Add(8792);
	used_foods.Add(8793);
	used_foods.Add(8794);
	used_foods.Add(8795);
	used_foods.Add(8796);
	used_foods.Add(8791);


	
	SortIndex(used_foods, StdLess<int>());
}

int Database::FindNutrition(String key) const {
	for(int i = 0; i < nutr_types.GetCount(); i++)
		if (nutr_types[i].tagname == key)
			return i;
	return -1;
}

void Database::SetCommonNutrs() {
	PROT = FindNutrition("PROCNT");
	FAT = FindNutrition("FAT");
	CARB = FindNutrition("CHOCDF");
	KCAL = FindNutrition("ENERC_KCAL");
	SODIUM = FindNutrition("NA");
	ASSERT(PROT >= 0 && FAT >= 0 && CARB >= 0 && KCAL >= 0 && SODIUM >= 0);
}









