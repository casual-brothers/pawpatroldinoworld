#pragma once

#include "Engine/DeveloperSettings.h"

#include "Managers/NebulaFlowAudioManager.h"
#include "NebulaFlowAudioSettings.generated.h"

UCLASS(Config = NebulaFlow, defaultconfig)
class NEBULAFLOW_API UNebulaFlowAudioSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

	static UNebulaFlowAudioSettings* Get();

public:
		
	UFUNCTION(BlueprintPure)
	static float GetSoundSurfaceValue(const TEnumAsByte<EPhysicalSurface>& inSurface);


	UPROPERTY(Config,EditAnywhere,meta = (ForceInlineRow), Category = "Audio Settings")
	TMap<TEnumAsByte<EPhysicalSurface>,FAudioSurfaceTableRow> SurfaceSettings;
protected:

};