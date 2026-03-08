// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/PDWInteractionComponentWidget.h"
#include "Gameplay/Components/PDWInteractionComponent.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"

void UPDWInteractionComponentWidget::SetReceiverComponent(UPDWInteractionReceiverComponent* InOwner)
{
	Owner = InOwner;
}

void UPDWInteractionComponentWidget::SetInteracterComponent(UPDWInteractionComponent* InInteracter)
{
	Interacter = InInteracter;
}

void UPDWInteractionComponentWidget::UpdateSupportedInteraction(FGameplayTag SupportedInteractions)
{
	BP_UpdateSupportedInteraction(SupportedInteractions);
}