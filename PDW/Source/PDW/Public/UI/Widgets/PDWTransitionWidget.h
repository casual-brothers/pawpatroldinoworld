// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PDWTransitionWidget.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWTransitionWidget : public UUserWidget
{
	GENERATED_BODY()

	UPDWTransitionWidget(const FObjectInitializer& ObjectInitializer);

public:
	
	void StartTransitionIn();

	void StartTransitionOut();
	
	bool IsScreenBlack() {return bIsScreenBlack;};

protected:
	
	UPROPERTY(EditAnywhere)
	float MinTimeFromEndTransitionIn = 0.3f;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* TransitionIn;
	UPROPERTY()
	FWidgetAnimationDynamicEvent TransitionInEvent;

	UFUNCTION()
	virtual void ManageTransitionInAnimationFinished();

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* TransitionOut;

	UPROPERTY()
	FWidgetAnimationDynamicEvent TransitionOutEvent;

	UPROPERTY()
	bool bStartTransitionOut = false;

	UPROPERTY()
	bool bStartCount = false;

	UPROPERTY()
	float CurrentTime = 0.f;

	UPROPERTY()
	bool bIsScreenBlack = false;

	UFUNCTION()
	virtual void ManageTransitionOutAnimationFinished();

	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void NativeDestruct() override;
};
