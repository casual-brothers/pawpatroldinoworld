// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWWaitForSpecifcPup.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Data/PDWPlayerState.h"
#include "Managers/PDWEventSubsytem.h"

const FName UPDWWaitForSpecifcPup::SUCCES = FName("Success");
const FName UPDWWaitForSpecifcPup::CLOSEMENUWRONGPUP = FName("OnCloseMenuWrongPup");
const FName UPDWWaitForSpecifcPup::CLOSEMENUWRONGONVEHICLE = FName("OnCloseMenuRightPupWrongOnVehicle");

UPDWWaitForSpecifcPup::UPDWWaitForSpecifcPup(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::Condition;
#endif

	//InputPins = {FFlowPin(TEXT("Start")), FFlowPin(TEXT("Stop"))};
	OutputPins = {FFlowPin(SUCCES), FFlowPin(CLOSEMENUWRONGPUP), /*FFlowPin(CLOSEMENUWRONGONVEHICLE)*/};
}

void UPDWWaitForSpecifcPup::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	ExecuteNode();
}

void UPDWWaitForSpecifcPup::CheckPupConfig()
{
	APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	if (P1->GetPDWPlayerState()->GetCurrentPup() == PupTag && bOnVehicle == P1->GetPDWPlayerState()->GetIsOnVehicle())
	{
		TriggerFirstOutput(true);
	}
}

void UPDWWaitForSpecifcPup::OnChangeCharacter(const FGameplayTag& NewPup, APDWPlayerController* inController)
{
	CheckPupConfig();
}

void UPDWWaitForSpecifcPup::OnSwapOnVehicle(const bool IsOnVehicle, APDWPlayerController* inController)
{
	CheckPupConfig();
}

void UPDWWaitForSpecifcPup::TriggerOutput(FName PinName, const bool bFinish /*= false*/, const EFlowPinActivationType ActivationType /*= EFlowPinActivationType::Default*/)
{
	APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	P1->OnChangeCharacter.RemoveDynamic(this, &ThisClass::OnChangeCharacter);
	P1->OnSwapVehicleCompleted.RemoveDynamic(this, &ThisClass::OnSwapOnVehicle);
	UPDWEventSubsytem::Get(this)->OnSwapCharacterMenuClosed.RemoveDynamic(this, &ThisClass::OnSwapCharacterMenuClosed);
	Super::TriggerOutput(PinName, bFinish, ActivationType);
}

void UPDWWaitForSpecifcPup::OnSwapCharacterMenuClosed()
{
	APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	if (P1->GetPDWPlayerState()->GetCurrentPup() == PupTag)
	{
		if (P1->GetIsOnVehicle() != bOnVehicle)
		{
			//TriggerOutput(CLOSEMENUWRONGONVEHICLE);
		}
	}
	else
	{
		TriggerOutput(CLOSEMENUWRONGPUP);
	}
}

void UPDWWaitForSpecifcPup::ExecuteNode()
{
	CheckPupConfig();
	
	APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	P1->OnChangeCharacter.AddUniqueDynamic(this, &ThisClass::OnChangeCharacter);
	P1->OnSwapVehicleCompleted.AddUniqueDynamic(this, &ThisClass::OnSwapOnVehicle);
	UPDWEventSubsytem::Get(this)->OnSwapCharacterMenuClosed.AddUniqueDynamic(this, &ThisClass::OnSwapCharacterMenuClosed);
}

void UPDWWaitForSpecifcPup::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	ExecuteNode();
}
