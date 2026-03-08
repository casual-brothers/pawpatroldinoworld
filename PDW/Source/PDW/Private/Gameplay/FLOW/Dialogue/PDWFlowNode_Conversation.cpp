// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/Dialogue/PDWFlowNode_Conversation.h"
#include "Managers/PDWDialogueSubSystem.h"
#include "PDWGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "../Classes/FMODBlueprintStatics.h"

UPDWFlowNode_Conversation::UPDWFlowNode_Conversation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Dialogue");
	NodeDisplayStyle = FlowNodeStyle::Node;
#endif
}

void UPDWFlowNode_Conversation::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	TriggerConversation();
}

void UPDWFlowNode_Conversation::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	TriggerConversation();
}

void UPDWFlowNode_Conversation::TriggerConversation()
{
	UPDWDialogueSubSystem::TriggerConversation(this,Conversation);
	
	if (Conversation.ConversationType == EConversationType::HUDComment)
	{
		TriggerFirstOutput(true);
	}
	else
	{
		UPDWDialogueSubSystem::OnConversationCompleted.AddUObject(this, &ThisClass::TriggerFirstOutput, true);
	}
}

void UPDWFlowNode_Conversation::Cleanup()
{
	UPDWDialogueSubSystem::OnConversationCompleted.RemoveAll(this);
}

#if WITH_EDITOR
void UPDWFlowNode_Conversation::PlayVoiceOver()
{
	if (GEditor)
	{
		UFMODAudioComponent* AudioComponent = NewObject<UFMODAudioComponent>();
		if (IsValid(AudioComponent))
		{
			AudioComponent->Event = VoiceOverEvent;
			if (!Conversation.DialogueLines[VoiceOverIndex].VoiceOverConfig.VoiceOverID.IsEmpty())
			{
				AudioComponent->SetProgrammerSoundName(Conversation.DialogueLines[VoiceOverIndex].VoiceOverConfig.VoiceOverID);
			}
			AudioComponent->Play();
		}
	}
}
#endif