// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2025.
#include "FMODPlatform.h"
#include "Interfaces/IPluginManager.h"

FString FMODPlatform_GetDllPath(const TCHAR *ShortName, bool bExplicitPath, bool bUseLibPrefix)
{
    FString BaseLibPath = IPluginManager::Get().FindPlugin(TEXT("FMODStudio"))->GetBaseDir();
    FString leafdir;

    while (BaseLibPath.Split(TEXT("/"), &BaseLibPath, &leafdir, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
    {
        if (leafdir.Equals(TEXT("Plugins")) && !BaseLibPath.EndsWith(TEXT("Plugins"), ESearchCase::IgnoreCase))
            break;
    }

    return FString::Printf(TEXT("%s/Platforms/WinGDK/Plugins/FMODStudio/Binaries/%s.dll"), *BaseLibPath, ShortName);
}

EFMODPlatforms::Type FMODPlatform_CurrentPlatform()
{
    return EFMODPlatforms::Windows;
}

FString FMODPlatform_PlatformName()
{
    return "Desktop";
}

void FMODPlatform_SetRealChannelCount(FMOD_ADVANCEDSETTINGS* advSettings)
{
    const UFMODSettings& Settings = *GetDefault<UFMODSettings>();
    advSettings->maxVorbisCodecs = Settings.RealChannelCount;
}

int FMODPlatform_MemoryPoolSize()
{
    return 0;
}

FMOD_RESULT FMODPlatformSystemSetup()
{
    return FMOD_OK;
}