#include "FoodTimeline.h"


FoodTimeline::FoodTimeline() {
	Title("Food Timeline");
	MaximizeBox().MinimizeBox().Sizeable();
	Maximize();
	
	
	Add(tl.SizePos());
	
	
}

