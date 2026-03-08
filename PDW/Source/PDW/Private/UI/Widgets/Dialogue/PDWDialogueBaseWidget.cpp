// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/Dialogue/PDWDialogueBaseWidget.h"
#include "UI/Widgets/Dialogue/PDWAnswerButton.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "UI/Widgets/NebulaFlowNavButtonBase.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Data/PDWGameSettings.h"
#include "Managers/PDWUIManager.h"
#include "Managers/PDWDialogueSubSystem.h"
#include "Managers/PDWAudioManager.h"
#include "FunctionLibraries/PDWUIFunctionLibrary.h"



void UPDWDialogueBaseWidget::StartDialogue(FConversation Conversation)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (Conversation.DialogueLines.Num() > 0 && Conversation.DialogueLines.IsValidIndex(CurrentSentenceIndex))
	{
		bIsDialogueEnd = false;
		//const FCharacterInfo& ChInfo = UPDWGameSettings::GetCharacterConfig(Conversation.DialogueLines[CurrentSentenceIndex].SpeakerName);
		const FCharacterInfo& ChInfo = UPDWUIFunctionLibrary::GetCharacterInfoByName(this, Conversation.DialogueLines[CurrentSentenceIndex].SpeakerID);
		bool IsVoiceOverPlaying = false;

		if (!Conversation.DialogueLines[CurrentSentenceIndex].VoiceOverConfig.VoiceOverID.IsEmpty())
		{
			TFunction<void()> CallBack = [=, this]() {VoiceOverEnd();};
			IsVoiceOverPlaying = UPDWAudioManager::PlayVoiceOver(GetWorld(), Conversation.DialogueLines[CurrentSentenceIndex].VoiceOverConfig, CallBack);
		}
		if ((bAutoSkipDialogue && (!IsVoiceOverPlaying)))
		{
			// multiple skips pressed bug, clear fist any consecutive timers
			GetWorld()->GetTimerManager().ClearTimer(AutoSkipTimerHandler);
			GetWorld()->GetTimerManager().SetTimer(AutoSkipTimerHandler, this, &UPDWDialogueBaseWidget::OnSkipPressed, AutoSkipTime, false);
		}

		if (SpeakerNameBlock)
		{
			if (Conversation.DialogueLines[CurrentSentenceIndex].OveridedDisplaySpeakerName.IsEmpty())
			{
				SpeakerNameBlock->SetText(ChInfo.DisplayCharacterName);
			}
			else
			{
				SpeakerNameBlock->SetText(Conversation.DialogueLines[CurrentSentenceIndex].OveridedDisplaySpeakerName);
			}
		}

		if (SpeakerImage && ChInfo.CharacterImage.ToSoftObjectPath().IsValid())
		{
			SpeakerImage->SetBrushFromSoftTexture(ChInfo.CharacterImage);
		}
		
		if (Conversation.DialogueLines[CurrentSentenceIndex].bIsLastSentence)
		{
			if(ArrowContainer) ArrowContainer->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			if(ArrowContainer) ArrowContainer->SetVisibility(ESlateVisibility::HitTestInvisible);
			if(ArrowAnim) PlayAnimation(ArrowAnim, 0, 0);
		}
		
		if(AnswersTab) AnswersTab->ClearChildren();
		ActualDialogue = Conversation;
		FullMessage = Conversation.DialogueLines[CurrentSentenceIndex].DialogueLine.ToString();
		MessageBlock->SetText(FText::FromString(""));
		bStartPrintSentece = true;

		if (Conversation.ConversationType != EConversationType::HUDComment)
		{
			UPDWDialogueSubSystem::TriggerDialogueLineAnimations(this,Conversation.DialogueLines[CurrentSentenceIndex]);
		}

		if (CurrentSentenceIndex == 0)
		{
			if (OpenPage)
			{
				UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), DialogueOpenUISoundID);
				PlayAnimation(OpenPage);
			}
		}
		else
		{
			if (StartDialogueAnim)
			{
				PlayAnimation(StartDialogueAnim);
			}
		}

		if (bShouldMakeAnimationTalking)
		{
			StartTalkAnimation(true);
		}
		ChangeAnimationState(false);
	}
	else
	{
		//EndDialogue();
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

/*TArray<UKS2BaseAnimInstance*>*/void UPDWDialogueBaseWidget::GetAnimInstancesByID(const FName ChID)
{
//	TArray<UKS2BaseAnimInstance*> Result {};
//
//	if (ChID == "Ocean")
//	{
//		AKS2Player* Player = UKS2GameplayFunctionLibrary::GetKS2PlayerCharacter(this);
//		if (Player)
//		{
//			UKS2BaseAnimInstance* PlayerAnimInstance = Cast<UKS2BaseAnimInstance>(Player->GetMesh()->GetAnimInstance());
//			if (PlayerAnimInstance)
//			{
//				Result.Add(PlayerAnimInstance);
//			}
//		}
//	}
//	else
//	{
//		const FCharacterInfo& ChInfo = UKS2UIFunctionLibrary::GetCharacterInfoByName(this, ChID);
//		for (FName ChId : ChInfo.CharacterObjectsIdRef)
//		{
//			const FObjectStateWorldRef& ObjectState = UKS2GameplayFunctionLibrary::FindObjectState(ChId, this);
//			if (ObjectState.SpawnedObject)
//			{
//				USkeletalMeshComponent* Skeletal = ObjectState.SpawnedObject->GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
//				if (Skeletal && Skeletal->GetAnimInstance())
//				{
//					UKS2BaseAnimInstance* BaseAnimInstance = Cast<UKS2BaseAnimInstance>(Skeletal->GetAnimInstance());
//					if (BaseAnimInstance)
//					{
//						Result.Add(BaseAnimInstance);
//					}
//				}
//			}
//		}
//	}
//
//	return Result;
}

void UPDWDialogueBaseWidget::StartTalkAnimation(bool Talk)
{
	//if (ActualDialogue.DialogueLines.IsValidIndex(CurrentSentenceIndex))
	//{
	//	FName ChIdName = ActualDialogue.DialogueLines[CurrentSentenceIndex].SpeakerName;
	//	TArray<UKS2BaseAnimInstance*> SpeakersAnimInstance = GetAnimInstancesByID(ChIdName);
	//	for (UKS2BaseAnimInstance* Anim : SpeakersAnimInstance)
	//	{
	//		Anim->SetTalking(Talk);
	//		bIsTalking = Talk;
	//	}
	//}
}

void UPDWDialogueBaseWidget::ChangeAnimationState(bool ToDefault)
{
	//if (ActualDialogue.DialogueLines.IsValidIndex(CurrentSentenceIndex) && ActualDialogue.DialogueLines[CurrentSentenceIndex].SpeakersAnimations.Num() > 0)
	//{
	//	for (TPair<FName, EIdleMode> Animation : ActualDialogue.DialogueLines[CurrentSentenceIndex].SpeakersAnimations)
	//	{
	//		TArray<UKS2BaseAnimInstance*> SpeakersAnimInstance = GetAnimInstancesByID(Animation.Key);
	//		for (UKS2BaseAnimInstance* AnimInstance : SpeakersAnimInstance)
	//		{
	//			AnimInstance->SetIdleMode(ToDefault? EIdleMode::None : Animation.Value);
	//		}
	//	}
	//}
}

void UPDWDialogueBaseWidget::OnSkipPressed()
{
	// do not clear timer here, clear when call new dialog line if necessary
	// 	if (AutoSkipTimerHandler.IsValid())
	// 	{
	// 		GetWorld()->GetTimerManager().ClearTimer(AutoSkipTimerHandler);
	// 	}

	if (bIsDialogueEnd || (OpenPage && IsAnimationPlaying(OpenPage)) || (ClosePage && IsAnimationPlaying(ClosePage)))
	{
		return;
	}

	if (bAutoSkipDialogue || CurrentCharIndex == FullMessage.Len())
	{
		NextLine();
	}
	else
	{
		CurrentCharIndex = FullMessage.Len();
		UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), DialogueSkipUISoundID);
	}
}

void UPDWDialogueBaseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (OpenPage && IsAnimationPlaying(OpenPage))
	{
		return;
	}

	if (bStartPrintSentece)
	{
		NextCharTimer += InDeltaTime;

		if (NextCharTimer >= NextCharDelay)
		{
			if (NextCharDelay <= 0.f || CurrentCharIndex == FullMessage.Len())
			{
				bStartPrintSentece = false;
				CurrentMessage = "";
				NextCharTimer = 0.f;
				CurrentCharIndex = FullMessage.Len();

				MessageBlock->SetText(FText::FromString(FullMessage));
				//if (ActualDialogue.DialogueLines[CurrentSentenceIndex].bIsDialogueAnswer && ActualDialogue.DialogueLines[CurrentSentenceIndex].Answers.Num() > 0)
				//{
				//	ShowAnswerOptions(ActualDialogue.DialogueLines[CurrentSentenceIndex].Answers);
				//}
				OnMessageCompleted.Broadcast();
			}
			else
			{
				NextCharTimer = 0.f;
				CurrentMessage.AppendChar(FullMessage[CurrentCharIndex]);
				MessageBlock->SetText(FText::FromString(CurrentMessage));
				CurrentCharIndex++;
			}
		}
	}

	if (bStartCountForLoop)
	{
		CurrentLoopTime += InDeltaTime;
		if (CurrentLoopTime >= ActualDialogue.LoopTime)
		{
			CurrentLoopTime = 0.f;
			bStartCountForLoop = false;
			CurrentSentenceIndex = 0;
			StartDialogue(ActualDialogue);
		}
	}
}

void UPDWDialogueBaseWidget::ManageOpenPageAnimationFinished()
{

}

void UPDWDialogueBaseWidget::EndDialogue()
{
	bIsDialogueEnd = true;
	if (bShouldMakeAnimationTalking)
	{
		StartTalkAnimation(false);
	}
	ChangeAnimationState(true);

	CurrentCharIndex = 0;
	CurrentSentenceIndex = 0;
	if (ClosePage)
	{
		StopAllAnimations();
		PlayAnimation(ClosePage);
	}
	else
	{
		ManageClosePageAnimationFinished();
	}
	if (!ActualDialogue.DialogueLines.IsEmpty() && ActualDialogue.DialogueLines.IsValidIndex(CurrentSentenceIndex))
	{
		UPDWAudioManager::StopVoiceOver(GetWorld(), ActualDialogue.DialogueLines[CurrentSentenceIndex].VoiceOverConfig.VoiceOverID);
	}
}

UNebulaFlowBaseButton* UPDWDialogueBaseWidget::GetFirstButton()
{
	if (AnswersTab && AnswersTab->GetAllChildren().Num() > 0)
	{
		return Cast<UPDWAnswerButton>(AnswersTab->GetChildAt(0));
	}
	return nullptr;
}

bool UPDWDialogueBaseWidget::IsAnswerShowing()
{
	return AnswersTab->GetChildrenCount() > 0;
}

void UPDWDialogueBaseWidget::Init()
{
	if(AnswersTab) AnswersTab->ClearChildren();

	if (OpenPage)
	{
		OpenPageFinished.BindUFunction(this, FName("ManageOpenPageAnimationFinished"));
		BindToAnimationFinished(OpenPage, OpenPageFinished);
	}

	if (ClosePage)
	{
		ClosePageFinished.BindUFunction(this, FName("ManageClosePageAnimationFinished"));
		BindToAnimationFinished(ClosePage, ClosePageFinished);
	}

	CurrentSentenceIndex = 0;
	CurrentLoopTime = 0;
	bStartCountForLoop = false;
}

void UPDWDialogueBaseWidget::UnInit()
{
	bIsDialogueEnd = true;
	if (AutoSkipTimerHandler.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoSkipTimerHandler);
	}
	if (!ActualDialogue.DialogueLines.IsEmpty() && ActualDialogue.DialogueLines.IsValidIndex(CurrentSentenceIndex))
	{
		UPDWAudioManager::StopVoiceOver(GetWorld(), ActualDialogue.DialogueLines[CurrentSentenceIndex].VoiceOverConfig.VoiceOverID);
	}
	StartTalkAnimation(false);
	ChangeAnimationState(true);
	
	if (OpenPage)
	{
		OpenPageFinished.Unbind();
	}

	if (ClosePage)
	{
		ClosePageFinished.Unbind();
	}
}

void UPDWDialogueBaseWidget::ManageClosePageAnimationFinished()
{
	UPDWDialogueSubSystem::CloseConversation(this);
	OnEndDialogue.Broadcast();
	UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UPDWGameSettings::GetEndConversationAction().ToString(), "");
}

void UPDWDialogueBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();	
	Init();
}

void UPDWDialogueBaseWidget::ShowAnswerOptions(TArray<FAnswer> Answers)
{
	if (AnswersTab && Answers.Num() > 0 && AnswersTab->GetAllChildren().Num() == 0)
	{
		TArray<UPDWAnswerButton*> AnswersButtons;

		for (int i = 0; i < Answers.Num(); i++)
		{
			UPDWAnswerButton* Button = CreateWidget<UPDWAnswerButton>(this, AnswerButtonClass);
			AnswersTab->AddChild(Button);
			Button->SetButtonText(Answers[i].DisplayAnswer);
			Button->ButtonActionClicked = Answers[i].ActionAnswerName.ToString();
			AnswersButtons.Add(Button);
		}

		if (AnswersButtons.Num() > 0)
		{
			for (int i = 0; i < AnswersButtons.Num(); i++)
			{
				if (i - 1 >= 0)
				{
					AnswersButtons[i]->SetNavigationRuleExplicit(EUINavigation::Up, AnswersButtons[i - 1]);
				}
				if (AnswersButtons.Num() > i + 1)
				{
					AnswersButtons[i]->SetNavigationRuleExplicit(EUINavigation::Down, AnswersButtons[i + 1]);
				}
			}
			UNebulaFlowUIFunctionLibrary::SetUserFocus(GetWorld(), AnswersButtons[0], GetOwningPlayer());
			if(ArrowContainer) ArrowContainer->SetVisibility(ESlateVisibility::Hidden);
			FNavbarButtonData Data = FNavbarButtonData();
			if (ConfirmNavButton) ConfirmNavButton->InitializeButton(Data);
		}
	}
}

void UPDWDialogueBaseWidget::NativeDestruct()
{
	UnInit();
	Super::NativeDestruct();
}

void UPDWDialogueBaseWidget::VoiceOverEnd()
{
	OnVoiceOverEnd.Broadcast();
	if (bAutoSkipDialogue)
	{
		OnSkipPressed();
	}
	else
	{
		StartTalkAnimation(false);
	}
}

bool UPDWDialogueBaseWidget::IsAnimationOpenPlaying()
{
	return IsAnimationPlaying(OpenPage);
}

void UPDWDialogueBaseWidget::NextLine()
{
	if (CurrentSentenceIndex < (ActualDialogue.DialogueLines.Num() - 1))
	{
		StartTalkAnimation(false);
		ChangeAnimationState(true);
		CurrentSentenceIndex++;
		CurrentCharIndex = 0;
		StartDialogue(ActualDialogue);
	}
	else
	{
		if(ActualDialogue.DialogueLines.IsValidIndex(CurrentSentenceIndex))
		{
			UPDWAudioManager::StopVoiceOver(GetWorld(), ActualDialogue.DialogueLines[CurrentSentenceIndex].VoiceOverConfig.VoiceOverID);
			StartTalkAnimation(false);
			ChangeAnimationState(true);
			CurrentSentenceIndex = 0;
			CurrentCharIndex = 0;

			if (ActualDialogue.bLoopConversation)
			{
				bStartCountForLoop = true;
			}
			else
			{
				/*if (ActualDialogue.DialogueLines[CurrentSentenceIndex].bIsDialogueAnswer)
				{
					TArray<FAnswer> Answers = ActualDialogue.DialogueLines[CurrentSentenceIndex].Answers;
					if (Answers.Num())
					{
						ShowAnswerOptions(Answers);
					}
				}*/
				//else
				//{
					EndDialogue();
				//}
				UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), DialogueCloseUISoundID);
			}
		}
	}
}
