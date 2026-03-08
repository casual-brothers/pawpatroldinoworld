// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2025.
#include "FMODPlatform.h"
#include "fmod_gamecore.h"
#include "Interfaces/IPluginManager.h"
#include "FMODUtils.h"
#include "Misc/Paths.h"

FString FMODPlatform_GetDllPath(const TCHAR *ShortName, bool bExplicitPath, bool bUseLibPrefix)
{
    FString libDir = FPlatformProcess::GetModulesDirectory();
    return FPaths::Combine(libDir, FString::Printf(TEXT("%s.dll"), ShortName));
}

EFMODPlatforms::Type FMODPlatform_CurrentPlatform()
{
    return EFMODPlatforms::XSX;
}

FString FMODPlatform_PlatformName()
{
    return "Scarlett";
}

void FMODPlatform_SetRealChannelCount(FMOD_ADVANCEDSETTINGS* advSettings)
{
    const UFMODSettings& Settings = *GetDefault<UFMODSettings>();
    advSettings->maxXMACodecs = Settings.RealChannelCount;
}

int FMODPlatform_MemoryPoolSize()
{
    return 0;
}

FMOD_RESULT FMODPlatformSystemSetup()
{
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_MIXER, FMOD_THREAD_AFFINITY_CORE_4));
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_STUDIO_UPDATE, FMOD_THREAD_AFFINITY_CORE_5));
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_STUDIO_LOAD_BANK, FMOD_THREAD_AFFINITY_CORE_5));
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_STUDIO_LOAD_SAMPLE, FMOD_THREAD_AFFINITY_CORE_5));
    return FMOD_OK;
}