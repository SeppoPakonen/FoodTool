#include "FoodCtrl.h"

#define GRADCOLOR(x) Rainbow(x)

GraphCtrl::GraphCtrl() {
	Add(split.SizePos());
	split.Horz() << list << graph;
	split.SetPos(2500);
	
	list.AddColumn(t_("Type"));
	
	list.Add(t_("Planned weight"));
	graph.Add()
		.Add(t_("Lean body weight"), 1, Color(28, 212, 150))
		.Add(t_("Fat body weight"), 1, Color(170, 85, 0))
		.Add(t_("Weight"), 2, Color(28, 127, 150));
	
	list.Add(t_("Planned calories"));
	graph.Add()
		.Add(t_("Maintain calories"), 1, Color(113, 42, 255))
		.Add(t_("Burned calories"), 1, Color(198, 42, 0))
		.Add(t_("Allowed calories"), 2, Color(28, 170, 0));
	
	list.Add(t_("Planned burned weight"));
	graph.Add()
		.Add(t_("Burned mass"), 2, Color(0, 120, 93));
	
	list.Add(t_("Planned macronutrients"));
	graph.Add()
		.Add(t_("Fat mass"), 2, Color(148, 146, 0))
		.Add(t_("Carbohydrate mass"), 2, Color(0, 116, 66))
		.Add(t_("Protein mass"), 2, Color(94, 0, 0));
	
	list.Add(t_("Menu calories"));
	graph.Add()
		.Add(t_("Allowed calories"), 1, Color(28, 85, 255))
		.Add(t_("Eaten calories"), 2, Color(0, 136, 94));
	
	list.Add(t_("Menu food mass"));
	graph.Add()
		.Add(t_("Planned mass"), 1, Color(85, 42, 0))
		.Add(t_("Eaten mass"), 2, Color(85, 85, 150));
		
	list.Add(t_("Menu macronutrients"));
	graph.Add()
		.Add(t_("Planned fat mass"), 1, Color(148, 146, 0))
		.Add(t_("Planned carbohydrate mass"), 1, Color(0, 116, 66))
		.Add(t_("Planned protein mass"), 1, Color(94, 0, 0))
		.Add(t_("Eaten fat mass"), 2, Color(168, 166, 20))
		.Add(t_("Eaten carbohydrate mass"), 2, Color(20, 136, 86))
		.Add(t_("Eaten protein mass"), 2, Color(114, 20, 20));
	
	list.Add(t_("Menu sodium (salt)"));
	graph.Add()
		.Add(t_("Planned sodium"), 1, GrayColor())
		.Add(t_("Eaten sodium"), 2, GrayColor(128+32));
	
	
	
	int i = 0;
	int vitamin_count = 13;
	list.Add(t_("Menu vitamins (\% of recommended)"));
	graph.Add().Vert(100)
		.Add("A", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("B1", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("B2", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("B3", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("B5", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("B6", 1, GRADCOLOR((double)i++ / vitamin_count))
		//.Add("B7", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("B9", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("B12", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("C", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("D", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("E", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("K", 1, GRADCOLOR((double)i++ / vitamin_count))
		.Add("Carotenoids", 1, GRADCOLOR((double)i++ / vitamin_count))
		;
	
	
	i = 0;
	int aminoacid_count = 14;
	list.Add(t_("Menu essential amino acids (\% of recommended)"));
	graph.Add().Vert(100)
		.Add(t_("Histidine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Isoleucine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Leucine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Lysine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Methionine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Phenylalanine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Threonine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Tryptophan"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Valine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Arginine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		//.Add(t_("Cystine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Glutamine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Glycine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Proline"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		.Add(t_("Tyrosine"), 1, GRADCOLOR((double)i++ / aminoacid_count))
		;
		
	int mineral_count = 10;
	list.Add(t_("Menu essential minerals (\% of recommended)"));
	graph.Add().Vert(100)
		.Add(t_("Potassium"), 1, GRADCOLOR(0.0 / mineral_count))
		.Add(t_("Sodium"), 1, GRADCOLOR(2.0 / mineral_count))
		//.Add(t_("Chloride"), 1, GRADCOLOR(1.0 / mineral_count))
		.Add(t_("Calcium"), 1, GRADCOLOR(3.0 / mineral_count))
		.Add(t_("Phosphorus"), 1, GRADCOLOR(4.0 / mineral_count))
		.Add(t_("Magnesium"), 1, GRADCOLOR(5.0 / mineral_count))
		.Add(t_("Iron"), 1, GRADCOLOR(6.0 / mineral_count))
		.Add(t_("Zinc"), 1, GRADCOLOR(7.0 / mineral_count))
		.Add(t_("Manganese"), 1, GRADCOLOR(8.0 / mineral_count))
		.Add(t_("Copper"), 1, GRADCOLOR(9.0 / mineral_count))
		//.Add(t_("Iodine"), 1, GRADCOLOR(10.0 / mineral_count))
		//.Add(t_("Chromium"), 1, GRADCOLOR(11.0 / mineral_count))
		//.Add(t_("Molybdenum"), 1, GRADCOLOR(12.0 / mineral_count))
		.Add(t_("Selenium"), 1, GRADCOLOR(13.0 / mineral_count))
		//.Add(t_("Cobalt"), 1, GRADCOLOR(14.0 / mineral_count))
		//.Add(t_("Boron"), 1, GRADCOLOR(15.0 / mineral_count))
		//.Add(t_("Silicon"), 1, GRADCOLOR(16.0 / mineral_count))
		//.Add(t_("Bromine"), 1, GRADCOLOR(17.0 / mineral_count))
		//.Add(t_("Arsenic"), 1, GRADCOLOR(18.0 / mineral_count))
		//.Add(t_("Nickel"), 1, GRADCOLOR(19.0 / mineral_count))
		//.Add(t_("Fluoride"), 1, GRADCOLOR(20.0 / mineral_count))
		//.Add(t_("Lithium"), 1, GRADCOLOR(21.0 / mineral_count))
		//.Add(t_("Strontium"), 1, GRADCOLOR(22.0 / mineral_count))
		//.Add(t_("Vanadium"), 1, GRADCOLOR(23.0 / mineral_count))
		//.Add(t_("Tin"), 1, GRADCOLOR(31.0 / mineral_count))
		//.Add(t_("Phosphate"), 1, GRADCOLOR(32.0 / mineral_count))
		//.Add(t_("Bicarbonate"), 1, GRADCOLOR(33.0 / mineral_count))
		;
	
	i = 0;
	int fattyacid_count = 7;
	list.Add(t_("Menu essential fatty acids (\% of recommended)"));
	graph.Add().Vert(100)
		.Add(t_("Cholesterol"), 1, GRADCOLOR((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total trans"), 1, GRADCOLOR((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total saturated"), 1, GRADCOLOR((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total monounsaturated"), 1, GRADCOLOR((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total polyunsaturated"), 1, GRADCOLOR((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total trans-monoenoic"), 1, GRADCOLOR((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total trans-polyenoic"), 1, GRADCOLOR((double)(i++) / fattyacid_count));
	
	i = 0;
	int other_count = 2;
	list.Add(t_("Menu essential other (\% of recommended)"));
	graph.Add().Vert(100)
		.Add(t_("Choline"), 1, GRADCOLOR((double)(i++) / other_count))
		.Add(t_("Fiber"), 1, GRADCOLOR((double)(i++) / other_count))
		//.Add(t_("Inositol"), 1, GRADCOLOR((double)(i++) / other_count))
		//.Add(t_("Taurine"), 1, GRADCOLOR((double)(i++) / other_count))
		//.Add(t_("Nucleotides"), 1, GRADCOLOR((double)(i++) / other_count))
		;
		
	
	i = 0;
	int electrolyte_count = 5;
	list.Add(t_("Menu electrolytes (\% of recommended)"));
	graph.Add().Vert(100)
		//.Add(t_("Bicarbonate"), 1, GRADCOLOR((double)(i++) / electrolyte_count))
		.Add(t_("Calcium"), 1, GRADCOLOR((double)(i++) / electrolyte_count))
		//.Add(t_("Chloride"), 1, GRADCOLOR((double)(i++) / electrolyte_count))
		.Add(t_("Magnesium"), 1, GRADCOLOR((double)(i++) / electrolyte_count))
		.Add(t_("Phosphorus"), 1, GRADCOLOR((double)(i++) / electrolyte_count))
		.Add(t_("Potassium"), 1, GRADCOLOR((double)(i++) / electrolyte_count))
		.Add(t_("Sodium"), 1, GRADCOLOR((double)(i++) / electrolyte_count));
	
	list.Add(t_("Measured weight (kg)"));
	graph.Add()
		.Add(t_("Weight (kg)"), 2, Color(109, 0, 117));
	
	list.Add(t_("Measured fat (kg)"));
	graph.Add()
		.Add(t_("Fat (kg)"), 2, Color(81, 48, 0));
		
	list.Add(t_("Measured liquid (kg)"));
	graph.Add()
		.Add(t_("Liquid (kg)"), 2, Color(0, 176, 137));
		
	list.Add(t_("Measured muscle (kg)"));
	graph.Add()
		.Add(t_("Muscle (kg)"), 2, Color(120, 0, 0));
	
	list.Add(t_("Measured internals (kg)"));
	graph.Add()
		.Add(t_("Internals (kg)"), 2, Color(120, 0, 0));
	
	list.Add(t_("Measured fat (\%)"));
	graph.Add()
		.Add(t_("Fat (\%)"), 2, Color(81, 48, 0));
		
	list.Add(t_("Measured liquid (\%)"));
	graph.Add()
		.Add(t_("Liquid (\%)"), 2, Color(0, 176, 137));
	
	list.Add(t_("Measured muscle (\%)"));
	graph.Add()
		.Add(t_("Muscle (\%)"), 2, Color(120, 0, 0));
	
	list.Add(t_("Measured internals (\%)"));
	graph.Add()
		.Add(t_("Internals (\%)"), 2, Color(120, 0, 0));
	
	list.Add(t_("Relative liquid (\%)"));
	graph.Add().Vert(100)
		.Add(t_("Liquid (\%)"), 2, Color(0, 176, 137));
	
	list.Add(t_("Measured BMI"));
	graph.Add()
		.Add(t_("BMI"), 2, GrayColor());
	
	list.Add(t_("Measured neck circumference"));
	graph.Add().Add(t_("Neck circumference"), 2, Color(0, 9, 98));
	
	list.Add(t_("Measured bicep circumference"));
	graph.Add().Add(t_("Bicep circumference"), 2, Color(0, 9, 98));
	
	list.Add(t_("Measured forearm circumference"));
	graph.Add().Add(t_("Forearm circumference"), 2, Color(0, 9, 98));
	
	list.Add(t_("Measured chest circumference"));
	graph.Add().Add(t_("Chest circumference"), 2, Color(0, 9, 98));
	
	list.Add(t_("Measured waist circumference"));
	graph.Add().Add(t_("Waist circumference"), 2, Color(0, 9, 98));
	
	list.Add(t_("Measured buttocks circumference"));
	graph.Add().Add(t_("Buttocks circumference"), 2, Color(0, 9, 98));
	
	list.Add(t_("Measured thigh circumference"));
	graph.Add().Add(t_("Thigh circumference"), 2, Color(0, 9, 98));
	
	list.Add(t_("Measured leg circumference"));
	graph.Add().Add(t_("Leg circumference"), 2, Color(0, 9, 98));
	
	list <<= THISBACK(SelectSource);
	
	if (!list.IsCursor() && list.GetCount())
		list.SetCursor(0);
	SelectSource();
}

void GraphCtrl::Data() {
	graph.Refresh();
	SplitterHotfix(split, 2500);
}

void GraphCtrl::SelectSource() {
	if (!list.IsCursor()) return;
	graph.new_src = list.GetCursor();
	graph.Refresh();
}
