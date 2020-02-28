#ifndef _Food_Deficit_h_
#define _Food_Deficit_h_


// Hair-sample deficits and overdoses
enum {
	HSAMP_CALCIUM,
	HSAMP_MAGNESIUM,
	HSAMP_SODIUM,
	HSAMP_POTASSIUM,
	HSAMP_COPPER,
	HSAMP_ZINC,
	HSAMP_MANGANESE,
	HSAMP_CHROMIUM,
	HSAMP_VANADIUM,
	HSAMP_MOLYBDENUM,
	HSAMP_BORON,
	HSAMP_IODINE,
	HSAMP_LITHIUM,
	HSAMP_PHOSPHORUS,
	HSAMP_SELENIUM,
	HSAMP_STRONTIUM,
	HSAMP_SULFUR,
	HSAMP_COBALT,
	HSAMP_IRON,
	HSAMP_GERMANIUM,
	HSAMP_RUBIDIUM,
	HSAMP_ZIRCONIUM,
	
	HSAMP_COUNT
};


String GetHairSampleDeficitElement(int i);

struct HairSampleDeficitProfile : Moveable<HairSampleDeficitProfile> {
	double elements[HSAMP_COUNT];
	Time added;
	
	void Reset() {
		for(int i = 0; i < HSAMP_COUNT; i++)
			elements[i] = 50;
	}
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			for(int i = 0; i < HSAMP_COUNT; i++)
				s % elements[i];
			s % added;
		}
	}
};

#endif
