#include "FoodTool.h"





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