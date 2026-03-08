#include "Managers/NebulaFlowAchievementManager.h"
#include "NebulaFlow.h"
#include "Managers/NebulaFlowManagerBase.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Core/NebulaFlowPlayerController.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Core/NebulaFlowPersistentUser.h"
#include "Core/NebulaFlowLocalPlayer.h"

UNebulaFlowAchievementManager::UNebulaFlowAchievementManager(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bShouldTick = true;
}

void UNebulaFlowAchievementManager::InitManager(UNebulaFlowGameInstance* InstanceOwner)
{
	Super::InitManager(InstanceOwner);

	//WITHOUT an IMPLEMENTATION of the ABSTRACT UNebulaFlowAchievementCheckerBase class, it trigger a series of warning
	/*if (!AchievementsTable)
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No Achievement Table defined for Achievement Manager"));
	}
	else
	{
		TArray<FName> AchievementRows = AchievementsTable->GetRowNames();
		for (FName CurrentRow : AchievementRows)
		{
			FAchievementConfigRow* Row = AchievementsTable->FindRow<FAchievementConfigRow>(CurrentRow, FString("GENERAL"));
			if (Row && Row->AchievementChecker != nullptr)
			{
				AchievementCheckersMap.Add(CurrentRow, NewObject<UNebulaFlowAchievementCheckerBase>(Row->AchievementChecker));
			}
		}
	}*/
}

void UNebulaFlowAchievementManager::TickManager(float DeltaTime)
{
	Super::TickManager(DeltaTime);

}

void UNebulaFlowAchievementManager::UnlockAchievement(ULocalPlayer* inPlayer, const FName& AchievementId)
{
	/* Checker Data Table
	if (!AchievementsTable || (AchievementsTable && !AchievementsTable->FindRow<FAchievementConfigRow>(AchievementId, FString("GENERAL"))))
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No Achievement Found in AchievementTable"));
		return;
	}
	if (!AchievementCheckersMap.Contains(AchievementId))
	{
		UnlockAchievement_Internal(inPlayer, AchievementId);
		return;
	}
	if (AchievementCheckersMap[AchievementId] != nullptr && AchievementCheckersMap[AchievementId]->IsUnlockConditionValid(inPlayer))
	{
		UnlockAchievement_Internal(inPlayer, AchievementId);
	}*/

	if (UnlockedServerAchievements.Contains(AchievementId))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Achievement Already Unlocked: %s"), *AchievementId.ToString()));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Achievement Unlocked: %s"), *AchievementId.ToString()));

	UNebulaFlowPersistentUser* PersistentUser = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(GetWorld())->GetPersistentUser();
	if (PersistentUser)
	{
		PersistentUser->UnlockedAchievement.AddUnique(AchievementId);
	}

	UnlockAchievement_Internal(inPlayer, AchievementId);
	return;
}

void UNebulaFlowAchievementManager::UpdateOfflineAchievements()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Offline Achievement")));

	UNebulaFlowLocalPlayer* InPlayer = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(GetWorld());

	UNebulaFlowPersistentUser* PersistentUser = InPlayer->GetPersistentUser();
	if (PersistentUser)
	{
		for (FName CurrentAchievement : PersistentUser->UnlockedAchievement)
		{
			if (!UnlockedServerAchievements.Contains(CurrentAchievement))
			{
				UnlockAchievement(InPlayer, CurrentAchievement);
			}
		}
	}
}

void UNebulaFlowAchievementManager::UnlockAchievement_Internal(ULocalPlayer* inPlayer, const FName& AchievementId)
{
	if (inPlayer)
	{
		ANebulaFlowPlayerController* PController = Cast< ANebulaFlowPlayerController>(inPlayer->GetPlayerController(GInstanceRef->GetWorld()));
		if (PController)
		{
			PController->UnlockAchievement(AchievementId);
		}
	}
}

void UNebulaFlowAchievementManager::QueryAchievement(ULocalPlayer* inPlayer)
{
	ANebulaFlowPlayerController* PController = Cast< ANebulaFlowPlayerController>(inPlayer->GetPlayerController(GInstanceRef->GetWorld()));
	if (PController)
	{
		PController->QueryAchievements();
	}
}

void UNebulaFlowAchievementManager::InitServerAchievements(ANebulaFlowPlayerController* PlayerController)
{
	UnlockedServerAchievements.Empty();

	PlayerController->QueryAchievements();
}
