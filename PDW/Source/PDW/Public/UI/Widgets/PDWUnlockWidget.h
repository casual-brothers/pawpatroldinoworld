// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/PDWUIBaseElement.h"
#include "PDWUnlockWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndShowAnimationFinished);

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UPDWUnlockWidget : public UPDWUIBaseElement
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* Show;

	UPROPERTY()
	FWidgetAnimationDynamicEvent OnShowAnimationFinished;

	void NativeConstruct() override;

	UFUNCTION()
	void ManageOnShowAnimationFinished();

	UFUNCTION(BlueprintCallable)
	TArray<FGameplayTag> CheckForDinoIDParents(FGameplayTag inTag);

public:

	UPROPERTY(BlueprintAssignable)
	FOnEndShowAnimationFinished OnEndShowAnimationFinished;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitUnlockWidget(const TSoftObjectPtr<UPDWUnlockableData>& Data);
};
