// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWTagComponent.h"

// Sets default values for this component's properties
UPDWTagComponent::UPDWTagComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


void UPDWTagComponent::AddTag(const FGameplayTag& NewTag)
{
	TagContainer.AddTag(NewTag);
	OnPDWTagAdded.Broadcast(TagContainer,NewTag);
}

void UPDWTagComponent::RemoveTag(const FGameplayTag& TagToRemove)
{
	TagContainer.RemoveTag(TagToRemove);
	OnPDWTagRemoved.Broadcast(TagContainer,TagToRemove);
}

bool UPDWTagComponent::HasTag(const FGameplayTag& TagToCheck) const
{
	 return TagContainer.HasTag(TagToCheck);
}

// Called when the game starts
void UPDWTagComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

