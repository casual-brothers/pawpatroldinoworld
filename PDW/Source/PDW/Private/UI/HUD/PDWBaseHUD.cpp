// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/PDWBaseHUD.h"
#include "Data/PDWGameSettings.h"
#include "Components/PanelWidget.h"
#include "UI/Widgets/NebulaFlowNavbarButton.h"
#include "Managers/PDWHUDSubsystem.h"
#include "Components/OverlaySlot.h"
#include "Gameplay/MiniGames/PDWButtonSequenceInputBehaviour.h"
#include "UI/Widgets/PDWMinigameNavButton.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "UI/Widgets/Dialogue/PDWDialogueBaseWidget.h"

void UPDWBaseHUD::InitHUD(const TArray<APlayerController*>& Players)
{
	BP_InitHUD(Players);
}

void UPDWBaseHUD::HideHUD()
{
	OnHUDCommentEnd();
}

void UPDWBaseHUD::ManageInputActionIcon(APlayerController* Sender, FInstancedStruct& InputActionToShow, FNavButtonTriggerCallback InTriggerCallBack, FNavButtonProgressCallback InProgressCallBack)
{
	if (Sender && NavButtonActionContainer)
	{
		const FButtonSequenceConfiguration* InputSequenceConfig = InputActionToShow.GetPtr<FButtonSequenceConfiguration>();
		if(InputSequenceConfig->InputActionsToUse == FGameplayTag::EmptyTag)
			return;
		TArray<UPDWMinigameNavButton*> NavButtons {};
		NavButtons.Add(CreateWidget<UPDWMinigameNavButton>(Sender, UPDWGameSettings::GetMinigameNavButtonClassReference()));
		if (NavButtons.Num() < 1)
		{
			return;
		}
		if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
		{
			NavButtons.Add(CreateWidget<UPDWMinigameNavButton>(UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this), UPDWGameSettings::GetMinigameNavButtonClassReference()));
		}
		
		FNavbarButtonData Data;
		Data.ButtonInputAction = UPDWGameSettings::GetInputActionByTag(InputSequenceConfig->InputActionsToUse);
		Data.bListenAction = true;
		Data.bListenAnyPlayer = false;

		for (int32 i = 0; i < NavButtons.Num(); i++)
		{
			NavButtons[i]->InitializeButton(Data);
			NavButtonActionContainer->AddChild(NavButtons[i]);
			UOverlaySlot* ButtonAsSlot = Cast<UOverlaySlot>(NavButtonActionContainer->Slot);
			ButtonAsSlot->SetHorizontalAlignment(HAlign_Center);
			ButtonAsSlot->SetVerticalAlignment(VAlign_Center);
			FVector2D TranslationToApply = FVector2D::Zero();
			if (!UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
			{
				TranslationToApply = InputSequenceConfig->SinglePlayerIconPosition;
			}
			else
			{
				TranslationToApply = i == 0? InputSequenceConfig->FirstPlayerIconPosition : InputSequenceConfig->SecondPlayerIconPosition;
				NavButtons[i]->ShowPlayerIndicator(i == 0);
			}
			NavButtons[i]->SetRenderTranslation(TranslationToApply);
			FPDWButtonCallbacks Callbacks;
			Callbacks.TriggerCallback = InTriggerCallBack;
			Callbacks.ProgressCallback = InProgressCallBack;
			CallbackToCallByButton.Add(NavButtons[i], Callbacks);
			NavButtons[i]->OnTriggerComplete.AddUniqueDynamic(this, &UPDWBaseHUD::OnNavIconTrigger);
			NavButtons[i]->OnProgressUpdate.AddUniqueDynamic(this, &UPDWBaseHUD::OnNavIconProgress);
		}
	}
}

void UPDWBaseHUD::OnNavIconTrigger(UPDWMinigameNavButton* Button, const FInputActionInstance& Instance)
{
	TArray<UWidget*> NavButtons = NavButtonActionContainer->GetAllChildren();
	for (int32 i = 0; i < NavButtons.Num(); i++)
	{
		UPDWMinigameNavButton* MinigameNavButton = Cast<UPDWMinigameNavButton>(NavButtons[i]);
		MinigameNavButton->OnTriggerComplete.RemoveDynamic(this, &UPDWBaseHUD::OnNavIconTrigger);
		MinigameNavButton->OnProgressUpdate.RemoveDynamic(this, &UPDWBaseHUD::OnNavIconProgress);
	}
	CallbackToCallByButton[Button].TriggerCallback.ExecuteIfBound(Instance);
	CallbackToCallByButton.Remove(Button);
	NavButtonActionContainer->ClearChildren();
}

void UPDWBaseHUD::OnNavIconProgress(UPDWMinigameNavButton* Button, const FInputActionInstance& Instance, float PreviousTime, float CurrentTime)
{
	CallbackToCallByButton[Button].ProgressCallback.ExecuteIfBound(Instance, PreviousTime, CurrentTime);
}

void UPDWBaseHUD::OnMultiplayerStateChanged()
{
	Super::OnMultiplayerStateChanged();
	TArray<APlayerController*> Players{};
	Players.Add(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this));
	if (APlayerController* Player2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this))
	{
		Players.Add(Player2);
	}
	InitHUD(Players);
}

void UPDWBaseHUD::ManageTriggerDialogue(const FConversation& Conversation)
{
	if (HUDComment)
	{
		HUDComment->StopAllAnimations();
		HUDComment->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		HUDComment->Init();
		HUDComment->StartDialogue(Conversation);
		HUDComment->OnEndDialogue.AddUniqueDynamic(this, &ThisClass::OnHUDCommentEnd);
		bHUDCommentIsPlaying = true;
	}
}

void UPDWBaseHUD::ManageEndCurrentDialogue()
{
	if (HUDComment)
	{
		HUDComment->EndDialogue();
	}
}

void UPDWBaseHUD::ManageTutorialInputIcon(const FPDWTutorialButtonData& InputIconData)
{
	if (TutorialNavButtonContainer)
	{
		TutorialNavButtonIcon = CreateWidget<UPDWTutorialNavButton>(this, TutorialNavButtonClass);
		if (TutorialNavButtonIcon)
		{
			TutorialNavButtonContainer->AddChild(TutorialNavButtonIcon);
			TutorialNavButtonIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
			TutorialNavButtonIcon->InitializeTutorialButton(InputIconData);
		}
	}
}

void UPDWBaseHUD::ManageEndTutorialInputIcon()
{
	if (TutorialNavButtonIcon)
	{
		TutorialNavButtonIcon->RemoveFromParent();
		TutorialNavButtonIcon = nullptr;
	}
}

void UPDWBaseHUD::OnHUDCommentEnd()
{
	if (HUDComment)
	{
		if (!HUDComment->IsAnimationOpenPlaying())
		{
			HUDComment->OnEndDialogue.RemoveDynamic(this, &ThisClass::OnHUDCommentEnd);
			HUDComment->UnInit();
			HUDComment->SetVisibility(ESlateVisibility::Hidden);
			bHUDCommentIsPlaying = false;
		}
	}
}

void UPDWBaseHUD::NativeDestruct()
{
	if (HUDComment)
	{
		HUDComment->EndDialogue();
	}
	Super::NativeDestruct();
}
