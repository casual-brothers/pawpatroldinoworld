// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/Dialogue/PDWFlowNode_CompleteDialogue.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Managers/QuestSubsystem.h"
#include "Data/FlowDeveloperSettings.h"
#include "Gameplay/FLOW/Dialogue/PDWDialogueFlowAsset.h"
#include "PDWGameInstance.h"
#include "Managers/PDWEventSubsytem.h"
#include "NebulaVehicle/PDWVehiclePawn.h"

UPDWFlowNode_CompleteDialogue::UPDWFlowNode_CompleteDialogue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Dialogue");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}

void UPDWFlowNode_CompleteDialogue::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	ResetCameraAndInput();
	//UNebulaFlowCoreFunctionLibrary::TriggerAction(this,UFlowDeveloperSettings::GetSwitcherTag().ToString(),"");
	TriggerFirstOutput(true);
}

void UPDWFlowNode_CompleteDialogue::ResetCameraAndInput()
{
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	APawn* Target = nullptr;

	if (PC)
	{
		if (PC->GetIsOnVehicle())
		{
			Target = PC->GetVehicleInstance();
		}
		else
		{
			Target = PC->GetPupInstance();
		}
	}

	PC->SetViewTargetWithBlend(Target, 0.5f, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.0f);

	//UPDWGameInstance* GameInstance = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(this));
	//if (GameInstance->GetIsMultiPlayerOn())
	//{
	//	APDWPlayerController* PCTwo = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld());
	//	PCTwo->SetViewTargetWithBlend(PCTwo->GetPawn(), 0.5f, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.0f);
	//}
}
