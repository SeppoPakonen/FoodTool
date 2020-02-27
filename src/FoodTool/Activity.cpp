#include "FoodTool.h"

#define MUSCLE_MODEL_MULT 43.7954542070352


Activity::Activity() {
	/*double oblique = (0.00057546) * MUSCLE_MODEL_MULT;
	double rectus_abdominis = (0.00019659) * MUSCLE_MODEL_MULT;
	double trapezius = (0.00039336) * MUSCLE_MODEL_MULT;
	double latissimus = (0.00055576) * MUSCLE_MODEL_MULT;
	double rhomboid = (0.00002938) * MUSCLE_MODEL_MULT;
	double levator_scapulae = (0.00002173) * MUSCLE_MODEL_MULT;
	double biceps_brachii = (0.00020033) * MUSCLE_MODEL_MULT;
	double gastrocnemius = (0.00068418) * MUSCLE_MODEL_MULT;
	double soleus = (0.00022631) * MUSCLE_MODEL_MULT;
	double tibialis_posterior = (0.00004056) * MUSCLE_MODEL_MULT;
	double pectoralis_major = (0.00005012) * MUSCLE_MODEL_MULT;
	double serratus_anterior = (0.00014046) * MUSCLE_MODEL_MULT;
	double flexor_carpi_radialis = (0.00003007) * MUSCLE_MODEL_MULT;
	double flexor_carpi_ulnaris = (0.00003850) * MUSCLE_MODEL_MULT;
	double palmaris_longus = (0.00003054) * MUSCLE_MODEL_MULT;
	double humeral_pronator_teres = (0.00001498) * MUSCLE_MODEL_MULT;
	double flexor_digitorum_superficialis = (0.00010554) * MUSCLE_MODEL_MULT;
	double brachioradialis = (0.00010340) * MUSCLE_MODEL_MULT;
	double ExtensorCarpiRadialisLongus = (0.00004648) * MUSCLE_MODEL_MULT;
	double ExtensorCarpiRadialisBrevis = (0.00002705) * MUSCLE_MODEL_MULT;
	double ExtensorDigitorum = (0.00004144) * MUSCLE_MODEL_MULT;
	double ExtensorCarpiUlnaris = (0.00003629) * MUSCLE_MODEL_MULT;
	double AbductorPollicisLongus = (0.00001413) * MUSCLE_MODEL_MULT;
	double ExtensorPollicisBrevis = (0.00000726) * MUSCLE_MODEL_MULT;
	double ExtensorPollicisLongus = (0.00000571) * MUSCLE_MODEL_MULT;
	double Anconeus = (0.00000723) * MUSCLE_MODEL_MULT;
	double ErectorSpinae = (0.00028489) * MUSCLE_MODEL_MULT;
	double Latissimus = (0.00055576) * MUSCLE_MODEL_MULT;
	double Supraspinatus = (0.00003774) * MUSCLE_MODEL_MULT;
	double TeresMinor = (0.00003085) * MUSCLE_MODEL_MULT;
	double Infraspinatus = (0.00008651) * MUSCLE_MODEL_MULT;
	double Sartorius = (0.00012856) * MUSCLE_MODEL_MULT;
	double RectusFemoris = (0.00044228) * MUSCLE_MODEL_MULT;
	double VastusMedialis = (0.00034851) * MUSCLE_MODEL_MULT;
	double VastusIntermedius = (0.00023002) * MUSCLE_MODEL_MULT;
	double VastusLateralis = (0.00049457) * MUSCLE_MODEL_MULT;
	double LongBicepsFemoris = (0.00053160) * MUSCLE_MODEL_MULT;
	double ShortBicepsFemoris = (0.00006373) * MUSCLE_MODEL_MULT;
	double  = ;
	double  = ;
	double  = ;
	
	double abdominals = oblique + rectus_abdominis;
	double back = trapezius + rhomboid + levator_scapulae;
	double biceps = biceps_brachii;
	double calves = gastrocnemius + soleus + tibialis_posterior;
	double chest = pectoralis_major + serratus_anterior;
	double forearms = flexor_carpi_radialis + palmaris_longus + flexor_carpi_ulnaris +
		flexor_digitorum_superficialis + brachioradialis + ExtensorCarpiRadialisLongus +
		ExtensorCarpiRadialisBrevis + ExtensorDigitorum + ExtensorCarpiUlnaris +
		AbductorPollicisLongus + ExtensorPollicisBrevis + ExtensorPollicisLongus +
		Anconeus;
	double lower_back = ErectorSpinae + latissimus;
	double shoulders = Supraspinatus + TeresMinor + Infraspinatus;
	double thighs = Sartorius + RectusFemoris + VastusMedialis + VastusIntermedius +
		VastusLateralis + LongBicepsFemoris + ShortBicepsFemoris +
		
	
	AddMuscleGroup("Abdominals", abdominals * 2);
	AddMuscleGroup("Back", back * 2);
	AddMuscleGroup("Biceps", biceps * 2);
	AddMuscleGroup("Calves", calves * 2);
	AddMuscleGroup("Chest", chest * 2);
	AddMuscleGroup("Forearms", forearms * 2);
	AddMuscleGroup("Lower Back", lower_back * 2);
	AddMuscleGroup("Shoulders", shoulders);
	AddMuscleGroup("Thighs", );
	AddMuscleGroup("Triceps", );*/
	
	LOG(GetBioObjectTreeString(body));
	LOG("");
}

