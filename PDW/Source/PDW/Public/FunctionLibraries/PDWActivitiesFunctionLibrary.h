// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PDWActivitiesFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWActivitiesFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Activity")
	static void StartMasterActivity(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Activity")
	static bool GetCompletedOnce(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Activity")
	static bool SetCompletedOnce(UObject* WorldContextObject, bool bInOption);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Activity")
	static bool GetFirstBoot(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Activity")
	static bool GetIsNewGame(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Activity")
	static void SetActivityAlreadyStartedOnTheSlot(UObject* WorldContextObject,bool inOption);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Activity")
	static void ForceResetAllActivities(UObject* WorldContextObject, const bool bInNewGame);
	
};
