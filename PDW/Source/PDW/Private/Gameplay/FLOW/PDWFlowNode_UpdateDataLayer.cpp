// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_UpdateDataLayer.h"
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"

UPDWFlowNode_UpdateDataLayer::UPDWFlowNode_UpdateDataLayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW");
	NodeDisplayStyle = FlowNodeStyle::Default;
#endif

	OutputPins.Empty();
	OutputPins.Add(FFlowPin(TEXT("Success")));
	OutputPins.Add(FFlowPin(TEXT("Finished")));
}

void UPDWFlowNode_UpdateDataLayer::ExecuteInput(const FName& PinName)
{
	UpdateDataLayer();

	TriggerOutput(TEXT("Success"), false);
	TriggerOutput(TEXT("Finished"), true);
}

void UPDWFlowNode_UpdateDataLayer::UpdateDataLayer()
{
	UPDWGameplayFunctionLibrary::SetDataLayerRuntimeState(GetWorld(), DataLayersInfo);
}

void UPDWFlowNode_UpdateDataLayer::OnPassThrough_Implementation()
{
	if (CheatMode)
	{
		UpdateDataLayer();
	}

	Super::OnPassThrough_Implementation();
}