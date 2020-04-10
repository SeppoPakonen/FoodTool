#include "FoodCtrl.h"
#include "data.brc"


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
	quotes.Add("Instead of indulging in 'comfort food' with high energy density, indulge in 'comfort food' with low energy density.");
	quotes.Add("The human body is very judgemental about unhealthy habits, people just have unconsciously trained their brain to ignore it.");
	quotes.Add("You should eat high energy density foods with very high awareness of the energy. You probably gain weight if you don't.");
	quotes.Add("A medicine can't replace healthy food nor otherwise. Do your research about both before ingesting.");
	quotes.Add("To ensure happiness: write your thoughts about everything hourly and read them later to spot mistake in your thought.");
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
	
	USize sz(GetSize());
	d.DrawRect(sz, Black());
	
	USize img_sz = img.GetSize();
	int img_x = (sz.cx - img_sz.cx) / 2;
	int img_y = (sz.cy - img_sz.cy) / 2;
	d.DrawImage(img_x, img_y, img);
	
	Font fnt = SansSerif(25);
	String quote = quotes[quote_i];
	USize txt_sz = GetTextSize(quote, fnt);
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
		double tgt_fat_perc = DEFAULT_FAT_PERC(prof.is_male);
		while (it != end) {
			if (it->date == now)
				today = it;
			if (it->date > now) {
				if (lowest_fat_perc == NULL && fabs(it->fat_perc - tgt_fat_perc) < 0.01)
					lowest_fat_perc = it;
				if (it->variant_type == VARIANT_MAINTENANCE)
					break;
			}
			it++;
		}
		int ydiff = 0;
		if (lowest_fat_perc) {
			int days_remaining = lowest_fat_perc - today;
			String remaining = Format(t_("%d days until lowest fat-\%"), days_remaining);
			if (days_remaining < 30) {
				fnt = SansSerif(40);
				remaining << "!!!";
			}
			else {
				fnt = SansSerif(10);
				remaining << "...";
			}
			USize txt2_sz = GetTextSize(remaining, fnt);
			int txt2_y = txt_y + txt_sz.cy + 15 + ydiff;
			int txt2_x = (sz.cx - txt2_sz.cx) / 2;
			d.DrawText(txt2_x, txt2_y, remaining, fnt, White());
			ydiff += fnt.GetHeight();
		}
		
		
		if (today) {
			int days_remaining = it - today;
			String remaining = Format(t_("%d days until maintenance"), days_remaining);
			if (days_remaining < 30) {
				fnt = SansSerif(40);
				remaining << "!!!";
			}
			else {
				fnt = SansSerif(10);
				remaining << "...";
			}
			USize txt2_sz = GetTextSize(remaining, fnt);
			int txt2_y = txt_y + txt_sz.cy + 15 + ydiff;
			int txt2_x = (sz.cx - txt2_sz.cx) / 2;
			d.DrawText(txt2_x, txt2_y, remaining, fnt, White());
		}
		
	}
}

