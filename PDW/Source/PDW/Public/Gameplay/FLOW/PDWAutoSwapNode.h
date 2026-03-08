// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWAutoSwapNode.generated.h"

class APDWPlayerController;

UENUM(BlueprintType)
enum class EPDWNodeType : uint8
{
	Vehicle = 0,
	Pup = 1,
};

UCLASS(NotBlueprintable, meta = (DisplayName = "Auto Swap Node"))
class PDW_API UPDWAutoSwapNode : public UFlowNode
{
	GENERATED_BODY()
	
	UPDWAutoSwapNode();

public:

	UPROPERTY(EditAnywhere)
	EPDWNodeType NodeType = EPDWNodeType::Vehicle;

protected:

	void ExecuteInput(const FName& PinName) override;

private:

	void Swap(APDWPlayerController* inController);
};
