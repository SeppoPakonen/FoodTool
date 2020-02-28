#include "FoodCtrl.h"

UsageCtrl::UsageCtrl() {
	Add(total.HSizePos().TopPos(0, 30));
	Add(list.HSizePos().VSizePos(30));
	
	total.SetFont(SansSerif(25).Bold());
	total.SetInk(Color(17, 29, 118));
	//total.SetAlign(ALIGN_CENTER);
	
	list.AddColumn(t_("Begin"));
	list.AddColumn(t_("End"));
	list.AddColumn(t_("Minutes"));
}

void UsageCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (prof.usage.GetCount() != list.GetCount()) {
		int total_seconds = 0;
		for(int i = 0; i < prof.usage.GetCount(); i++) {
			int row = prof.usage.GetCount() - 1 - i;
			const ProgramUsageStat& u = prof.usage[i];
			int seconds = u.end.Get() - u.begin.Get();
			list.Set(row, 0, u.begin);
			list.Set(row, 1, u.end);
			list.Set(row, 2, seconds / 60);
			total_seconds += seconds;
		}
		int total_mins = total_seconds / 60;
		total.SetLabel(Format(t_("You have used this program for %d minutes."), total_mins));
	}
}
