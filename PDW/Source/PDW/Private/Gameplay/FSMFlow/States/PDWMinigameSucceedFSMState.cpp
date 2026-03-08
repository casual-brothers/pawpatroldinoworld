// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FSMFlow/States/PDWMinigameSucceedFSMState.h"
#include "Data/FlowDeveloperSettings.h"
#include "Managers/PDWEventSubsytem.h"
#include "Data/PDWGameSettings.h"
#include "UI/Pages/PDWQuestMessagePage.h"
#include "Managers/QuestSubsystem.h"
#include "Gameplay/FLOW/BaseFlowQuest.h"
#include "Data/PDWGameplayStructures.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"

void UPDWMinigameSucceedFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode && GameMode->CurrentMinigame && !GameMode->CurrentMinigame->bShowSuccessMessage)
	{
		UNebulaFlowCoreFunctionLibrary::TriggerAction(GetWorld(), UPDWGameSettings::GetActionSuccedAnimationCompleted().ToString(), "", nullptr);
		UNebulaFlowCoreFunctionLibrary::TriggerAction(GetWorld(), UFlowDeveloperSettings::GetMinigameCompletedTag().ToString(), "", nullptr);
	}
	else if (PageRef)
	{
		UPDWQuestMessagePage* MessagePage = Cast<UPDWQuestMessagePage>(PageRef);
		if (MessagePage)
		{
			FPDWQuestMessageData NewMessage;
			if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
			{
				if (QuestSubsystem)
				{
					if (UBaseFlowQuest* CurrentQuest = QuestSubsystem->GetCurrentQuest())
					{
						 NewMessage.QuestName = CurrentQuest->GetQuestName();
					}
				}
			}
			NewMessage.QuestMessageType = MessageType;
			MessagePage->InitQuestMessage(NewMessage);
		}
	}
}

void UPDWMinigameSucceedFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	if (Action == UPDWGameSettings::GetActionSuccedAnimationCompleted())
	{
		FInteractionEventSignature EventSignature = FInteractionEventSignature();
		EventSignature.EventTag = UFlowDeveloperSettings::GetMinigameSucceedTag();
		UPDWEventSubsytem::Get(this)->TriggerInteractionEvent(UFlowDeveloperSettings::GetMinigameSucceedTag(), EventSignature);
	}
	if (Action == UFlowDeveloperSettings::GetMinigameCompletedTag().ToString())
	{
		TriggerTransition(UFlowDeveloperSettings::GetMinigameCompletedTag().GetTagName(), "");
	}
}