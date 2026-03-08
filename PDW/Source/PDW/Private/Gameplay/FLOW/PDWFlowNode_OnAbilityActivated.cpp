// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_OnAbilityActivated.h"
#include "GameplayEffect.h"
#include "Gameplay/Components/PDWGASComponent.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Gameplay/GAS/PDWGASAbility.h"
#include "GameplayAbilities/Public/GameplayAbilitySpec.h"
#include "Gameplay/Pawns/PDWPlayerController.h"

UPDWFlowNode_OnAbilityActivated::UPDWFlowNode_OnAbilityActivated(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Wait for event");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif

	OutputPins.Empty();
	OutputPins.Add(FFlowPin(TEXT("Start")));
	OutputPins.Add(FFlowPin(TEXT("End")));
}

void UPDWFlowNode_OnAbilityActivated::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	GasComp = PC->GetGASComponent();
	if (!IsValid(GasComp) || AbilityToCheck == nullptr)
	{
		TriggerFirstOutput(true);
		return;
	}
	OnAbilityActivatedHandle = GasComp->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::OnAbilityActivated);
	OnAbilityEndedHandle = GasComp->AbilityEndedCallbacks.AddUObject(this, &ThisClass::OnAbilityActivated);
	
}

void UPDWFlowNode_OnAbilityActivated::OnAbilityActivated(UGameplayAbility* Data)
{
	if (Data->IsA(AbilityToCheck))
	{
		GasComp->AbilityActivatedCallbacks.Remove(OnAbilityActivatedHandle);
		TriggerOutput(TEXT("Start"), false);
	}
}

void UPDWFlowNode_OnAbilityActivated::OnAbilityEnd(FAbilityEndedData Data)
{
	if (Data.AbilityThatEnded && Data.AbilityThatEnded->IsA(AbilityToCheck))
	{
		GasComp->AbilityEndedCallbacks.Remove(OnAbilityEndedHandle);
		TriggerOutput(TEXT("End"), true);
	}
}


#if WITH_EDITOR
FString UPDWFlowNode_OnAbilityActivated::GetNodeDescription() const
{
	return AbilityToCheck ? FString::Printf(TEXT("%s"), *AbilityToCheck->GetName()) : FString::Printf(TEXT(""));
}

#endif