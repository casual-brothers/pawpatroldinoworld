// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NebulaFlowScrollingText.generated.h"

class UTextBlock;
class USizeBox;
class UCanvasPanel;
class UCanvasPanelSlot;
class UOverlaySlot;
class UOverlay;
class UScaleBox;
/**
 * 
 */
UCLASS()
class NEBULAFLOW_API UNebulaFlowScrollingText : public UUserWidget
{
	GENERATED_BODY()
	
public :

	////////// WIDGETS ////////////

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		USizeBox* MovingSizeBox {};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UCanvasPanel* MovingCanvas {};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UOverlay* FirstTextOverlay {};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UOverlay* SecondTextOverlay {};

	//UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	//UCanvasPanel* FakeCanvas {};

	UPROPERTY(BlueprintReadWrite , meta = (BindWidget))
	UTextBlock* FirstText{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* SecondText {};

	///////////////////////////////

	UPROPERTY(BlueprintReadWrite , EditAnywhere)
	FVector2D SizeBoxSize  = FVector2D(500 , 100);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DefaultText {};

	UPROPERTY(BlueprintReadWrite)
	bool ForceStart = false;

	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	bool DoubleText = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AutoStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpaceBetweenTextes = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartingTimeDelay = 5;

	UFUNCTION(BlueprintCallable)
	void InitTextes(FText NewText);

	UFUNCTION(BlueprintCallable)
	void Reset();

protected:

	UPROPERTY()
	float TextLocalWidth = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bEnableSecondText = false;

	UPROPERTY(BlueprintReadWrite)
	float CurrentTimer;

	UPROPERTY(BlueprintReadWrite)
	bool bCanBeContained = false;

	UPROPERTY(BlueprintReadWrite)
		bool bFrameSkippedToUpdateGeometry = false;


	UFUNCTION()
		void CheckCorrectTextSize();

	UFUNCTION()
		void SetupTextBasedOnWidth(bool TooBig);

	virtual void NativePreConstruct()override;
	virtual void NativeConstruct()override;
	virtual void NativeDestruct()override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;

private:

	bool bIsArabicLanguage = false;
};
