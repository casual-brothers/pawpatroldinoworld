// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Widgets/NebulaFlowBaseWidget.h"
#include "Managers/PDWDialogueSubSystem.h"
#include "UI/HUD/PDWUIBaseElement.h"
#include "PDWDialogueBaseWidget.generated.h"

class URichTextBlock;
class UTextBlock;
class UImage;
class UNebulaFlowNavButtonBase;
class UPDWAnswerButton;
class UNebulaFlowBaseButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMessageCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndDialogue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVoiceOverEnd);

UCLASS()
class PDW_API UPDWDialogueBaseWidget : public UPDWUIBaseElement
{
	GENERATED_BODY()
protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
		UTextBlock* SpeakerNameBlock;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		URichTextBlock* MessageBlock;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
		UPanelWidget* ArrowContainer;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
		UWidgetAnimation* ArrowAnim;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
		UPanelWidget* AnswersTab;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UPDWAnswerButton> AnswerButtonClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetOptional))
		UNebulaFlowNavButtonBase* ConfirmNavButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
		UImage* SpeakerImage;

	UPROPERTY(EditAnywhere)
		FName DialogueOpenUISoundID = "UI_DialogueOpen";

	UPROPERTY(EditAnywhere)
		FName DialogueSkipUISoundID = "UI_DialogueSkip";

	UPROPERTY(EditAnywhere)
		FName DialogueCloseUISoundID = "UI_DialogueClose";

	UPROPERTY()
		bool bStartPrintSentece = false;

	UPROPERTY()
		FConversation ActualDialogue = {};

	UPROPERTY()
		FString FullMessage = "";

	UPROPERTY()
		FString CurrentMessage = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float NextCharDelay = .7f;

	UPROPERTY()
		float NextCharTimer = 0.f;

	UPROPERTY()
		int CurrentCharIndex = 0;

	UPROPERTY()
		int CurrentSentenceIndex = 0;

	UPROPERTY()
		bool bIsDialogueEnd = false;

	UPROPERTY()
		bool bStartCountForLoop = false;

	UPROPERTY()
		float CurrentLoopTime = 0.f;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY()
		FWidgetAnimationDynamicEvent OpenPageFinished;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
		UWidgetAnimation* OpenPage;

	UFUNCTION()
		virtual void ManageOpenPageAnimationFinished();

	UPROPERTY()
		FWidgetAnimationDynamicEvent ClosePageFinished;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
		UWidgetAnimation* ClosePage;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
		UWidgetAnimation* StartDialogueAnim;

	UPROPERTY()
		AActor* WidgetOwner = nullptr;
		
	UPROPERTY()
	FTimerHandle AutoSkipTimerHandler;

	UPROPERTY()
		bool bIsTalking = false;

	UFUNCTION()
		virtual void ManageClosePageAnimationFinished();

	void NativeConstruct() override;

	UFUNCTION()
		void ShowAnswerOptions(TArray<FAnswer> Answers);

		void NativeDestruct() override;

	UFUNCTION()
	void VoiceOverEnd();

public:

	UPROPERTY(EditAnywhere)
		bool bAutoSkipDialogue = false;

	UPROPERTY(EditAnywhere)
		bool bShouldMakeAnimationTalking = false;

	UPROPERTY(EditAnywhere)
		float AutoSkipTime = 2.f;

	UPROPERTY(BlueprintAssignable)
	FOnMessageCompleted OnMessageCompleted;
	
	UPROPERTY(BlueprintAssignable)
	FOnEndDialogue OnEndDialogue;

	UPROPERTY(BlueprintAssignable)
	FOnVoiceOverEnd OnVoiceOverEnd;

	UFUNCTION(BlueprintCallable)
		virtual void StartDialogue(FConversation Dialogue);

	UFUNCTION(BlueprintCallable)
		/*TArray<UKS2BaseAnimInstance*>*/void GetAnimInstancesByID(const FName ChID);

	UFUNCTION(BlueprintCallable)
		void StartTalkAnimation(bool Talk);

	UFUNCTION(BlueprintCallable)
		void ChangeAnimationState(bool ToDefault);

	UFUNCTION(BlueprintCallable)
		void OnSkipPressed();

	UFUNCTION(BlueprintCallable)
		virtual void EndDialogue();

	UFUNCTION(BlueprintCallable)
		UNebulaFlowBaseButton* GetFirstButton();

	UFUNCTION(BlueprintCallable)
		int GetCurrentSentenceIndex() {return CurrentSentenceIndex; }

	UFUNCTION(BlueprintCallable)
		void SetCurrentSentenceIndex(int NewIndex) { CurrentSentenceIndex = NewIndex; }

	UFUNCTION(BlueprintCallable)
		bool IsAnswerShowing();

	UFUNCTION()
		void Init();

	UFUNCTION()
		void UnInit();

	UFUNCTION()
		bool IsAnimationOpenPlaying();

	UFUNCTION(BlueprintCallable)
	void NextLine();
};
