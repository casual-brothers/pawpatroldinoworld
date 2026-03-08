// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWQuestTargetContainer.generated.h"

class UPDWQuestTargetElement;

/**
 * 
 */
UCLASS()
class PDW_API UPDWQuestTargetContainer : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void InitTargetContainer(FPDWQuestTargetData StepData);

protected:
	
	UFUNCTION()
	void OnQuestTargetUpdated(FPDWQuestTargetData NewQuestTarget);

	void NativeDestruct() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> TargetsContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	TSubclassOf<UPDWQuestTargetElement> TargetElementClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	FMargin ElementPadding;
};