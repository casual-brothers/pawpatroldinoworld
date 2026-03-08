// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PDWUIBaseElement.h"
#include "GameplayTagContainer.h"
#include "PDWCharacterSelector.generated.h"

class UHorizontalBox;
class URichTextBlock;
class USizeBox;
class UImage;
/**
 *
 */
UCLASS()
class PDW_API UPDWCharacterSelector : public UPDWUIBaseElement
{
	GENERATED_BODY()
	
protected:

#pragma region WidgetBinding

	UPROPERTY(BlueprintReadWrite , meta = (BindWidget))
	UPanelWidget* CharactersContainer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UNebulaFlowNavbarButton* ConfirmNavButton;

#pragma endregion

#pragma region WidgetAnimations

	UPROPERTY(BlueprintReadWrite , EditAnywhere, Category = "Design|Animation")
	float SwapCharacterAnimationSpeed = 2;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Intro;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Outro;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* OutroWithSelection;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Forward {};

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Reverse {};

	UPROPERTY()
	FWidgetAnimationDynamicEvent IntroAnimationFinished;

	UPROPERTY()
	FWidgetAnimationDynamicEvent OutroAnimationFinished;

	UPROPERTY()
	FWidgetAnimationDynamicEvent MovementAnimationFinished;

	UFUNCTION()
	void ManageIntroAnimationFinished();

	UFUNCTION()
	void ManageOutroAnimationFinished();

	UFUNCTION()
	void ManageMovementAnimationFinished();

#pragma endregion

	UPROPERTY()
	TArray<FGameplayTag> PupOrder;
	
	UPROPERTY()
	TArray<FGameplayTag> CurrentPupAllowed;

	UPROPERTY()
	TArray<FGameplayTag> CurrentPupsHighlighted;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CurrentPup;

	UFUNCTION()
	void UpdateCharacters();

	UFUNCTION()
	int32 GetRightIndex(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	void ManageMovementRequest(bool IsRight);

#pragma region Audio

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design|Audio")
		FName  OpeningSoundID = FName("UI_PupPickerOpen");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Design|Audio")
		FName SelectSoundID = FName("MoveSelect");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Design|Audio")
		FName ConfirmSoundID = FName("Confirm");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Design|Audio")
		FName CloseSoundID = FName("Back");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Design|Audio")
		FName CharacterBlockChangeSoundID = FName("UI_CharacterBlockChange");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Design|Audio")
		FName OpenSoundVoId = "";

#pragma endregion

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	UFUNCTION()
	void OnAllowedPupsChanged();

public:
	
	UFUNCTION(BlueprintCallable)
	void InitializeCharacterPicker(APlayerController* Instigator);
	
	UFUNCTION(BlueprintCallable)
	void CloseWithoutSelection();
	
	UFUNCTION(BlueprintCallable)
	void CloseWithSelection();
};
