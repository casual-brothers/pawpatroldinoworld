// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_ApplyGameplayEffect.h"
#include "GameplayEffect.h"
#include "Gameplay/Components/PDWGASComponent.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Gameplay/Pawns/PDWPlayerController.h"

UPDWFlowNode_ApplyGameplayEffect::UPDWFlowNode_ApplyGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Gameplay");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}

void UPDWFlowNode_ApplyGameplayEffect::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	UPDWGASComponent* GasComp = PC->GetGASComponent();
	if (!IsValid(GasComp)) return;
	if (EffectClassToActivate)
	{
		GasComp->ApplyGameplayEffectToSelf(EffectClassToActivate->GetDefaultObject<UGameplayEffect>(), 1.0f, GasComp->MakeEffectContext());
	}
	if (EffectClassToDeactivate)
	{
		GasComp->RemoveActiveGameplayEffectBySourceEffect(EffectClassToDeactivate, GasComp,1);
	}
	TriggerFirstOutput(true);
}

#if WITH_EDITOR
FString UPDWFlowNode_ApplyGameplayEffect::GetNodeDescription() const
{
	FString String1 = EffectClassToActivate ? FString::Printf(TEXT("Activate %s"), *EffectClassToActivate->GetName()) : FString::Printf(TEXT(""));
	FString String2 = EffectClassToDeactivate ? FString::Printf(TEXT("Deactivate %s"), *EffectClassToDeactivate->GetName()) : FString::Printf(TEXT(""));
	return FString::Printf(TEXT("%s %s"), *String1, *String2);
}

#endif