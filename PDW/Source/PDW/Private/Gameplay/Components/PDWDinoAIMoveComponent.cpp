// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWDinoAIMoveComponent.h"
#include "Gameplay/Components/PDWPaleoDinoComponent.h"
#include "Data/PDWGameplayTagSettings.h"

bool UPDWDinoAIMoveComponent::CanMove()
{
	UPDWPaleoDinoComponent* DinoComp = Cast<UPDWPaleoDinoComponent>(OwnerPawn->GetComponentByClass(UPDWPaleoDinoComponent::StaticClass()));
	return IsPenActive && (DinoComp && DinoComp->GetCurrentNeed() == UPDWGameplayTagSettings::GetHappyDino()) && !IsDoingAMontage;
}
