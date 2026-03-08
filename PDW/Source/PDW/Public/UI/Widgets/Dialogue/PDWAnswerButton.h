// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/NebulaFlowBaseButton.h"
#include "PDWAnswerButton.generated.h"

class URichTextBlock;

/**
 * 
 */
UCLASS()
class PDW_API UPDWAnswerButton : public UNebulaFlowBaseButton
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		URichTextBlock* ButtonText;

	UFUNCTION(BlueprintImplementableEvent)
	void AlterateButton(bool Alterate);

public:
	
	UFUNCTION(BlueprintCallable)
	void SetButtonText(FText Text);
};
