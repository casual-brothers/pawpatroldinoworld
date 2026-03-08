#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Managers/NebulaFlowAchievementManager.h"
#include "Managers/NebulaFlowAudioManager.h"
#include "Managers/NebulaFlowCommandManager.h"
#include "Managers/NebulaFlowErrorManager.h"
#include "Managers/NebulaFlowFSMManager.h"
#include "Managers/NebulaFlowUIManager.h"

#include "NebulaFlowSingletonFunctionLibrary.generated.h"

UCLASS()
class NEBULAFLOW_API UNebulaFlowSingletonFunctionLibrary :  public UBlueprintFunctionLibrary
{

	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "NebulaFlow Managers", meta = (WorldContext = "WorldContextObject"))
	static UNebulaFlowFSMManager* GetNebulaFlowFSMManager (const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "NebulaFlow Managers", meta = (WorldContext = "WorldContextObject"))
	static UNebulaFlowUIManager* GetNebulaFlowUIManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "NebulaFlow Managers", meta = (WorldContext = "WorldContextObject"))
	static UNebulaFlowErrorManager* GetNebulaFlowErrorManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "NebulaFlow Managers", meta = (WorldContext = "WorldContextObject"))
	static UNebulaFlowCommandManager* GetNebulaFlowCommandManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "NebulaFlow Managers", meta = (WorldContext = "WorldContextObject"))
	static UNebulaFlowAchievementManager* GetNebulaFlowAchievementManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "NebulaFlow Managers", meta = (WorldContext = "WorldContextObject"))
	static UNebulaFlowAudioManager* GetNebulaFlowAudioManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Gameplay", meta = (WorldContext = "WorldContextObject"))
	static class UNebulaFlowUserSubSystem* GetUserSubsystem(const UObject* WorldContextObject);
// 
// 	UFUNCTION(BlueprintCallable, Category = "Gameplay", meta = (WorldContext = "WorldContextObject"))
// 	static class UNebulaFlowUserSubSystem* GetUserSubsystemNOCONST(UObject* WorldContextObject);
// 

};