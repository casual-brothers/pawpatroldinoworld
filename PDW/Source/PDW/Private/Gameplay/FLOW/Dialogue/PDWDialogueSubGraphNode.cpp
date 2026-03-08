// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/FLOW/Dialogue/PDWDialogueSubGraphNode.h"
#include "Gameplay/FLOW/Dialogue/PDWDialogueFlowAsset.h"
#include "FlowSubsystem.h"
#include "Types/FlowDataPinProperties.h"
#include "Managers/QuestSubsystem.h"

UPDWDialogueSubGraphNode::UPDWDialogueSubGraphNode()
{
#if WITH_EDITOR
	AllowedAssignedAssetClasses = { UPDWDialogueFlowAsset::StaticClass() };
	Category = TEXT("PDW|Dialogue");
#endif
}

void UPDWDialogueSubGraphNode::InitializeInstance()
{
	InitDialogueSystem();
	Super::InitializeInstance();
}

void UPDWDialogueSubGraphNode::ExecuteInput(const FName& PinName)
{
	if (CanBeAssetInstanced() == false)
	{
		if (Asset.IsNull())
		{
			LogError(TEXT("Missing Flow Asset"));
		}
		else
		{
			LogError(FString::Printf(TEXT("Asset %s cannot be instance, probably is the same as the asset owning this SubGraph node."), *Asset.ToString()));
		}
		
		Finish();
		return;
	}
}

void UPDWDialogueSubGraphNode::InitDialogueSystem()
{
	if (GetFlowSubsystem())
	{
		UFlowAsset* DialogueAsset = GetFlowSubsystem()->CreateSubFlow(this);
		UPDWDialogueFlowAsset* DialogueSubgraph = Cast<UPDWDialogueFlowAsset>(DialogueAsset);
		if (DialogueSubgraph)
		{
			UQuestSubsystem::Get(this)->SetCurrentQuestDialogueFlow(DialogueSubgraph);
		}
	}
}

