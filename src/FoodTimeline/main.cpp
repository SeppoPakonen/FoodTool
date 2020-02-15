#include "FoodTimeline.h"

GUI_APP_MAIN
{
	Timeline& tl = GetTimeline();
	
	tl.RegisterComponentBase<DummyComponent>();
	tl.RegisterComponentEdit<DummyEdit>();
	tl.RegisterComponentView<DummyView>();
	
	tl.LoadThis();
	
	{
		FoodTimeline().Run();
	}
	
	tl.StoreThis();
}
