// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWTriggerTutorialNavButton.h"
#include "Managers/PDWHUDSubsystem.h"

UPDWTriggerTutorialNavButton::UPDWTriggerTutorialNavButton(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::Condition;
#endif
	ButtonData.NavButtonData.bListenAction = true;
	ConversationToPlay.ConversationType = EConversationType::HUDComment;
	ConversationToPlay.bLoopConversation = true;
}

void UPDWTriggerTutorialNavButton::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	ExecuteNode();
}

void UPDWTriggerTutorialNavButton::OnIputTrigger()
{
	UPDWHUDSubsystem::Get(this)->EndCurrentHUDDialogue();
	UPDWHUDSubsystem::Get(this)->EndCurrentTutorialInputAction();
	TriggerFirstOutput(true);
}

void UPDWTriggerTutorialNavButton::ExecuteNode()
{
	if (ButtonData.TriggerType == ETriggerInputType::OnEvent)
	{
		TriggerFirstOutput(true);
	}
	else
	{
		ButtonData.CallBack = [&](){OnIputTrigger();};
	}

	if(UPDWHUDSubsystem::Get(this)->GetCurrentHUD())
	{
		OnHUDReady();
	}
	else
	{
		UPDWHUDSubsystem::Get(this)->OnHUDReady.AddDynamic(this, &UPDWTriggerTutorialNavButton::OnHUDReady);
	}
}

void UPDWTriggerTutorialNavButton::OnHUDReady()
{
	UPDWHUDSubsystem::Get(this)->OnHUDReady.RemoveDynamic(this, &UPDWTriggerTutorialNavButton::OnHUDReady);
	UPDWHUDSubsystem::Get(this)->RequestTutorialInputAction(ButtonData);
	UPDWHUDSubsystem::Get(this)->RequestHUDDialogue(ConversationToPlay);
}

void UPDWTriggerTutorialNavButton::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	ExecuteNode();
}
