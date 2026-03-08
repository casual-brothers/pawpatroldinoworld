// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/NebulaFlowBasePage.h"
#include "PDWCreditsPage.generated.h"

class UScrollBox;

/**
 * Base class for the game Credits Page
 */
UCLASS(MinimalAPI, Abstract)
class UPDWCreditsPage : public UNebulaFlowBasePage
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreditsEndReached);

	UPROPERTY(BlueprintAssignable)
	FOnCreditsEndReached OnCreditsEndReached;

	bool GetLoopScrolling() const { return bLoopScrolling; }
	void SetLoopScrolling(const bool bShouldScroll) { bLoopScrolling = bShouldScroll; }

protected:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Setup Properties */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	float ScrollSpeed{ 2.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	float ScrollFastSpeed{ 2.0f };


	/** Widget Bindings */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UScrollBox> CreditsScroller{ nullptr };

private:

	bool bLoopScrolling{ true };
	bool bScrollingEnded{ false };	
};
