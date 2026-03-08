// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWInteractionOutlineComponent.h"
#include "Data/PDWGameSettings.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Actors/NebulaGraphicsSkyActor.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

#define OUTLINE_DISABLED 0
#define OUTLINE_ENABLED 3
#define OUTLINE_RIGHT_PUP 1


UPDWInteractionOutlineComponent::UPDWInteractionOutlineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionProfileName(UPDWGameSettings::GetOutlineCollisionProfileName().Name);
}

void UPDWInteractionOutlineComponent::BeginPlay()
{
	OnComponentBeginOverlap.AddUniqueDynamic(this, &UPDWInteractionOutlineComponent::BeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UPDWInteractionOutlineComponent::EndOverlap);
	InteractionReceiver = GetOwner()->FindComponentByClass<UPDWInteractionReceiverComponent>();
	if (InteractionReceiver.Pin())
	{
		InteractionReceiver->OnStatusTagAdded.AddUniqueDynamic(this,&UPDWInteractionOutlineComponent::OnInteractionReceiverChangeState);
	}
	for (const auto& [ComponentTag,MaterialStruct] : OutlineConfiguration.OutlineMaterialConfig)
	{
		TArray<UActorComponent*> InteractableMeshes = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(),ComponentTag.GetTagName());
		for(UActorComponent* ActorComponent : InteractableMeshes)
		{
			if (UMeshComponent* InteractableMesh = Cast<UMeshComponent>(ActorComponent))
			{
				UMaterialInterface* OverlayMaterial = InteractableMesh->GetOverlayMaterial();
				if (OverlayMaterial)
				{
					OverlayMaterialsMap.Add(InteractableMesh,OverlayMaterial);
					InteractableMesh->SetOverlayMaterial(nullptr);
				}
				else
				{
					const int32 MaterialIndex = InteractableMesh->GetMaterialIndex(MaterialStruct.MaterialSlotName);
					if (MaterialIndex != INDEX_NONE)
					{
						UMaterialInstanceDynamic* MID = InteractableMesh->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndex, InteractableMesh->GetMaterial(MaterialIndex));
						OverlayMIDMap.Add(InteractableMesh,MID);
					}
				}
				InteractableMesh->SetCustomDepthStencilValue(OUTLINE_DISABLED);
			}
		}
	}
	Super::BeginPlay();
}

void UPDWInteractionOutlineComponent::EndPlay(EEndPlayReason::Type Reason)
{
	if (InteractionReceiver.Pin())
	{
		InteractionReceiver->OnStatusTagAdded.RemoveDynamic(this,&UPDWInteractionOutlineComponent::OnInteractionReceiverChangeState);
		InteractionReceiver.Reset();
	}
	OnComponentBeginOverlap.RemoveDynamic(this, &UPDWInteractionOutlineComponent::BeginOverlap);
	OnComponentEndOverlap.RemoveDynamic(this, &UPDWInteractionOutlineComponent::EndOverlap);
	Super::EndPlay(Reason);
}

void UPDWInteractionOutlineComponent::BeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult)
{
	if (!InteractionReceiver.Pin())
	{
		InteractionReceiver = GetOwner()->FindComponentByClass<UPDWInteractionReceiverComponent>();
	}
	ensureMsgf(InteractionReceiver.Get(), TEXT("UPDWInteractionOutlineComponent::BeginOverlap - InteractionReceiver is null for %s"), *GetOwner()->GetName());
	
	if (IPDWPlayerInterface* Player = Cast<IPDWPlayerInterface>(InOtherActor))
	{
		if (Player->GetPDWPlayerController())
		{
			Player->GetPDWPlayerController()->OnChangeCharacter.AddUniqueDynamic(this,&UPDWInteractionOutlineComponent::OnPupSwap);
			EvaluateCurrentPupSkills(Player);
		}
	}
}

void UPDWInteractionOutlineComponent::EndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex)
{
	if (IPDWPlayerInterface* Player = Cast<IPDWPlayerInterface>(InOtherActor))
	{
		if(!Player->GetPDWPlayerController())
			return;
		Player->GetPDWPlayerController()->OnChangeCharacter.RemoveDynamic(this,&UPDWInteractionOutlineComponent::OnPupSwap);
		for (auto& [MeshComponent, MaterialInterface] : OverlayMaterialsMap)
		{
			MeshComponent->SetCustomDepthStencilValue(OUTLINE_DISABLED);
			MeshComponent->SetOverlayMaterial(nullptr);
		}
		for (auto& [MeshComponent, MID] : OverlayMIDMap)
		{
			for(auto& [Tag, MaterialStruct] : OutlineConfiguration.OutlineMaterialConfig)
			{
				if (MeshComponent->ComponentTags.Contains(Tag.GetTagName()))
				{
					MID->SetScalarParameterValueByInfo(MaterialStruct.MaterialInfo, MaterialStruct.ScanDisabledMaterialValue);
					MeshComponent->SetCustomDepthStencilValue(OUTLINE_DISABLED);
				}
			}
		}
	}
}

void UPDWInteractionOutlineComponent::OnPupSwap(const FGameplayTag& inNewPup,APDWPlayerController* inController)
{
	ForceEvaluation();
}

void UPDWInteractionOutlineComponent::OnInteractionReceiverChangeState(UPDWInteractionReceiverComponent* inComponent, const FGameplayTag& ChangedTag, const FGameplayTag& inNewTag)
{
	ForceEvaluation(inNewTag);
}

void UPDWInteractionOutlineComponent::EvaluateCurrentPupSkills(IPDWPlayerInterface* inPlayer, FGameplayTag inTag/* = FGameplayTag::EmptyTag*/)
{
	if (!InteractionReceiver.Pin())
	{
		InteractionReceiver = GetOwner()->FindComponentByClass<UPDWInteractionReceiverComponent>();
	}

	if (InteractionReceiver.Pin())
	{
		const FGameplayTagContainer SkillTags = inPlayer->GetInteractionComponent()->GetSkillTypeInteractions();
		const FGameplayTagContainer CurrentStateAllowedInteractions = inTag.IsValid() ?  InteractionReceiver.Get()->GetAllowedInteractionBasedOnState(inTag) : InteractionReceiver.Get()->GetAllowedInteractionInCurrentState();
	
		for (auto& [MeshComponent, MaterialInterface] : OverlayMaterialsMap)
		{
			if (CurrentStateAllowedInteractions.HasAny(SkillTags))
			{
				MeshComponent->SetOverlayMaterial(MaterialInterface);
			}
			else
			{
				MeshComponent->SetOverlayMaterial(nullptr);
			}
			const bool bInteractableUsed = UPDWDataFunctionLibrary::IsInteractableUsed(this,InteractionReceiver->GetInteractableID());
			if (bInteractableUsed)
			{
				MeshComponent->SetCustomDepthStencilValue(OUTLINE_ENABLED);
			}
			else
			{
				MeshComponent->SetCustomDepthStencilValue(OUTLINE_RIGHT_PUP);
			}
		}

		for (auto& [MeshComponent, MID] : OverlayMIDMap)
		{
			for(auto& [Tag, MaterialStruct] : OutlineConfiguration.OutlineMaterialConfig)
			{
				if (MeshComponent->ComponentTags.Contains(Tag.GetTagName()))
				{
					if (CurrentStateAllowedInteractions.HasAny(SkillTags))
					{
						MID->SetScalarParameterValueByInfo(MaterialStruct.MaterialInfo, MaterialStruct.ScanEnabledMaterialValue);
					}
					else
					{
						MID->SetScalarParameterValueByInfo(MaterialStruct.MaterialInfo, MaterialStruct.ScanDisabledMaterialValue);
					}
				}
			}
			const bool bInteractableUsed = UPDWDataFunctionLibrary::IsInteractableUsed(this,InteractionReceiver->GetInteractableID());
			if (bInteractableUsed)
			{
				MeshComponent->SetCustomDepthStencilValue(OUTLINE_ENABLED);
				
			}
			else
			{
				MeshComponent->SetCustomDepthStencilValue(OUTLINE_RIGHT_PUP);
			}
			
			if (CurrentStateAllowedInteractions.IsEmpty())
			{
				MeshComponent->SetCustomDepthStencilValue(OUTLINE_DISABLED);
			}
		}
	}
}

void UPDWInteractionOutlineComponent::ForceEvaluation(FGameplayTag inTag/* = FGameplayTag::EmptyTag*/)
{
	TSet<AActor*> CurrentOverlappingActors;
	GetOverlappingActors(CurrentOverlappingActors);
	if (CurrentOverlappingActors.Num())
	{
		for (AActor* OverlappingActor : CurrentOverlappingActors)
		{
			if (IPDWPlayerInterface* Player = Cast<IPDWPlayerInterface>(OverlappingActor))
			{
				EvaluateCurrentPupSkills(Player,inTag);
			}
		}
	}
}
