// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Managers/NebulaFlowActivitiesManager.h"
#include "PDWActivitiesManager.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(LogPDWctivities, Warning, All);

class UPDWEventSubsytem;

UCLASS(BlueprintType, Blueprintable)
class PDW_API UPDWActivitiesManager : public UNebulaFlowActivitiesManager
{
	GENERATED_BODY()
	
	friend class UPDWActivitiesFunctionLibrary;

public:
	void StartActivity(const FString& ActivityId, ULocalPlayer* LocalPlayer = nullptr) override;


	void SetActivityAvailability(const FString& ActivityId, bool bEnabled, ULocalPlayer* LocalPlayer = nullptr, bool bForceEvenIfNotInitialized = false) override;

	/*Id of Last activity Task, (when completed, activity itself will be completed)*/
	UPROPERTY(EditDefaultsOnly, Category = "Activity")
	FString LastTaskId {};
	/* Id of the first Activity Task (Will be made available with activity itself) */
	UPROPERTY(EditDefaultsOnly, Category = "Activity")
	FString FirstTaskId {};

	UPROPERTY(EditDefaultsOnly, Category = "Activity")
	FString MasterActivityID = {};

	void SetNewGame(const bool bInNewGame);
protected:

	virtual TMap<FString, FActivityData>* GetPlayerActivitiesData(ULocalPlayer* LocalPlayer = nullptr)override;

	void EndActivity(const FString& ActivityId, EOnlineActivityOutcome Outcome, ULocalPlayer* LocalPlayer = nullptr) override;

	void InitializeLocalPlayerActivities(ULocalPlayer* LocalPlayer) override;

	bool bIsNewGame = false;

	UPROPERTY(BlueprintReadOnly, Category = "Activity")
	FString CurrentStartedTask {};
private:

	UPROPERTY()
	UPDWEventSubsytem* EventManager = nullptr;

	FString GetNextTaskId(int32 inCurrentTaskIndex);
};