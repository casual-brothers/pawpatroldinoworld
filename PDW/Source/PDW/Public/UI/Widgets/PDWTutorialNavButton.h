// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Widgets/PDWNavButtonWithParameter.h"
#include "PDWTutorialNavButton.generated.h"

UENUM(BlueprintType)
enum class ETriggerInputType : uint8
{
	OnInputPressed,
	OnInputHoldPressed,
	OnTimePassed,
	OnEvent
};

USTRUCT(BlueprintType)
struct PDW_API FPDWTutorialButtonData
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FNavbarButtonData NavButtonData{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETriggerInputType TriggerType{ ETriggerInputType::OnInputPressed };

	TFunction<void()> CallBack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="TriggerType==ETriggerInputType::OnInputHoldPressed", EditConditionHides))
	float TimeInputHold{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="TriggerType==ETriggerInputType::OnTimePassed", EditConditionHides))
	float TimeToWait{ 3.f };
};

/**
 * 
 */
UCLASS()
class PDW_API UPDWTutorialNavButton : public UPDWNavButtonWithParameter
{
	GENERATED_BODY()
	
protected:
	void OnButtonTriggered() override;

	UPROPERTY()
	FPDWTutorialButtonData TutorialButtonData;

	float CurrentTriggerPassedTime = 0.f;
	float CurrentTimePassed = 0.f;

	bool bStartCountForTimePassed = false;
	bool bCallBackCalled = false;

	void OnInputTriggered(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC) override;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void TriggerCallback();

public:
	
	UFUNCTION(BlueprintCallable)
	void InitializeTutorialButton(const FPDWTutorialButtonData& NewTutorialButtonData);
};
