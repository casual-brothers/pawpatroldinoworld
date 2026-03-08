// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "CoreMinimal.h"
#include "UI/NebulaFlowBasePage.h"
#include "PDWAnimatedUIPage.generated.h"


/**
 * 
 */
UCLASS()
class PDW_API UPDWAnimatedUIPage : public UNebulaFlowBasePage
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUIAnimationFinished);

public:
	
	UPROPERTY(BlueprintAssignable)
		FOnUIAnimationFinished OnUIAnimationFinished;

protected:

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* WidgetAnimation {};

	UPROPERTY()
		FWidgetAnimationDynamicEvent WidgetAnimationFinished;	

protected:

	virtual void NativeConstruct()override;
	virtual void NativeDestruct()override;
	
	FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	
	UFUNCTION()
		void KeyDown();

	UFUNCTION()
		void AnimationFinishedEvent();
};
