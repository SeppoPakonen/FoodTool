#include "Activity.h"

#define MUSCLE_MODEL_MULT 43.7954542070352


Activity::Activity() {
	LOG(GetBioObjectTreeString(body));
	LOG("");
}

