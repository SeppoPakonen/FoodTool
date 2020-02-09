#include "FoodTool.h"
#include "Data.brc"
#include <plugin/bz2/bz2.h>

#define IMAGECLASS Images
#define IMAGEFILE <FoodTool/Images.iml>
#include <Draw/iml_source.h>


FoodTool::FoodTool()
{
	Title("FoodTool");
	Icon(Images::icon);
	Sizeable().MaximizeBox().MinimizeBox();
	Maximize();
	
	Add(tabs.SizePos());
	
	tabs.Add(motivation.SizePos(), "Motivation");
	tabs.Add(status.SizePos(), "Status");
	tabs.Add(graphs.SizePos(), "Graphs");
	tabs.Add(weight.SizePos(), "Weight");
	tabs.Add(exc.SizePos(), "Exceptions");
	tabs.Add(notes.SizePos(), "Notes");
	tabs.Add(usage.SizePos(), "Usage");
	tabs.Add(conf.SizePos(), "Configuration");
	tabs.WhenSet << THISBACK(Data);
	
	tc.Set(-500, THISBACK(Data));
}

void FoodTool::Data() {
	int tab = tabs.Get();
	
	if (tab == 0)
		motivation.Data();
	else if (tab == 1)
		status.Data();
	else if (tab == 3)
		weight.Data();
	else if (tab == 4)
		exc.Data();
	else if (tab == 5)
		notes.Data();
	else if (tab == 6)
		usage.Data();
	else if (tab == 7)
		conf.Data();
	
}






MotivationCtrl::MotivationCtrl() {
	quotes.Add("“If you have discipline, drive, and determination… nothing is impossible.”– Dana Linn Bailey");
	quotes.Add("“Make time for it. Just get it done. Nobody ever got strong or got in shape by thinking about it. They did it.”– Jim Wendler");
	quotes.Add("“The groundwork of all happiness is health.”– Leigh Hunt");
	quotes.Add("“The scale is merely a measure of my relationship with gravity.”– Lauren Harris-Pincus");
	quotes.Add("“Don’t work out because you hate your body — work out because you love it.”– Author Unknown");
	quotes.Add("“Obstacles don’t have to stop you. If you run into a wall, don’t turn around and give up. Figure out how to climb it, go through it, or work around it.”– Michael Jordan");
	quotes.Add("“Strength does not come from what you can do. It comes from overcoming the things you once thought you couldn’t.”– Nikki Rogers");
	quotes.Add("“Wisdom is doing now what you are going to be happy with later on.”– Joyce Meyer");
	quotes.Add("“Decide. Commit. Succeed.”– Justin Seedman");
	quotes.Add("“Eliminate the mindset of can’t — because you can do anything.”– Toni Horton");
	quotes.Add("“Exercise should be regarded as a tribute to the heart.”– Gene Tunney");
	quotes.Add("“Unless you puke, faint, or die, keep going!”– Jillian Michaels");
	quotes.Add("“To give anything less than your best is to sacrifice the gift.”– Steve Prefontaine");
	quotes.Add("“When you feel like quitting, think about why you started.”– Author Unknown");
	quotes.Add("“My weight does not determine my worth.”– Kristin Oja");
	quotes.Add("“Some people want it to happen. Some wish it would happen. Others make it happen.”– Michael Jordan");
	quotes.Add("“If you wait, all that happens is you get older.”– Larry McMurtry");
	quotes.Add("“You miss 100 percent of the shots you never take.”– Wayne Gretzky");
	quotes.Add("“If you don’t do what’s best for your body, you’re the one who comes up on the short end.”– Julius Erving");
	quotes.Add("“If you keep good food in your fridge, you will eat good food.”– Errick McAdams");
	quotes.Add("“Instead of indulging in ‘comfort food,’ indulge in comfort meditation, comfort journaling, comfort walking, comfort talking, comfort manicures, comfort reading, comfort yoga, comfort hugging.”– Karen Salmansohn");
	quotes.Add("“My body is less judgmental of my diet than my mind is.”– Mackenzie Banta");
	quotes.Add("“You can have your cake and lose weight, too!”– Anonymous");
	quotes.Add("“Let food be thy medicine, and medicine be thy food.”– Hippocrates");
	quotes.Add("“To ensure good health: eat lightly, breathe deeply, live moderately, cultivate cheerfulness, and maintain an interest in life.”– William Londen");
	quotes.Add("“New meal; fresh start.”– Michelle Hyman");
	quotes.Add("“I am a better person when I have less on my plate.”– Elizabeth Gilbert");
	quotes.Add("“Food can be both enjoyable and nourishing.”– Alyssa Ardolino");
	quotes.Add("“If I don’t eat junk, I don’t gain weight.”– Paula Christensen");
	quotes.Add("“The cardiologist’s diet: If it tastes good spit it out.”– Anonymous");
	quotes.Add("“When I buy cookies I just eat four and throw the rest away. But first I spray them with Raid so I won’t dig them out of the garbage later. Be careful, though, because that Raid really doesn’t taste that bad.”– Janette Barber");
	quotes.Add("“I’m allergic to food. Every time I eat it breaks out into fat.”– Jennifer Greene Duncan");
	quotes.Add("“The only way to lose weight is to check it in as airline baggage.”– Peggy Ryan");
	quotes.Add("“It would be far easier to lose weight permanently if replacement parts weren’t so handy in the refrigerator.”– Hugh Allen");
	quotes.Add("“When we lose twenty pounds, we may be losing the twenty best pounds we have! We may be losing the pounds that contain our genius, our humanity, our love and honesty.”– Woody Allen");
	quotes.Add("“A diet is when you watch what you eat and wish you could eat what you watch.”– Hermione Gingold");
	quotes.Add("“Keep an open mind and a closed refrigerator.”– Anonymous");
	quotes.Add("“Never eat more than you can lift.”– Miss Piggy");
	quotes.Add("“My favorite exercise is a cross between a lunge and a crunch. I call it lunch.”– Anonymous");
	quotes.Add("“Only I can change my life. No one can do it for me.”– Anonymous");
	quotes.Add("“Weight loss doesn’t begin in the gym with a dumbbell; it starts in your head with a decision.”– Toni Sorenson");
	quotes.Add("“Just believe in yourself. Even if you don’t, pretend that you do, and at some point, you will.”– Venus Williams");
	quotes.Add("“I always believed if you take care of your body it will take care of you.”– Ted Lindsay");
	quotes.Add("“Don’t wait until you’ve reached your goal to be proud of yourself. Be proud of every step you take toward reaching that goal.”– Author Unknown");
	quotes.Add("“I choose self-care.”– Alyssa Ardolino");
	quotes.Add("“Success is the sum of small efforts — repeated day-in and day-out.”– Robert Collier");
	quotes.Add("“Triumph by putting a little soul into it!”– Nancy Mure");
	quotes.Add("“The mind is everything. What you think you become.”– Buddha");
	quotes.Add("“It is better to take small steps in the right direction than to make a great leap forward only to stumble backward.”– Proverb");
	quotes.Add("Only I Can Change My Life, No One Can Do It For Me");
	quotes.Add("Slow And Steady Wins The Race");
	quotes.Add("The Struggle You Are In Today Is Developing The Strength You Need for Tomorrow");
	quotes.Add("Food, Like Your Money, Should Be Working For You");
	quotes.Add("The Road May Be Bumpy But Stay Committed To The Process");
	quotes.Add("If You Are Tired Of Starting Over, Stop Giving Up");
	quotes.Add("Be The Best Version Of You");
	quotes.Add("Be Stronger Than Your Excuse");
	quotes.Add("It’s Not A Diet, It’s A Lifestyle Change");
	quotes.Add("Doubt Kills More Dreams Than Failure Ever Will");
	quotes.Add("Nothing Tastes As Good As Being Thin Feels");
	quotes.Add("Will Is A Skill");
	quotes.Add("Stressed Spelled Backwards Is Desserts. Coincidence? I think not!");
	quotes.Add("An Active Mind Cannot Exist In An Inactive Body");
	quotes.Add("Strive For Progress, Not Perfection");
	quotes.Add("Success Is Never Certain, Failure Is Never Final");
	quotes.Add("No Amount Of Security Is Worth The Suffering Of A Life Chained To A Routine That Has Killed Your Dreams");
	quotes.Add("A Goal Without A Plan Is Just A Wish");
	quotes.Add("Success Is The Sum Of Small Efforts, Repeated Day In And Day Out");
	quotes.Add("You Can’t Cross The Sea Merely By Standing And Staring At The Water");
	quotes.Add("It has to be hard so you'll never ever forget.");
	quotes.Add("Weight loss doesn't begin in the gym with a dumb bell; it starts in your head with a decision.");
	quotes.Add("If you have discipline, drive, and determination … nothing is impossible.");
	quotes.Add("Eliminate the mindset of can't — because you can do anything.");
	quotes.Add("Most people give up right before the big break comes — don’t let that person be you.");
	quotes.Add("Make time for it. Just get it done. Nobody ever got strong or got in shape by thinking about it. They did it.");
	quotes.Add("Fitness is like marriage, you can't cheat on it and expect it to work.");
	quotes.Add("Just believe in yourself. Even if you don't, pretend that you do and, at some point, you will.");
	quotes.Add("If it doesn't challenge you, it doesn't change you.");
	quotes.Add("It's going to be a journey. It's not a sprint to get in shape.");
	quotes.Add("Nobody is perfect, so get over the fear of being or doing everything perfectly. Besides, perfect is boring.");
	quotes.Add("Exercise should be regarded as a tribute to the heart.");
	quotes.Add("You didn't gain all your weight in one day; you won't lose it in one day. Be patient with yourself.");
	quotes.Add("Ability is what you're capable of doing. Motivation determines what you do. Attitude determines how well you do it.");
	quotes.Add("I always believed if you take care of your body it will take care of you.");
	quotes.Add("Looking after my health today gives me a better hope for tomorrow.");
	quotes.Add("You may be disappointed if you fail, but you are doomed if you don't try.");
	quotes.Add("Unless you puke, faint, or die, keep going!");
	quotes.Add("Obstacles don't have to stop you. If you run into a wall, don't turn around and give up. Figure out how to climb it, go through it, or work around it.");
	quotes.Add("If you don't do what's best for your body, you're the one who comes up on the short end.");
	quotes.Add(" “If it was about knowledge, we would all be skinny and rich. It’s not about what you know but what you do!”");
	quotes.Add("“The distance between who am I am and who I want to be is only separated by what I do!”");
	quotes.Add("“The only bad workout is the one that didn’t happen.”");
	quotes.Add("“To change your body you must first change your mind.”");
	quotes.Add("“Someone busier than you is running right now.”");
	quotes.Add("“Working out is never convenient. But neither is illness, diabetes and obesity!”");
	quotes.Add("“I don’t stop when I’m tired, I stop when I’m DONE!”");
	quotes.Add("“I don’t work hard because I hate my body. I workout because I love it!”");
	quotes.Add("“Get comfortable with being uncomfortable!”");
	quotes.Add("“When it burns, is when you’re just getting started. That’s when you get stronger!”");
	quotes.Add("“If you have 30 minutes for Facebook, you have 1 hour for the gym!”");
	quotes.Add("“The best way to predict your health is to create it.”");
	quotes.Add("“You can’t run from all your problems, but it will help you lose weight.”");
	quotes.Add("“Get started as if you are motivated. Pretend. And the motivation will come!”");
	quotes.Add("“Excuses don’t burn calories.”");
	quotes.Add("“Your body hears everything your mind says. Keep going. You can!”");
	quotes.Add("“Don’t stop until you’re proud.”");
	quotes.Add("“You don’t have to go fast, you just have to go.”");
	quotes.Add("“If you’re tired of starting over; stop giving up!”");
	quotes.Add("“On the other side of your workout is the body and health you want!”");
	quotes.Add("“The question isn’t can you, it’s will you!”");
	quotes.Add("“Workouts are like life. The harder it is, the STRONGER YOU BECOME!”");
	quotes.Add("“If no one thinks you can, then you have to!”");
	quotes.Add("“If you still look good at the end of your work out…you didn’t work hard enough!”");
	quotes.Add("“I’m not running a marathon or a 5 K. I have no upcoming competitions and I don’t plan on going to the beach. I am working out like my life depends on it. Because it does!”");
	quotes.Add("“The reason fat men are good natured is they can neither fight nor run.”  – Theodore Roosevelt");
	quotes.Add("“Fitness is like marriage, you can’t cheat on it and expect it to work.” – Bonnie Pfiester");
	quotes.Add("To enjoy the glow of good health, you must exercise. – Gene Tunney");
	quotes.Add("“Make time for it. Just get it done. Nobody ever got strong or got in shape by thinking about it. They did it.” – Jim Wendler");
	quotes.Add("“Weight loss is not impossible. Weight loss is hard, but hard is not the same as impossible.” – Author Unknown");
	quotes.Add("“Even if you are on the right track, you’ll get run over if you just sit there.” – Will Rogers");
	quotes.Add("“Marathon runners don’t worry about the conditions, they just run anyway!” – Author Unknown");
	quotes.Add("“Nothing tastes as good as being thin feels.” ― Elizabeth Berg");
	quotes.Add("Champions keep playing until they get it right – Billie Jean King");
	quotes.Add("“Walking: the most ancient exercise and still the best modern exercise.” ~Carrie Latet");
	quotes.Add("If you aren’t going all the way, why go at all? – Joe Namath");
	quotes.Add("“Success is the sum of small efforts, repeated day-in and day-out.” –Robert Collier");
	quotes.Add("“Every day, in every way, I am getting better and better.” ~Emilie Coue");
	quotes.Add("“Those who think they have not time for bodily exercise will sooner or later have to find time for illness.” ~Edward Stanley");
	quotes.Add("“Just believe in yourself. Even if you don’t, pretend that you do and, at some point, you will.” – Venus Williams");
	quotes.Add("“Don’t reward yourself with food. You are not a dog. – Unknown");
	quotes.Add("“Every weight loss program, no matter how positively it’s packaged, whispers to you that you’re not right. You’re not good enough. You’re unacceptable and you need to be fixed.” ― Kim Brittingham");
	quotes.Add("“The only successful way to reach and maintain a healthy weight is to find what works for you.” – Author Unknown");
	quotes.Add("“Getting my lifelong weight struggle under control has come from a process of treating myself as well as I treat others in every way.” – Oprah Winfrey");
	quotes.Add("Don’t compare yourself to others. Compare yourself to the person from yesterday. – Author Unknown");
	quotes.Add("“A huge part of losing weight is believing you can do it and realizing it’s not going to happen overnight.” – Author Unknown");
	quotes.Add("“The hardest step to fitness is the first. Take it now!” — Heather Montgomery");
	quotes.Add("“Your body can do anything. It’s your mind that needs convincing” – Unknown");
	quotes.Add("Your goals are as good as your actions. No action taken, no goals achieved – Derric Yuh Ndim");
	quotes.Add("“If you have discipline, drive, and determination… nothing is impossible.”– Dana Linn Bailey");
	quotes.Add("“Eliminate the mindset of can’t — because you can do anything.”– Toni Horton");
	quotes.Add("“Some people want it to happen. Some wish it would happen. Others make it happen.”– Michael Jordan");
	quotes.Add("“Exercise should be regarded as a tribute to the heart.”– Gene Tunney");
	quotes.Add("“If you don’t do what’s best for your body, you’re the one who comes up on the short end.”– Julius Erving");
	quotes.Add("“When you feel like quitting, think about why you started.”– Author Unknown");
	quotes.Add("“If you keep good food in your fridge, you will eat good food.”– Errick McAdams");
	quotes.Add("“If you wait, all that happens is you get older.”– Larry McMurtry");
	quotes.Add("“To ensure good health: eat lightly, breathe deeply, live moderately, cultivate cheerfulness, and maintain an interest in life.”– William Londen");
	quotes.Add("“Only I can change my life. No one can do it for me.”– Anonymous");
	quotes.Add("“I always believed if you take care of your body it will take care of you.”– Ted Lindsay");
	quotes.Add("“It is better to take small steps in the right direction than to make a great leap forward only to stumble backward.”– Proverb");
	quotes.Add("“The past cannot be changed. The future is yet in your power.”― Mary Pickford");
	quotes.Add("“Weight loss doesn’t begin in the gym with a dumbbell; it starts in your head with a decision.”– Toni Sorenson");
	quotes.Add("“Triumph by putting a little soul into it!”– Nancy Mure");
	
}

void MotivationCtrl::Paint(Draw& d) {
	if (motivation_i < 0 || quote_i < 0) {
		Date begin(2020,2,9);
		Date now = GetSysTime();
		int days_between = now.Get() - begin.Get();
		while (days_between < 0) days_between += 1000;
		motivation_i = days_between % motivation_mask_count;
		quote_i = days_between % quotes.GetCount();
		
		String image_jpg = BZ2Decompress(motivation_mask[motivation_i], motivation_mask_length[motivation_i]);
		img = StreamRaster::LoadStringAny(image_jpg);
	}
	
	Size sz(GetSize());
	d.DrawRect(sz, Black());
	
	Size img_sz = img.GetSize();
	int img_x = (sz.cx - img_sz.cx) / 2;
	int img_y = (sz.cy - img_sz.cy) / 2;
	d.DrawImage(img_x, img_y, img);
	
	Font fnt = SansSerif(25);
	String quote = quotes[quote_i];
	Size txt_sz = GetTextSize(quote, fnt);
	int txt_y = img_y + img_sz.cy + 15;
	int txt_x = (sz.cx - txt_sz.cx) / 2;
	d.DrawText(txt_x, txt_y, quote, fnt, White());
	
	Profile& prof = GetProfile();
	if (prof.planned_daily.GetCount()) {
		Date now = GetSysTime();
		int days_remaining = prof.planned_daily.Top().date.Get() - now.Get() + 1;
		if (days_remaining >= 0) {
			String remaining = IntStr(days_remaining) + " days remaining";
			if (days_remaining < 30) {
				fnt = SansSerif(40);
				remaining << "!!!";
			}
			else {
				fnt = SansSerif(10);
				remaining << "...";
			}
			Size txt2_sz = GetTextSize(remaining, fnt);
			int txt2_y = txt_y + txt_sz.cy + 15;
			int txt2_x = (sz.cx - txt2_sz.cx) / 2;
			d.DrawText(txt2_x, txt2_y, remaining, fnt, White());
		}
	}
}






StatusCtrl::StatusCtrl() {
	CtrlLayout(*this);
	
	cur_date = Date(0,0,0);
	
	Data();
}

void StatusCtrl::Data() {
	Profile& prof = GetProfile();
	Date now = GetSysTime();
	
	if (now != cur_date) {
		cur_date = now;
		
		Date shop_begin = prof.storage.GetLastShopping();
		Date shop_end = prof.storage.GetNextShopping();
		int64 shop_total = shop_end.Get() - shop_begin.Get();
		int64 shop_actual = now.Get() - shop_begin.Get();
		shop_prog.Set(shop_actual, shop_total);
		
		today.SetData(prof.storage.GetTodaysMenu());
		
		next_shopping.SetDate(shop_end);
		
		shopping_list.SetData(prof.storage.GetNextShoppingList());
		
		Date total_begin = prof.GetCurrentTotalBegin();
		Date total_end = prof.GetCurrentTotalEnd();
		int64 total_total = total_end.Get() - total_begin.Get();
		int64 total_actual = now.Get() - total_begin.Get();
		total_prog.Set(total_actual, total_total);
		
		Date week_begin = prof.GetCurrentWeekBegin();
		Date week_end = week_begin + 7;
		if (week_end > total_end) week_end = total_end;
		int64 week_total = week_end.Get() - week_begin.Get();
		int64 week_actual = now.Get() - week_begin.Get();
		week_prog.Set(week_actual, week_total);
		
		Date month_begin = prof.GetCurrentMonthBegin();
		Date month_end = month_begin + GetMonthDays();
		if (month_end > total_end) month_end = total_end;
		int64 month_total = month_end.Get() - month_begin.Get();
		int64 month_actual = now.Get() - month_begin.Get();
		month_prog.Set(month_actual, month_total);
		
		Date quarter_begin = prof.GetCurrentQuarterBegin();
		Date quarter_end = quarter_begin + GetQuarterDays();
		if (quarter_end > total_end) quarter_end = total_end;
		int64 quarter_total = quarter_end.Get() - quarter_begin.Get();
		int64 quarter_actual = now.Get() - quarter_begin.Get();
		quart_prog.Set(quarter_actual, quarter_total);
	}
}




ConfigurationCtrl::ConfigurationCtrl() {
	Add(split.SizePos());
	split.Horz() << list << conf;
	split.SetPos(2500);
	
	CtrlLayout(conf);
	conf.next.SetLabel("Add new");
	conf.weight.Disable();
	conf.gender.SetEditable(false);
	conf.next <<= THISBACK(AddConf);
	conf.height <<= THISBACK(UpdateTargetWeight);
	conf.fatref <<= THISBACK(ShowWeightReference);
	list.AddColumn("#");
	list.AddColumn("Added");
	list.ColumnWidths("1 5");
	list.WhenAction = THISBACK(SelectConf);
	list.WhenLeftClick = THISBACK(SelectConf);
	
}

void ConfigurationCtrl::ShowWeightReference() {
	if (!iw.IsOpen())
		iw.Open(this);
}

void ConfigurationCtrl::UpdateTargetWeight() {
	conf.tgt_weight.SetData(GetTargetWeight((double)conf.height.GetData() * 0.01, 19));
}

void ConfigurationCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (prof.confs.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.confs.GetCount(); i++) {
			int row = prof.confs.GetCount() - 1 - i;
			const Configuration& c = prof.confs[i];
			list.Set(row, 0, i+1);
			list.Set(row, 1, c.added);
		}
		list.SetCursor(0);
		SelectConf();
	}
}

void ConfigurationCtrl::SelectConf() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int conf_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	const Configuration& c = prof.confs[conf_i];
	
	conf.gender.SetData(prof.is_male);
	conf.height.SetData(c.height);
	conf.age.SetData(c.age);
	conf.bodyfat.SetData(c.bodyfat);
	conf.activity.SetData(c.activity);
	conf.walking_dist.SetData(c.walking_dist);
	conf.tgt_weight.SetData(c.tgt_weight);
	conf.tgt_walking_dist.SetData(c.tgt_walking_dist);
	conf.tgt_jogging_dist.SetData(c.tgt_jogging_dist);
	conf.shop_interval.SetData(c.shop_interval);
	conf.hours_between_meals.SetData(c.hours_between_meals);
	conf.easy_day_interval.SetData(c.easy_day_interval);
	conf.waking.SetTime(c.waking_hour,c.waking_minute, 0);
	conf.sleeping.SetTime(c.sleeping_hour,c.sleeping_minute,0);
}

void ConfigurationCtrl::AddConf() {
	Profile& prof = GetProfile();
	Configuration& c = prof.confs.Add();
	
	c.added = GetSysTime();
	c.end_date = Null;
	c.height = conf.height.GetData();
	c.age = conf.age.GetData();
	c.bodyfat = conf.bodyfat.GetData();
	c.activity = conf.activity.GetData();
	c.walking_dist = conf.walking_dist.GetData();
	c.tgt_weight = conf.tgt_weight.GetData();
	c.tgt_walking_dist = conf.tgt_walking_dist.GetData();
	c.tgt_jogging_dist = conf.tgt_jogging_dist.GetData();
	c.shop_interval = conf.shop_interval.GetData();
	c.hours_between_meals = conf.hours_between_meals.GetData();
	c.easy_day_interval = conf.easy_day_interval.GetData();
	c.waking_hour = conf.waking.GetHour();
	c.waking_minute = conf.waking.GetMinute();
	c.sleeping_hour = conf.sleeping.GetHour();
	c.sleeping_minute = conf.sleeping.GetMinute();
	
	prof.UpdatePlan();
	
	Data();
}




ExceptionsCtrl::ExceptionsCtrl() {
	CtrlLayout(exc);
	Add(split.SizePos());
	split.Horz() << list << exc;
	split.SetPos(3333);
	
	list.AddColumn("#");
	list.AddColumn("Begin");
	list.AddColumn("Days");
	list.AddColumn("Deficit");
	list.AddColumn("Reason");
	list.ColumnWidths("1 4 2 2 6");
	
	exc.cal_deficit.SetData(20);
	exc.days.SetData(1);
	exc.add <<= THISBACK(AddException);
}

void ExceptionsCtrl::Data(bool force) {
	Profile& prof = GetProfile();
	
	if (prof.exceptions.GetCount() != list.GetCount() || force) {
		for(int i = 0; i < prof.exceptions.GetCount(); i++) {
			int row = prof.exceptions.GetCount() - 1 - i;
			const IntakeExceptions& e = prof.exceptions[i];
			list.Set(row, 0, i+1);
			list.Set(row, 1, e.begin);
			list.Set(row, 2, e.end.Get() - e.begin.Get());
			list.Set(row, 3, e.calorie_deficit);
			list.Set(row, 4, e.reason);
		}
		list.SetCursor(0);
	}
}

void ExceptionsCtrl::AddException() {
	Profile& prof = GetProfile();
	Date begin = GetSysTime();
	begin++;
	int existing = -1;
	for(int i = prof.exceptions.GetCount()-1; i >= 0; i--)
		if (prof.exceptions[i].begin == begin)
			{existing = i; break;}
	IntakeExceptions& e = existing < 0 ? prof.exceptions.Add() : prof.exceptions[existing];
	e.reason = exc.reason.GetData();
	e.calorie_deficit = exc.cal_deficit.GetData();
	e.begin = begin;
	e.end = e.begin + (int)exc.days.GetData();
	
	exc.reason.Clear();
	exc.cal_deficit.Clear();
	exc.days.Clear();
	
	prof.UpdatePlan();
	
	Data(true);
}




NoteCtrl::NoteCtrl() {
	CtrlLayout(note);
	Add(split.SizePos());
	split.Horz() << list << note;
	split.SetPos(2500);
	
	list.AddColumn("Added");
	list.AddColumn("Title");
	list.ColumnWidths("1 2");
	list.WhenAction << THISBACK(SelectNote);
	list.WhenLeftClick << THISBACK(SelectNote);
	
	note.add <<= THISBACK(AddNote);
}

void NoteCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (prof.notes.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.notes.GetCount(); i++) {
			int row = prof.notes.GetCount() - 1 - i;
			const Note& n = prof.notes[i];
			list.Set(row, 0, n.added);
			list.Set(row, 1, n.title);
		}
		list.SetCursor(0);
		SelectNote();
	}
}

void NoteCtrl::AddNote() {
	Profile& prof = GetProfile();
	Note& n = prof.notes.Add();
	n.added = GetSysTime();
	n.title = note.title.GetData();
	n.content = note.note.GetData();
	
	Data();
}

void NoteCtrl::SelectNote() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int note_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	const Note& n = prof.notes[note_i];
	note.title.SetData(n.title);
	note.note.SetData(n.content);
}





UsageCtrl::UsageCtrl() {
	Add(total.HSizePos().TopPos(0, 30));
	Add(list.HSizePos().VSizePos(30));
	
	total.SetFont(SansSerif(25).Bold());
	total.SetInk(Color(17, 29, 118));
	//total.SetAlign(ALIGN_CENTER);
	
	list.AddColumn("Begin");
	list.AddColumn("End");
	list.AddColumn("Minutes");
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
		total.SetLabel("You have used this program for " + IntStr(total_mins) + " minutes.");
	}
}





WeightCtrl::WeightCtrl() {
	LoadThis();
	
	Add(split.SizePos());
	split.Horz() << list << edit;
	split.SetPos(2500);
	
	list.AddColumn("Date");
	list.AddColumn("Weight (kg)");
	list.ColumnWidths("2 1");
	list <<= THISBACK(SelectWeightStat);
	list.WhenLeftClick << THISBACK(SelectWeightStat);
	
	CtrlLayout(edit);
	edit.weight <<= THISBACK(UpdateBMI);
	
	int count = cap.GetCount();
	if (last_camera_count != count) {
		last_camera_i = 0;
		last_camera_count = count;
	}
	
	for(int i = 0; i < count; i++)
		edit.cameras.Add("Source #" + IntStr(i));
	
	if (edit.cameras.GetCount())
		edit.cameras.SetIndex(last_camera_i);
	
	edit.capture_images <<= THISBACK(CaptureImages);
	edit.preview_cam <<= THISBACK(PreviewCamera);
	edit.add <<= THISBACK(AddWeightStat);
	edit.reset <<= THISBACK(Reset);
}

void WeightCtrl::Reset() {
	edit.weight.Clear();
	edit.fat.Clear();
	edit.liquid.Clear();
	edit.muscle.Clear();
	edit.bmi.Clear();
	front.Clear();
	right.Clear();
	back.Clear();
	edit.front.SetImage(Image());
	edit.right.SetImage(Image());
	edit.back.SetImage(Image());
	edit.preview_cam.Enable();
	edit.capture_images.Enable();
	edit.add.Enable();
}

void WeightCtrl::UpdateBMI() {
	double weight = edit.weight.GetData();
	if (!IsFin(weight)) return;
	edit.bmi.SetData(GetBMI(GetProfile().confs.Top().height * 0.01, weight));
}

void WeightCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (prof.weights.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.weights.GetCount(); i++) {
			int row = prof.weights.GetCount() - 1 - i;
			const WeightLossStat& w = prof.weights[i];
			list.Set(row, 0, Date(w.added));
			list.Set(row, 1, w.weight);
		}
	}
}

void WeightCtrl::SelectWeightStat() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int note_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	const WeightLossStat& w = prof.weights[note_i];
	
	edit.weight.SetData(w.weight);
	edit.fat.SetData(w.fat);
	edit.liquid.SetData(w.liquid);
	edit.muscle.SetData(w.muscle);
	edit.bmi.SetData(w.bmi);
	edit.dexa.Set(w.is_dexa);
	
	Thread::Start(THISBACK3(LoadImages, w.GetFrontFile(),w.GetRightFile(), w.GetBackFile()));
	
	edit.preview_cam.Disable();
	edit.capture_images.Disable();
	edit.add.Disable();
}

void WeightCtrl::LoadImages(String f, String r, String b) {
	front = StreamRaster::LoadFileAny(f);
	PostCallback(THISBACK(SetFrontImage));
	
	right = StreamRaster::LoadFileAny(r);
	PostCallback(THISBACK(SetRightImage));
	
	back = StreamRaster::LoadFileAny(b);
	PostCallback(THISBACK(SetBackImage));
}

void WeightCtrl::AddWeightStat() {
	Profile& prof = GetProfile();
	WeightLossStat& w = prof.weights.Add();
	w.added = GetSysTime();
	w.weight = (double)edit.weight.GetData();
	w.fat = (double)edit.fat.GetData();
	w.liquid = (double)edit.liquid.GetData();
	w.muscle = (double)edit.muscle.GetData();
	w.bmi = (double)edit.bmi.GetData();
	w.is_dexa = edit.dexa.Get();
	
	if (!front.IsEmpty())
		JPGEncoder().SaveFile(w.GetFrontFile(), front);
	if (!right.IsEmpty())
		JPGEncoder().SaveFile(w.GetRightFile(), right);
	if (!back.IsEmpty())
		JPGEncoder().SaveFile(w.GetBackFile(), back);
	
	prof.StoreThis();
	
	Data();
};

void WeightCtrl::PreviewCamera() {
	if (!stopped)
		return;
	
	running = true; stopped = false;
	Thread::Start(THISBACK2(ProcessCapture, true, edit.cameras.GetIndex()));
}

void WeightCtrl::CaptureImages() {
	if (!stopped) {
		running = false;
		return;
	}
	
	running = true; stopped = false;
	Thread::Start(THISBACK2(ProcessCapture, false, edit.cameras.GetIndex()));
}

void WeightCtrl::OpenCamera(int i) {
	cap_lock.Enter();
	cap.OpenSize(i, Size(1920,1080));
	cap_lock.Leave();
	last_camera_i = i;
	StoreThis();
}

void WeightCtrl::CloseCamera() {
	cap_lock.Enter();
	cap.Close();
	cap_lock.Leave();
}

void SetScaledImageCtrl(ImageCtrl& ic, Image i) {
	Size sz = ic.GetSize();
	if (i.IsEmpty()) {
		ic.SetImage(Image());
		return;
	}
	Size img_sz = i.GetSize();
	double hratio = (double)sz.cx / img_sz.cx;
	double vratio = (double)sz.cy / img_sz.cy;
	double ratio = min(hratio, vratio);
	img_sz.cx *= ratio;
	img_sz.cy *= ratio;
	Image img = CachedRescale(i, img_sz);
	ic.SetImage(img);
}

void WeightCtrl::SetCamImage() {
	live_lock.Enter();
	Image i = last_cap;
	live_lock.Leave();
	SetScaledImageCtrl(edit.live, i);
}

void WeightCtrl::ProcessCapture(bool preview, int cam) {
	OpenCamera(cam);
	
	if (preview) {
		cap_lock.Enter();
		cap.Process(100);
		Image img = cap.GetImage(Size(1920,1080));
		
		live_lock.Enter();
		last_cap = img;
		live_lock.Leave();
		
		PostCallback(THISBACK(SetCamImage));
		cap_lock.Leave();
	}
	else {
		capture_mode = COUNTDOWN_FRONT;
		SetCountdown(5);
		while (running) {
			
			cap_lock.Enter();
			cap.Process(100);
			Image img = cap.GetImage(Size(1920,1080));
			
			live_lock.Enter();
			last_cap = img;
			live_lock.Leave();
			
			PostCallback(THISBACK(SetCamImage));
			cap_lock.Leave();
			
			if (capture_mode == COUNTDOWN_FRONT ||
				capture_mode == COUNTDOWN_RIGHT ||
				capture_mode == COUNTDOWN_BACK) {
				if (ts.Elapsed() >= 1000) {
					SetCountdown(countdown_seconds-1);
					if (!countdown_seconds)
						capture_mode++;
				}
			}
			else {
				PlayCameraShutter();
				
				if (capture_mode == FRONT) {
					front = img;
					PostCallback(THISBACK(SetFrontImage));
				}
				else if (capture_mode == RIGHT) {
					right = img;
					PostCallback(THISBACK(SetRightImage));
				}
				else if (capture_mode == BACK) {
					back = img;
					PostCallback(THISBACK(SetBackImage));
				}
				
				capture_mode++;
				SetCountdown(3);
				if (capture_mode >= MODE_COUNT)
					break;
			}
			
			Sleep(50);
		}
		
		live_lock.Enter();
		last_cap.Clear();
		live_lock.Leave();
		PostCallback(THISBACK(SetCamImage));
	}
	
	PostCallback(THISBACK1(SetInstruction, ""));
	
	
	CloseCamera();
	running = false;
	stopped = true;
}

void WeightCtrl::SetCountdown(int seconds) {
	countdown_seconds = seconds;
	UpdateCountdown();
	ts.Reset();
}


GraphCtrl::GraphCtrl() {
	
}
