#ifndef _Activity_Body_h_
#define _Activity_Body_h_

void GetMuscleGroups(VectorMap<String, String>& idx);
void GetPrimaryTypes(VectorMap<String, String>& idx);
void GetPrimaryTypeWeights(VectorMap<String, double>& idx);

#if 0

#define BSTRUCT_CONST(x) \
	String GetName() const override {return #x;}
#define MUSCLE_MODEL_MULT 43.7954542070352


class BioObject {
	BioObject* parent = NULL;
	Vector<BioObject*> sub;
	double mass;
	
protected:
	
	void SetMass(double m) {mass = m;}
	
public:
	
	double GetMass() const {return mass;}
	BioObject* GetParent() const {return parent;}
	BioObject& At(int i) {return *sub[i];}
	int GetCount() const {return sub.GetCount();}
	void Add(BioObject& s) {s.parent = this; sub.Add(&s);}
	virtual void Refresh(double dt) {}
	virtual void DeepRefresh(double dt) {for(int i = 0; i < sub.GetCount(); i++) sub[i]->DeepRefresh(dt); Refresh(dt);}
	virtual String ToString() const {return GetName();}
	virtual String GetName() const {return "unnamed";}
	
	template <class T> void GetDeep(Vector<T*>& v) {
		T* o = dynamic_cast<T*>(this);
		if (o) v.Add(o);
		for(int i = 0; i < sub.GetCount(); i++)
			sub[i]->GetDeep<T>(v);
	}
};

String GetBioObjectTreeString(BioObject& s, int indent=0);



class BodyPart : public BioObject {
	
};



template <class T> struct Symmetric : public BioObject {
	T l, r;
	
	Symmetric() {
		Add(l);
		Add(r);
	}
	BSTRUCT_CONST(Symmetric<T>)
};









struct MusclePart : public BioObject {
	double muscle_activity_factor = 0;
	
	void SetMuscleActivity(double d) {ASSERT(d >= 0.0 && d <= 1.0); muscle_activity_factor = d;}
	virtual double GetRelativeMuscleVolume() const = 0;
};






struct NeckErectorSpinae : public MusclePart {
	
	BSTRUCT_CONST(NeckErectorSpinae)
	double GetRelativeMuscleVolume() const override {return (0.00008783) * MUSCLE_MODEL_MULT;}
};

struct LevatorScapulae : public MusclePart {
	
	BSTRUCT_CONST(LevatorScapulae)
	double GetRelativeMuscleVolume() const override {return (0.00002173) * MUSCLE_MODEL_MULT;}
};

struct Stemocleidomastoid : public MusclePart {
	
	BSTRUCT_CONST(Stemocleidomastoid)
	double GetRelativeMuscleVolume() const override {return (0.00003702) * MUSCLE_MODEL_MULT;}
};

struct Stemohyoid : public MusclePart {
	
	BSTRUCT_CONST(Stemohyoid)
	double GetRelativeMuscleVolume() const override {return (0.00000881) * MUSCLE_MODEL_MULT;}
};

struct Scalenus : public MusclePart {
	
	BSTRUCT_CONST(Scalenus)
	double GetRelativeMuscleVolume() const override {return (0.00000807) * MUSCLE_MODEL_MULT;}
};


struct Neck : public BodyPart {
	Symmetric<NeckErectorSpinae>	erspi;
	Symmetric<LevatorScapulae>		lesca;
	Symmetric<Stemocleidomastoid>	stemoc;
	Symmetric<Stemohyoid>			stemoh;
	Symmetric<Scalenus>				scale;
	
	Neck();
	BSTRUCT_CONST(Neck)
};


struct StemocostalPectoralisMajor : public MusclePart {
	
	BSTRUCT_CONST(StemocostalPectoralisMajor)
	double GetRelativeMuscleVolume() const override {return (0.00036753) * MUSCLE_MODEL_MULT;}
};

struct RectusAbdominis : public MusclePart {
	
	BSTRUCT_CONST(RectusAbdominis)
	double GetRelativeMuscleVolume() const override {return (0.00019659) * MUSCLE_MODEL_MULT;}
};

struct Oblique : public MusclePart {
	
	BSTRUCT_CONST(Oblique)
	double GetRelativeMuscleVolume() const override {return (0.00057546) * MUSCLE_MODEL_MULT;}
};


struct Thorax : public BodyPart {
	Symmetric<StemocostalPectoralisMajor>	stemo;
	Symmetric<RectusAbdominis>				rectus;
	Symmetric<Oblique>						obliq;
	
	Thorax();
	BSTRUCT_CONST(Thorax)
};


struct Abdomen : public BodyPart {
	
	
	Abdomen();
	BSTRUCT_CONST(Abdomen)
};


struct Pelvis : public BodyPart {
	
	
	Pelvis();
	BSTRUCT_CONST(Pelvis)
};


struct Trapezius : public MusclePart {
	
	BSTRUCT_CONST(Trapezius)
	double GetRelativeMuscleVolume() const override {return (0.00039336) * MUSCLE_MODEL_MULT;}
};

struct Latissimus : public MusclePart {
	
	BSTRUCT_CONST(Latissimus)
	double GetRelativeMuscleVolume() const override {return (0.00055576) * MUSCLE_MODEL_MULT;}
};

struct ErectorSpinae : public MusclePart {
	
	BSTRUCT_CONST(ErectorSpinae)
	double GetRelativeMuscleVolume() const override {return (0.00028489) * MUSCLE_MODEL_MULT;}
};

struct Rhomboid : public MusclePart {
	
	BSTRUCT_CONST(Rhomboid)
	double GetRelativeMuscleVolume() const override {return (0.00002938) * MUSCLE_MODEL_MULT;}
};

struct Back : public BodyPart {
	Symmetric<Trapezius>		trap;
	Symmetric<Latissimus>		lat;
	Symmetric<ErectorSpinae>	erspi;
	Symmetric<Rhomboid>			rhom;
	
	Back();
	BSTRUCT_CONST(Back)
};


struct Trunk : public BodyPart {
	Thorax				thorax;
	Abdomen				abdomen;
	Pelvis				pelvis;
	Back				back;
	
	
	Trunk();
	BSTRUCT_CONST(Trunk)
};


struct Axilla : public BodyPart {
	
	
	BSTRUCT_CONST(Axilla)
};


struct PectoralisMajor : public MusclePart {
	
	BSTRUCT_CONST(PectoralisMajor)
	double GetRelativeMuscleVolume() const override {return (0.00005012) * MUSCLE_MODEL_MULT;}
};

struct ClavicularDeltoid : public MusclePart {
	
	BSTRUCT_CONST(ClavicularDeltoid)
	double GetRelativeMuscleVolume() const override {return (0.00004418) * MUSCLE_MODEL_MULT;}
};

struct AcromialDeltoid : public MusclePart {
	
	BSTRUCT_CONST(AcromialDeltoid)
	double GetRelativeMuscleVolume() const override {return (0.00010449) * MUSCLE_MODEL_MULT;}
};

struct SpinalDeltoid : public MusclePart {
	
	BSTRUCT_CONST(SpinalDeltoid)
	double GetRelativeMuscleVolume() const override {return (0.00017228) * MUSCLE_MODEL_MULT;}
};

struct Infraspinatus : public MusclePart {
	
	BSTRUCT_CONST(Infraspinatus)
	double GetRelativeMuscleVolume() const override {return (0.00008651) * MUSCLE_MODEL_MULT;}
};

struct Supraspinatus : public MusclePart {
	
	BSTRUCT_CONST(Supraspinatus)
	double GetRelativeMuscleVolume() const override {return (0.00003774) * MUSCLE_MODEL_MULT;}
};

struct SerratusAnterior : public MusclePart {
	
	BSTRUCT_CONST(SerratusAnterior)
	double GetRelativeMuscleVolume() const override {return (0.00014046) * MUSCLE_MODEL_MULT;}
};

struct TeresMinor : public MusclePart {
	
	BSTRUCT_CONST(TeresMinor)
	double GetRelativeMuscleVolume() const override {return (0.00003085) * MUSCLE_MODEL_MULT;}
};

struct TeresMajor : public MusclePart {
	
	BSTRUCT_CONST(TeresMajor)
	double GetRelativeMuscleVolume() const override {return (0.00003230) * MUSCLE_MODEL_MULT;}
};

struct BicepsBrachii : public MusclePart {
	
	BSTRUCT_CONST(BicepsBrachii)
	double GetRelativeMuscleVolume() const override {return (0.00020033) * MUSCLE_MODEL_MULT;}
};

struct Coracobrachialis : public MusclePart {
	
	BSTRUCT_CONST(Coracobrachialis)
	double GetRelativeMuscleVolume() const override {return (0.00002659) * MUSCLE_MODEL_MULT;}
};

struct TricepsBrachii : public MusclePart {
	
	BSTRUCT_CONST(TricepsBrachii)
	double GetRelativeMuscleVolume() const override {return (0.00027624) * MUSCLE_MODEL_MULT;}
};

struct Brachialis : public MusclePart {
	
	BSTRUCT_CONST(Brachialis)
	double GetRelativeMuscleVolume() const override {return (0.00007818) * MUSCLE_MODEL_MULT;}
};

struct MedialTricepsBrachii : public MusclePart {
	
	BSTRUCT_CONST(MedialTricepsBrachii)
	double GetRelativeMuscleVolume() const override {return (0.00005713) * MUSCLE_MODEL_MULT;}
};

struct Arm : public BodyPart {
	PectoralisMajor					pecto;
	ClavicularDeltoid				clavi;
	AcromialDeltoid					acro;
	SpinalDeltoid					spinal;
	Infraspinatus					infra;
	Supraspinatus					supra;
	SerratusAnterior				serant;
	TeresMinor						minor;
	TeresMajor						major;
	BicepsBrachii					biceps;
	Coracobrachialis				coraco;
	TricepsBrachii					triceps;
	Brachialis						brachi;
	MedialTricepsBrachii			mtriceps;
	
	
	
	Arm();
	BSTRUCT_CONST(Arm)
};


struct Elbow : public BodyPart {
	
	
	BSTRUCT_CONST(Elbow)
};


struct Brachioradialis : public MusclePart {
	
	BSTRUCT_CONST(Brachioradialis)
	double GetRelativeMuscleVolume() const override {return (0.00010340) * MUSCLE_MODEL_MULT;}
};

struct ExtensorCarpiRadialisLongus : public MusclePart {
	
	BSTRUCT_CONST(ExtensorCarpiRadialisLongus)
	double GetRelativeMuscleVolume() const override {return (0.00004648) * MUSCLE_MODEL_MULT;}
};

struct ExtensorCarpiRadialisBrevis : public MusclePart {
	
	BSTRUCT_CONST(ExtensorCarpiRadialisBrevis)
	double GetRelativeMuscleVolume() const override {return (0.00002705) * MUSCLE_MODEL_MULT;}
};

struct HumeralPronatorTeres : public MusclePart {
	
	BSTRUCT_CONST(HumeralPronatorTeres)
	double GetRelativeMuscleVolume() const override {return (0.00001498) * MUSCLE_MODEL_MULT;}
};

struct FlexorCarpiRadialis : public MusclePart {
	
	BSTRUCT_CONST(FlexorCarpiRadialis)
	double GetRelativeMuscleVolume() const override {return (0.00003007) * MUSCLE_MODEL_MULT;}
};

struct FlexorDigitorumSuperficialis : public MusclePart {
	
	BSTRUCT_CONST(FlexorDigitorumSuperficialis)
	double GetRelativeMuscleVolume() const override {return (0.00010554) * MUSCLE_MODEL_MULT;}
};

struct PalmarisLongus : public MusclePart {
	
	BSTRUCT_CONST(PalmarisLongus)
	double GetRelativeMuscleVolume() const override {return (0.00003054) * MUSCLE_MODEL_MULT;}
};

struct HumeralFlexorCarpiUlnaris : public MusclePart {
	
	BSTRUCT_CONST(HumeralFlexorCarpiUlnaris)
	double GetRelativeMuscleVolume() const override {return (0.00003850) * MUSCLE_MODEL_MULT;}
};

struct UlnarFlexorCarpiUlnaris : public MusclePart {
	
	BSTRUCT_CONST(UlnarFlexorCarpiUlnaris)
	double GetRelativeMuscleVolume() const override {return (0.00005632) * MUSCLE_MODEL_MULT;}
};

struct ExtensorDigitorum : public MusclePart {
	
	BSTRUCT_CONST(ExtensorDigitorum)
	double GetRelativeMuscleVolume() const override {return (0.00004144) * MUSCLE_MODEL_MULT;}
};

struct Anconeus : public MusclePart {
	
	BSTRUCT_CONST(Anconeus)
	double GetRelativeMuscleVolume() const override {return (0.00000723) * MUSCLE_MODEL_MULT;}
};

struct ExtensorCarpiUlnaris : public MusclePart {
	
	BSTRUCT_CONST(ExtensorCarpiUlnaris)
	double GetRelativeMuscleVolume() const override {return (0.00003629) * MUSCLE_MODEL_MULT;}
};

struct AbductorPollicisLongus : public MusclePart {
	
	BSTRUCT_CONST(AbductorPollicisLongus)
	double GetRelativeMuscleVolume() const override {return (0.00001413) * MUSCLE_MODEL_MULT;}
};

struct ExtensorPollicisBrevis : public MusclePart {
	
	BSTRUCT_CONST(ExtensorPollicisBrevis)
	double GetRelativeMuscleVolume() const override {return (0.00000726) * MUSCLE_MODEL_MULT;}
};

struct ExtensorPollicisLongus : public MusclePart {
	
	BSTRUCT_CONST(ExtensorPollicisLongus)
	double GetRelativeMuscleVolume() const override {return (0.00000571) * MUSCLE_MODEL_MULT;}
};

struct PronatorQuadratus : public MusclePart {
	
	BSTRUCT_CONST(PronatorQuadratus)
	double GetRelativeMuscleVolume() const override {return (0.00000693) * MUSCLE_MODEL_MULT;}
};

struct FlexorPollicisBrevis : public MusclePart {
	
	BSTRUCT_CONST(FlexorPollicisBrevis)
	double GetRelativeMuscleVolume() const override {return (0.00000323) * MUSCLE_MODEL_MULT;}
};

struct ThenarMuscle : public MusclePart {
	
	BSTRUCT_CONST(ThenarMuscle)
	double GetRelativeMuscleVolume() const override {return (0.00001297) * MUSCLE_MODEL_MULT;}
};

struct Forearm : public BodyPart {
	Brachioradialis					brachio;
	ExtensorCarpiRadialisLongus		extcrl;
	ExtensorCarpiRadialisBrevis		extcrb;
	HumeralPronatorTeres			hump;
	FlexorCarpiRadialis				carpirad;
	FlexorDigitorumSuperficialis	digitsuper;
	PalmarisLongus					palmaris;
	HumeralFlexorCarpiUlnaris		hcarpiuln;
	UlnarFlexorCarpiUlnaris			ucarpiuln;
	ExtensorDigitorum				extdigi;
	Anconeus						anco;
	ExtensorCarpiUlnaris			ecarpiuln;
	AbductorPollicisLongus			abduct;
	ExtensorPollicisBrevis			extpb;
	ExtensorPollicisLongus			extpl;
	PronatorQuadratus				proq;
	FlexorPollicisBrevis			flexpb;
	ThenarMuscle					thenar;
	
	
	Forearm();
	BSTRUCT_CONST(Forearm)
};


struct TransverseAbductorPollicis : public MusclePart {
	
	BSTRUCT_CONST(TransverseAbductorPollicis)
	double GetRelativeMuscleVolume() const override {return (0.00000723) * MUSCLE_MODEL_MULT;}
};

struct DorsalInterossei : public MusclePart {
	
	BSTRUCT_CONST(DorsalInterossei)
	double GetRelativeMuscleVolume() const override {return (0.00000728) * MUSCLE_MODEL_MULT;}
};

struct HypothenalMuscle : public MusclePart {
	
	BSTRUCT_CONST(HypothenalMuscle)
	double GetRelativeMuscleVolume() const override {return (0.00001435) * MUSCLE_MODEL_MULT;}
};

struct DorsalIntersosseus : public MusclePart {
	
	BSTRUCT_CONST(DorsalIntersosseus)
	double GetRelativeMuscleVolume() const override {return (0.00001067) * MUSCLE_MODEL_MULT;}
};


struct Hand : public BodyPart {
	TransverseAbductorPollicis	transap;
	DorsalInterossei			dorsint;
	HypothenalMuscle			hypo;
	DorsalIntersosseus			dorsinus;
	
	
	Hand();
	BSTRUCT_CONST(Hand)
};

struct UpperLimb : public BodyPart {
	Axilla							axil;
	Arm								arm;
	Elbow							elbow;
	Forearm							forearm;
	Hand							hand;
	
	
	UpperLimb();
	BSTRUCT_CONST(UpperLimb)
};


struct GluteusMaximus : public MusclePart {
	
	BSTRUCT_CONST(GluteusMaximus)
	double GetRelativeMuscleVolume() const override {return (0.00133299) * MUSCLE_MODEL_MULT;}
};

struct Buttocks : public BodyPart {
	GluteusMaximus		glutmax;
	
	Buttocks();
	BSTRUCT_CONST(Buttocks)
};


struct Hip : public BodyPart {
	
	BSTRUCT_CONST(Hip)
};



struct Iliacus : public MusclePart {
	
	BSTRUCT_CONST(Iliacus)
	double GetRelativeMuscleVolume() const override {return (0.00018306) * MUSCLE_MODEL_MULT;}
};

struct GluteusMedius : public MusclePart {
	
	BSTRUCT_CONST(GluteusMedius)
	double GetRelativeMuscleVolume() const override {return (0.00028396) * MUSCLE_MODEL_MULT;}
};

struct Piriformis : public MusclePart {
	
	BSTRUCT_CONST(Piriformis)
	double GetRelativeMuscleVolume() const override {return (0.00005357) * MUSCLE_MODEL_MULT;}
};

struct RectusFemoris : public MusclePart {
	
	BSTRUCT_CONST(RectusFemoris)
	double GetRelativeMuscleVolume() const override {return (0.00044228) * MUSCLE_MODEL_MULT;}
};

struct Sartorius : public MusclePart {
	
	BSTRUCT_CONST(Sartorius)
	double GetRelativeMuscleVolume() const override {return (0.00012856) * MUSCLE_MODEL_MULT;}
};

struct VastusMedialis : public MusclePart {
	
	BSTRUCT_CONST(VastusMedialis)
	double GetRelativeMuscleVolume() const override {return (0.00034851) * MUSCLE_MODEL_MULT;}
};

struct VastusIntermedius : public MusclePart {
	
	BSTRUCT_CONST(VastusIntermedius)
	double GetRelativeMuscleVolume() const override {return (0.00023002) * MUSCLE_MODEL_MULT;}
};

struct VastusLateralis : public MusclePart {
	
	BSTRUCT_CONST(VastusLateralis)
	double GetRelativeMuscleVolume() const override {return (0.00049457) * MUSCLE_MODEL_MULT;}
};

struct Pectineus : public MusclePart {
	
	BSTRUCT_CONST(Pectineus)
	double GetRelativeMuscleVolume() const override {return (0.00006767) * MUSCLE_MODEL_MULT;}
};

struct Adductor : public MusclePart {
	
	BSTRUCT_CONST(Adductor)
	double GetRelativeMuscleVolume() const override {return (0.00022117) * MUSCLE_MODEL_MULT;}
};

struct AdductorMagnus : public MusclePart {
	
	BSTRUCT_CONST(AdductorMagnus)
	double GetRelativeMuscleVolume() const override {return (0.00044714) * MUSCLE_MODEL_MULT;}
};

struct Gracilis : public MusclePart {
	
	BSTRUCT_CONST(Gracilis)
	double GetRelativeMuscleVolume() const override {return (0.00020679) * MUSCLE_MODEL_MULT;}
};

struct Semimembranosus : public MusclePart {
	
	BSTRUCT_CONST(Semimembranosus)
	double GetRelativeMuscleVolume() const override {return (0.00029725) * MUSCLE_MODEL_MULT;}
};

struct Semitendinosus : public MusclePart {
	
	BSTRUCT_CONST(Semitendinosus)
	double GetRelativeMuscleVolume() const override {return (0.00013458) * MUSCLE_MODEL_MULT;}
};

struct LongBicepsFemoris : public MusclePart {
	
	BSTRUCT_CONST(LongBicepsFemoris)
	double GetRelativeMuscleVolume() const override {return (0.00053160) * MUSCLE_MODEL_MULT;}
};

struct ShortBicepsFemoris : public MusclePart {
	
	BSTRUCT_CONST(ShortBicepsFemoris)
	double GetRelativeMuscleVolume() const override {return (0.00006373) * MUSCLE_MODEL_MULT;}
};

struct Thigh : public BodyPart {
	Iliacus				ilia;
	GluteusMedius		glume;
	Piriformis			piri;
	RectusFemoris		refem;
	Sartorius			sarto;
	VastusMedialis		vasme;
	VastusIntermedius	vasin;
	VastusLateralis		vasla;
	Pectineus			pecti;
	Adductor			add;
	AdductorMagnus		addma;
	Gracilis			graci;
	Semimembranosus		semimem;
	Semitendinosus		semitend;
	LongBicepsFemoris	lbiceps;
	ShortBicepsFemoris	sbiceps;
	
	
	Thigh();
	BSTRUCT_CONST(Thigh)
};



struct Knee : public BodyPart {
	
	
	Knee();
	BSTRUCT_CONST(Knee)
};


struct TibialisAnterior : public MusclePart {
	
	BSTRUCT_CONST(TibialisAnterior)
	double GetRelativeMuscleVolume() const override {return (0.00014993) * MUSCLE_MODEL_MULT;}
};

struct TibialisPosterior : public MusclePart {
	
	BSTRUCT_CONST(TibialisPosterior)
	double GetRelativeMuscleVolume() const override {return (0.00004056) * MUSCLE_MODEL_MULT;}
};

struct Soleus : public MusclePart {
	
	BSTRUCT_CONST(Soleus)
	double GetRelativeMuscleVolume() const override {return (0.00022631) * MUSCLE_MODEL_MULT;}
};

struct Popliteus : public MusclePart {
	
	BSTRUCT_CONST(Popliteus)
	double GetRelativeMuscleVolume() const override {return (0.00001492) * MUSCLE_MODEL_MULT;}
};

struct Gastrocnemius : public MusclePart {
	
	BSTRUCT_CONST(Gastrocnemius)
	double GetRelativeMuscleVolume() const override {return (0.00068418) * MUSCLE_MODEL_MULT;}
};

struct ExtensorDigitorumLongus : public MusclePart {
	
	BSTRUCT_CONST(ExtensorDigitorumLongus)
	double GetRelativeMuscleVolume() const override {return (0.00006484) * MUSCLE_MODEL_MULT;}
};

struct FibularisLongus : public MusclePart {
	
	BSTRUCT_CONST(FibularisLongus)
	double GetRelativeMuscleVolume() const override {return (0.00005723) * MUSCLE_MODEL_MULT;}
};

struct FlexorDigitorumLongus : public MusclePart {
	
	BSTRUCT_CONST(FlexorDigitorumLongus)
	double GetRelativeMuscleVolume() const override {return (0.00004675) * MUSCLE_MODEL_MULT;}
};

struct FlexorHallucisLongus : public MusclePart {
	
	BSTRUCT_CONST(FlexorHallucisLongus)
	double GetRelativeMuscleVolume() const override {return (0.00003920) * MUSCLE_MODEL_MULT;}
};

struct ExtensorHallucisLongus : public MusclePart {
	
	BSTRUCT_CONST(ExtensorHallucisLongus)
	double GetRelativeMuscleVolume() const override {return (0.00001004) * MUSCLE_MODEL_MULT;}
};

struct FibularisBrevis : public MusclePart {
	
	BSTRUCT_CONST(FibularisBrevis)
	double GetRelativeMuscleVolume() const override {return (0.00001638) * MUSCLE_MODEL_MULT;}
};

struct Leg : public BodyPart {
	TibialisAnterior			tibiaant;
	TibialisPosterior			tibipo;
	Soleus						soleus;
	Popliteus					popli;
	Gastrocnemius				gastro;
	ExtensorDigitorumLongus		edigil;
	FibularisLongus				fiblon;
	FlexorDigitorumLongus		fdigil;
	FlexorHallucisLongus		fhallul;
	ExtensorHallucisLongus		ehallul;
	FibularisBrevis				fibbre;
	
	Leg();
	BSTRUCT_CONST(Leg)
};



struct FootDorsalIntersosseus : public MusclePart {
	
	BSTRUCT_CONST(FootDorsalIntersosseus)
	double GetRelativeMuscleVolume() const override {return (0.00001242) * MUSCLE_MODEL_MULT;}
};

struct FootFlexorHallucisBrevis : public MusclePart {
	
	BSTRUCT_CONST(FootFlexorHallucisBrevis)
	double GetRelativeMuscleVolume() const override {return (0.0000072) * MUSCLE_MODEL_MULT;}
};

struct AbductorHallucis : public MusclePart {
	
	BSTRUCT_CONST(AbductorHallucis)
	double GetRelativeMuscleVolume() const override {return (0.00001905) * MUSCLE_MODEL_MULT;}
};

struct FlexorDigitorumBrevis : public MusclePart {
	
	BSTRUCT_CONST(FlexorDigitorumBrevis)
	double GetRelativeMuscleVolume() const override {return (0.00001840) * MUSCLE_MODEL_MULT;}
};

struct AbductorDigitMinimi : public MusclePart {
	
	BSTRUCT_CONST(AbductorDigitMinimi)
	double GetRelativeMuscleVolume() const override {return (0.00000729) * MUSCLE_MODEL_MULT;}
};

struct FlexorDigitMinimiBrevis : public MusclePart {
	
	BSTRUCT_CONST(FlexorDigitMinimiBrevis)
	double GetRelativeMuscleVolume() const override {return (0.00000313) * MUSCLE_MODEL_MULT;}
};

struct Foot : public BodyPart {
	FootDorsalIntersosseus		dorsin;
	FootFlexorHallucisBrevis	fhallub;
	AbductorHallucis			abhallu;
	FlexorDigitorumBrevis		fdigib;
	AbductorDigitMinimi			abmini;
	FlexorDigitMinimiBrevis		fdigim;
	
	Foot();
	BSTRUCT_CONST(Foot)
};



struct LowerLimb : public BodyPart {
	Buttocks			butt;
	Hip					hip;
	Thigh				thigh;
	Knee				knee;
	Leg					leg;
	Foot				foot;
	
	
	LowerLimb();
	BSTRUCT_CONST(LowerLimb)
};




struct Body : public BioObject {
	Neck					neck;
	Trunk					trunk;
	Symmetric<UpperLimb>	upper;
	Symmetric<LowerLimb>	lower;
	
	
	Body();
	BSTRUCT_CONST(Body)
};


#endif
#endif
