// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWDinoPen.h"
#include "Managers/PDWEventSubsytem.h"
#include "Managers/PDWPaleoCenterSubsystem.h"
#include "FlowSubsystem.h"
#include "FlowComponent.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Gameplay/Actors/PDWDinoNestHandler.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

#include "Misc/AutomationTest.h"
#include "Gameplay/Actors/PDWDinoPenArea.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Gameplay/Components/PDWWaterStationHandler.h"
#include "Gameplay/Components/PDWFoodStationHandler.h"
#include "Gameplay/Components/PDWPaleoDinoComponent.h"
#include "Gameplay/Actors/PDWPaleoCenterCustomization.h"
#include "Gameplay/FLOW/PaleoCenter/PDWFoodFlow.h"
#include "Gameplay/FLOW/PaleoCenter/PDWEggsFlow.h"
#include "FunctionLibraries/PDWUIFunctionLibrary.h"
#include "Data/PDWPaleoCenterSettings.h"
#include "Gameplay/Components/PDWTagComponent.h"
#include "QuestSettings.h"
#include "Gameplay/Components/PDWBabyPaleoDinoComponent.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/FLOW/PaleoCenter/PDWFlowNode_GetFoodFlowConfig.h"
#include "Gameplay/Components/PDWDinoAIMoveComponent.h"

// Sets default values
UPDWDinoPen::UPDWDinoPen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = false;
}

void UPDWDinoPen::OnStartPlay()
{
	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnItemCollected.AddUniqueDynamic(this, &UPDWDinoPen::OnItemCollected);
	}

	if (UFlowSubsystem* FlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTagFast(EggsFlowTag);
		TagContainer.AddTagFast(DinoNestTag);
		TagContainer.AddTagFast(FoodStationTag);
		TagContainer.AddTagFast(WaterStationTag);
		TagContainer.AddTagFast(PenAreaTag);
		TagContainer.AddTagFast(FoodFlowTag);
		TagContainer.AddTagFast(DinoTag);
		TagContainer.AddTagFast(BabyDinoTag);
		TagContainer.AppendTags(WaterHoseMinigameTag);
		TagContainer.AppendTags(MedicalKitMinigameTag);
		
		for (const TWeakObjectPtr<UFlowComponent>& Component : FlowSubsystem->GetComponents<UFlowComponent>(TagContainer, EGameplayContainerMatchType::Any, true))
		{
			OnFlowCompRegistered(Component.Get());
		}

		for (const TWeakObjectPtr<UFlowComponent>& Component : FlowSubsystem->GetComponents<UFlowComponent>(CustomizationTypeTag.GetSingleTagContainer(), EGameplayContainerMatchType::Any, false))
		{
			OnFlowCompRegistered(Component.Get());
		}
	}

	BindFlowCompEvents();
}

void UPDWDinoPen::StartPaleoQuests(bool FromLoad)
{
	StartEggsQuest(FromLoad);

	if (DinoCompRef && GetCurrentDinoNeed() == UPDWGameplayTagSettings::GetHungryDino() && DinoCompRef->GetNeedPlayerHelp())
	{
		StartFoodQuest(FromLoad);
	}
}

void UPDWDinoPen::Uninit()
{	
	if(!UPDWDataFunctionLibrary::IsDinoPenActive(this, DinoPenTag))
		return;

	UnbindFlowCompEvents();
}

void UPDWDinoPen::UnlockPen(bool StartInactive)
{
	TMap<TObjectPtr<UDataLayerAsset>,bool> DL = {{MainDL, true}};
	UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(this, DL);
	UPDWDataFunctionLibrary::SetDinoNeed(this, DinoPenTag, UPDWGameplayTagSettings::GetHappyDino());
	UPDWDataFunctionLibrary::SetFakeDinoNeed(this, DinoPenTag, UPDWGameplayTagSettings::GetHappyDino());
	SetIsPenActive(!StartInactive, true);

	if (UPDWDataFunctionLibrary::GetItemsByFilter(this, EggTypeTag).Num() >= MaxEggs)
	{
		OnNestFull(nullptr);
	}
}

void UPDWDinoPen::HandleFakeNeed(FGameplayTag FakeNeed)
{
	UPDWDataFunctionLibrary::SetFakeDinoNeed(this, DinoPenTag, FakeNeed);

	if (DinoCompRef)
	{
		DinoCompRef->HandleFakeNeed(FakeNeed);
	}
}

void UPDWDinoPen::BindFlowCompEvents()
{
	if (UFlowSubsystem* FlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
	{	
		FlowSubsystem->OnComponentRegistered.AddUniqueDynamic(this, &UPDWDinoPen::OnFlowCompRegistered);
		FlowSubsystem->OnComponentUnregistered.AddUniqueDynamic(this, &UPDWDinoPen::OnFlowCompUnregistered);
	}
}

void UPDWDinoPen::UnbindFlowCompEvents()
{
	if (UFlowSubsystem* FlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
	{
		FlowSubsystem->OnComponentRegistered.RemoveDynamic(this, &UPDWDinoPen::OnFlowCompRegistered);
		FlowSubsystem->OnComponentUnregistered.RemoveDynamic(this, &UPDWDinoPen::OnFlowCompUnregistered);
	}

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnItemCollected.RemoveDynamic(this, &UPDWDinoPen::OnItemCollected);
	}
}

void UPDWDinoPen::UpdateDinoNeeds(FGameplayTag NewNeed)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("DinoPenTag: %s, NewNeed: %s"), *DinoPenTag.ToString(), *NewNeed.ToString()));
	UPDWDataFunctionLibrary::SetDinoNeed(this, DinoPenTag, NewNeed);
	
	if (!DinoCompRef)
		return;

	DinoCompRef->HandleNeed(NewNeed);

	FPDWDinoPenNeed DinoNeedInfo = {};
	DinoNeedInfo.DinoNeed = GetCurrentDinoNeed();
	DinoNeedInfo.bDinoWaitingPlayerHelp = DinoCompRef->GetNeedPlayerHelp();
	if (DinoNeedInfo.DinoNeed == UPDWGameplayTagSettings::GetHungryDino())
	{
		DinoNeedInfo.NeededItem = FoodTypeTag;
	}

	if (PenAreaRef && PenAreaRef->IsPlayerInsideArea())
	{
		CheckWaterStation();
		CheckFoodStation();

		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnDinoNeedUpdateEvent(DinoNeedInfo);
		}
	}
}

float UPDWDinoPen::DecrementFoodStation(float RemovedFood)
{
	float CurrentFood = UPDWDataFunctionLibrary::GetStationFood(this,DinoPenTag);
	float UpdatedQuantity = FMath::Clamp(CurrentFood - RemovedFood, 0, CurrentFood);
	UPDWDataFunctionLibrary::UpdateFoodInStation(this, DinoPenTag, UpdatedQuantity);
	return CurrentFood - UpdatedQuantity;
}

float UPDWDinoPen::DecrementWaterStation(float RemovedWater)
{
	float CurrentWater = UPDWDataFunctionLibrary::GetStationWater(this,DinoPenTag);
	float UpdatedQuantity = FMath::Clamp(CurrentWater - RemovedWater, 0, CurrentWater);
	UPDWDataFunctionLibrary::UpdateWaterInStation(this, DinoPenTag, UpdatedQuantity);
	return CurrentWater - UpdatedQuantity;
}

bool UPDWDinoPen::GetShouldSimulate()
{
	return !DinoCompRef || !WaterStationRef || !FoodStationRef;
}

FGameplayTag UPDWDinoPen::GetCurrentDinoNeed()
{
	return UPDWDataFunctionLibrary::GetDinoNeed(this, DinoPenTag);;
}

FGameplayTag UPDWDinoPen::GetCurrentFakeDinoNeed()
{
	return UPDWDataFunctionLibrary::GetFakeDinoNeed(this, DinoPenTag);;
}

bool UPDWDinoPen::GetIsPenUnlocked()
{
	if (UDataLayerManager* DataLayerManager = UDataLayerManager::GetDataLayerManager(this))
	{
		const UDataLayerInstance* DLInstance = DataLayerManager->GetDataLayerInstanceFromAsset(MainDL);
		if (DLInstance)
		{
			return DLInstance->GetRuntimeState() == EDataLayerRuntimeState::Activated;
		}
	}

	return false;
}

bool UPDWDinoPen::GetIsPenActive()
{
	return 	UPDWDataFunctionLibrary::IsDinoPenActive(this, DinoPenTag);
}

void UPDWDinoPen::OnItemCollected(FGameplayTag ItemTag)
{
	if (ItemTag.MatchesTag(EggTypeTag))
	{
		if (NestRefHandler)
		{
			NestRefHandler->OnEggCollected(ItemTag);
		}
		
		if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
		{
			PaleoCenterSubsystem->SetLastItemNestInfo(DinoPenTag);
		}		
	}
	else if (ItemTag.MatchesTag(SeedTypeTag))
	{
		TMap<TObjectPtr<UDataLayerAsset>,bool> DL = {{FoodSourcesDL, true}};
		UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(this, DL);
	}
	else if (ItemTag.MatchesTag(CustomizationTypeTag))
	{
		for (APDWPaleoCenterCustomization* Customization : CustomizationsRef)
		{
			if (Customization->GetFlowComponent() && Customization->GetFlowComponent()->IdentityTags.HasTagExact(ItemTag))
			{
				Customization->ChangeCustomizationVisibility(true);
			}
		}
		if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
		{
			PaleoCenterSubsystem->SetLastItemNestInfo(DinoPenTag);
		}
	}
}

void UPDWDinoPen::OnDinoPenAreaEnter(AActor* OverlappingActor)
{
	if (GetIsPenUnlocked() && DinoCompRef)
	{
		DinoCompRef->SetCanShowNeeds(true);

		CheckWaterStation();

		if (GetCurrentDinoNeed() == UPDWGameplayTagSettings::GetHungryDino() && DinoCompRef->GetNeedPlayerHelp())
		{
			if (PenAreaRef && UPDWDataFunctionLibrary::GetItemQuantityFromInventory(this, FoodTypeTag) <= 0)
			{
				StartFoodQuest();
			}
		}

		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnPenAreaBeginOverlapEvent(GetPenInfo(), OverlappingActor);		
		}
	}

}

void UPDWDinoPen::SetIsPenActive(bool _IsActive, bool ResetNeedTimer)
{
	UPDWDataFunctionLibrary::ChangeDinoPenActivity(this, DinoPenTag, _IsActive);

	if (DinoCompRef)
	{	
		FGameplayTag CurrentNeed = _IsActive ? GetCurrentDinoNeed() : GetCurrentFakeDinoNeed();
		if(MinigamesDL.Contains(CurrentNeed))
		{
			TMap<TObjectPtr<UDataLayerAsset>, bool> DL = { {MinigamesDL[CurrentNeed], _IsActive} };
			UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(this, DL);
		}
	}

	if (_IsActive && ResetNeedTimer)
	{
		if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
		{
			PaleoCenterSubsystem->ResetDinoNeedTimer();
		}
	}

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnDinoPenActivityChangeEvent(DinoPenTag, _IsActive);
	}
}

FPDWDinoPenInfo UPDWDinoPen::GetPenInfo()
{
	FPDWDinoPenInfo PenInfo;
	PenInfo.DinoPenTag = DinoPenTag;
	PenInfo.IsPenUnlocked = GetIsPenUnlocked();

	//Eggs
	PenInfo.DinoPenEggs.EggId = EggTypeTag;
	TMap<FGameplayTag, int32> EggsData = UPDWDataFunctionLibrary::GetItemsByFilter(this, EggTypeTag);
	PenInfo.DinoPenEggs.EggCollected = EggsData.Num();
	PenInfo.DinoPenEggs.MaxEggs = MaxEggs;
	if (UDataLayerManager* DataLayerManager = UDataLayerManager::GetDataLayerManager(this))
	{
		const UDataLayerInstance* DLInstance = DataLayerManager->GetDataLayerInstanceFromAsset(BabyDinoDL);
		if (DLInstance)
		{
			PenInfo.DinoPenEggs.IsBabyDinoBorn = DLInstance->GetRuntimeState() == EDataLayerRuntimeState::Activated;
		}
	}	

	//Dino needs
	PenInfo.DinoNeed.DinoNeed = GetCurrentDinoNeed();
	if (PenInfo.DinoNeed.DinoNeed == UPDWGameplayTagSettings::GetHungryDino())
	{
		PenInfo.DinoNeed.NeededItem = FoodTypeTag;
		PenInfo.DinoNeed.bPlayerHaveTheItem = UPDWDataFunctionLibrary::GetItemQuantityFromInventory(this, FoodTypeTag) > 0;
	}
	if (DinoCompRef)
	{
		PenInfo.DinoNeed.bDinoWaitingPlayerHelp = DinoCompRef->GetNeedPlayerHelp();
	}

	//Customizations
	PenInfo.DinoPenDecorations.MaxDecorations = MaxCustomizations;
	PenInfo.DinoPenDecorations.DecorationsUnlocked = UPDWDataFunctionLibrary::GetItemsByFilter(this, CustomizationTypeTag).Num();
	PenInfo.DinoPenDecorations.DinoId = DinoTag;

	//Dino Display Text
	PenInfo.DinoDisplayName = DinoDisplayName;
	PenInfo.DinoDisplayIcon = DinoDisplayIcon;

	return PenInfo;
}

void UPDWDinoPen::OnDinoPenAreaExit(AActor* OverlappingActor)
{
	if (GetIsPenUnlocked())
	{
		if (DinoCompRef)
		{
			DinoCompRef->SetCanShowNeeds(false);
		}

		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnPenAreaEndOverlapEvent(OverlappingActor);
		}
	}
}


void UPDWDinoPen::CheckWaterStation()
{
	if(!WaterStationRef || !GetIsPenActive())
		return;

	UPDWTagComponent* TagComponent = WaterStationRef->GetComponentByClass<UPDWTagComponent>();
	if (!TagComponent)
		return;

	if (GetCurrentDinoNeed() == UPDWGameplayTagSettings::GetThirstyDino() && DinoCompRef->GetNeedPlayerHelp())
	{
		TagComponent->AddTag(UQuestSettings::GetPaleoQuestTargetTag());
	}
	else if (TagComponent->HasTag(UQuestSettings::GetPaleoQuestTargetTag()))
	{
		TagComponent->RemoveTag(UQuestSettings::GetPaleoQuestTargetTag());
	}
}

void UPDWDinoPen::CheckFoodStation()
{
	if (GetCurrentDinoNeed() == UPDWGameplayTagSettings::GetHungryDino() && DinoCompRef->GetNeedPlayerHelp())
	{
		if (UPDWDataFunctionLibrary::GetItemQuantityFromInventory(this, FoodTypeTag) <= 0)
		{
			StartFoodQuest();
		}
	}
}

void UPDWDinoPen::OnMinigameActivation(FGameplayTag DinoNeed)
{
	if(MinigamesDL.Contains(DinoNeed))
	{
		TMap<TObjectPtr<UDataLayerAsset>,bool> DL = {{MinigamesDL[DinoNeed], true}};
		UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(this, DL);
	}
}

void UPDWDinoPen::OnFoodStationMinigameSuccess(const FMiniGameEventSignature& inSignature)
{
	if (!FoodStationRef)
		return;

	UPDWFoodStationHandler* FoodStationHandler = Cast<UPDWFoodStationHandler>(FoodStationRef->GetComponentByClass(UPDWFoodStationHandler::StaticClass()));
	if (!FoodStationHandler)
		return;

	FoodStationHandler->HandleMinigameEnd();

	if (!DinoCompRef)
		return;
	
	DinoCompRef->HandleFoodStationPlayerInteraction();	
}

void UPDWDinoPen::OnWaterStationInteractionSuccess(const FPDWInteractionPayload& Payload)
{
	if (APawn* Executioner = Cast<APawn>(Payload.Executioner))
	{
		if(!Executioner->IsPlayerControlled())
			return;

		if (!WaterStationRef)
			return;

		UPDWWaterStationHandler* WaterStationHandler = Cast<UPDWWaterStationHandler>(WaterStationRef->GetComponentByClass(UPDWWaterStationHandler::StaticClass()));
		if (!WaterStationHandler)
			return;

		WaterStationHandler->HandleEndInteraction();

		if (!DinoCompRef)
			return;

		DinoCompRef->HandleWaterStationPlayerInteraction();	
		CheckWaterStation();
	}
}

void UPDWDinoPen::OnNestMinigameSuccess(const FMiniGameEventSignature& inSignature)
{
	TMap<TObjectPtr<UDataLayerAsset>,bool> DL = {{BabyDinoDL, true}};
	UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(this, DL);
	if (NestRefHandler)
	{
		NestRefHandler->HideEggs();
	}

	UPDWInteractionReceiverComponent* InteractionComp = Cast<UPDWInteractionReceiverComponent>(NestRef->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass()));
	if (InteractionComp)
	{
		InteractionComp->AddStateTag(UPDWGameplayTagSettings::GetInteractionInactiveState());
	}

	if (UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld()))
	{
		EventSubsystem->OnEggHatchEvent(DinoNestTag);
	}
}

void UPDWDinoPen::OnWaterHoseMinigameSuccess(const FMiniGameEventSignature& inSignature)
{
	if (GetIsPenActive())
	{
		if (DinoCompRef)
		{
			DinoCompRef->HandleMinigameEnd(UPDWGameplayTagSettings::GetDirtyDino());
		}

		if (MinigamesDL.Contains(UPDWGameplayTagSettings::GetDirtyDino()))
		{
			TMap<TObjectPtr<UDataLayerAsset>, bool> DL = { {MinigamesDL[UPDWGameplayTagSettings::GetDirtyDino()], false} };
			UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(this, DL);
		}

		OnDinoHappy();
	}
}

void UPDWDinoPen::OnMedicalKitMinigameSuccess(const FMiniGameEventSignature& inSignature)
{
	if (GetIsPenActive())
	{
		if (DinoCompRef)
		{
			DinoCompRef->HandleMinigameEnd(UPDWGameplayTagSettings::GetHurtDino());
		}

		if (MinigamesDL.Contains(UPDWGameplayTagSettings::GetHurtDino()))
		{
			TMap<TObjectPtr<UDataLayerAsset>, bool> DL = { {MinigamesDL[UPDWGameplayTagSettings::GetHurtDino()], false} };
			UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(this, DL);
		}

		OnDinoHappy();
	}
}

void UPDWDinoPen::OnNestFull(AActor* Source)
{
	if (NestRef && GetIsPenActive())
	{
		UPDWInteractionReceiverComponent* InteractionComp = Cast<UPDWInteractionReceiverComponent>(NestRef->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass()));
		if (InteractionComp)
		{
			InteractionComp->AddStateTag(UPDWGameplayTagSettings::GetInteractionDefaultState());
		}
	}
}

void UPDWDinoPen::OnDinoHappy()
{
	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		PaleoCenterSubsystem->ResetDinoNeedTimer();
	}

	UpdateDinoNeeds(UPDWGameplayTagSettings::GetHappyDino());

	UPDWDataFunctionLibrary::SaveGame(GetWorld());
}

void UPDWDinoPen::StartFoodQuest(bool Load)
{
	if (FoodFlowCompRef && !FoodFlowCompRef->GetRootFlowInstance())
	{
		UPDWFoodFlow* FoodFlowAsset = Cast<UPDWFoodFlow>(FoodFlowCompRef->RootFlow);
		FPDWFoodFlowConfig FlowConfig = {};
		FlowConfig.FoodStationTag = FGameplayTagContainer(FoodStationTag);
		FlowConfig.FoodPlantTag = FGameplayTagContainer(FoodPlantTag);
		FlowConfig.FoodTypeTag = FoodTypeTag;
		FoodFlowAsset->SetFoodFlowConfig(FlowConfig);
		if (Load)
		{
			FoodFlowCompRef->LoadRootFlow();
		}
		else
		{
			FoodFlowCompRef->StartRootFlow();
		}
	}
}

void UPDWDinoPen::StartEggsQuest(bool Load)
{
	if (EggsFlowCompRef && !EggsFlowCompRef->GetRootFlowInstance())
	{
		UPDWEggsFlow* EggsFlowAsset = Cast<UPDWEggsFlow>(EggsFlowCompRef->RootFlow);
		FPDWEggFlowConfig FlowConfig = {};
		FlowConfig.EggTag = EggTypeTag;
		FlowConfig.NestTag = FGameplayTagContainer(DinoNestTag);
		FlowConfig.OutNestTeleportTag = NestTeleportTag;
		FlowConfig.PenTag = DinoPenTag;
		EggsFlowAsset->SetEggFlowConfig(FlowConfig);
		if (Load)
		{
			EggsFlowCompRef->LoadRootFlow();
		}
		else
		{
			EggsFlowCompRef->StartRootFlow();
		}	
	}
}

void UPDWDinoPen::OnFlowCompRegistered(UFlowComponent* Component)
{
	if(!Component)
		return;

	if (Component->IdentityTags.HasTagExact(DinoTag))
	{
		AActor* DinoRef = Cast<AActor>(Component->GetOwner());
		if(!DinoRef)
			return;

		DinoCompRef = Cast<UPDWPaleoDinoComponent>(DinoRef->GetComponentByClass(UPDWPaleoDinoComponent::StaticClass()));
		if (!DinoCompRef)
			return;

		DinoCompRef->OnActivateMinigame.AddUniqueDynamic(this, &UPDWDinoPen::OnMinigameActivation);
		DinoCompRef->Init(DinoPenTag,PenAreaRef,GetIsPenActive());

		UPDWDinoAIMoveComponent* DinoAIMoveComp = Cast<UPDWDinoAIMoveComponent>(DinoRef->GetComponentByClass(UPDWDinoAIMoveComponent::StaticClass()));
		if (!DinoAIMoveComp)
			return;

		DinoAIMoveComp->Init(DinoPenTag,PenAreaRef);

		if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
		{
			PaleoCenterSubsystem->OnDinoLoadedEvent(DinoRef);
		}
	}
	else if (Component->IdentityTags.HasTagExact(FoodStationTag))
	{
		FoodStationRef = Cast<AActor>(Component->GetOwner());
		if (!FoodStationRef)
			return;

		UPDWFoodStationHandler* FoodStationHandler = Cast<UPDWFoodStationHandler>(FoodStationRef->GetComponentByClass(UPDWFoodStationHandler::StaticClass()));
		if (!FoodStationHandler)
			return;

		FoodStationHandler->InitFoodStation(DinoPenTag,FoodTypeTag);

		UPDWMinigameConfigComponent* MinigameConfigComponent = Cast<UPDWMinigameConfigComponent>(FoodStationRef->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass()));
		if (!MinigameConfigComponent)
			return;

		MinigameConfigComponent->OnMinigameComplete.AddUniqueDynamic(this, &UPDWDinoPen::OnFoodStationMinigameSuccess);	
	}
	else if (Component->IdentityTags.HasTagExact(WaterStationTag))
	{
		WaterStationRef = Cast<AActor>(Component->GetOwner());
		if (!WaterStationRef)
			return;

		UPDWWaterStationHandler* WaterStationHandler = Cast<UPDWWaterStationHandler>(WaterStationRef->GetComponentByClass(UPDWWaterStationHandler::StaticClass()));
		if (!WaterStationHandler)
			return;

		WaterStationHandler->InitWaterStation(DinoPenTag);

		UPDWInteractionReceiverComponent* InteractionComp = Cast<UPDWInteractionReceiverComponent>(WaterStationRef->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass()));
		if (!InteractionComp)
			return;

		InteractionComp->OnInteractionSuccessful.AddUniqueDynamic(this, &UPDWDinoPen::OnWaterStationInteractionSuccess);		
	}
	else if (Component->IdentityTags.HasTagExact(DinoNestTag))
	{
		NestRef = Cast<AActor>(Component->GetOwner());
		if(!NestRef)
			return;

		NestRefHandler = Cast<UPDWDinoNestHandler>(NestRef->GetComponentByClass(UPDWDinoNestHandler::StaticClass()));
		if(!NestRefHandler)
			return;

		if (UDataLayerManager* DataLayerManager = UDataLayerManager::GetDataLayerManager(this))
		{
			const UDataLayerInstance* DLInstance = DataLayerManager->GetDataLayerInstanceFromAsset(BabyDinoDL);
			FGameplayTag InteractionTag = {};

			if (DLInstance && DLInstance->GetRuntimeState() == EDataLayerRuntimeState::Activated)
			{
				InteractionTag = UPDWGameplayTagSettings::GetInteractionInactiveState();
				NestRefHandler->HideEggs();
			}
			else
			{
				InteractionTag = UPDWDataFunctionLibrary::GetItemsByFilter(this, EggTypeTag).Num() >= MaxEggs ? 
				UPDWGameplayTagSettings::GetInteractionDefaultState() : UPDWGameplayTagSettings::GetInteractionInactiveState();
				NestRefHandler->OnNestFull.AddUniqueDynamic(this, &UPDWDinoPen::OnNestFull);
				NestRefHandler->SetMaxEgss(MaxEggs);
				NestRefHandler->UpdateEggs();

				UPDWMinigameConfigComponent* MinigameConfigComponent = Cast<UPDWMinigameConfigComponent>(NestRef->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass()));
				if (!MinigameConfigComponent)
					return;

				MinigameConfigComponent->OnMinigameComplete.AddUniqueDynamic(this, &UPDWDinoPen::OnNestMinigameSuccess);
			}

			UPDWInteractionReceiverComponent* InteractionComp = Cast<UPDWInteractionReceiverComponent>(NestRef->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass()));
			if (InteractionComp)
			{
				InteractionComp->AddStateTag(InteractionTag);
			}
		}
	}
	else if (Component->IdentityTags.HasTagExact(PenAreaTag))
	{
		PenAreaRef = Cast<APDWDinoPenArea>(Component->GetOwner());
		if(!PenAreaRef)
			return;

		PenAreaRef->OnPenAreaBeginOverlap.AddUniqueDynamic(this, &UPDWDinoPen::OnDinoPenAreaEnter);
		PenAreaRef->OnPenAreaEndOverlap.AddUniqueDynamic(this, &UPDWDinoPen::OnDinoPenAreaExit);
	}
	else if (Component->IdentityTags.HasTag(CustomizationTypeTag))
	{
		APDWPaleoCenterCustomization* CustomizationActor = Cast<APDWPaleoCenterCustomization>(Component->GetOwner());
		CustomizationsRef.Add(CustomizationActor);
		if (UPDWDataFunctionLibrary::GetItemQuantityFromInventory(this, Component->IdentityTags.First()) > 0)
		{
			CustomizationActor->ChangeCustomizationVisibility(true);		
		}
	}
	else if (Component->IdentityTags.HasTagExact(FoodFlowTag))
	{
		FoodFlowCompRef = Component;
	}
	else if (Component->IdentityTags.HasTagExact(EggsFlowTag))
	{
		EggsFlowCompRef = Component;
	}
	else if (Component->IdentityTags.HasTag(BabyDinoTag))
	{
		UPDWBabyPaleoDinoComponent* BabyDinoComp = Cast<UPDWBabyPaleoDinoComponent>(Component->GetOwner()->GetComponentByClass(UPDWBabyPaleoDinoComponent::StaticClass()));
		if (!BabyDinoComp) return;
		
		BabyDinoComp->Init(DinoPenTag, PenAreaRef);

		UPDWAIMoveComponent* DinoAIMoveComp = Cast<UPDWAIMoveComponent>(Component->GetOwner()->GetComponentByClass(UPDWAIMoveComponent::StaticClass()));
		if (!DinoAIMoveComp)
			return;

		DinoAIMoveComp->Init(DinoPenTag,PenAreaRef);
	}
	else if (Component->IdentityTags == WaterHoseMinigameTag)
	{
		UPDWMinigameConfigComponent* MinigameConfigComponent = Cast<UPDWMinigameConfigComponent>(Component->GetOwner()->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass()));
		if (!MinigameConfigComponent)
			return;

		MinigameConfigComponent->OnMinigameComplete.AddUniqueDynamic(this, &UPDWDinoPen::OnWaterHoseMinigameSuccess);	
	}
	else if (Component->IdentityTags == MedicalKitMinigameTag)
	{
		UPDWMinigameConfigComponent* MinigameConfigComponent = Cast<UPDWMinigameConfigComponent>(Component->GetOwner()->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass()));
		if (!MinigameConfigComponent)
			return;

		MinigameConfigComponent->OnMinigameComplete.AddUniqueDynamic(this, &UPDWDinoPen::OnMedicalKitMinigameSuccess);	
	}

	if (!GetShouldSimulate() && DinoCompRef && !DinoCompRef->OnDinoHappy.IsBound())
	{
		if (GetIsPenActive())
		{
			UpdateDinoNeeds(GetCurrentDinoNeed());
		}
		else
		{
			DinoCompRef->HandleFakeNeed(GetCurrentFakeDinoNeed());
		}

		DinoCompRef->OnDinoHappy.AddUniqueDynamic(this, &UPDWDinoPen::OnDinoHappy);		
	}
}

void UPDWDinoPen::OnFlowCompUnregistered(UFlowComponent* Component)
{
	if(!Component)
		return;

	if (Component->IdentityTags.HasTagExact(DinoTag) && DinoCompRef)
	{
		if (!DinoCompRef)
			return;

		DinoCompRef->OnDinoHappy.RemoveDynamic(this, &UPDWDinoPen::OnDinoHappy);
		DinoCompRef->OnActivateMinigame.RemoveDynamic(this, &UPDWDinoPen::OnMinigameActivation);
		DinoCompRef = nullptr;
	}
	else if (Component->IdentityTags.HasTagExact(FoodStationTag) && FoodStationRef)
	{
		UPDWMinigameConfigComponent* MinigameConfigComponent = Cast<UPDWMinigameConfigComponent>(FoodStationRef->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass()));
		if (MinigameConfigComponent)
		{
			MinigameConfigComponent->OnMinigameComplete.RemoveDynamic(this, &UPDWDinoPen::OnFoodStationMinigameSuccess);
		}
		FoodStationRef = nullptr;
	}
	else if (Component->IdentityTags.HasTagExact(WaterStationTag) && WaterStationRef)
	{
		UPDWInteractionReceiverComponent* InteractionComp = Cast<UPDWInteractionReceiverComponent>(WaterStationRef->GetComponentByClass(UPDWInteractionReceiverComponent::StaticClass()));
		if (InteractionComp)
		{
			InteractionComp->OnInteractionSuccessful.RemoveDynamic(this, &UPDWDinoPen::OnWaterStationInteractionSuccess);
		}
		WaterStationRef = nullptr;
	}
	else if (Component->IdentityTags.HasTagExact(DinoNestTag) && NestRef)
	{
		UPDWMinigameConfigComponent* MinigameConfigComponent = Cast<UPDWMinigameConfigComponent>(NestRef->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass()));
		if (MinigameConfigComponent)
		{
			MinigameConfigComponent->OnMinigameComplete.RemoveDynamic(this, &UPDWDinoPen::OnNestMinigameSuccess);
		}
		if (NestRefHandler)
		{
			NestRefHandler->OnNestFull.RemoveDynamic(this, &UPDWDinoPen::OnNestFull);
		}
		NestRef = nullptr;
		
	}
	else if (Component->IdentityTags.HasTagExact(PenAreaTag) && PenAreaRef)
	{
		PenAreaRef->OnPenAreaBeginOverlap.RemoveDynamic(this, &UPDWDinoPen::OnDinoPenAreaEnter);
		PenAreaRef->OnPenAreaEndOverlap.RemoveDynamic(this, &UPDWDinoPen::OnDinoPenAreaExit);
		PenAreaRef = nullptr;
	}
	else if (Component->IdentityTags.HasTag(CustomizationTypeTag))
	{
		APDWPaleoCenterCustomization* CustomizationActor = Cast<APDWPaleoCenterCustomization>(Component->GetOwner());
		CustomizationsRef.Remove(CustomizationActor);
	}
	else if (Component->IdentityTags == WaterHoseMinigameTag)
	{
		UPDWMinigameConfigComponent* MinigameConfigComponent = Cast<UPDWMinigameConfigComponent>(Component->GetOwner()->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass()));
		if (!MinigameConfigComponent)
			return;

		MinigameConfigComponent->OnMinigameComplete.RemoveDynamic(this, &UPDWDinoPen::OnWaterHoseMinigameSuccess);	
	}
	else if (Component->IdentityTags == MedicalKitMinigameTag)
	{
		UPDWMinigameConfigComponent* MinigameConfigComponent = Cast<UPDWMinigameConfigComponent>(Component->GetOwner()->GetComponentByClass(UPDWMinigameConfigComponent::StaticClass()));
		if (!MinigameConfigComponent)
			return;

		MinigameConfigComponent->OnMinigameComplete.RemoveDynamic(this, &UPDWDinoPen::OnMedicalKitMinigameSuccess);	
	}
}

