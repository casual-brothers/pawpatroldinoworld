// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "PDWManageStreamingSourceNode.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ESwitchStreamingSourceBehavior : uint8
{
	Enable UMETA(DisplayName = "Enable"),
	Disable UMETA(DisplayName = "Disable")
};

USTRUCT(BlueprintType)
struct FStreamingSourceList
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> StreamingSourceIDs;
};

UCLASS(NotBlueprintable, meta = (DisplayName = "Toggle StreamingSource"))
class PDW_API UPDWManageStreamingSourceNode : public UFlowNode
{
	GENERATED_BODY()
	
public:

	UPDWManageStreamingSourceNode(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere)
	TMap<ESwitchStreamingSourceBehavior, FStreamingSourceList> StreamingSourceBehaviorMap;

	void ExecuteInput(const FName& PinName) override;

};
