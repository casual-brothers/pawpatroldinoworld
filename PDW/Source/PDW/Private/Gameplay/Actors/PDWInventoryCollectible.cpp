// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWInventoryCollectible.h"
#include "FlowComponent.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"

void APDWInventoryCollectible::OnPickUpStart(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController)
{
	if (FlowComp)
	{
		UPDWDataFunctionLibrary::AddItemToInventory(this, FlowComp->IdentityTags.First(), 1);
	}

	Super::OnPickUpStart(PickUp, PlayerController);
}

void APDWInventoryCollectible::OnPickUpEnd(UPDWPickUpComponent* PickUp, APDWPlayerController* PlayerController)
{
	if (FlowComp)
	{
		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnItemCollectedEvent(FlowComp->IdentityTags.First());
		}
	}

	Super::OnPickUpEnd(PickUp, PlayerController);
}

void APDWInventoryCollectible::BeginPlay()
{
	Super::BeginPlay();
	
	FlowComp = Cast<UFlowComponent>(GetComponentByClass(UFlowComponent::StaticClass()));

	if (DestroyIfAlreadyTaken && FlowComp)
	{
		int32 QuantityInInventory = UPDWDataFunctionLibrary::GetItemQuantityFromInventory(this,FlowComp->IdentityTags.First());

		if (QuantityInInventory > 0)
		{
			Destroy();
		}
	}
}