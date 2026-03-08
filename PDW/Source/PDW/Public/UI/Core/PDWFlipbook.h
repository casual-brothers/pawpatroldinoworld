// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PDWFlipbook.generated.h"

class UImage;
class UPaperFlipbook;

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UPDWFlipbook : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> FlipbookImage {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPaperFlipbook* RelatedFlipbook {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Playing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Loop = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Loop == false", EditConditionHides))
	bool ResetInitialStateAtEnd = true;

	UPROPERTY(BlueprintReadOnly)
	float TimeAccumulator = 0;

	UPROPERTY(BlueprintReadWrite , EditAnywhere)
	FVector2D WantedSize;

	UFUNCTION()
	void Reset();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;
	virtual void NativeConstruct()override;
	virtual void NativePreConstruct()override;

	UFUNCTION()
	FSlateBrush MakeBrushFromSprite(UPaperSprite* Sprite);

	UFUNCTION(BlueprintCallable)
	void SetPlaying(bool StartPlay);

};
