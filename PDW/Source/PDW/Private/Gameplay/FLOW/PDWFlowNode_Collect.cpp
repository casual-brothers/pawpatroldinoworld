// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_Collect.h"
#include "Managers/PDWEventSubsytem.h"

UPDWFlowNode_Collect::UPDWFlowNode_Collect(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Quest");
	NodeDisplayStyle = FlowNodeStyle::Condition;
#endif

	InputPins = {FFlowPin(TEXT("Start")), FFlowPin(TEXT("Stop"))};
	OutputPins = {FFlowPin(TEXT("Success")), FFlowPin(TEXT("Completed")), FFlowPin(TEXT("Stopped")), FFlowPin(TEXT("Last"))};

	//if (InitWithInputPins)
	//{
	InputPins.Add(FFlowPin(TEXT("CollectibleTag"), EFlowPinType::GameplayTag));
	//}

}

void UPDWFlowNode_Collect::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	if (InitWithInputPins)
	{
		FFlowDataPinResult_GameplayTag PinResult = UFlowNodeBase::TryResolveDataPinAsGameplayTag("CollectibleTag");
		CollectibleTag =  PinResult.Value;
	}

	if (PinName == TEXT("Start"))
	{
		BindCollectEvent();
	}
	else if (PinName == TEXT("Stop"))
	{
		TriggerOutput(TEXT("Stopped"), true);
	}
}

#if WITH_EDITOR
FString UPDWFlowNode_Collect::GetNodeDescription() const
{
	return CollectibleTag.ToString();
}

void UPDWFlowNode_Collect::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property &&
		PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UPDWFlowNode_Collect, InitWithInputPins))
	{
		//OnReconstructionRequested.ExecuteIfBound(); TODO
	}
}

#endif

void UPDWFlowNode_Collect::OnItemCollected(FGameplayTag ItemTag)
{
	if (ItemTag.MatchesTag(CollectibleTag))
	{
		QuantityCollected++;

		if (QuantityCollected >= QuantityToCollect)
		{
			UnbindCollectEvent();
			if (bUseLastOutputPin)
			{
				TriggerOutput(TEXT("Last"), false);
			}
			else
			{
				TriggerFirstOutput(false);
			}
			TriggerOutput(TEXT("Completed"), true);
		}
		else
		{
			TriggerFirstOutput(false);
		}
	}
}

void UPDWFlowNode_Collect::BindCollectEvent()
{
	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnItemCollected.AddUniqueDynamic(this, &UPDWFlowNode_Collect::OnItemCollected);	
	}
}

void UPDWFlowNode_Collect::UnbindCollectEvent()
{
	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnItemCollected.RemoveDynamic(this, &UPDWFlowNode_Collect::OnItemCollected);	
	}
}

void UPDWFlowNode_Collect::TriggerOutput(FName PinName, const bool bFinish /*= false*/, const EFlowPinActivationType ActivationType /*= EFlowPinActivationType::Default*/)
{
	Super::TriggerOutput(PinName, bFinish, ActivationType);
}

void UPDWFlowNode_Collect::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	
	if (InitWithInputPins)
	{
		FFlowDataPinResult_GameplayTag PinResult = UFlowNodeBase::TryResolveDataPinAsGameplayTag("CollectibleTag");
		CollectibleTag =  PinResult.Value;
	}

	if (QuantityCollected >= QuantityToCollect)
	{
		TriggerOutput(TEXT("Completed"), true);
	}
	else
	{
		BindCollectEvent();
	}
}