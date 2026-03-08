// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWTriggerInteraction.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Flow/Public/FlowComponent.h"
#include "FlowSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDWTriggerInteraction)

UPDWTriggerInteraction::UPDWTriggerInteraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MatchType(EGameplayContainerMatchType::All)
	, bExactMatch(true)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Gameplay");
	NodeDisplayStyle = FlowNodeStyle::Condition;
#endif
}

void UPDWTriggerInteraction::ExecuteInput(const FName& PinName)
{
	if (IdentityTags.IsEmpty())
	{
		UPDWInteractionReceiverComponent* comp = Cast<UPDWInteractionReceiverComponent>(TryGetRootFlowObjectOwner());
		if (IsValid(comp))
		{
			comp->ExecuteInteractionBehaviour(InteractionTag,  nullptr, ShouldRemoveInteractionWhenFinished);
		}
	}
	/*else
	{
		if (const UFlowSubsystem* FlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
		{
			for (const TWeakObjectPtr<UFlowComponent>& Component : FlowSubsystem->GetComponents<UFlowComponent>(IdentityTags, MatchType, bExactMatch))
			{
				UPDWInteractionReceiverComponent* comp = Cast<UPDWInteractionReceiverComponent>(Component);
				if (IsValid(comp))
				{
					comp->ExecuteInteractionBehaviour(InteractionTag,nullptr, ShouldRemoveInteractionWhenFinished);
				}
			}
		}
	}*/


	TriggerFirstOutput(true);
}

#if WITH_EDITOR
FString UPDWTriggerInteraction::GetNodeDescription() const
{
	if (!IdentityTags.IsEmpty())
	{
		return FString::Printf(TEXT("%s %s"), *GetIdentityTagsDescription(IdentityTags), *InteractionTag.ToString());
	}
	else
	{
		return FString::Printf(TEXT("SELF %s"),*InteractionTag.ToString());
	}
}

#endif