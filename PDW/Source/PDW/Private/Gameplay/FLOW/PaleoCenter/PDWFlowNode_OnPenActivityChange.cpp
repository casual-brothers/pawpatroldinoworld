// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PaleoCenter/PDWFlowNode_OnPenActivityChange.h"
#include "Managers/PDWEventSubsytem.h"

UPDWFlowNode_OnPenActivityChange::UPDWFlowNode_OnPenActivityChange(const FObjectInitializer& ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|PaleoCenter");
	NodeDisplayStyle = FlowNodeStyle::Condition;
#endif

	InputPins.Add(FFlowPin(TEXT("DinoPenTag"), EFlowPinType::GameplayTag));
}

void UPDWFlowNode_OnPenActivityChange::ExecuteInput(const FName& PinName)
{
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		EventSubsystem->OnDinoPenActivityChange.AddUniqueDynamic(this, &UPDWFlowNode_OnPenActivityChange::OnDinoPenActivityChange);
	}
}

void UPDWFlowNode_OnPenActivityChange::OnDinoPenActivityChange(const FGameplayTag PenTag, bool IsActive)
{
	FGameplayTag PenTagToCheck = UFlowNodeBase::TryResolveDataPinAsGameplayTag("DinoPenTag").Value;

	if (PenTagToCheck == PenTag && IsActive == WaitIsActive)
	{
		UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
		if (EventSubsystem)
		{
			EventSubsystem->OnDinoPenActivityChange.RemoveDynamic(this, &UPDWFlowNode_OnPenActivityChange::OnDinoPenActivityChange);
		}

		TriggerFirstOutput(true);
	}
}

void UPDWFlowNode_OnPenActivityChange::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();

	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		EventSubsystem->OnDinoPenActivityChange.AddUniqueDynamic(this, &UPDWFlowNode_OnPenActivityChange::OnDinoPenActivityChange);
	}
}
