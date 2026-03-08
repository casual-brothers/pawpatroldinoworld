// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PDWCharacterSelectorElement.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDirectionPressed, bool, IsRight);

class UButton;

/**
 *
 */
UCLASS()
class PDW_API UPDWCharacterSelectorElement : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* CharButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design")
	int32 PositionIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Design")
	FName OnClickButtonAction = "Confirm";

	UPROPERTY()
	bool bIsBlocked = false;

	UPROPERTY()
	bool bIsHighlighted = false;

	UFUNCTION()
	void OnButtonClicked();

	void NativeConstruct() override;
	void NativeDestruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlockWidget(bool Block);

	UFUNCTION(BlueprintImplementableEvent)
	void HighlightWidget(bool Highlight);

public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDirectionPressed OnDirectionPressed;

	UFUNCTION(BlueprintImplementableEvent)
	void ChangePupInfo(FGameplayTag PupTag);

	UFUNCTION(BlueprintCallable)
	const int32 GetPositionIndex() { return PositionIndex; };

	UFUNCTION()
	UButton* GetCharButton() { return CharButton; };

	UFUNCTION(BlueprintPure)
	const bool IsBlocked() { return bIsBlocked; };

	UFUNCTION(BlueprintPure)
	const bool IsHighlighted() { return bIsHighlighted; };

	UFUNCTION(BlueprintCallable)
	void SetIsBlocked(bool Block);

	UFUNCTION(BlueprintCallable)
	void SetIsHighlighted(bool Highlight);
};
