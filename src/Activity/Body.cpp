#include "Activity.h"

#if 0



Neck::Neck() {
	Add(erspi);
	Add(lesca);
	Add(stemoc);
	Add(stemoh);
	Add(scale);
	
}

Thorax::Thorax() {
	Add(stemo);
	Add(rectus);
	Add(obliq);
	
}

Abdomen::Abdomen() {
	
}

Pelvis::Pelvis() {
	
}

Back::Back() {
	Add(trap);
	Add(lat);
	Add(erspi);
	Add(rhom);
	
}

Trunk::Trunk() {
	Add(thorax);
	Add(abdomen);
	Add(pelvis);
	Add(back);
	
}

Arm::Arm() {
	Add(pecto);
	Add(clavi);
	Add(acro);
	Add(spinal);
	Add(infra);
	Add(supra);
	Add(serant);
	Add(minor);
	Add(major);
	Add(biceps);
	Add(coraco);
	Add(triceps);
	Add(brachi);
	Add(mtriceps);
	
}

Forearm::Forearm() {
	Add(brachio);
	Add(extcrl);
	Add(extcrb);
	Add(hump);
	Add(carpirad);
	Add(digitsuper);
	Add(palmaris);
	Add(hcarpiuln);
	Add(ucarpiuln);
	Add(extdigi);
	Add(anco);
	Add(ecarpiuln);
	Add(abduct);
	Add(extpb);
	Add(extpl);
	Add(proq);
	Add(flexpb);
	Add(thenar);
	
}

Hand::Hand() {
	Add(transap);
	Add(dorsint);
	Add(hypo);
	Add(dorsinus);
	
}

UpperLimb::UpperLimb() {
	Add(axil);
	Add(arm);
	Add(elbow);
	Add(forearm);
	Add(hand);
	
}

Buttocks::Buttocks() {
	Add(glutmax);
	
}

Thigh::Thigh() {
	Add(ilia);
	Add(glume);
	Add(piri);
	Add(refem);
	Add(sarto);
	Add(vasme);
	Add(vasin);
	Add(vasla);
	Add(pecti);
	Add(add);
	Add(addma);
	Add(graci);
	Add(semimem);
	Add(semitend);
	Add(lbiceps);
	Add(sbiceps);
	
}

Knee::Knee() {
	
}

Leg::Leg() {
	Add(tibiaant);
	Add(tibipo);
	Add(soleus);
	Add(popli);
	Add(gastro);
	Add(edigil);
	Add(fiblon);
	Add(fdigil);
	Add(fhallul);
	Add(ehallul);
	Add(fibbre);
	
}

Foot::Foot() {
	Add(dorsin);
	Add(fhallub);
	Add(abhallu);
	Add(fdigib);
	Add(abmini);
	Add(fdigim);
	
}

LowerLimb::LowerLimb() {
	Add(butt);
	Add(hip);
	Add(thigh);
	Add(knee);
	Add(leg);
	Add(foot);
	
}

Body::Body() {
	Add(neck);
	Add(trunk);
	Add(upper);
	Add(lower);
	
}













String GetBioObjectTreeString(BioObject& s, int indent) {
	String out;
	for(int i = 0; i < indent; i++)
		out.Cat('\t');
	out.Cat(s.ToString());
	out.Cat('\n');
	
	for(int i = 0; i < s.GetCount(); i++) {
		out << GetBioObjectTreeString(s.At(i), indent+1);
	}
	
	return out;
}



#endif




void GetMuscleGroups(VectorMap<String, String>& idx) {
	idx.Add("ANDEL", ("Anterior Deltoids"));
	idx.Add("BICEP", ("Biceps"));
	idx.Add("BIFEM", ("Biceps Femoris"));
	idx.Add("BRACH", ("Brachialis"));
	idx.Add("DELTO", ("Deltoids"));
	idx.Add("FOREX", ("Forearm Extensors"));
	idx.Add("FORFL", ("Forearm Flexors"));
	idx.Add("GASTR", ("Gastrocnemius"));
	idx.Add("GLUTE", ("Gluteus Maximus"));
	idx.Add("INQUA", ("Inner Quadriceps"));
	idx.Add("INCOS", ("Intercostals"));
	idx.Add("LATDO", ("Latissimus Dorsi"));
	idx.Add("LOWPR", ("Lower Pectorals"));
	idx.Add("LOWRE", ("Lower Rectus Abdominis"));
	idx.Add("MEDDE", ("Medial Deltoids"));
	idx.Add("OBLIQ", ("Obliques"));
	idx.Add("OUTQU", ("Outer Quadriceps"));
	idx.Add("PECTO", ("Pectorals"));
	idx.Add("QUADR", ("Quadriceps"));
	idx.Add("REARD", ("Rear Deltoids"));
	idx.Add("REABD", ("Rectus Abdominis"));
	idx.Add("SOLEU", ("Soleus"));
	idx.Add("SPINE", ("Spinal Erectors"));
	idx.Add("THIGH", ("Thighs"));
	idx.Add("TRAPE", ("Trapezius"));
	idx.Add("TRICE", ("Triceps"));
	idx.Add("UPPEC", ("Upper Pectorals"));
	idx.Add("UPREC", ("Upper Rectus Abdominis"));
	
	Index<String> chk;
	for(int i = 0; i < idx.GetCount(); i++) {
		String k = idx.GetKey(i);
		ASSERT(chk.Find(k) == -1);
		chk.Add(k);
	}
}

void GetPrimaryTypeWeights(VectorMap<String, double>& idx) {
	idx.Add("Shoulder", 0.4);
	idx.Add("Arm", 0.7);
	idx.Add("Leg", 1.1);
	idx.Add("Front", 1.0);
	idx.Add("Back", 1.0);
}

void GetPrimaryTypes(VectorMap<String, String>& idx) {
	idx.Add("ANDEL", ("Shoulder"));
	idx.Add("BICEP", ("Arm"));
	idx.Add("BIFEM", ("Leg"));
	idx.Add("BRACH", ("Arm"));
	idx.Add("DELTO", ("Shoulder"));
	idx.Add("FOREX", ("Arm"));
	idx.Add("FORFL", ("Arm"));
	idx.Add("GASTR", ("Leg"));
	idx.Add("GLUTE", ("Leg"));
	idx.Add("INQUA", ("Leg"));
	idx.Add("INCOS", ("Front"));
	idx.Add("LATDO", ("Back"));
	idx.Add("LOWPR", ("Front"));
	idx.Add("LOWRE", ("Front"));
	idx.Add("MEDDE", ("Shoulder"));
	idx.Add("OBLIQ", ("Front"));
	idx.Add("OUTQU", ("Leg"));
	idx.Add("PECTO", ("Front"));
	idx.Add("QUADR", ("Leg"));
	idx.Add("REARD", ("Shoulder"));
	idx.Add("REABD", ("Front"));
	idx.Add("SOLEU", ("Leg"));
	idx.Add("SPINE", ("Back"));
	idx.Add("THIGH", ("Leg"));
	idx.Add("TRAPE", ("Back"));
	idx.Add("TRICE", ("Arm"));
	idx.Add("UPPEC", ("Front"));
	idx.Add("UPREC", ("Front"));
	/*
	idx.Add("ANDEL", "");
	idx.Add("BICEP", "");
	idx.Add("BIFEM", "");
	idx.Add("BRACH", "");
	idx.Add("DELTO", "");
	idx.Add("FOREX", "");
	idx.Add("FORFL", "");
	idx.Add("GASTR", "");
	idx.Add("GLUTE", "");
	idx.Add("INQUA", "");
	idx.Add("INCOS", "");
	idx.Add("LATDO", "");
	idx.Add("LOWPR", "");
	idx.Add("LOWRE", "");
	idx.Add("MEDDE", "");
	idx.Add("OBLIQ", "");
	idx.Add("OUTQU", "");
	idx.Add("PECTO", "");
	idx.Add("QUADR", "");
	idx.Add("REARD", "");
	idx.Add("REABD", "");
	idx.Add("SOLEU", "");
	idx.Add("SPINE", "");
	idx.Add("THIGH", "");
	idx.Add("TRAPE", "");
	idx.Add("TRICE", "");
	idx.Add("UPPEC", "");
	idx.Add("UPREC", "");*/
}

