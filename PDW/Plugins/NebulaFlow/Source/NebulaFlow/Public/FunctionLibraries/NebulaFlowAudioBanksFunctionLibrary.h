
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "NebulaFlowAudioBanksFunctionLibrary.generated.h"

class UFMODBank;
class UNebulaFlowGameInstance;

UCLASS()
class NEBULAFLOW_API UNebulaFlowAudioBanksFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void LoadLocalizedBanks(UNebulaFlowGameInstance* GInstance, FString LocaleToUse);
	static void LoadDefaultBanks(UNebulaFlowGameInstance* GInstance);
	static void LoadBanks(TArray<UFMODBank*> Banks);
	static void LoadAllBanks(UNebulaFlowGameInstance* GInstance);
	static void LoadZoneBanks(UNebulaFlowGameInstance* GInstance, FString fromZone, FString toZone);

	static FString GetZoneNum(const FString& Text);

	
	static void DebugLoadAllBanks(UNebulaFlowGameInstance* GInstance);
};