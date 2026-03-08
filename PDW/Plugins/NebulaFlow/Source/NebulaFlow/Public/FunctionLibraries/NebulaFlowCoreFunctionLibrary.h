// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "NebulaFlowCoreFunctionLibrary.generated.h"

class UNebulaFlowLocalPlayer;
class UNebulaFlowFSMManager;
class APlayerController;
class IOnlineSubsystem;

UCLASS()
class NEBULAFLOW_API UNebulaFlowCoreFunctionLibrary :  public UBlueprintFunctionLibrary
{

	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Flow", meta = (WorldContext = "WorldContextObject"))
	static void TriggerAction(const UObject* WorldContextObject, const FString& Action, const FString& Parameter, APlayerController* ControllerSender = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Gameplay", Meta = (DisplayName = "Get Local Player"))
	static void PlayerControllerGetLocalPlayer(APlayerController* PlayerController, bool& Success, ULocalPlayer*& LocalPlayer);

	UFUNCTION(BlueprintCallable, Category = "Gameplay", meta = (WorldContext = "WorldContextObject"))
	static UNebulaFlowLocalPlayer* GetCurrentOwnerPlayer(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Gameplay", meta = (WorldContext = "WorldContextObject"))
	static AActor* GetFirstActorOfClassByTag (const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FName Tag);

	UFUNCTION(BlueprintCallable, Category = "Gameplay", meta = (WorldContext = "WorldContextObject"))
	static void LoadLevelByName(const UObject* WorldContextObject, FName LevelName, bool bAbsolute = true, FString Options = FString(TEXT("")));

	UFUNCTION(BlueprintCallable, Category = "Gameplay", meta = (WorldContext = "WorldContextObject"))
	static void SetSystemPaused(const UObject* WorldContextObject, bool bIsPaused);

	UFUNCTION(BlueprintCallable, Category = "Gameplay", meta = (WorldContext = "WorldContextObject"))
	static void SetGamePaused(const UObject* WorldContextObject, bool bIsPaused);
	
	static IOnlineSubsystem* GetOnlineSubsytem();
	static IOnlineIdentityPtr GetOnlineIdentity();

	UFUNCTION(BlueprintCallable, Category = "Gameplay", meta = (WorldContext = "WorldContextObject"))
	static void SetNewFSMEntryPointLabel(const UObject* WorldContextObject, FName inNewLable);


private:

	static void Internal_LoadLevelByName(UObject* WorldContextObject, FName LevelName, bool bAbsolute = true, FString Options = FString(TEXT("")));
	

};