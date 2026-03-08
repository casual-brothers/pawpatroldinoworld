// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/StateBehavior/ManageInputStateBehavior.h"
#include "Managers/PDWInputSubsystem.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Pawns/PDWPlayerController.h"

void UManageInputStateBehavior::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	if (bRemoveInput)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (APlayerController* PC = Iterator->Get())
			{
				UPDWInputSubsystem::RemoveInputs(PC);
			}
		}
	}
	else
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (APlayerController* PC = Iterator->Get())
			{
				UPDWInputSubsystem::ApplyMappingContextByTag(MapTagToAdd, PC);
			}
		}
	}
}