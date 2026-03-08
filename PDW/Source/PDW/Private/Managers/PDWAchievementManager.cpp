// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PDWAchievementManager.h"
#include "Managers/PDWEventSubsytem.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Data/PDWGameplayTagSettings.h"
#include "GameplayTagsManager.h"

UPDWAchievementManager::UPDWAchievementManager(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bShouldTick = false;
}

void UPDWAchievementManager::InitManager(UNebulaFlowGameInstance* InstanceOwner)
{
	Super::InitManager(InstanceOwner);

	//Set up variables
	if (AchievementsTable)
	{
		TArray<FAchievementDataTableRow*> AchievementsRows;
		AchievementsTable->GetAllRows(TEXT("GENERAL"), AchievementsRows);

		for (FAchievementDataTableRow* Row : AchievementsRows)
		{
			AchievementsMap.Add(Row->AchievementType, *Row);
		}
	}

	//Bind Events
	if (UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld()))
	{
		EventSubsystem->OnItemCollected.AddUniqueDynamic(this, &UPDWAchievementManager::OnItemCollected);
		EventSubsystem->OnPupCustomize.AddUniqueDynamic(this, &UPDWAchievementManager::OnPupCustomize);
		EventSubsystem->OnDinoCustomize.AddUniqueDynamic(this, &UPDWAchievementManager::OnDinoCustomize);
		EventSubsystem->OnDinoNeedCompleted.AddUniqueDynamic(this, &UPDWAchievementManager::OnDinoNeedCompleted);
		EventSubsystem->OnPupTreatCollected.AddUniqueDynamic(this, &UPDWAchievementManager::OnPupTreatCollected);
		EventSubsystem->OnQuestCompleted.AddUniqueDynamic(this, &UPDWAchievementManager::OnQuestCompleted);
		EventSubsystem->OnEggHatch.AddUniqueDynamic(this, &UPDWAchievementManager::OnEggHatch);
		EventSubsystem->OnInteractionComplete.AddUniqueDynamic(this, &UPDWAchievementManager::OnInteractionComplete);
		EventSubsystem->OnDinoPenUnlocked.AddUniqueDynamic(this, &UPDWAchievementManager::OnDinoPenUnlocked);
	}
}

void UPDWAchievementManager::UninitManager()
{
	//Unbind Events
	if (UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld()))
	{
		EventSubsystem->OnItemCollected.RemoveDynamic(this, &UPDWAchievementManager::OnItemCollected);
		EventSubsystem->OnPupCustomize.RemoveDynamic(this, &UPDWAchievementManager::OnPupCustomize);
		EventSubsystem->OnDinoCustomize.RemoveDynamic(this, &UPDWAchievementManager::OnDinoCustomize);
		EventSubsystem->OnDinoNeedCompleted.RemoveDynamic(this, &UPDWAchievementManager::OnDinoNeedCompleted);
		EventSubsystem->OnPupTreatCollected.RemoveDynamic(this, &UPDWAchievementManager::OnPupTreatCollected);
		EventSubsystem->OnQuestCompleted.RemoveDynamic(this, &UPDWAchievementManager::OnQuestCompleted);
		EventSubsystem->OnEggHatch.RemoveDynamic(this, &UPDWAchievementManager::OnEggHatch);
		EventSubsystem->OnInteractionComplete.RemoveDynamic(this, &UPDWAchievementManager::OnInteractionComplete);
		EventSubsystem->OnDinoPenUnlocked.RemoveDynamic(this, &UPDWAchievementManager::OnDinoPenUnlocked);
	}
}

void UPDWAchievementManager::UnlockAchievement(ULocalPlayer* inPlayer, const FName& AchievementId)
{
	#if !UE_BUILD_SHIPPING	
	if (CVarShowAchievementsDebugData.GetValueOnAnyThread())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Achievement %s conditiond satisfied"), *AchievementId.ToString()), true, FVector2D(2.f, 2.f));
	}
	#endif

	Super::UnlockAchievement(inPlayer, AchievementId);
}

void UPDWAchievementManager::UnlockAchievement(const FName& AchievementId)
{
	APDWPlayerController* PlayerController = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	if (PlayerController)
	{
		ULocalPlayer* PlayerOwner = PlayerController->GetLocalPlayer();
		{
			UnlockAchievement(PlayerOwner, AchievementId);
		}
	}
}

void UPDWAchievementManager::OnItemCollected(FGameplayTag ItemTag)
{
	FGameplayTag ParentTag {};
	FAchievementDataTableRow* AchivementRow {};

	if (ItemTag.MatchesTag(UPDWGameplayTagSettings::GetPupCustomizationTag()))
	{
		ParentTag = UPDWGameplayTagSettings::GetPupCustomizationTag();
		AchivementRow = AchievementsMap.Find(EAchievementType::EA_Pupswardrobe);
	}
	else if (ItemTag.MatchesTag(UPDWGameplayTagSettings::GetDinoCustomizationTag()))
	{
		ParentTag = UPDWGameplayTagSettings::GetDinoCustomizationTag();
		AchivementRow = AchievementsMap.Find(EAchievementType::EA_Dinowardrobe);
	}
	else if (ItemTag.MatchesTag(UPDWGameplayTagSettings::GetEggTag()))
	{
		ParentTag = UPDWGameplayTagSettings::GetEggTag();
		AchivementRow = AchievementsMap.Find(EAchievementType::EA_Eggstraordinary);
	}
	else if (ItemTag.MatchesTag(UPDWGameplayTagSettings::GetDinoPenCustomizationTag()))
	{
		UnlockAchievement(AchievementsMap.Find(EAchievementType::EA_Acozydinohome)->AchievementID);
		return;
	}
	else
	{
		return;
	}

    FGameplayTagContainer TagContainer = UGameplayTagsManager::Get().RequestGameplayTagChildren(ParentTag);
	TArray<FGameplayTag> LeafTags;

	for (const FGameplayTag& Tag : TagContainer)
	{
		FGameplayTagContainer SubChildren = UGameplayTagsManager::Get().RequestGameplayTagChildren(Tag);
		if (SubChildren.Num() == 0)
		{
			LeafTags.Add(Tag);
		}
	}

	if (AchivementRow && UPDWDataFunctionLibrary::GetItemsByFilter(this, ParentTag).Num() == LeafTags.Num())
	{
		UnlockAchievement(AchivementRow->AchievementID);
	}
}

void UPDWAchievementManager::OnPupCustomize()
{
	UnlockAchievement(AchievementsMap.Find(EAchievementType::EA_Suchatrendypup)->AchievementID);
}

void UPDWAchievementManager::OnDinoCustomize()
{
	UnlockAchievement(AchievementsMap.Find(EAchievementType::EA_Dinofashion)->AchievementID);
}

void UPDWAchievementManager::OnDinoNeedCompleted()
{
	FAchievementDataTableRow* EA_NodinoistoobigAchievement = AchievementsMap.Find(EAchievementType::EA_Nodinoistoobig);
	UPDWDataFunctionLibrary::IncrementDinoNeedsSolved(GetWorld());
	int32 DinoNeedsSolved = UPDWDataFunctionLibrary::GetDinoNeedsSolved(GetWorld());

	if (DinoNeedsSolved >= EA_NodinoistoobigAchievement->QuantityValue)
	{
		UnlockAchievement(EA_NodinoistoobigAchievement->AchievementID);
	}
}

void UPDWAchievementManager::OnPupTreatCollected()
{
	FAchievementDataTableRow* TreatTimeAchievement = AchievementsMap.Find(EAchievementType::EA_Treattime);
	FAchievementDataTableRow* BellyFullAchievement = AchievementsMap.Find(EAchievementType::EA_Bellyfull);
	UPDWDataFunctionLibrary::IncrementPupTreatsCollected(GetWorld());
	int32 CollectedTreats = UPDWDataFunctionLibrary::GetPupTreatsCollected(GetWorld());

	if (CollectedTreats >= BellyFullAchievement->QuantityValue)
	{
		UnlockAchievement(BellyFullAchievement->AchievementID);
	}
	else if (CollectedTreats >= TreatTimeAchievement->QuantityValue)
	{
		UnlockAchievement(TreatTimeAchievement->AchievementID);
	}
}

void UPDWAchievementManager::OnQuestCompleted(const FGameplayTag& QuestID)
{
	FAchievementDataTableRow* WelcometoDinoIslandAchievement = AchievementsMap.Find(EAchievementType::EA_WelcometoDinoIsland);
	FAchievementDataTableRow* PupsSaveDinoIslandAchievement = AchievementsMap.Find(EAchievementType::EA_PupssaveDinoIsland);
	UPDWDataFunctionLibrary::IncrementPupTreatsCollected(GetWorld());
	int32 CollectedTreats = UPDWDataFunctionLibrary::GetPupTreatsCollected(GetWorld());

	if (QuestID.MatchesTagExact(WelcometoDinoIslandAchievement->TagValue))
	{
		UnlockAchievement(WelcometoDinoIslandAchievement->AchievementID);
	}
	else if (QuestID.MatchesTagExact(PupsSaveDinoIslandAchievement->TagValue))
	{
		UnlockAchievement(PupsSaveDinoIslandAchievement->AchievementID);
	}
}

void UPDWAchievementManager::OnEggHatch(FGameplayTag NestTag)
{
	FAchievementDataTableRow* EggstralongneckbabyAchievement = AchievementsMap.Find(EAchievementType::EA_Eggstralongneckbaby);
	FAchievementDataTableRow* EggsitterAchievement = AchievementsMap.Find(EAchievementType::EA_Eggsitter);
	FAchievementDataTableRow* EggcellentAchievement = AchievementsMap.Find(EAchievementType::EA_Eggcellent);
	FAchievementDataTableRow* EggperiencednannyAchievement = AchievementsMap.Find(EAchievementType::EA_Eggperiencednanny);

	if (NestTag.MatchesTagExact(EggstralongneckbabyAchievement->TagValue))
	{
		UnlockAchievement(EggstralongneckbabyAchievement->AchievementID);
	}
	else if (NestTag.MatchesTagExact(EggsitterAchievement->TagValue))
	{
		UnlockAchievement(EggsitterAchievement->AchievementID);
	}
	else if (NestTag.MatchesTagExact(EggcellentAchievement->TagValue))
	{
		UnlockAchievement(EggcellentAchievement->AchievementID);
	}
	else if (NestTag.MatchesTagExact(EggperiencednannyAchievement->TagValue))
	{
		UnlockAchievement(EggperiencednannyAchievement->AchievementID);
	}
}

void UPDWAchievementManager::OnInteractionComplete(UPDWInteractionReceiverComponent* Interaction)
{
	FAchievementDataTableRow* LetsdinodothisAchievement = AchievementsMap.Find(EAchievementType::EA_Letsdinodothis);
	FAchievementDataTableRow* LetsrollAchievement = AchievementsMap.Find(EAchievementType::EA_Letsroll);
	int32 UsedInteractions = UPDWDataFunctionLibrary::GetUsedInteractableCount(GetWorld());

	if (UsedInteractions >= LetsrollAchievement->QuantityValue)
	{
		UnlockAchievement(LetsrollAchievement->AchievementID);
	}
	else if (UsedInteractions >= LetsdinodothisAchievement->QuantityValue)
	{
		UnlockAchievement(LetsdinodothisAchievement->AchievementID);
	}

	AActor* InteractableActor = Interaction->GetOwner();
	if (InteractableActor && InteractableActor->IsA(ChestClass))
	{
		FAchievementDataTableRow* OpenallChestsAchievement = AchievementsMap.Find(EAchievementType::EA_Treasurehunters);
		UPDWDataFunctionLibrary::IncrementOpenedChests(GetWorld());
		int32 OpenedChest = UPDWDataFunctionLibrary::GetOpenedChests(GetWorld());
		if (OpenedChest >= OpenallChestsAchievement->QuantityValue)
		{
			UnlockAchievement(OpenallChestsAchievement->AchievementID);
		}
	}
}

void UPDWAchievementManager::OnDinoPenUnlocked(const FGameplayTag& DinoPenTag)
{
	FAchievementDataTableRow* PupssaveBrachioDaddyAchievement = AchievementsMap.Find(EAchievementType::EA_PupssaveBrachioDaddy);
	FAchievementDataTableRow* PupssaveMommaTopsAchievement = AchievementsMap.Find(EAchievementType::EA_PupssaveMommaTops);
	FAchievementDataTableRow* PupssaveAuntieStegAchievement = AchievementsMap.Find(EAchievementType::EA_PupssaveAuntieSteg);
	FAchievementDataTableRow* PupssaveMahakaBrahAchievement = AchievementsMap.Find(EAchievementType::EA_PupssaveMahakaBrah);

	if (DinoPenTag.MatchesTagExact(PupssaveBrachioDaddyAchievement->TagValue))
	{
		UnlockAchievement(PupssaveBrachioDaddyAchievement->AchievementID);
	}
	else if (DinoPenTag.MatchesTagExact(PupssaveMommaTopsAchievement->TagValue))
	{
		UnlockAchievement(PupssaveMommaTopsAchievement->AchievementID);
	}
	else if (DinoPenTag.MatchesTagExact(PupssaveAuntieStegAchievement->TagValue))
	{
		UnlockAchievement(PupssaveAuntieStegAchievement->AchievementID);
	}
	else if (DinoPenTag.MatchesTagExact(PupssaveMahakaBrahAchievement->TagValue))
	{
		UnlockAchievement(PupssaveMahakaBrahAchievement->AchievementID);
	}
}
