#ifndef _FoodTool_Ingredient_h_
#define _FoodTool_Ingredient_h_



template <class T>
struct IngredientT : Moveable<IngredientT<T>> {
	T grams = 0, kcals = 0, fat = 0, carbs = 0, protein = 0, salt = 0;
	
	
	void Reset() {
		grams = 0, kcals = 0, fat = 0, carbs = 0, protein = 0, salt = 0;
	}
	String Export() {
		String s;
		for(int i = 0; i < NUTR_COUNT; i++) {
			String line;
			line << GetNurientString(i) << ": ";
			if (line.GetCount() < 32)
				line.Cat(' ', 32 - line.GetCount());
			line << EncodeValue(values[i]) << "\n";
			s << line;
		}
		return s;
	}
	void Serialize(Stream& s) {s % grams % kcals % fat % carbs % protein % salt;}
	void Limit(T kcals, T macro, T salt) {
		this->kcals = max(this->kcals, kcals);
		this->fat = max(this->fat, macro);
		this->carbs = max(this->carbs, macro);
		this->protein = max(this->protein, macro);
		this->grams = max(this->grams, fat+carbs+protein);
		this->salt = max(this->salt, salt);
	}
	template <class K>
	void operator +=(const K& d) {
		grams += d.grams;
		kcals += d.kcals;
		fat += d.fat;
		carbs += d.carbs;
		protein += d.protein;
		salt += d.salt;
	}
	template <class K>
	void operator -=(const K& d) {
		grams -= d.grams;
		kcals -= d.kcals;
		fat -= d.fat;
		carbs -= d.carbs;
		protein -= d.protein;
		salt -= d.salt;
	}
	void operator*=(double mul) {
		grams *= mul;
		kcals *= mul;
		fat *= mul;
		carbs *= mul;
		protein *= mul;
		salt *= mul;
	}
	void ChangeGrams(double new_grams) {
		double mul = new_grams / grams;
		kcals *= mul;
		fat *= mul;
		carbs *= mul;
		protein *= mul;
		salt *= mul;
		ASSERT(IsFin(grams) && IsFin(kcals) && IsFin(fat) && IsFin(carbs) && IsFin(protein) && IsFin(salt));
		grams = new_grams;
	}
	
	
};

template <class T>
inline IngredientT<T> operator-(const IngredientT<T>& a, const IngredientT<T>& b) {
	IngredientT<T> out;
	out.grams = a.grams - b.grams;
	out.kcals = a.kcals - b.kcals;
	out.fat = a.fat - b.fat;
	out.carbs = a.carbs - b.carbs;
	out.protein = a.protein - b.protein;
	out.salt = a.salt - b.salt;
	return out;
}

typedef IngredientT<float> Ingredient;
typedef IngredientT<double> IngredientDouble;


#endif
