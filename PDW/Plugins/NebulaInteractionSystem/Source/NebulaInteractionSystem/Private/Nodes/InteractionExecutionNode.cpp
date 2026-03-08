// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/InteractionExecutionNode.h"
#include "NebulaInteractionSystem.h"
#include "Conditions/NebulaInteractionConditionCheck.h"

FName UInteractionExecutionNode::INPIN_InteractionInfoInstancedStruct;
FName UInteractionExecutionNode::OUTPIN_FailedCondition(TEXT("FailedCondition"));
FName UInteractionExecutionNode::OUTPIN_InteractionSuccess(TEXT("Success"));
FName UInteractionExecutionNode::OUTPIN_InteractionInterrupted(TEXT("Interrupted"));

UInteractionExecutionNode::UInteractionExecutionNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

#if WITH_EDITOR
	Category = TEXT("Interaction");
	NodeDisplayStyle =FlowNodeStyle::InOut;
#endif
	OutputPins.Empty();
	OutputPins.Add(FFlowPin(OUTPIN_InteractionSuccess));
	OutputPins.Add(FFlowPin(OUTPIN_InteractionInterrupted));
	OutputPins.Add(FFlowPin(OUTPIN_FailedCondition));
	
	INPIN_InteractionInfoInstancedStruct = GET_MEMBER_NAME_CHECKED(UInteractionExecutionNode, InteractionInfoPin);
}

#if WITH_EDITOR
FString UInteractionExecutionNode::GetNodeDescription() const
{
	return FString::Printf(TEXT("%s") , BehaviourToExecute ? *BehaviourToExecute->GetName() : TEXT("Invalid Interaction Behaviour"));
}
#endif
void UInteractionExecutionNode::ExecuteInput(const FName& PinName)
{

	FInstancedStruct ResolvedInteractionInfo = ResolveInstancedStruct();
	if (const FInteractionInfo* InteractionInfo = ResolvedInteractionInfo.GetPtr<FInteractionInfo>())
	{
		if (!CanBeInteracted(InteractionInfo->Interactor))
		{
			return;
		}

		if (InitializeInteractionBehaviour(*InteractionInfo))
		{
			if (EvaluateCondition(*InteractionInfo))
			{
				BehaviourToExecute->ExecuteBehaviour();
				Interactors.Add(InteractionInfo->Interactor);
				BehaviourToExecute->OnBehaviourFinished.AddUObject(this, &UInteractionExecutionNode::OnInteractionFinished);
				BehaviourToExecute->OnBehaviourInterruped.AddUObject(this, &UInteractionExecutionNode::OnInteractionInterrupted);
			}
			else
			{
				TriggerOutput(OUTPIN_FailedCondition);
			}
		}
		else
		{
			UE_LOG(NebulaInteractionLog, Warning, TEXT("Failed to initialize interaction behaviour for %s"), *InteractionInfo->Interactable->GetName());
		}
	}
	
}

void UInteractionExecutionNode::OnInteractionFinished(const FInteractionBehaviourEventInfo& inEventInfo)
{
	Unbind();
	Interactors.Remove(inEventInfo.Interactor);
	TriggerOutput(OUTPIN_InteractionSuccess);
}

void UInteractionExecutionNode::OnInteractionInterrupted(const FInteractionBehaviourInterruptInfo& inEventInfo)
{
	Unbind();
	Interactors.Empty();
	TriggerOutput(OUTPIN_InteractionInterrupted);
}

bool UInteractionExecutionNode::EvaluateCondition(const FInteractionInfo& inInteractionInfoSIgnature)
{
	for (UNebulaInteractionConditionCheck* Condition : InteractionConditions)
	{
		Condition->EvaluateCondition(inInteractionInfoSIgnature);
	}
	return true;
}

bool UInteractionExecutionNode::InitializeInteractionBehaviour(const FInteractionInfo& inInteractionInfo) const
{
	if (!BehaviourToExecute)
	{
		return false;
	}
	
	FBehaviourInfo Info;
	Info.Interactor = inInteractionInfo.Interactor;
	Info.InteractionBehaviourOwner = inInteractionInfo.Interactable;
	BehaviourToExecute->InitBehaviour(Info);
	return true;
}


bool UInteractionExecutionNode::CanBeInteracted(AActor* inInteractor)
{
	if (bAcceptMultipleConcurrentInteractor)
	{
		if (Interactors.Contains(inInteractor))
		{
			return false;
		}
	}
	else
	{
		if (Interactors.Num())
		{
			return false;
		}
	}
	return true;
}

#pragma region NODE UTILITY
void UInteractionExecutionNode::Unbind()
{
	BehaviourToExecute->OnBehaviourFinished.RemoveAll(this);
	BehaviourToExecute->OnBehaviourInterruped.RemoveAll(this);
}

void UInteractionExecutionNode::Cleanup()
{
	Unbind();
	Interactors.Empty();
	Super::Cleanup();
}

FInstancedStruct UInteractionExecutionNode::ResolveInstancedStruct() const
{
	FFlowDataPinResult_InstancedStruct InstancedStructResult = TryResolveDataPinAsInstancedStruct(INPIN_InteractionInfoInstancedStruct);

	if (InstancedStructResult.Result == EFlowDataPinResolveResult::FailedMissingPin)
	{
		InstancedStructResult.Result = EFlowDataPinResolveResult::Success;
		InstancedStructResult.Value = InteractionInfoPin.Value;
	}

	check(InstancedStructResult.Result == EFlowDataPinResolveResult::Success);

	return static_cast<FInstancedStruct>(InstancedStructResult.Value);
}
#pragma endregion