#pragma once

#include "fmod_common.h"

extern "C" FMOD_RESULT F_API FMOD_GameCore_SetXApuStreamCount(unsigned int decodeCount, unsigned int decodeConvertCount);
extern "C" FMOD_RESULT F_API FMOD_GameCore_XDspConfigure(unsigned int numberOfConvolutionStreams, unsigned int numberOfFFTStreams, unsigned int aggregateImpulseResponseInSeconds);
extern "C" FMOD_RESULT F_API FMOD_GameCore_XMAConfigure(FMOD_BOOL enable);