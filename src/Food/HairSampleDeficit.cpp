#include "Food.h"


String GetHairSampleDeficitElement(int i) {
	switch (i) {
		case HSAMP_CALCIUM: return t_("Calcium");
		case HSAMP_MAGNESIUM: return t_("Magnesium");
		case HSAMP_SODIUM: return t_("Sodium");
		case HSAMP_POTASSIUM: return t_("Potassium");
		case HSAMP_COPPER: return t_("Copper");
		case HSAMP_ZINC: return t_("Zinc");
		case HSAMP_MANGANESE: return t_("Manganese");
		case HSAMP_CHROMIUM: return t_("Chromium");
		case HSAMP_VANADIUM: return t_("Vanadium");
		case HSAMP_MOLYBDENUM: return t_("Molybdenum");
		case HSAMP_BORON: return t_("Boron");
		case HSAMP_IODINE: return t_("Iodine");
		case HSAMP_LITHIUM: return t_("Lithium");
		case HSAMP_PHOSPHORUS: return t_("Phosphorus");
		case HSAMP_SELENIUM: return t_("Selenium");
		case HSAMP_STRONTIUM: return t_("Strontium");
		case HSAMP_SULFUR: return t_("Sulfur");
		case HSAMP_COBALT: return t_("Cobalt");
		case HSAMP_IRON: return t_("Iron");
		case HSAMP_GERMANIUM: return t_("Germanium");
		case HSAMP_RUBIDIUM: return t_("Rubidium");
		case HSAMP_ZIRCONIUM: return t_("Zirconium");
		default: return "<Invalid element id>";
	}
}

