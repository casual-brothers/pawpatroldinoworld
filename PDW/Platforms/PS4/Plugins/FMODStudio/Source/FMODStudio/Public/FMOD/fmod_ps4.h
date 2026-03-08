#pragma once

#include <user_service.h>
#include "fmod_common.h"

extern "C" FMOD_RESULT F_API FMOD_PS4_GetPadVolume(FMOD_SYSTEM* system, SceUserServiceUserId userID, float* volume);
extern "C" FMOD_RESULT F_API FMOD_PS4_SetComputeDevice(int computePipe, int computeQueue, void* garlicMem, size_t garlicMemSize, void* onionMem, size_t onionMemSize);
extern "C" FMOD_RESULT F_API FMOD_PS4_ReleaseComputeDevice();
