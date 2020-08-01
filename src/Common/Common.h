#ifndef _Common_Common_h
#define _Common_Common_h

#define int64 int64_fuck_opencv
#define uint64 uint64_fuck_opencv
#undef CPU_SSE2
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#undef int64
#undef uint64

#include <random>
#include <Core/Core.h>
#include <Draw/Draw.h>
#include <plugin/bz2/bz2.h>
using namespace Upp;

#define USING_SDK_NAMESPACE

#include "Time.h"
//#include "UnitsGenerated.h"
#include "Util.h"
#include "StepOptimizer.h"
#include "Optimizer.h"
#include "VideoCapture.h"
#include "Sound.h"

#include "Math.h"

#endif
