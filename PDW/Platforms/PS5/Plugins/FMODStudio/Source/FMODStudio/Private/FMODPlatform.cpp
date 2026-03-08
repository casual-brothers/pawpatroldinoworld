// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2025.
#include "FMODPlatform.h"
#include "fmod_ps5.h"
#include "FMODUtils.h"

FString FMODPlatform_GetDllPath(const TCHAR *ShortName, bool bExplicitPath, bool bUseLibPrefix)
{
    const TCHAR *DirPrefix = (bExplicitPath ? TEXT("/app0/prx/") : TEXT(""));
    return FString::Printf(TEXT("%s%s%s.prx"), DirPrefix, bUseLibPrefix ? TEXT("lib") : TEXT(""), ShortName);
}

EFMODPlatforms::Type FMODPlatform_CurrentPlatform()
{
    return EFMODPlatforms::PS5;
}

void FMODPlatform_SetRealChannelCount(FMOD_ADVANCEDSETTINGS* advSettings)
{
    const UFMODSettings& Settings = *GetDefault<UFMODSettings>();
    advSettings->maxAT9Codecs = Settings.RealChannelCount;
}

int FMODPlatform_MemoryPoolSize()
{
    return 0;
}

FString FMODPlatform_PlatformName()
{
    return "PS5";
}

FMOD_RESULT FMODPlatformSystemSetup()
{
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_MIXER, FMOD_THREAD_AFFINITY_CORE_4));
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_STUDIO_UPDATE, FMOD_THREAD_AFFINITY_CORE_5));
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_STUDIO_LOAD_BANK, FMOD_THREAD_AFFINITY_CORE_5));
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_STUDIO_LOAD_SAMPLE, FMOD_THREAD_AFFINITY_CORE_5));
    return FMOD_OK;
}