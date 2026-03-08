// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "OnlineStats.h"
#include "NebulaFlowPlayerController.generated.h"

UCLASS()
class NEBULAFLOW_API ANebulaFlowPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ANebulaFlowPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	void QueryAchievements(bool bWasOffline = false);

	void UnlockAchievement(const FName& Id);

	static const FName ACTION_BACK;
	
	static const FName ACTION_BACK_JP;

protected:

	UFUNCTION()
		virtual void OnBackPressed();

	UFUNCTION()
		virtual void OnBackAction();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupInputComponent() override;

	bool InnerUnlockAchievement(const FName& Id);

private:

	/** Achievements write object */
	FOnlineAchievementsWritePtr WriteObject;

	void OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful);
	void OnQueryAchievementsCompleteOffline(const FUniqueNetId& PlayerId, const bool bWasSuccessful);

	TArray<FName> AchievementQueue;

	float AchievementTimer = 0.f;

	float AchievementMaxTimer = 1.f;

	float AchievementReadTimer = 0.f;

	float AchievementReadMaxTimer = 1.f;
};