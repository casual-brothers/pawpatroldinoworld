// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Pages/PDWDialogueBasePage.h"
#include "UI/Widgets/Dialogue/PDWDialogueBaseWidget.h"
#include "Managers/PDWDialogueSubSystem.h"

void UPDWDialogueBasePage::StartConversation()
{
	DialogueWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	DialogueWidget->StartDialogue(UPDWDialogueSubSystem::Get(this)->CurrentConversation);
	DialogueWidget->OnEndDialogue.AddUniqueDynamic(this, &ThisClass::OnEndConversation);
}

void UPDWDialogueBasePage::OnSkipPressed()
{
	DialogueWidget->OnSkipPressed();
}

void UPDWDialogueBasePage::OnEndConversation()
{
	DialogueWidget->SetVisibility(ESlateVisibility::Collapsed);
}