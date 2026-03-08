// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWPaleoDinoComponent.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Gameplay/Actors/PDWDinoPenArea.h"
#include "Components/NebulaGraphicsCustomizationComponent.h"
#include "Data/PDWPaleoCenterSettings.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

UPDWPaleoDinoComponent::UPDWPaleoDinoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPDWPaleoDinoComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPDWPaleoDinoComponent::Init(FGameplayTag _PenTag, APDWDinoPenArea* _PenAreaRef, bool _CanMove)
{
	PenTag = _PenTag;
	PenAreaRef = _PenAreaRef;
	CanMove = _CanMove;
	InitCustomization();

	OnInit.Broadcast();
}

void UPDWPaleoDinoComponent::InitCustomization()
{
	if (!GetOwner()) return;

	UNebulaGraphicsCustomizationComponent* CustomizationComp = GetOwner()->GetComponentByClass<UNebulaGraphicsCustomizationComponent>();

	if(!CustomizationComp) return;

	FGameplayTag SavedCustomizationTag = UPDWDataFunctionLibrary::GetDinoCustomization(this, PenTag);

	if (SavedCustomizationTag != FGameplayTag::EmptyTag)
	{
		CustomizationComp->CustomizeStaticMesh(SavedCustomizationTag.GetSingleTagContainer());
	}	
}

void UPDWPaleoDinoComponent::SetCanShowNeeds(bool _CanShowNeeds)
{
	CanShowNeeds = _CanShowNeeds;
	FGameplayTag CurrentNeed = GetCurrentNeed();
	FGameplayTag CurrentFakeNeed = GetCurrentFakeNeed();

	if (CurrentFakeNeed.IsValid() && CurrentFakeNeed != UPDWGameplayTagSettings::GetHappyDino())
	{
		OnShowFakeNeed.Broadcast(CanShowNeeds ? CurrentFakeNeed : UPDWGameplayTagSettings::GetHappyDino());
	}
	else
	{
		OnShowNeed.Broadcast(CanShowNeeds && NeedPlayerHelp ? CurrentNeed : UPDWGameplayTagSettings::GetHappyDino());	
	}
}

FGameplayTag UPDWPaleoDinoComponent::GetCurrentNeed()
{
	return UPDWDataFunctionLibrary::GetDinoNeed(this, PenTag);
}

FGameplayTag UPDWPaleoDinoComponent::GetCurrentFakeNeed()
{
	return UPDWDataFunctionLibrary::GetFakeDinoNeed(this, PenTag);
}

void UPDWPaleoDinoComponent::HandleMinigameEnd(FGameplayTag MinigameTag)
{
	OnMinigameEnd.Broadcast(MinigameTag);
}

void UPDWPaleoDinoComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	BeforeEndPlay.Broadcast();
	Super::EndPlay(EndPlayReason);
}

void UPDWPaleoDinoComponent::HandleNeed(FGameplayTag NewNeed)
{
	OnShowNeedMat.Broadcast(NewNeed);

	if (NewNeed == UPDWGameplayTagSettings::GetHappyDino())
	{
		OnActivateInteraction.Broadcast(NewNeed);
		OnShowNeed.Broadcast(NewNeed);		
	}
	else
	{
		OnDeactivateInteraction.Broadcast(NewNeed);
		OnSearchSO.Broadcast(NewNeed);
	}
}

void UPDWPaleoDinoComponent::HandleFakeNeed(FGameplayTag FakeNeed)
{	
	if (CanShowNeeds)
	{
		OnShowFakeNeed.Broadcast(FakeNeed);
	}
}

void UPDWPaleoDinoComponent::HandleWaterStationPlayerInteraction()
{
	FGameplayTag CurrentNeed = GetCurrentNeed();
	if (NeedPlayerHelp && CurrentNeed == UPDWGameplayTagSettings::GetThirstyDino())
	{	
		NeedPlayerHelp = false;
		OnSearchSO.Broadcast(CurrentNeed);
	}
}

void UPDWPaleoDinoComponent::HandleFoodStationPlayerInteraction()
{
	FGameplayTag CurrentNeed = GetCurrentNeed();
	if (NeedPlayerHelp && CurrentNeed == UPDWGameplayTagSettings::GetHungryDino())
	{
		NeedPlayerHelp = false;
		OnSearchSO.Broadcast(CurrentNeed);
	}
}

void UPDWPaleoDinoComponent::OnSearchSOCompleted(bool Success)
{
	FGameplayTag CurrentNeed = GetCurrentNeed();

	if (Success && (CurrentNeed == UPDWGameplayTagSettings::GetHungryDino() || CurrentNeed == UPDWGameplayTagSettings::GetThirstyDino()))
	{
		OnDinoHappy.Broadcast();
	}
	else
	{
		NeedPlayerHelp = true;

		if (CanShowNeeds)
		{
			OnShowNeed.Broadcast(CurrentNeed);
		}

		if (CurrentNeed == UPDWGameplayTagSettings::GetDirtyDino() || CurrentNeed == UPDWGameplayTagSettings::GetHurtDino())
		{
			OnActivateMinigame.Broadcast(CurrentNeed);
		}
		else if (CurrentNeed == UPDWGameplayTagSettings::GetBoredDino())
		{
			OnActivateInteraction.Broadcast(CurrentNeed);
		}
	}
}

void UPDWPaleoDinoComponent::OnInteractionSuccess()
{
	FGameplayTag CurrentNeed = GetCurrentNeed();

	if (CurrentNeed == UPDWGameplayTagSettings::GetBoredDino())
	{
		OnDinoHappy.Broadcast();
	}
}

