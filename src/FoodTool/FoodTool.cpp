#include "FoodTool.h"
#include "Data.brc"
#include <plugin/bz2/bz2.h>

#define IMAGECLASS Images
#define IMAGEFILE <FoodTool/Images.iml>
#include <Draw/iml_source.h>

Color Rainbow(float progress) {
    float div = (fabs(progress - (int)progress) * 6);
    int ascending = (int) ((div - (int)div) * 255);
    int descending = 255 - ascending;

    switch ((int) div)
    {
        case 0:
            return Color(255, ascending, 0);
        case 1:
            return Color(descending, 255, 0);
        case 2:
            return Color(0, 255, ascending);
        case 3:
            return Color(0, descending, 255);
        case 4:
            return Color(ascending, 0, 255);
        default: // case 5:
            return Color(255, 0, descending);
    }
}

Image GetSmiley(String s) {
	static VectorMap<String, Image> cache;
	if (cache.IsEmpty()) {
		for(int i = 0; i < smileys_count; i++) {
			String key = smileys_files[i];
			String png = BZ2Decompress(smileys[i], smileys_length[i]);
			cache.Add(key, StreamRaster::LoadStringAny(png));
		}
	}
	int i = cache.Find(s);
	if (i < 0)
		return Image();
	return cache[i];
}

Image GetSmiley(double progress) {
	progress = max(0.0, min(1.0, progress));
	int div = progress * 9;
	switch (div) {
		case 0: return GetSmiley("worst.png");
		case 1: return GetSmiley("verybad.png");
		case 2: return GetSmiley("bad.png");
		case 3: return GetSmiley("stillbad.png");
		case 4: return GetSmiley("halfway.png");
		case 5: return GetSmiley("slightlygood.png");
		case 6: return GetSmiley("good.png");
		case 7: return GetSmiley("verygood.png");
		case 8: return GetSmiley("best.png");
    }
    return Image();
}











FoodTool::FoodTool()
{
	Title("FoodTool");
	Icon(Images::icon);
	Sizeable().MaximizeBox().MinimizeBox();
	Maximize();
	
	AddFrame(menu);
	menu.Set(THISBACK(MainMenu));
	
	Add(updating_lbl.HCenterPos(300).VCenterPos(30));
	Add(tabs.SizePos());
	
	updating_lbl.Show();
	updating_lbl.SetLabel("Updating plan and meals...");
	updating_lbl.SetFont(SansSerif(30*0.8));
	updating_lbl.SetInk(Color(85, 42, 0));
	
	today.WhenAlert = THISBACK(SetTodayTab);
	
	tabs.Hide();
	tabs.Add(motivation.SizePos(), "Motivation");
	tabs.Add(today.SizePos(), "Today");
	tabs.Add(status.SizePos(), "Status");
	tabs.Add(nutr.SizePos(), "Daily Nutritions");
	tabs.Add(fins.SizePos(), "Food Instructions");
	tabs.Add(graphs.SizePos(), "Graphs");
	tabs.Add(weight.SizePos(), "Weight");
	tabs.Add(def.SizePos(), "Nutrient Deficits");
	tabs.Add(exc.SizePos(), "Exceptions");
	tabs.Add(notes.SizePos(), "Notes");
	tabs.Add(usage.SizePos(), "Usage");
	tabs.Add(db.SizePos(), "Food Database");
	tabs.Add(preset.SizePos(), "Meal Presets");
	tabs.Add(supp.SizePos(), "Food Supplements");
	tabs.Add(wish.SizePos(), "Food Wish-list");
	tabs.Add(storage.SizePos(), "Food Storage");
	tabs.Add(conf.SizePos(), "Configuration");
	tabs.WhenSet << THISBACK(Data);
	
	tc.Set(-500, THISBACK(Data));
	
	GetProfile().WhenUpdateReady << THISBACK(PostData);
}

void FoodTool::MainMenu(Bar& bar) {
	bar.Sub("App", [this](Bar& bar) {
		bar.Add("Save Profile", THISBACK(SaveProfile)).Key(K_CTRL_S);
		bar.Add("Replan", THISBACK(Replan)).Key(K_CTRL_R);
	});
	
}

void FoodTool::SaveProfile() {
	GetProfile().StoreThis();
}

void FoodTool::Replan() {
	GetProfile().Start(true);
}

void FoodTool::SetTodayTab() {
	tabs.Set(1);
	Data();
}

void FoodTool::Data() {
	bool is_updating = GetProfile().IsRunning();
	if (is_updating) {
		if (!was_updating) {
			tabs.Hide();
			updating_lbl.Show();
		}
	}
	else {
		if (was_updating) {
			tabs.Show();
			updating_lbl.Hide();
		}
		int tab = tabs.Get();
		int i = 0;
		if      (tab == i++)	motivation.Data();
		else if (tab == i++)	today.Data();
		else if (tab == i++)	status.Data();
		else if (tab == i++)	nutr.Data();
		else if (tab == i++)	fins.Data();
		else if (tab == i++)	graphs.Data();
		else if (tab == i++)	weight.Data();
		else if (tab == i++)	def.Data();
		else if (tab == i++)	exc.Data();
		else if (tab == i++)	notes.Data();
		else if (tab == i++)	usage.Data();
		else if (tab == i++)	db.Data();
		else if (tab == i++)	preset.Data();
		else if (tab == i++)	supp.Data();
		else if (tab == i++)	wish.Data();
		else if (tab == i++)	storage.Data();
		else if (tab == i++)	conf.Data();
	}
	was_updating = is_updating;
}






MotivationCtrl::MotivationCtrl() {
	quotes.Add("Discipline, drive, and determination makes weight loss possible, but not everything.");
	quotes.Add("Don't just start doing, but plan it beforehand. Those who just thought about getting to a weight loss TV-show got the all weight back.");
	quotes.Add("You don't need to be lightweight to be happy, but it helps.");
	quotes.Add("The scale doesn't lie, unlike people.");
	quotes.Add("Use the naturally evolved instinct of seeing the beauty in a healthy body for your motivation.");
	quotes.Add("Obstacles can occur, which might stop you entirely. Plan beforehand and avoid them.");
	quotes.Add("Strength comes from what you can do. Use your strengths to gain more strength.");
	quotes.Add("Wisdom is doing what makes you happy now and tomorrow as you think of yesterday.");
	quotes.Add("Be curious about health.");
	quotes.Add("Eliminate the mindset of \"everything is possible\". It's not, but your weight loss is. Focus it.");
	quotes.Add("There is nothing holy or sacred in an physical exercise. It is just something, what we have evolved to do for surviving.");
	quotes.Add("You should be respectful to all of your millions of ancestors. Their choices caused the evolution, which made you fat for not eating like them.");
	quotes.Add("You have better chance to live healthy live than any of millions of your ancestors. Why not use it?");
	quotes.Add("If you have a feeling, that you might puke, faint, or die, stop and make a new plan.");
	quotes.Add("When you feel like quitting, make an exception to your schedule to have less or none calorie deficit for some time.");
	quotes.Add("High fat percentage makes you worthless and high lean muscle percentage makes you the most valuable.");
	quotes.Add("Wanting to make something happen is the first step of making it happen. Hold onto it. Cherish it.");
	quotes.Add("Be sensitive to your environment even when you are just waiting for something. It's not all about you getting older.");
	quotes.Add("Don't try to do something repeatedly. Plan to do it once, without possibility of fail, but only with a possibility delaying the success.");
	quotes.Add("If you don't do what's best for your body, others will suffer too in the long term.");
	quotes.Add("The only \"bad\" food is the unethically made one. What people means with \"bad\" is too high/low density, too nutritionally weak or too unpleasantly tasting food.");
	quotes.Add("Instead of indulging in 'comfort food' with high energy density, indulge in 'comfort food' with low energy density. With a little bit of salt, vinegar, oil and spices even cabbage and cucumber tastes good.");
	quotes.Add("The human body is very judgemental about unhealthy habits, people just have unconsciously trained their brain to ignore it.");
	quotes.Add("You should eat high energy density foods with very high awareness of the energy. You probably gain weight if you don't.");
	quotes.Add("A medicine can't replace healthy food nor otherwise. Do your research about both before ingesting.");
	quotes.Add("To ensure happiness: write your thoughts about everything hourly and read them later to spot mistake in your thought. Nothing can ensure health in your life, but using this app helps.");
	quotes.Add("New meal, same old life.");
	quotes.Add("You are a better person, when you have a lot of low energy density food in your plate.");
	quotes.Add("The high energy density food will always reward your brain the most, but you can choose to have lower expectations about the reward.");
	quotes.Add("You can gain weight even with the low energy density food. It is just more improbable to gain weight with it after losing the track of calories.");
	quotes.Add("Oversimplified and stereotypical rules about unhealthy food tasting good and healthy food tasting bad only hurts everybody");
	quotes.Add("Using this app helps you to resist eating everything in your house.");
	/*quotes.Add("\"The only way to lose weight is to check it in as airline baggage.\" - Peggy Ryan");
	quotes.Add("\"It would be far easier to lose weight permanently if replacement parts weren't so handy in the refrigerator.\" - Hugh Allen");
	quotes.Add("\"When we lose twenty pounds, we may be losing the twenty best pounds we have! We may be losing the pounds that contain our genius, our humanity, our love and honesty.\" - Woody Allen");
	quotes.Add("\"Keep an open mind and a closed refrigerator.\" - Anonymous");
	quotes.Add("\"My favorite exercise is a cross between a lunge and a crunch. I call it lunch.\" - Anonymous");
	quotes.Add("\"Only I can change my life. No one can do it for me.\" - Anonymous");
	quotes.Add("\"Weight loss doesn't begin in the gym with a dumbbell; it starts in your head with a decision.\" - Toni Sorenson");
	quotes.Add("\"Just believe in yourself. Even if you don't, pretend that you do, and at some point, you will.\" - Venus Williams");
	quotes.Add("\"I always believed if you take care of your body it will take care of you.\" - Ted Lindsay");
	quotes.Add("\"Don't wait until you've reached your goal to be proud of yourself. Be proud of every step you take toward reaching that goal.\" - Author Unknown");
	quotes.Add("\"I choose self-care.\" - Alyssa Ardolino");
	quotes.Add("\"Success is the sum of small efforts — repeated day-in and day-out.\" - Robert Collier");
	quotes.Add("\"Triumph by putting a little soul into it!\" - Nancy Mure");
	quotes.Add("\"The mind is everything. What you think you become.\" - Buddha");
	quotes.Add("\"It is better to take small steps in the right direction than to make a great leap forward only to stumble backward.\" - Proverb");
	quotes.Add("Only I Can Change My Life, No One Can Do It For Me");
	quotes.Add("Slow And Steady Wins The Race");
	quotes.Add("The Struggle You Are In Today Is Developing The Strength You Need for Tomorrow");
	quotes.Add("Food, Like Your Money, Should Be Working For You");
	quotes.Add("The Road May Be Bumpy But Stay Committed To The Process");
	quotes.Add("If You Are Tired Of Starting Over, Stop Giving Up");
	quotes.Add("Be The Best Version Of You");
	quotes.Add("Be Stronger Than Your Excuse");
	quotes.Add("It's Not A Diet, It's A Lifestyle Change");
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
	quotes.Add("You Can't Cross The Sea Merely By Standing And Staring At The Water");
	quotes.Add("It has to be hard so you'll never ever forget.");
	quotes.Add("Weight loss doesn't begin in the gym with a dumb bell; it starts in your head with a decision.");
	quotes.Add("If you have discipline, drive, and determination ... nothing is impossible.");
	quotes.Add("Eliminate the mindset of can't — because you can do anything.");
	quotes.Add("Most people give up right before the big break comes — don't let that person be you.");
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
	quotes.Add(" \"If it was about knowledge, we would all be skinny and rich. It's not about what you know but what you do!\"");
	quotes.Add("\"The distance between who am I am and who I want to be is only separated by what I do!\"");
	quotes.Add("\"The only bad workout is the one that didn't happen.\"");
	quotes.Add("\"To change your body you must first change your mind.\"");
	quotes.Add("\"Someone busier than you is running right now.\"");
	quotes.Add("\"Working out is never convenient. But neither is illness, diabetes and obesity!\"");
	quotes.Add("\"I don't stop when I'm tired, I stop when I'm DONE!\"");
	quotes.Add("\"I don't work hard because I hate my body. I workout because I love it!\"");
	quotes.Add("\"Get comfortable with being uncomfortable!\"");
	quotes.Add("\"When it burns, is when you're just getting started. That's when you get stronger!\"");
	quotes.Add("\"If you have 30 minutes for Facebook, you have 1 hour for the gym!\"");
	quotes.Add("\"The best way to predict your health is to create it.\"");
	quotes.Add("\"You can't run from all your problems, but it will help you lose weight.\"");
	quotes.Add("\"Get started as if you are motivated. Pretend. And the motivation will come!\"");
	quotes.Add("\"Excuses don't burn calories.\"");
	quotes.Add("\"Your body hears everything your mind says. Keep going. You can!\"");
	quotes.Add("\"Don't stop until you're proud.\"");
	quotes.Add("\"You don't have to go fast, you just have to go.\"");
	quotes.Add("\"If you're tired of starting over; stop giving up!\"");
	quotes.Add("\"On the other side of your workout is the body and health you want!\"");
	quotes.Add("\"The question isn't can you, it's will you!\"");
	quotes.Add("\"Workouts are like life. The harder it is, the STRONGER YOU BECOME!\"");
	quotes.Add("\"If no one thinks you can, then you have to!\"");
	quotes.Add("\"If you still look good at the end of your work out...you didn't work hard enough!\"");
	quotes.Add("\"I'm not running a marathon or a 5 K. I have no upcoming competitions and I don't plan on going to the beach. I am working out like my life depends on it. Because it does!\"");
	quotes.Add("\"The reason fat men are good natured is they can neither fight nor run.\"  - Theodore Roosevelt");
	quotes.Add("\"Fitness is like marriage, you can't cheat on it and expect it to work.\" - Bonnie Pfiester");
	quotes.Add("To enjoy the glow of good health, you must exercise. - Gene Tunney");
	quotes.Add("\"Make time for it. Just get it done. Nobody ever got strong or got in shape by thinking about it. They did it.\" - Jim Wendler");
	quotes.Add("\"Weight loss is not impossible. Weight loss is hard, but hard is not the same as impossible.\" - Author Unknown");
	quotes.Add("\"Even if you are on the right track, you'll get run over if you just sit there.\" - Will Rogers");
	quotes.Add("\"Marathon runners don't worry about the conditions, they just run anyway!\" - Author Unknown");
	quotes.Add("\"Nothing tastes as good as being thin feels.\" ― Elizabeth Berg");
	quotes.Add("Champions keep playing until they get it right - Billie Jean King");
	quotes.Add("\"Walking: the most ancient exercise and still the best modern exercise.\" ~Carrie Latet");
	quotes.Add("If you aren't going all the way, why go at all? - Joe Namath");
	quotes.Add("\"Success is the sum of small efforts, repeated day-in and day-out.\" -Robert Collier");
	quotes.Add("\"Every day, in every way, I am getting better and better.\" ~Emilie Coue");
	quotes.Add("\"Those who think they have not time for bodily exercise will sooner or later have to find time for illness.\" ~Edward Stanley");
	quotes.Add("\"Just believe in yourself. Even if you don't, pretend that you do and, at some point, you will.\" - Venus Williams");
	quotes.Add("\"Don't reward yourself with food. You are not a dog. - Unknown");
	quotes.Add("\"Every weight loss program, no matter how positively it's packaged, whispers to you that you're not right. You're not good enough. You're unacceptable and you need to be fixed.\" ― Kim Brittingham");
	quotes.Add("\"The only successful way to reach and maintain a healthy weight is to find what works for you.\" - Author Unknown");
	quotes.Add("\"Getting my lifelong weight struggle under control has come from a process of treating myself as well as I treat others in every way.\" - Oprah Winfrey");
	quotes.Add("Don't compare yourself to others. Compare yourself to the person from yesterday. - Author Unknown");
	quotes.Add("\"A huge part of losing weight is believing you can do it and realizing it's not going to happen overnight.\" - Author Unknown");
	quotes.Add("\"The hardest step to fitness is the first. Take it now!\" — Heather Montgomery");
	quotes.Add("\"Your body can do anything. It's your mind that needs convincing\" - Unknown");
	quotes.Add("Your goals are as good as your actions. No action taken, no goals achieved - Derric Yuh Ndim");
	quotes.Add("\"If you have discipline, drive, and determination... nothing is impossible.\" - Dana Linn Bailey");
	quotes.Add("\"Eliminate the mindset of can't — because you can do anything.\" - Toni Horton");
	quotes.Add("\"Some people want it to happen. Some wish it would happen. Others make it happen.\" - Michael Jordan");
	quotes.Add("\"Exercise should be regarded as a tribute to the heart.\" - Gene Tunney");
	quotes.Add("\"If you don't do what's best for your body, you're the one who comes up on the short end.\" - Julius Erving");
	quotes.Add("\"When you feel like quitting, think about why you started.\" - Author Unknown");
	quotes.Add("\"If you keep good food in your fridge, you will eat good food.\" - Errick McAdams");
	quotes.Add("\"If you wait, all that happens is you get older.\" - Larry McMurtry");
	quotes.Add("\"To ensure good health: eat lightly, breathe deeply, live moderately, cultivate cheerfulness, and maintain an interest in life.\" - William Londen");
	quotes.Add("\"Only I can change my life. No one can do it for me.\" - Anonymous");
	quotes.Add("\"I always believed if you take care of your body it will take care of you.\" - Ted Lindsay");
	quotes.Add("\"It is better to take small steps in the right direction than to make a great leap forward only to stumble backward.\" - Proverb");
	quotes.Add("\"The past cannot be changed. The future is yet in your power.\"― Mary Pickford");
	quotes.Add("\"Weight loss doesn't begin in the gym with a dumbbell; it starts in your head with a decision.\" - Toni Sorenson");
	quotes.Add("\"Triumph by putting a little soul into it!\" - Nancy Mure");*/
	
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
		DailyPlan* it = prof.planned_daily.Begin();
		DailyPlan* today = it;
		DailyPlan* end = prof.planned_daily.End();
		DailyPlan* lowest_fat_perc = NULL;
		while (it != end) {
			if (it->date == now)
				today = it;
			if (it->date > now) {
				if (lowest_fat_perc == NULL && (it->variant_type == VARIANT_MUSCLEGAIN || it->variant_type == VARIANT_MAINTENANCE))
					lowest_fat_perc = it;
				if (it->variant_type == VARIANT_MAINTENANCE)
					break;
			}
			it++;
		}
		int ydiff = 0;
		int days_remaining = lowest_fat_perc - today;
		if (days_remaining >= 0) {
			String remaining = IntStr(days_remaining) + t_(" days until lowest fat-\%");
			if (days_remaining < 30) {
				fnt = SansSerif(40);
				remaining << "!!!";
			}
			else {
				fnt = SansSerif(10);
				remaining << "...";
			}
			Size txt2_sz = GetTextSize(remaining, fnt);
			int txt2_y = txt_y + txt_sz.cy + 15 + ydiff;
			int txt2_x = (sz.cx - txt2_sz.cx) / 2;
			d.DrawText(txt2_x, txt2_y, remaining, fnt, White());
			ydiff += fnt.GetHeight();
		}
		
		days_remaining = it - today;
		if (days_remaining >= 0) {
			String remaining = IntStr(days_remaining) + t_(" days until maintenance");
			if (days_remaining < 30) {
				fnt = SansSerif(40);
				remaining << "!!!";
			}
			else {
				fnt = SansSerif(10);
				remaining << "...";
			}
			Size txt2_sz = GetTextSize(remaining, fnt);
			int txt2_y = txt_y + txt_sz.cy + 15 + ydiff;
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
	
	today.Refresh();
}




ConfigurationCtrl::ConfigurationCtrl() {
	Add(split.SizePos());
	split.Horz() << list << conf;
	split.SetPos(2500);
	
	CtrlLayout(conf);
	conf.next.SetLabel(t_("Add new"));
	conf.gender.SetEditable(false);
	conf.next <<= THISBACK(AddConf);
	list.AddColumn("#");
	list.AddColumn(t_("Added"));
	list.ColumnWidths("1 5");
	list.WhenAction = THISBACK(SelectConf);
	list.WhenLeftClick = THISBACK(SelectConf);
	
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
		if (!list.IsCursor() && list.GetCount())
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
	conf.activity.SetData(c.activity);
	conf.walking_dist.SetData(c.walking_dist);
	conf.tgt_walking_dist.SetData(c.tgt_walking_dist);
	conf.tgt_jogging_dist.SetData(c.tgt_jogging_dist);
	conf.shop_interval.SetData(c.shop_interval);
	conf.hours_between_meals.SetData(c.hours_between_meals);
	conf.hours_between_making_meals.SetData(c.hours_between_making_meals);
	conf.easy_day_interval.SetData(c.easy_day_interval);
	conf.waking.SetTime(c.waking_hour,c.waking_minute, 0);
	conf.sleeping.SetTime(c.sleeping_hour,c.sleeping_minute,0);
	conf.coffee.SetData(c.daily_coffee);
}

void ConfigurationCtrl::AddConf() {
	Profile& prof = GetProfile();
	Configuration& c = prof.confs.Add();
	
	c.added = GetSysTime();
	c.end_date = Null;
	c.height = conf.height.GetData();
	c.age = conf.age.GetData();
	c.activity = conf.activity.GetData();
	c.walking_dist = conf.walking_dist.GetData();
	c.tgt_walking_dist = conf.tgt_walking_dist.GetData();
	c.tgt_jogging_dist = conf.tgt_jogging_dist.GetData();
	c.shop_interval = conf.shop_interval.GetData();
	c.hours_between_meals = conf.hours_between_meals.GetData();
	c.hours_between_making_meals = conf.hours_between_making_meals.GetData();
	c.easy_day_interval = conf.easy_day_interval.GetData();
	c.waking_hour = conf.waking.GetHour();
	c.waking_minute = conf.waking.GetMinute();
	c.sleeping_hour = conf.sleeping.GetHour();
	c.sleeping_minute = conf.sleeping.GetMinute();
	c.daily_coffee = conf.coffee.GetData();
	
	prof.Start(true);
	
	Data();
}




ExceptionsCtrl::ExceptionsCtrl() {
	CtrlLayout(exc);
	Add(split.SizePos());
	split.Horz() << list << exc;
	split.SetPos(3333);
	
	list.AddColumn("#");
	list.AddColumn(t_("Begin"));
	list.AddColumn(t_("Days"));
	list.AddColumn(t_("Deficit"));
	list.AddColumn(t_("Reason"));
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
		if (!list.IsCursor() && list.GetCount())
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
	
	prof.Start(true);
	
	Data(true);
}




NoteCtrl::NoteCtrl() {
	CtrlLayout(note);
	Add(split.SizePos());
	split.Horz() << list << note;
	split.SetPos(2500);
	
	list.AddColumn(t_("Added"));
	list.AddColumn(t_("Title"));
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
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectNote();
		split.SetPos(2500);
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
		.Add("A", 1, Rainbow((double)i++ / vitamin_count))
		.Add("B1", 1, Rainbow((double)i++ / vitamin_count))
		.Add("B2", 1, Rainbow((double)i++ / vitamin_count))
		.Add("B3", 1, Rainbow((double)i++ / vitamin_count))
		.Add("B5", 1, Rainbow((double)i++ / vitamin_count))
		.Add("B6", 1, Rainbow((double)i++ / vitamin_count))
		//.Add("B7", 1, Rainbow((double)i++ / vitamin_count))
		.Add("B9", 1, Rainbow((double)i++ / vitamin_count))
		.Add("B12", 1, Rainbow((double)i++ / vitamin_count))
		.Add("C", 1, Rainbow((double)i++ / vitamin_count))
		.Add("D", 1, Rainbow((double)i++ / vitamin_count))
		.Add("E", 1, Rainbow((double)i++ / vitamin_count))
		.Add("K", 1, Rainbow((double)i++ / vitamin_count))
		.Add("Carotenoids", 1, Rainbow((double)i++ / vitamin_count))
		;
	
	
	i = 0;
	int aminoacid_count = 14;
	list.Add(t_("Menu essential amino acids (\% of recommended)"));
	graph.Add().Vert(100)
		.Add(t_("Histidine"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Isoleucine"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Leucine"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Lysine"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Methionine"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Phenylalanine"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Threonine"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Tryptophan"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Valine"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Arginine"), 1, Rainbow((double)i++ / aminoacid_count))
		//.Add(t_("Cystine"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Glutamine"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Glycine"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Proline"), 1, Rainbow((double)i++ / aminoacid_count))
		.Add(t_("Tyrosine"), 1, Rainbow((double)i++ / aminoacid_count))
		;
		
	int mineral_count = 10;
	list.Add(t_("Menu essential minerals (\% of recommended)"));
	graph.Add().Vert(100)
		.Add(t_("Potassium"), 1, Rainbow(0.0 / mineral_count))
		.Add(t_("Sodium"), 1, Rainbow(2.0 / mineral_count))
		//.Add(t_("Chloride"), 1, Rainbow(1.0 / mineral_count))
		.Add(t_("Calcium"), 1, Rainbow(3.0 / mineral_count))
		.Add(t_("Phosphorus"), 1, Rainbow(4.0 / mineral_count))
		.Add(t_("Magnesium"), 1, Rainbow(5.0 / mineral_count))
		.Add(t_("Iron"), 1, Rainbow(6.0 / mineral_count))
		.Add(t_("Zinc"), 1, Rainbow(7.0 / mineral_count))
		.Add(t_("Manganese"), 1, Rainbow(8.0 / mineral_count))
		.Add(t_("Copper"), 1, Rainbow(9.0 / mineral_count))
		//.Add(t_("Iodine"), 1, Rainbow(10.0 / mineral_count))
		//.Add(t_("Chromium"), 1, Rainbow(11.0 / mineral_count))
		//.Add(t_("Molybdenum"), 1, Rainbow(12.0 / mineral_count))
		.Add(t_("Selenium"), 1, Rainbow(13.0 / mineral_count))
		//.Add(t_("Cobalt"), 1, Rainbow(14.0 / mineral_count))
		//.Add(t_("Boron"), 1, Rainbow(15.0 / mineral_count))
		//.Add(t_("Silicon"), 1, Rainbow(16.0 / mineral_count))
		//.Add(t_("Bromine"), 1, Rainbow(17.0 / mineral_count))
		//.Add(t_("Arsenic"), 1, Rainbow(18.0 / mineral_count))
		//.Add(t_("Nickel"), 1, Rainbow(19.0 / mineral_count))
		//.Add(t_("Fluoride"), 1, Rainbow(20.0 / mineral_count))
		//.Add(t_("Lithium"), 1, Rainbow(21.0 / mineral_count))
		//.Add(t_("Strontium"), 1, Rainbow(22.0 / mineral_count))
		//.Add(t_("Vanadium"), 1, Rainbow(23.0 / mineral_count))
		//.Add(t_("Tin"), 1, Rainbow(31.0 / mineral_count))
		//.Add(t_("Phosphate"), 1, Rainbow(32.0 / mineral_count))
		//.Add(t_("Bicarbonate"), 1, Rainbow(33.0 / mineral_count))
		;
	
	i = 0;
	int fattyacid_count = 7;
	list.Add(t_("Menu essential fatty acids (\% of recommended)"));
	graph.Add().Vert(100)
		.Add(t_("Cholesterol"), 1, Rainbow((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total trans"), 1, Rainbow((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total saturated"), 1, Rainbow((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total monounsaturated"), 1, Rainbow((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total polyunsaturated"), 1, Rainbow((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total trans-monoenoic"), 1, Rainbow((double)(i++) / fattyacid_count))
		.Add(t_("Fatty acids, total trans-polyenoic"), 1, Rainbow((double)(i++) / fattyacid_count));
	
	i = 0;
	int other_count = 2;
	list.Add(t_("Menu essential other (\% of recommended)"));
	graph.Add().Vert(100)
		.Add(t_("Choline"), 1, Rainbow((double)(i++) / other_count))
		.Add(t_("Fiber"), 1, Rainbow((double)(i++) / other_count))
		//.Add(t_("Inositol"), 1, Rainbow((double)(i++) / other_count))
		//.Add(t_("Taurine"), 1, Rainbow((double)(i++) / other_count))
		//.Add(t_("Nucleotides"), 1, Rainbow((double)(i++) / other_count))
		;
		
	
	i = 0;
	int electrolyte_count = 5;
	list.Add(t_("Menu electrolytes (\% of recommended)"));
	graph.Add().Vert(100)
		//.Add(t_("Bicarbonate"), 1, Rainbow((double)(i++) / electrolyte_count))
		.Add(t_("Calcium"), 1, Rainbow((double)(i++) / electrolyte_count))
		//.Add(t_("Chloride"), 1, Rainbow((double)(i++) / electrolyte_count))
		.Add(t_("Magnesium"), 1, Rainbow((double)(i++) / electrolyte_count))
		.Add(t_("Phosphorus"), 1, Rainbow((double)(i++) / electrolyte_count))
		.Add(t_("Potassium"), 1, Rainbow((double)(i++) / electrolyte_count))
		.Add(t_("Sodium"), 1, Rainbow((double)(i++) / electrolyte_count));
	
	list.Add(t_("Measured weight"));
	graph.Add()
		.Add(t_("Weight"), 2, Color(109, 0, 117));
	
	list.Add(t_("Measured fat"));
	graph.Add()
		.Add(t_("Fat"), 2, Color(81, 48, 0));
		
	list.Add(t_("Measured liquid"));
	graph.Add()
		.Add(t_("Liquid"), 2, Color(0, 176, 137));
		
	list.Add(t_("Measured muscle"));
	graph.Add()
		.Add(t_("Muscle"), 2, Color(120, 0, 0));
	
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

String MultipurposeGraph::GetLineTitle(int s, int l) {return src[s].lines[l].title;}
int MultipurposeGraph::GetLineCount(int s) {return src[s].lines.GetCount();}
Color MultipurposeGraph::GetLineColor(int s, int l) {return src[s].lines[l].color;}
int MultipurposeGraph::GetLineWidth(int s, int l) {return src[s].lines[l].width;}

#define MEASURE_FORECAST 10

int MultipurposeGraph::GetCount(int s) {
	if (s < 4)
		return GetProfile().planned_daily.GetCount();
	else if (s < 14)
		return GetProfile().storage.days.GetCount();
	else if (s < 27)
		return GetProfile().weights.GetCount() + MEASURE_FORECAST;
	Panic("Invalid source");
	return -1;
}

const Vector<double>& MultipurposeGraph::GetValue(int src, int l) {
	if (tmp.GetCount() <= l)
		tmp.SetCount(l+1);
	
	const Profile& prof = GetProfile();
	const Configuration& conf = prof.confs.Top();
	const Database& db = DB();
	
	Vector<double>& v = tmp[l];
	if (v.IsEmpty()) {
		v.SetCount(GetCount(src));
		
		if (src == 0) {
			if (l == 0) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].lean_kgs;
			}
			else if (l == 1) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].fat_kgs;
			}
			else if (l == 2) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].weight;
			}
		}
		else if (src == 1) {
			if (l == 0) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].maintain_calories;
			}
			else if (l == 1) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].burned_calories;
			}
			else if (l == 2) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].allowed_calories;
			}
		}
		else if (src == 2) {
			if (l == 0) {
				for(int i = 0; i < prof.planned_daily.GetCount(); i++)
					v[i] = prof.planned_daily[i].burned_kgs;
			}
		}
		else if (src == 3) {
			int n;
			if (l == 0) {
				n = FAT;
			}
			else if (l == 1) {
				n = CARB;
			}
			else if (l == 2) {
				n = PROT;
			}
			for(int i = 0; i < prof.planned_daily.GetCount(); i++)
				v[i] = prof.planned_daily[i].food.nutr[n];
		}
		
		else if (src == 4) {
			int n = KCAL;
			if (l == 0) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].target_sum.nutr[n];
			}
			else if (l == 1) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].total_sum.nutr[n];
			}
		}
		else if (src == 5) {
			if (l == 0) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].target_sum.grams;
			}
			else if (l == 1) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].total_sum.grams;
			}
		}
		else if (src == 6) {
			int n = -1;
			if (l == 0 || l == 3) {
				n = FAT;
			}
			else if (l == 1 || l == 4) {
				n = CARB;
			}
			else if (l == 2 || l == 5) {
				n = PROT;
			}
			if (l < 3) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].target_sum.nutr[n];
			}
			else {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].total_sum.nutr[n];
			}
		}
		else if (src == 7) {
			int n = SODIUM;
			if (l == 0) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].target_sum.nutr[n];
			}
			else if (l == 1) {
				for(int i = 0; i < prof.storage.days.GetCount(); i++)
					v[i] = prof.storage.days[i].total_sum.nutr[n];
			}
		}
		
		else if (src == 8) {
			String key;
			switch (l) {
				case 0: key = ("VITA_RAE"); break;
				case 1: key = ("THIA"); break;
				case 2: key = ("RIBF"); break;
				case 3: key = ("NIA"); break;
				case 4: key = ("PANTAC"); break;
				case 5: key = ("VITB6A"); break;
				case 6: key = ("FOL"); break;
				case 7: key = ("VITB12"); break;
				case 8: key = ("VITC"); break;
				case 9: key = ("VITDMCR"); break;
				case 10: key = ("TOCPHA"); break;
				case 11: key = ("VITK1"); break;
				case 12: key = ("CARTB"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		else if (src == 9) {
			String key;
			switch (l) {
				case 0: key = ("HISTN_G"); break;
				case 1: key = ("ILE_G"); break;
				case 2: key = ("LEU_G"); break;
				case 3: key = ("LYS_G"); break;
				case 4: key = ("MET_G"); break;
				case 5: key = ("PHE_G"); break;
				case 6: key = ("THR_G"); break;
				case 7: key = ("TRP_G"); break;
				case 8: key = ("VAL_G"); break;
				case 9: key = ("ARG_G"); break;
				case 10: key = ("GLU_G"); break;
				case 11: key = ("GLY_G"); break;
				case 12: key = ("PRO_G"); break;
				case 13: key = ("TYR_G"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		else if (src == 10) {
			String key;
			switch (l) {
				case 0: key = ("K"); break;
				case 1: key = ("NA"); break;
				case 2: key = ("CA"); break;
				case 3: key = ("P"); break;
				case 4: key = ("MG"); break;
				case 5: key = ("FE"); break;
				case 6: key = ("ZN"); break;
				case 7: key = ("MN"); break;
				case 8: key = ("CU"); break;
				case 9: key = ("SE"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		else if (src == 11) {
			String key;
			switch (l) {
				case 0: key = ("CHOLE"); break;
				case 1: key = ("FATRN"); break;
				case 2: key = ("FASAT"); break;
				case 3: key = ("FAMS"); break;
				case 4: key = ("FAPU"); break;
				case 5: key = ("FATRNM"); break;
				case 6: key = ("FATRNP"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		else if (src == 12) {
			String key;
			switch (l) {
				case 0: key = ("CHOLN"); break;
				case 1: key = ("FIBTG"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		else if (src == 13) {
			String key;
			switch (l) {
				case 0: key = ("CA"); break;
				case 1: key = ("MG"); break;
				case 2: key = ("P"); break;
				case 3: key = ("K"); break;
				case 4: key = ("NA"); break;
			}
			int n = db.FindNutrition(key);
			int r = db.FindNutritionRecommendation(key);
			const auto& recom = db.nutr_recom[r];
			for(int i = 0; i < prof.storage.days.GetCount(); i++)
				v[i] = prof.storage.days[i].total_sum.nutr[n] / recom.GetValue(prof.planned_daily[i].weight) * 100;
		}
		
		else if (src == 14) {
			if (l == 0) {
				int i = 0;
				for(; i < prof.weights.GetCount(); i++) {
					const WeightLossStat& s = prof.weights[i];
					v[i] = s.weight;
				}
				
				const DailyPlan* p = prof.planned_daily.Begin();
				const DailyPlan* end = prof.planned_daily.End();
				
				Date begin = Date(prof.weights.Top().added) + 1;
				while (p != end) {
					if (p->date >= begin)
						break;
					p++;
				}
				int count = 0;
				while (p != end) {
					v[i + count++] = p->weight;
					v[i + count++] = p->weight;
					if (count >= MEASURE_FORECAST)
						break;
					p++;
				}
			}
		}
		else if (src == 15) {
			if (l == 0) {
				int i = 0;
				for(; i < prof.weights.GetCount(); i++) {
					const WeightLossStat& s = prof.weights[i];
					v[i] = s.weight * s.fat * 0.01;
				}
				
				const DailyPlan* p = prof.planned_daily.Begin();
				const DailyPlan* end = prof.planned_daily.End();
				
				Date begin = Date(prof.weights.Top().added) + 1;
				while (p != end) {
					if (p->date >= begin)
						break;
					p++;
				}
				int count = 0;
				while (p != end) {
					v[i + count++] = p->fat_kgs;
					v[i + count++] = p->fat_kgs;
					if (count >= MEASURE_FORECAST)
						break;
					p++;
				}
			}
			FillVector(v);
		}
		else if (src == 16) {
			if (l == 0) {
				#define MEAS(var) \
				for(int i = 0; i < prof.weights.GetCount(); i++) \
					v[i] = prof.weights[i].var; \
				double diff = (prof.weights.Top().var - prof.weights[0].var) / prof.weights.GetCount(); \
				for(int i = prof.weights.GetCount(); i < v.GetCount(); i++) \
					v[i] = v[i-1] + diff;
				MEAS(GetLiquidKg());
			}
			FillVector(v);
		}
		else if (src == 17) {
			if (l == 0) {
				int i = 0;
				for(; i < prof.weights.GetCount(); i++) {
					const WeightLossStat& s = prof.weights[i];
					v[i] = s.weight * s.muscle * 0.01;
				}
				
				const DailyPlan* p = prof.planned_daily.Begin();
				const DailyPlan* end = prof.planned_daily.End();
				
				Date begin = Date(prof.weights.Top().added) + 1;
				while (p != end) {
					if (p->date >= begin)
						break;
					p++;
				}
				int count = 0;
				while (p != end) {
					v[i + count++] = p->lean_kgs;
					v[i + count++] = p->lean_kgs;
					if (count >= MEASURE_FORECAST)
						break;
					p++;
				}
			}
			FillVector(v);
		}
		else if (src == 18) {
			if (l == 0) {
				int i = 0;
				for(; i < prof.weights.GetCount(); i++) {
					const WeightLossStat& s = prof.weights[i];
					v[i] = GetBMI(conf.height * 0.01, s.weight);
				}
				
				const DailyPlan* p = prof.planned_daily.Begin();
				const DailyPlan* end = prof.planned_daily.End();
				
				Date begin = Date(prof.weights.Top().added) + 1;
				while (p != end) {
					if (p->date >= begin)
						break;
					p++;
				}
				int count = 0;
				while (p != end) {
					double bmi = GetBMI(conf.height * 0.01, p->weight);
					v[i + count++] = bmi;
					v[i + count++] = bmi;
					if (count >= MEASURE_FORECAST)
						break;
					p++;
				}
			}
			FillVector(v);
		}
		else if (src == 19) {
			if (l == 0) {
				MEAS(neck);
			}
			FillVector(v);
		}
		else if (src == 20) {
			if (l == 0) {
				MEAS(bicep);
			}
			FillVector(v);
		}
		else if (src == 21) {
			if (l == 0) {
				MEAS(forearm);
			}
			FillVector(v);
		}
		else if (src == 22) {
			if (l == 0) {
				MEAS(chest);
			}
			FillVector(v);
		}
		else if (src == 23) {
			if (l == 0) {
				MEAS(waist);
			}
			FillVector(v);
		}
		else if (src == 24) {
			if (l == 0) {
				MEAS(buttocks);
			}
			FillVector(v);
		}
		else if (src == 25) {
			if (l == 0) {
				MEAS(thigh);
			}
			FillVector(v);
		}
		else if (src == 26) {
			if (l == 0) {
				MEAS(leg);
			}
			FillVector(v);
		}
	}
	return v;
}

int MultipurposeGraph::GetVertLine(int s) {
	Src& ss = src[s];
	return ss.vert_line;
}

int MultipurposeGraph::GetHorzLine(int s) {
	const Profile& prof = GetProfile();
	if (s < 4) {
		Date today = GetSysTime();
		for(int i = 0; i < prof.planned_daily.GetCount(); i++)
			if (prof.planned_daily[i].date == today)
				return i;
		return 0;
	}
	else if (s < 14) {
		Date today = GetSysTime();
		for(int i = 0; i < prof.storage.days.GetCount(); i++)
			if (prof.storage.days[i].date == today)
				return i;
		return 0;
	}
	else if (s < 27)
		return prof.weights.GetCount()-1;
	Panic("Invalid source");
	return -1;
}

void GraphCtrl::SelectSource() {
	if (!list.IsCursor()) return;
	graph.new_src = list.GetCursor();
	graph.Refresh();
}

void MultipurposeGraph::Paint(Draw& d) {
	int src = new_src;
	for(auto& a : tmp)
		a.SetCount(0);
	
	Size sz(GetSize());
	d.DrawRect(sz, White());
	
	int vert = GetVertLine(src);
	
	bool get_min = vert || src >= MEASURES_BEGIN;
	
	double min = get_min ? +DBL_MAX : 0;
	double max = -DBL_MAX;
	double last = 0.0;
	
	
	int count = GetCount(src);
	int line_count = GetLineCount(src);
	for(int l = 0; l < line_count; l++) {
		const auto& values = GetValue(src, l);
		for(int j = 0; j < count; j++) {
			double d = values[j];
			if (d > max)
				max = d;
			if (get_min && d < min)
				min = d;
		}
	}
	
	Font fnt = Monospace(10);
	
	if (count > 1 && max >= min) {
		double diff = max - min;
		double xstep = (double)sz.cx / (count - 1);
		
		int xoff = 0; // 16
		int text_y = 0;
		
		for(int l = 0; l < line_count; l++) {
			double line_max = -DBL_MAX;
			double line_min = +DBL_MAX;
			const auto& values = GetValue(src, l);
			
			polyline.SetCount(0);
			for(int j = 0; j < count; j++) {
				double v = values[j];
				last = v;
				int x = (int)(j * xstep);
				int y = (int)(sz.cy - (v - min) / diff * sz.cy);
				polyline.Add(Point(x, y));
				if (v > line_max) line_max = v;
				if (v < line_min) line_min = v;
			}
			if (polyline.GetCount() >= 2) {
				Color clr = GetLineColor(src, l);
				d.DrawPolyline(polyline, GetLineWidth(src, l), clr);
				
				String txt = GetLineTitle(src, l) + " - min " + Format("%2n", line_min) + " - max " + Format("%2n", line_max);
				Size str_sz = GetTextSize(txt, fnt);
				d.DrawRect(xoff, text_y, str_sz.cx, str_sz.cy, White());
				d.DrawText(xoff, text_y, txt, fnt.Bold(), Blend(clr, Black()));
				text_y += 10;
			}
		}
		
		if (vert) {
			int y = (int)(sz.cy - (vert - min) / diff * sz.cy);
			d.DrawLine(0, y, sz.cx, y, 1, Black());
		}
		
		int horz = GetHorzLine(src);
		if (horz) {
			int x = (int)(horz * xstep);
			d.DrawLine(x, 0, x, sz.cy, 1, Black());
		}
	}
	else {
		String txt = "Waiting for data";
		Size txt_sz = GetTextSize(txt, fnt);
		d.DrawText((sz.cx - txt_sz.cx) / 2, (sz.cy - txt_sz.cy) / 2, txt, fnt);
	}
}











TodayScheduleCtrl::TodayScheduleCtrl() {
	Profile& prof = GetProfile();
	prof.MakeTodaySchedule(sched);
	prof_version = prof.version;
	
	waking_top = Color(255, 254, 0);
	waking_btm = Color(255, 255, 200);
	eating_top = Color(255, 165, 133);
	eating_btm = Color(255, 195, 163);
	walking_top = Color(187, 255, 184);
	walking_btm = Color(241, 255, 160);
	muscletraining_top = Color(106, 31, 0);
	muscletraining_btm = Color(198, 42, 0);
	sleeping_top = Color(156, 158, 193);
	sleeping_btm = Color(180, 196, 229);
	
	tc.Set(-1000, THISBACK(CheckAlerts));
}

void TodayScheduleCtrl::CheckAlerts() {
	String alert_str;
	
	Time now = GetSysTime();
	for(auto& it : sched.items) {
		if (it.time <= now && !it.done) {
			if (alert_str.IsEmpty()) {
				alert_str = it.msg;
			}
			it.done = true;
		}
	}
	
	if (alert_str.GetCount())
		Alert(alert_str);
}

void TodayScheduleCtrl::Alert(String alert_str) {
	GetTopWindow()->Restore();
	GetTopWindow()->Maximize();
	PlayAlert();
	PromptOK(alert_str);
	WhenAlert();
}

void TodayScheduleCtrl::Paint(Draw& d) {
	Profile& prof = GetProfile();
	Date today = GetSysTime();
	if (prof.version > prof_version || sched.items.IsEmpty() || sched.day != today) {
		prof.MakeTodaySchedule(sched);
		prof_version = prof.version;
	}
	
	Size sz(GetSize());
	d.DrawRect(sz, White());
	
	if (sched.items.IsEmpty())
		return;
	
	double ystep = min(60.0, (double)sz.cy / sched.items.GetCount());
	Font fnt = SansSerif(ystep * 0.8);
	
	for(int i = 0; i < sched.items.GetCount(); i++) {
		const auto& it = sched.items[i];
		int y = i * ystep;
		int h = (i + 1) * ystep - y;
		
		Color top, btm;
		String typestr;
		if (it.type == ScheduleToday::WAKING) {
			typestr = t_("Waking");
			top = waking_top;
			btm = waking_btm;
		}
		else if (it.type == ScheduleToday::EATING) {
			typestr = t_("Eating");
			top = eating_top;
			btm = eating_btm;
		}
		else if (it.type == ScheduleToday::WALKING) {
			typestr = t_("Walking");
			top = walking_top;
			btm = walking_btm;
		}
		else if (it.type == ScheduleToday::RUNNING) {
			typestr = t_("Running");
			top = running_top;
			btm = running_btm;
		}
		else if (it.type == ScheduleToday::SLEEPING) {
			typestr = t_("Sleeping");
			top = sleeping_top;
			btm = sleeping_btm;
		}
		else if (it.type == ScheduleToday::MUSCLETRAINING) {
			typestr = t_("Training");
			top = muscletraining_top;
			btm = muscletraining_btm;
		}
		String txt;
		txt << Format("%d:%02d ", (int)it.time.hour, (int)it.time.minute)
			<< typestr << ": " << it.msg;
			
		DrawGradientVert(d, RectC(0, y, sz.cx, h), top, btm);
		d.DrawText(5+2, y+2, txt, fnt, White());
		d.DrawText(5, y, txt, fnt, Black());
	}
	
}














NutrientDeficitCtrl::NutrientDeficitCtrl() {
	Add(split.SizePos());
	split.Horz() << list << right;
	split.SetPos(2500);
	
	right.Add(edit.HSizePos().VSizePos(0,30));
	right.Add(add.BottomPos(0,30).RightPos(0, 200));
	
	add.SetLabel("Add");
	add <<= THISBACK(AddItem);
	
	list.AddColumn("Added");
	list.WhenAction << THISBACK(SelectItem);
	list.WhenLeftClick << THISBACK(SelectItem);
}

void NutrientDeficitCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (prof.defs.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.defs.GetCount(); i++) {
			int row = prof.defs.GetCount() - 1 - i;
			const NutrientDeficitProfile& n = prof.defs[i];
			list.Set(row, 0, n.added);
		}
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectItem();
	}
}

void NutrientDeficitCtrl::SelectItem() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int note_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	const NutrientDeficitProfile& n = prof.defs[note_i];
	edit.prof = n;
	edit.Refresh();
}

void NutrientDeficitCtrl::AddItem() {
	Profile& prof = GetProfile();
	NutrientDeficitProfile& n = prof.defs.Add();
	n = edit.prof;
	n.added = GetSysTime();
	
	Data();
}


void FillVector(Vector<double>& v) {
	int first = -1;
	for(int i = 0; i < v.GetCount(); i++)
		if (v[i] != 0.0)
			{first = i; break;}
	if (first < 0)
		return;
	double left_pad = v[first];
	for(int i = first-1; i >= 0; i--)
		v[i] = left_pad;
	double prev = left_pad;
	for(int i = first+1; i < v.GetCount(); i++) {
		double& d = v[i];
		if (!d)
			d = prev;
		else
			prev = d;
	}
}














FoodInstructionCtrl::FoodInstructionCtrl() {
	Add(hsplit.SizePos());
	hsplit.Horz() << vsplit0 << vsplit1;
	hsplit.SetPos(3333);
	vsplit0.Vert() <<    list << meallist;
	vsplit1.Vert() << inglist << nutrlist;
	
	list.AddIndex();
	list.AddColumn("Date");
	list.AddColumn("Variant");
	list <<= THISBACK(SelectDate);
	
	meallist.AddIndex();
	meallist.AddIndex();
	meallist.AddColumn("#");
	meallist.AddColumn("Name");
	meallist.AddColumn("Grams");
	meallist.ColumnWidths("1 5 1");
	meallist <<= THISBACK(SelectMeal);
	
	inglist.AddIndex();
	inglist.AddIndex();
	inglist.AddColumn("Ingredient");
	inglist.AddColumn("Mass (g)");
	inglist.AddColumn("Energy");
	inglist.AddColumn("Protein");
	inglist.AddColumn("Carbs");
	inglist.AddColumn("Fat");
	inglist.ColumnWidths("4 1 1 1 1");
	inglist <<= THISBACK(SelectIngredient);
	
	nutrlist.AddColumn("Key");
	nutrlist.AddColumn("Value");
	
}

void FoodInstructionCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (prof.storage.days.GetCount() != list.GetCount()) {
		Date today = GetSysTime();
		for(int i = 0; i < prof.storage.days.GetCount(); i++) {
			int row = prof.storage.days.GetCount() - 1 - i;
			const FoodDay& day = prof.storage.days[i];
			const DailyPlan& plan = prof.planned_daily[i];
			String txt = Format("%d.%d.%d", (int)day.date.day, (int)day.date.month, (int)day.date.year);
			
			list.Set(row, 0, (int)plan.variant_type);
			if (day.date == today)
				list.Set(row, 1, AttrText(txt).Paper(Color(28, 85, 0)).Ink(White()));
			else
				list.Set(row, 1, txt);
			list.Set(row, 2, GetVariantString(plan.variant_type));
		}
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectDate();
	}
}

void FoodInstructionCtrl::SelectDate() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int day_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	const FoodDay& day = prof.storage.days[day_i];
	
	VectorMap<int, int> mealtype_grams;
	for(int i = 0; i < day.meals.GetCount(); i++) {
		const Meal& m = day.meals[i];
		int j = prof.FindMealPreset(m.key);
		if (j >= 0)
			mealtype_grams.GetAdd(j, 0) += m.grams;
	}
	
	for(int i = 0; i < mealtype_grams.GetCount(); i++) {
		int preset_i = mealtype_grams.GetKey(i);
		int grams = mealtype_grams[i];
		const MealPreset& mp = prof.presets[preset_i];
		meallist.Set(i, 0, preset_i);
		meallist.Set(i, 1, grams);
		meallist.Set(i, 2, i+1);
		meallist.Set(i, 3, mp.name);
		meallist.Set(i, 4, grams);
	}
	meallist.SetCount(mealtype_grams.GetCount());
	
	if (!meallist.IsCursor() && meallist.GetCount())
		meallist.SetCursor(0);
	
	SelectMeal();
}

void FoodInstructionCtrl::SelectMeal() {
	if (!list.IsCursor() || !meallist.IsCursor())
		return;
	int cursor = list.GetCursor();
	int variant = list.Get(cursor, 0);
	int mcursor = meallist.GetCursor();
	int preset_i = meallist.Get(mcursor, 0);
	int grams = meallist.Get(mcursor, 1);
	
	const Database& db = DB();
	const Profile& prof = GetProfile();
	const MealPreset& mp = prof.presets[preset_i];
	const MealPresetVariant& var = mp.variants[variant];
	
	Ingredient ing;
	var.GetNutritions(ing);
	ing.ChangeGrams(grams);
	
	int row = 0;
	double mul = (double)grams / ing.grams;
	
	for(int i = 0; i < var.ingredients.GetCount(); i++) {
		const MealIngredient& mi = var.ingredients[i];
		const FoodDescription& d = db.food_descriptions[mi.db_food_no];
		int grams = mi.max_grams * mul;
		if (grams > 0) {
			Ingredient ing;
			ing.Set(grams, d);
			inglist.Set(row, 0, mi.db_food_no);
			inglist.Set(row, 1, grams);
			inglist.Set(row, 2, d.long_desc);
			inglist.Set(row, 3, grams);
			inglist.Set(row, 4, Format("%2n", ing.nutr[KCAL]));
			inglist.Set(row, 5, Format("%2n", ing.nutr[PROT]));
			inglist.Set(row, 6, Format("%2n", ing.nutr[CARB]));
			inglist.Set(row, 7, Format("%2n", ing.nutr[FAT]));
			row++;
		}
	}
	
	ing.ChangeGrams(grams);
	inglist.Set(row, 0, -1);
	inglist.Set(row, 1, -1);
	inglist.Set(row, 2, AttrText("Total").NormalPaper(Color(204, 206, 255)));
	inglist.Set(row, 3, ing.grams);
	inglist.Set(row, 4, Format("%2n", ing.nutr[KCAL]));
	inglist.Set(row, 5, Format("%2n", ing.nutr[PROT]));
	inglist.Set(row, 6, Format("%2n", ing.nutr[CARB]));
	inglist.Set(row, 7, Format("%2n", ing.nutr[FAT]));
	row++;
	
	inglist.SetCount(row);
	
	if (!inglist.IsCursor() && inglist.GetCount())
		inglist.SetCursor(0);
	
	SelectIngredient();
}

void FoodInstructionCtrl::SelectIngredient() {
	if (!list.IsCursor() || !meallist.IsCursor() || !inglist.IsCursor())
		return;
	int icursor = inglist.GetCursor();
	int db_no;
	
	int cursor = list.GetCursor();
	int variant = list.Get(cursor, 0);
	int mcursor = meallist.GetCursor();
	int preset_i = meallist.Get(mcursor, 0);
	int grams = meallist.Get(mcursor, 1);
	
	const Database& db = DB();
	const Profile& prof = GetProfile();
	const MealPreset& mp = prof.presets[preset_i];
	const MealPresetVariant& var = mp.variants[variant];
	
	if (icursor >= 0) {
		Ingredient ing;
		db_no = inglist.Get(icursor, 0);
		if (db_no >= 0) {
			const Database& db = DB();
			const FoodDescription& d = db.food_descriptions[db_no];
			int grams = inglist.Get(icursor, 1);
			
			ing.Set(grams, d);
		}
		else {
			var.GetNutritions(ing);
			ing.ChangeGrams(grams);
		}
		int row = 0;
		nutrlist.Set(row, 0, "Mass");
		nutrlist.Set(row++, 1, ing.grams);
		for(int i = 0; i < ing.nutr.GetCount(); i++) {
			double value = ing.nutr[i];
			if (value) {
				nutrlist.Set(row, 0, db.nutr_types[i].nutr_desc);
				nutrlist.Set(row++, 1, Format("%4n", value));
			}
		}
		nutrlist.SetCount(row);
	}
	else nutrlist.SetCount(0);
}
























DailyNutritionsCtrl::DailyNutritionsCtrl() {
	Add(hsplit.SizePos());
	hsplit.Horz() << list << vsplit;
	hsplit.SetPos(3333);
	vsplit.Vert() << srclist << nutrlist;
	vsplit.SetPos(2000);
	
	list.AddIndex();
	list.AddIndex();
	list.AddColumn("Date");
	list.AddColumn("Variant");
	list.AddColumn("Target kcal");
	list.AddColumn("Target weight");
	list.AddColumn("Target protein");
	list.ColumnWidths("2 2 1 1 1");
	list <<= THISBACK(SelectDate);
	
	srclist.AddColumn("Ingredient");
	srclist.AddColumn("Mass (g)");
	srclist.AddColumn("Energy");
	srclist.AddColumn("Protein");
	srclist.AddColumn("Carbs");
	srclist.AddColumn("Fat");
	srclist.ColumnWidths("4 1 1 1 1");
	srclist <<= THISBACK(SelectSource);
	
	nutrlist.AddColumn("Key");
	nutrlist.AddColumn("Value");
	nutrlist.AddColumn("Recommended");
	nutrlist.AddColumn("\% of recommended");
	nutrlist.ColumnWidths("3 1 1 1");
}

void DailyNutritionsCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (prof.storage.days.GetCount() != list.GetCount()) {
		Date today = GetSysTime();
		for(int i = 0; i < prof.storage.days.GetCount(); i++) {
			int row = prof.storage.days.GetCount() - 1 - i;
			const FoodDay& day = prof.storage.days[i];
			const DailyPlan& plan = prof.planned_daily[i];
			String txt = Format("%d.%d.%d", (int)day.date.day, (int)day.date.month, (int)day.date.year);
			
			list.Set(row, 0, i);
			list.Set(row, 1, (int)plan.variant_type);
			if (day.date == today)
				list.Set(row, 2, AttrText(txt).Paper(Color(28, 85, 0)).Ink(White()));
			else
				list.Set(row, 2, txt);
			list.Set(row, 3, GetVariantString(plan.variant_type));
			list.Set(row, 4, (int)plan.food.nutr[KCAL]);
			list.Set(row, 5, Format("%1n", plan.weight));
			list.Set(row, 6, Format("%1n", plan.food.nutr[PROT]));
		}
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectDate();
	}
}

void DailyNutritionsCtrl::SelectDate() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int day_i = list.Get(cursor, 0);
	int variant = list.Get(cursor, 1);
	
	Profile& prof = GetProfile();
	const FoodDay& day = prof.storage.days[day_i];
	
	src.Clear();
	
	Ingredient& total = src.Add("Total");
	{
		Ingredient& food_total = src.Insert(0, "Food");
		
		VectorMap<int, int> mealtype_grams;
		for(int i = 0; i < day.meals.GetCount(); i++) {
			const Meal& m = day.meals[i];
			int j = prof.FindMealPreset(m.key);
			if (j >= 0)
				mealtype_grams.GetAdd(j, 0) += m.grams;
		}
		
		for(int i = 0; i < mealtype_grams.GetCount(); i++) {
			int preset_i = mealtype_grams.GetKey(i);
			int grams = mealtype_grams[i];
			const MealPreset& mp = prof.presets[preset_i];
			
			const MealPresetVariant& var = mp.variants[variant];
		
			Ingredient ing;
			var.GetNutritions(ing);
			ing.ChangeGrams(grams);
			food_total += ing;
		}
		
		total += food_total;
	}
	
	for(int i = 0; i < src.GetCount(); i++) {
		const Ingredient& ing = src[i];
		srclist.Set(i, 0, src.GetKey(i));
		srclist.Set(i, 1, Format("%2n", ing.grams));
		srclist.Set(i, 2, Format("%2n", ing.nutr[KCAL]));
		srclist.Set(i, 3, Format("%2n", ing.nutr[PROT]));
		srclist.Set(i, 4, Format("%2n", ing.nutr[CARB]));
		srclist.Set(i, 5, Format("%2n", ing.nutr[FAT]));
	}
	srclist.SetCount(src.GetCount());
	
	if (!srclist.IsCursor() && srclist.GetCount())
		srclist.SetCursor(0);
	
	SelectSource();
}

void DailyNutritionsCtrl::SelectSource() {
	if (!list.IsCursor() || !srclist.IsCursor())
		return;
	
	int daycursor = list.GetCursor();
	int day_i = list.Get(daycursor, 0);
	int cursor = srclist.GetCursor();
	double weight = GetProfile().planned_daily[day_i].weight;
	
	if (cursor >= 0) {
		const Database& db = DB();
		Ingredient& ing = src[cursor];
		VectorMap<int, double> values;
		for(int i = 0; i < ing.nutr.GetCount(); i++) {
			double value = ing.nutr[i];
			if (value)
				values.Add(i, value);
		}
		
		int row = 0;
		nutrlist.Set(row, 0, "Mass");
		nutrlist.Set(row++, 1, Format("%2n", ing.grams));
		for(const NutritionRecommendation& recom : db.nutr_recom) {
			double value = values.Get(recom.nutr_no, 0);
			if (value > 0.0) {
				double recom_value = recom.GetValue(weight);
				const NutritionType& nt = db.nutr_types[recom.nutr_no];
				nutrlist.Set(row, 0, nt.nutr_desc);
				nutrlist.Set(row, 1, Format("%2n%s", value, nt.units));
				nutrlist.Set(row, 2, Format("%2n%s", recom_value, nt.units));
				nutrlist.Set(row, 3, Format("%1n", value / recom_value * 100));
				row++;
			}
		}
		nutrlist.SetCount(row);
	}
	else nutrlist.SetCount(0);
}












SupplementCtrl::SupplementCtrl() {
	CtrlLayout(*this);
	
	nutrlist.AddColumn("Nutrition");
	nutrlist.AddColumn("Recommendation");
	nutrlist.AddColumn("Weight-Loss");
	nutrlist.AddColumn("Maintenance");
	nutrlist.ColumnWidths("4 1 1 1");
	nutrlist <<= THISBACK(SelectNutrition);
	
	usedlist.AddIndex();
	usedlist.AddColumn("Food Name");
	usedlist.AddColumn("Value /100g");
	usedlist.AddColumn("\% of recommendation");
	usedlist.ColumnWidths("4 1 1");
	
	foodlist.AddIndex();
	foodlist.AddColumn("Food Name");
	foodlist.AddColumn("Value /100g");
	foodlist.AddColumn("\% of recommendation");
	foodlist.ColumnWidths("4 1 1");
	
	add <<= THISBACK(AddFood);
	remove <<= THISBACK(RemoveFood);
	
}

void SupplementCtrl::Data() {
	const Database& db = DB();
	Profile& prof = GetProfile();
	
	if (db.nutr_recom.GetCount() != nutrlist.GetCount()) {
		DailyPlan& today = *prof.GetTodayPlan();
		
		prof.supplements.SetCount(db.nutr_recom.GetCount());
		is_weightloss.SetCount(db.nutr_recom.GetCount());
		is_maintenance.SetCount(db.nutr_recom.GetCount());
		
		for(int i = 0; i < db.nutr_recom.GetCount(); i++) {
			const NutritionRecommendation& recom = db.nutr_recom[i];
			const NutritionType& nt = db.nutr_types[recom.nutr_no];
			Option& wl = is_weightloss[i];
			Option& ma = is_maintenance[i];
			const NutritionSupplement& s = prof.supplements[i];
			wl <<= THISBACK1(ValueChanged, i);
			ma <<= THISBACK1(ValueChanged, i);
			
			nutrlist.Set(i, 0, nt.nutr_desc);
			nutrlist.Set(i, 1, Format("%2n", today.food.nutr[recom.nutr_no]));
			nutrlist.SetCtrl(i, 2, wl);
			nutrlist.SetCtrl(i, 3, ma);
			nutrlist.Set(i, 2, s.is_weightloss);
			nutrlist.Set(i, 3, s.is_maintenance);
		}
		if (!nutrlist.IsCursor() && nutrlist.GetCount())
			nutrlist.SetCursor(0);
		
		SelectNutrition();
	}
}

void SupplementCtrl::SelectNutrition() {
	if (!nutrlist.IsCursor())
		return;
	
	const Database& db = DB();
	Profile& prof = GetProfile();
	int cursor = nutrlist.GetCursor();
	const NutritionSupplement& ns = prof.supplements[cursor];
	const NutritionRecommendation& recom = db.nutr_recom[cursor];
	const DailyPlan& today = *prof.GetTodayPlan();
	Ingredient ing;
	double recom_value = today.food.nutr[recom.nutr_no];
	
	for(int i = 0; i < ns.used_food.GetCount(); i++) {
		int db_no = ns.used_food[i];
		const FoodDescription& d = db.food_descriptions[db_no];
		ing.Set(100, d);
		usedlist.Set(i, 0, db_no);
		usedlist.Set(i, 1, d.long_desc);
		usedlist.Set(i, 2, Format("%2n", ing.nutr[recom.nutr_no]));
		//usedlist.Set(i, 3, Format("%1n", ing.nutr[nutr_i] / recom_value * 100);
		usedlist.Set(i, 3, ing.nutr[recom.nutr_no] / recom_value * 100);
	}
	usedlist.SetCount(ns.used_food.GetCount());
	usedlist.SetSortColumn(2, true);
	
	int row = 0;
	for(int i = 0; i < db.food_descriptions.GetCount(); i++) {
		const FoodDescription& d = db.food_descriptions[i];
		ing.Set(100, d);
		double value = ing.nutr[recom.nutr_no];
		
		if (value > 0) {
			foodlist.Set(row, 0, i);
			foodlist.Set(row, 1, d.long_desc);
			foodlist.Set(row, 2, Format("%2n", value));
			foodlist.Set(row, 3, value / recom_value * 100);
			row++;
		}
	}
	foodlist.SetCount(row);
	foodlist.SetSortColumn(2, true);
	
}

void SupplementCtrl::AddFood() {
	if (!nutrlist.IsCursor() || !foodlist.IsCursor())
		return;
	
	const Database& db = DB();
	Profile& prof = GetProfile();
	int ncursor = nutrlist.GetCursor();
	NutritionSupplement& ns = prof.supplements[ncursor];
	int fcursor = foodlist.GetCursor();
	int db_no = foodlist.Get(fcursor, 0);
	
	ns.used_food.FindAdd(db_no);
	
	SelectNutrition();
	
	if (fcursor+1 < foodlist.GetCount())
		foodlist.SetCursor(fcursor+1);
}

void SupplementCtrl::RemoveFood() {
	if (!nutrlist.IsCursor() || !usedlist.IsCursor())
		return;
	
	const Database& db = DB();
	Profile& prof = GetProfile();
	int ncursor = nutrlist.GetCursor();
	NutritionSupplement& ns = prof.supplements[ncursor];
	int ucursor = usedlist.GetCursor();
	int db_no = usedlist.Get(ucursor, 0);
	
	ns.used_food.RemoveKey(db_no);
	
	SelectNutrition();
}

void SupplementCtrl::ValueChanged(int nutr_i) {
	if (!nutrlist.IsCursor())
		return;
	
	const Database& db = DB();
	Profile& prof = GetProfile();
	NutritionSupplement& ns = prof.supplements[nutr_i];
	
	ns.is_weightloss  = is_weightloss[nutr_i].Get();
	ns.is_maintenance = is_maintenance[nutr_i].Get();
}















FoodWishCtrl::FoodWishCtrl() {
	Add(vsplit.SizePos());
	vsplit << list << ingredients;
	
	list.AddIndex();
	list.AddColumn("Preset");
	list.AddColumn("Score (0-10)");
	list.ColumnWidths("4 1");
	list <<= THISBACK(SelectIngredient);
	
	ingredients.AddColumn("Ingredient");
	ingredients.AddColumn("Average mass");
	ingredients.ColumnWidths("3 1");
}

void FoodWishCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (prof.presets.GetCount() != list.GetCount()) {
		VectorMap<int, double> preset_scores;
		for(int i = 0; i < prof.presets.GetCount(); i++)
			preset_scores.Add(i, prof.presets[i].score);
		SortByValue(preset_scores, StdGreater<double>());
		
		edits.SetCount(max(edits.GetCount(), prof.presets.GetCount()));
		for(int i = 0; i < preset_scores.GetCount(); i++) {
			int preset_i = preset_scores.GetKey(i);
			const MealPreset& mp = prof.presets[preset_i];
			EditIntSpin& edit = edits[preset_i];
			edit.WhenAction = THISBACK1(ValueChanged, preset_i);
			edit.MinMax(0, 10);
			
			list.Set(i, 0, preset_i);
			list.Set(i, 1, mp.name);
			list.SetCtrl(i, 1, edit);
			list.Set(i, 2, mp.wished_factor);
		}
		list.SetCount(prof.presets.GetCount());
	}
	
	if (!list.IsCursor() && list.GetCount())
		list.SetCursor(0);
	
	SelectIngredient();
}

void FoodWishCtrl::SelectIngredient() {
	if (!list.IsCursor())
		return;
	
	int cursor = list.GetCursor();
	int preset_i = list.Get(cursor, 0);
	const Profile& prof = GetProfile();
	const MealPreset& mp = prof.presets[preset_i];
	const Database& db = DB();
	
	for(int i = 0; i < mp.ingredients.GetCount(); i++) {
		const MealIngredient& mi = mp.ingredients[i];
		ingredients.Set(i, 0, db.food_descriptions[mi.db_food_no].long_desc);
		ingredients.Set(i, 1, Format("%1n", (mi.min_grams + mi.max_grams) * 0.5));
	}
	ingredients.SetCount(mp.ingredients.GetCount());
}

void FoodWishCtrl::ValueChanged(int preset_i) {
	Profile& prof = GetProfile();
	int value = edits[preset_i].GetData();
	prof.presets[preset_i].wished_factor = value;
}
