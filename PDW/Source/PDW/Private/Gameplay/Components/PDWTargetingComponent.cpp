// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWTargetingComponent.h"

UPDWTargetingComponent::UPDWTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick=true;
	PrimaryComponentTick.bStartWithTickEnabled=false;
}

bool UPDWTargetingComponent::GetIsTargeted() const
{
	return bIsTargeted;
}

void UPDWTargetingComponent::SetIsTargeted(const bool bTarget)
{
	bIsTargeted = bTarget;
}
