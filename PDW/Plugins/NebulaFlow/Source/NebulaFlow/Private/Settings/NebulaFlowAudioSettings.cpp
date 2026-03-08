#include "Settings/NebulaFlowAudioSettings.h"

 UNebulaFlowAudioSettings::UNebulaFlowAudioSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CategoryName = "Nebula";
	SectionName = "NebulaAudioSettings";
}

UNebulaFlowAudioSettings* UNebulaFlowAudioSettings::Get()
{
	return CastChecked<UNebulaFlowAudioSettings>(UNebulaFlowAudioSettings::StaticClass()->GetDefaultObject());
}

float UNebulaFlowAudioSettings::GetSoundSurfaceValue(const TEnumAsByte<EPhysicalSurface>& inSurface)
{
	if (UNebulaFlowAudioSettings::Get()->SurfaceSettings.Contains(inSurface))
	{
		return UNebulaFlowAudioSettings::Get()->SurfaceSettings[inSurface].Value;
	}
	return 0.0f;
}