// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_UpdateInteractions.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "FlowSubsystem.h"

UPDWFlowNode_UpdateInteractions::UPDWFlowNode_UpdateInteractions(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MatchType(EGameplayContainerMatchType::All)
	, bExactMatch(true)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Gameplay");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}


#if WITH_EDITOR
FString UPDWFlowNode_UpdateInteractions::GetNodeDescription() const
{
	return FString::Printf(TEXT("%s"), *GetIdentityTagsDescription(IdentityTags)); ;
}
#endif

void UPDWFlowNode_UpdateInteractions::ExecuteInput(const FName& PinName)
{
	//DEPRECATO
	/*if (const UFlowSubsystem* FlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
	{
		for (const TWeakObjectPtr<UFlowComponent>& Component : FlowSubsystem->GetComponents<UFlowComponent>(IdentityTags, MatchType, bExactMatch))
		{
			UPDWInteractionReceiverComponent* comp = Cast<UPDWInteractionReceiverComponent>(Component);
			if (IsValid(comp))
			{
				comp->AddInteractionTag(InteractionsToAdd);
				comp->RemoveInteractionTag(InteractionsToRemove);
			}
		}
	}*/
	TriggerFirstOutput(true);
}
