// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Actors/PDWDinoNestHandler.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Managers/PDWEventSubsytem.h"
#include "FlowComponent.h"
#include "Gameplay/Actors/TagMeshComponent.h"

// Sets default values
UPDWDinoNestHandler::UPDWDinoNestHandler()
{
}

void UPDWDinoNestHandler::UpdateEggs()
{
	TMap<FGameplayTag,int32> CurrentCollectedEgss = UPDWDataFunctionLibrary::GetItemsByFilter(this,EggTag);
	TArray<FGameplayTag> EggsArray = {};
	CurrentCollectedEgss.GenerateKeyArray(EggsArray);
	GetOwner()->GetComponents(UTagMeshComponent::StaticClass(), EggsComp);

	for (FGameplayTag Tag : EggsArray)
	{
		OnEggCollected(Tag);
	}
}

void UPDWDinoNestHandler::HideEggs()
{
	for(UActorComponent* EggComp : EggsComp)
	{
		UTagMeshComponent* MeshComp = Cast<UTagMeshComponent>(EggComp);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetHiddenInGame(true);	
	}
}

void UPDWDinoNestHandler::OnEggCollected(FGameplayTag EggCollected)
{	
	for(UActorComponent* EggComp : EggsComp)
	{
		UTagMeshComponent* MeshComp = Cast<UTagMeshComponent>(EggComp);

		if (MeshComp->MeshTag.MatchesTag(EggCollected))
		{
			CollectedEgss++;
			MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			MeshComp->SetHiddenInGame(false);
			OnNestEggCollected.Broadcast(CollectedEgss);

			if (CollectedEgss >= EggsToCollect)
			{
				OnNestFull.Broadcast(GetOwner());
			}	
		}
	}
}

