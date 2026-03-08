// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2025.
#include "FMODPlatform.h"
#include "fmod_switch.h"
#include "FMODUtils.h"

#include <nn/os.h>
#include <nn/fs.h>
#include <nn/nifm.h>
#include <nn/nifm/nifm_ApiIpAddress.h>
#include <nn/socket.h>

FString FMODPlatform_GetDllPath(const TCHAR *ShortName, bool bExplicitPath, bool bUseLibPrefix)
{
    return "";
}

EFMODPlatforms::Type FMODPlatform_CurrentPlatform()
{
    return EFMODPlatforms::Switch;
}

void FMODPlatform_SetRealChannelCount(FMOD_ADVANCEDSETTINGS* advSettings)
{
    const UFMODSettings& Settings = *GetDefault<UFMODSettings>();
    advSettings->maxVorbisCodecs = Settings.RealChannelCount;
}

int FMODPlatform_MemoryPoolSize()
{
    const UFMODSettings& Settings = *GetDefault<UFMODSettings>();
    return Settings.MemoryPoolSizes.Switch;
}

FString FMODPlatform_PlatformName()
{
    return "Switch";
}

FMOD_RESULT FMODPlatformSystemSetup()
{
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_MIXER, FMOD_THREAD_AFFINITY_CORE_1));
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_STUDIO_UPDATE, FMOD_THREAD_AFFINITY_CORE_0));
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_STUDIO_LOAD_BANK, FMOD_THREAD_AFFINITY_CORE_0));
    verifyfmod(FMOD::Thread_SetAttributes(FMOD_THREAD_TYPE_STUDIO_LOAD_SAMPLE, FMOD_THREAD_AFFINITY_CORE_0));

    // Enable socket for Live Update.
    const UFMODSettings& Settings = *GetDefault<UFMODSettings>();
    if (Settings.bEnableLiveUpdate)
    {
        UE_LOG(LogFMOD, Verbose, TEXT("Setting up network socket."));

        // FSwitchSocketSubsystem_StaticInit already initializes nifm & socket
        nn::nifm::SubmitNetworkRequest();

        while (nn::nifm::IsNetworkRequestOnHold())
        {
            UE_LOG(LogFMOD, Verbose, TEXT("Waiting for network interface availability..."));
            nn::os::SleepThread(nn::TimeSpan::FromSeconds(1));
        }

        if (!nn::nifm::IsNetworkAvailable())
        {
            UE_LOG(LogFMOD, Warning, TEXT("Network interface is not available."));
            return FMOD_OK;
        }

        nn::socket::InAddr address = { 0 };
        nn::Result res = nn::nifm::GetCurrentPrimaryIpAddress(&address);
        if (res.IsSuccess())
        {
            UE_LOG(LogFMOD, Log, TEXT("IP address: %s."), *FString(nn::socket::InetNtoa(address)));
        }
    }

    return FMOD_OK;
}