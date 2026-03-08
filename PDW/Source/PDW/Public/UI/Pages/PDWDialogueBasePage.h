// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Pages/PDWUIBasePage.h"
#include "PDWDialogueBasePage.generated.h"

class UPDWDialogueBaseWidget;

/**
 * 
 */
UCLASS()
class PDW_API UPDWDialogueBasePage : public UPDWUIBasePage
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UPDWDialogueBaseWidget* DialogueWidget;

public:
	UFUNCTION(BlueprintCallable)
	void StartConversation();

	UFUNCTION(BlueprintCallable)
	void OnSkipPressed();

	UFUNCTION()
	void OnEndConversation();
};
