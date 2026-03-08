// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "UI/NebulaFlowDialogDataStructures.h"
#include "PDWShowModalNode.generated.h"

class UNebulaFlowDialog;

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Show Modal"))
class PDW_API UPDWShowModalNode : public UFlowNode
{
	GENERATED_BODY()
	
	UPDWShowModalNode();

protected:

	UPROPERTY(EditAnywhere, Category = "Configuration", meta=(GetOptions = "GetAllDTModalRows"))
	FName ModalID;

	UPROPERTY()
	UNebulaFlowDialog* CurrentModal = nullptr;

	UPROPERTY()
	FName DefaultOutputPin = "Default";

	void OnModalResponse(FString Response);
public:
#if WITH_EDITOR
	UFUNCTION()
    TArray<FName> GetAllDTModalRows();

	TArray<FFlowPin> GetContextOutputs() const override;
	bool SupportsContextPins() const override;
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	void ExecuteInput(const FName& PinName) override;

};
