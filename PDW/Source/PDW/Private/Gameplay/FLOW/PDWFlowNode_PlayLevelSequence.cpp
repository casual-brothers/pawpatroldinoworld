// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_PlayLevelSequence.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "Gameplay/FSMFlow/Helpers/PDWFsmHelper.h"


UPDWFlowNode_PlayLevelSequence::UPDWFlowNode_PlayLevelSequence(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
	{
	#if WITH_EDITOR
		Category = TEXT("PDW|Quest");
		NodeDisplayStyle = FlowNodeStyle::Default;
	#endif
	}


#if WITH_EDITOR
void UPDWFlowNode_PlayLevelSequence::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (CutsceneConfig.SequenceCollectionNew.Num() && !CutsceneConfig.IsCinematicAndCutscene)
	{
		for (FPDWSequenceDataElement& Sequencedata : CutsceneConfig.SequenceCollectionNew)
		{
			Sequencedata.IsCinematic = CutsceneConfig.IsCinematic;
			Sequencedata.Conversation.ConversationType = EConversationType::Cutscene;
		}
	}
}

#endif

void UPDWFlowNode_PlayLevelSequence::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	if (CutsceneConfig.CutsceneSoundtrack)
	{
		UPDWAudioManager::Get(this)->PlayCutsceneMusicEvent(CutsceneConfig.CutsceneSoundtrack,CutsceneConfig.CutsceneSoundtrackID);
	}
}

void UPDWFlowNode_PlayLevelSequence::ChangeState()
{
	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
	
	if (GM && GM->FSMHelper)
	{
		GM->FSMHelper->PendingSequence = CutsceneConfig;
	}

	UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UFlowDeveloperSettings::GetCutsceneTag().ToString(),"");
}

void UPDWFlowNode_PlayLevelSequence::TriggerOutput(FName PinName, const bool bFinish /*= false*/, const EFlowPinActivationType ActivationType /*= EFlowPinActivationType::Default*/)
{
	if (CutsceneConfig.CutsceneSoundtrack)
	{
		UPDWAudioManager::Get(this)->StopCutsceneMusicEvent(CutsceneConfig.CutsceneSoundtrackID);
	}
	Super::TriggerOutput(PinName,bFinish,ActivationType);
}
