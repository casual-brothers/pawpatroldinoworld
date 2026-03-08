// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_GameplayAbility.h"
#include "GameplayEffect.h"
#include "Gameplay/Components/PDWGASComponent.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Gameplay/GAS/PDWGASAbility.h"
#include "GameplayAbilities/Public/GameplayAbilitySpec.h"
#include "Gameplay/Pawns/PDWPlayerController.h"

UPDWFlowNode_GameplayAbility::UPDWFlowNode_GameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Gameplay");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}

void UPDWFlowNode_GameplayAbility::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	UPDWGASComponent* GasComp = PC->GetGASComponent();
	if (!IsValid(GasComp)) return;
	if (AbilityRef)
	{
		if (bRemoveAbility)
		{

		}
		else
		{
			FGameplayAbilitySpec Spec = FGameplayAbilitySpec(AbilityRef, 1, -1);
			if (bGrantAndActivate)
				GasComp->GiveAbilityAndActivateOnce(Spec);
			else
				GasComp->GiveAbility(Spec);
		}
	}
	TriggerFirstOutput(true);
}

#if WITH_EDITOR
FString UPDWFlowNode_GameplayAbility::GetNodeDescription() const
{
	if (bRemoveAbility)
	{
		return AbilityRef ? FString::Printf(TEXT("Remove %s"), *AbilityRef->GetName()) : FString::Printf(TEXT(""));
	}
	FString String1 = AbilityRef ? FString::Printf(TEXT("%s"), *AbilityRef->GetName()) : FString::Printf(TEXT(""));
	return FString::Printf(TEXT("%s %s"), bGrantAndActivate ? TEXT("Activate") : TEXT("Grant"), *String1);
}

#endif