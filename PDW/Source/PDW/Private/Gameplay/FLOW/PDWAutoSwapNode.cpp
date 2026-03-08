// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWAutoSwapNode.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "PDWGameInstance.h"

UPDWAutoSwapNode::UPDWAutoSwapNode()
{
#if WITH_EDITOR
	Category = TEXT("PDW|Quest");
#endif
}

void UPDWAutoSwapNode::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	Swap(PC);
	UPDWGameInstance* GameInstance = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance->GetIsMultiPlayerOn())
	{
		PC = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
		Swap(PC);
	}

	TriggerFirstOutput(true);
}

void UPDWAutoSwapNode::Swap(APDWPlayerController* inController)
{

	if (NodeType == EPDWNodeType::Vehicle)
	{
		if (!inController->GetIsOnVehicle())
		{
			inController->ChangeVehicle();
		}
	}
	else
	{
		if (inController->GetIsOnVehicle())
		{
			inController->ChangeVehicle();
		}
	}

}
