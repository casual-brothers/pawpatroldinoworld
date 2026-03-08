
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Core/NebulaFlowGameInstance.h"
#include "NebulaFlowLocaleFunctionLibrary.generated.h"


UCLASS()
class NEBULAFLOW_API UNebulaFlowLocaleFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void InitializeLocale(UNebulaFlowGameInstance* GameInstance);
	
	static FString AdjustComplexLocale(FString InCultureName, TArray<FString> AcceptedCultures);

	static void SetCurrentCulture(FString LocaleToUse);

	static TArray<FString> GetCultures(const TCHAR* key);

private:
	static UNebulaFlowGameInstance* GInstance;
};