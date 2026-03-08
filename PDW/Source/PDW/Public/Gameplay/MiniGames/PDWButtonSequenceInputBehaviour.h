// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MiniGames/PDWMiniGameInputBehaviour.h"
#include "PDWButtonSequenceInputBehaviour.generated.h"

UENUM(BlueprintType)
enum class ESequenceBehaviour : uint8
{
	Random =			0,
	Specific =			1,
};

USTRUCT(BlueprintType)
struct PDW_API FButtonSequenceConfiguration : public FInputActionInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration")
	EInputActionDisplayType InputActionDisplayType = EInputActionDisplayType::None;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration", meta = (EditCondition = "InputActionDisplayType == EInputActionDisplayType::TransformOnScreen", EditConditionHides))
	FVector2D SinglePlayerIconPosition = FVector2D::Zero();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration", meta = (EditCondition = "InputActionDisplayType == EInputActionDisplayType::TransformOnScreen", EditConditionHides))
	FVector2D FirstPlayerIconPosition = FVector2D::Zero();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration", meta = (EditCondition = "InputActionDisplayType == EInputActionDisplayType::TransformOnScreen", EditConditionHides))
	FVector2D SecondPlayerIconPosition = FVector2D::Zero();

};

/**
 * 
 */
UCLASS()
class PDW_API UPDWButtonSequenceInputBehaviour : public UPDWMiniGameInputBehaviour
{
	GENERATED_BODY()
	
public:

	void ExecuteBehaviour(const FInputActionInstance& inInputInstance) override;
	void InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp) override;
	void UninitializeBehaviour() override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta = (ToolTip = "If Specific will use the array order of the actions"))
	ESequenceBehaviour SequenceBehaviour = ESequenceBehaviour::Random;

	void ShuffleSequence();

	void BindInputAction() override;

	const int32 GetCurrentIndex() const;

	void UpdateCurrentIndex();

	int32 CurrentIndex = 0;

};
