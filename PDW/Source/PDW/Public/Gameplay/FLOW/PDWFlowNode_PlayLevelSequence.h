// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/Actor/FlowNode_PlayLevelSequence.h"
#include "GameplayTagContainer.h"
#include "PDWFlowNode_ChangeState.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWFlowNode_PlayLevelSequence.generated.h"

UCLASS(NotBlueprintable, meta = (DisplayName = "Play Level Sequence"))
class PDW_API UPDWFlowNode_PlayLevelSequence : public UPDWFlowNode_ChangeState
{
	GENERATED_UCLASS_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "CutsceneConfig"))
	FPDWSequenceCollection CutsceneConfig {};

#if WITH_EDITORONLY_DATA
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


	void ExecuteInput(const FName& PinName) override;

protected:

	void ChangeState() override;


	void TriggerOutput(FName PinName, const bool bFinish = false, const EFlowPinActivationType ActivationType = EFlowPinActivationType::Default) override;

};
