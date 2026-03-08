// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nodes/FlowNode.h"
#include "Managers/PDWUIManager.h"
#include "PDWManageHudElementsNode.generated.h"

USTRUCT(BlueprintType)
struct PDW_API FWidgetHudCommand
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHudWidgetNames WidgetToHide = EHudWidgetNames::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHide = false;
};

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Change Hud Elements Visibility"))
class PDW_API UPDWManageHudElementsNode : public UFlowNode
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWidgetHudCommand> HudElementsToChange;

	void OnLoad_Implementation() override;

	UFUNCTION()
	void ChangeElements();
	
	void OnPassThrough_Implementation() override;

public:
	void ExecuteInput(const FName& PinName) override;

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
#endif
};
