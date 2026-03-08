// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_Quest.h"
#include "Gameplay/FLOW/BaseFlowQuest.h"
#include "Managers/QuestSubsystem.h"
#include "FlowSubsystem.h"

UPDWFlowNode_Quest::UPDWFlowNode_Quest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	AllowedAssignedAssetClasses = { UBaseFlowQuest::StaticClass() };
#endif
}

void UPDWFlowNode_Quest::ExecuteInput(const FName& PinName)
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

	if (PinName == TEXT("Start"))
	{
		StartQuest();
	}
}

void UPDWFlowNode_Quest::StartQuest()
{
	if (GetFlowSubsystem())
	{
		if(!FlowQuest)
		{
			FlowQuest = Cast<UBaseFlowQuest>(GetFlowSubsystem()->CreateSubFlow(this, FString(), true));
		}

		if (FlowQuest)
		{
			//if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
			//{
			//	QuestSubsystem->QuestStart(FlowQuest);
			//}
			GetFlowSubsystem()->StartSubFlow(this);
		}
	}
}

bool UPDWFlowNode_Quest::Cheat_CreateQuest()
{
	if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		if (GetFlowSubsystem())
		{
			FlowQuest = Cast<UBaseFlowQuest>(GetFlowSubsystem()->CreateSubFlow(this, FString(), true));

			if (FlowQuest)
			{
				GetFlowSubsystem()->StartSubFlow(this,"Cheat");
				return true;
			}
		}
	}

	return false;
}

void UPDWFlowNode_Quest::OnLoad_Implementation()
{
	if (!SavedAssetInstanceName.IsEmpty() && !Asset.IsNull())
	{
		//GetFlowSubsystem()->LoadSubFlow(this, SavedAssetInstanceName);

		UFlowAsset* SubGraphAsset = Asset.LoadSynchronous();
		UFlowAsset* LoadedInstance = nullptr;
		FFlowAssetSaveData AssetRecordToUse;
		for (const FFlowAssetSaveData& AssetRecord : GetFlowSubsystem()->GetLoadedSaveGame()->FlowInstances)
		{
			if (AssetRecord.InstanceName == SavedAssetInstanceName
				&& ((SubGraphAsset && SubGraphAsset->IsBoundToWorld() == false) || AssetRecord.WorldName == GetWorld()->GetName()))
			{
				LoadedInstance = GetFlowSubsystem()->CreateSubFlow(this, SavedAssetInstanceName);
				AssetRecordToUse = AssetRecord;
				break;
			}
		}
		if (!LoadedInstance)
		{
			return;
		}
		FlowQuest = Cast<UBaseFlowQuest>(LoadedInstance);
		if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
		{
			QuestSubsystem->QuestStart(FlowQuest, true);
			LoadedInstance->LoadInstance(AssetRecordToUse);
			QuestSubsystem->LoadStep();
		}
	}
}

#if WITH_EDITOR

FString UPDWFlowNode_Quest::GetNodeDescription() const
{
	return Super::GetNodeDescription();
}

#endif