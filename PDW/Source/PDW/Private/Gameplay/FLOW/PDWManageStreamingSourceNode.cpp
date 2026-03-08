// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWManageStreamingSourceNode.h"
#include "WPHelperSubsystem.h"
#include "Actors/WPBaseStreamingSourceActor.h"

UPDWManageStreamingSourceNode::UPDWManageStreamingSourceNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
#if WITH_EDITOR
		Category = TEXT("WorldPartition");
		NodeDisplayStyle = FlowNodeStyle::Default;
#endif
}

void UPDWManageStreamingSourceNode::ExecuteInput(const FName& PinName)
{
	UWPHelperSubsystem* WPHelperSubsystem = UWPHelperSubsystem::Get(this);
	if(WPHelperSubsystem)
	{
		for (auto& [Toggle, SourceList] : StreamingSourceBehaviorMap)
		{
			for (const FName& ID : SourceList.StreamingSourceIDs)
			{
				if (Toggle == ESwitchStreamingSourceBehavior::Disable)
				{
					WPHelperSubsystem->RequestDestructionOfStreamingSource(ID);
				}
				else
				{
					WPHelperSubsystem->EnableStreamingSource(ID);
				}
			}
		}
	}

	TriggerFirstOutput(true);
}
