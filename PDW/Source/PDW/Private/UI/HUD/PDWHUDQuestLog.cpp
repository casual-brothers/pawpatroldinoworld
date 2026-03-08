// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/PDWHUDQuestLog.h"
#include "Managers/QuestSubsystem.h"

void UPDWHUDQuestLog::InitQuestLog()
{
	UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
	if (QuestSubsystem)
	{
		QuestSubsystem->OnQuestDescriptionUpdated.AddUniqueDynamic(this, &ThisClass::OnQuestDescriptionUpdate);
		QuestSubsystem->OnQuestStepStartSignature.AddUniqueDynamic(this, &ThisClass::OnStepStart);
		QuestSubsystem->OnQuestEnded.AddUniqueDynamic(this, &ThisClass::OnQuestEnd);
		QuestSubsystem->OnQuestStarted.AddUniqueDynamic(this, &ThisClass::OnQuestStart);
		
		if (QuestSubsystem->GetCurrentQuest())
		{
			BP_InitQuestLog(QuestSubsystem->GetCurrentQuestStepData());
		}
		else
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UPDWHUDQuestLog::OnQuestDescriptionUpdate(FPDWStepDescriptionData Description)
{
	BP_OnQuestDescriptionUpdate(Description);
}

void UPDWHUDQuestLog::OnStepStart(FPDWQuestStepData InitStepData)
{
	InitQuestLog();
}

void UPDWHUDQuestLog::OnQuestEnd(UBaseFlowQuest* Quest)
{
	BP_OnQuestEnd();
}

void UPDWHUDQuestLog::OnQuestStart(UBaseFlowQuest* Quest)
{
	BP_OnQuestStart();
}

void UPDWHUDQuestLog::NativeDestruct()
{
	UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
	if (QuestSubsystem)
	{
		QuestSubsystem->OnQuestDescriptionUpdated.RemoveDynamic(this, &ThisClass::OnQuestDescriptionUpdate);
		QuestSubsystem->OnQuestStepStartSignature.RemoveDynamic(this, &ThisClass::OnStepStart);
		QuestSubsystem->OnQuestEnded.RemoveDynamic(this, &ThisClass::OnQuestEnd);
		QuestSubsystem->OnQuestStarted.RemoveDynamic(this, &ThisClass::OnQuestStart);
	}
	Super::NativeDestruct();
}