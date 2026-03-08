// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/Dialogue/PDWDialogueFlowAsset.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Core/NebulaFlowCoreDelegates.h"

UPDWDialogueFlowAsset::UPDWDialogueFlowAsset()
{
}

void UPDWDialogueFlowAsset::SetEntryPoint(const FGameplayTag& inEntryPoint)
{
	CurrentEntryPoint = inEntryPoint;
}

FGameplayTag UPDWDialogueFlowAsset::GetEntryPoint() const
{
	return CurrentEntryPoint;
}

//#TODO DM : think if we need to wait for this page show or is overkill
void UPDWDialogueFlowAsset::OnDialogueUIReady(IFlowDataPinValueSupplierInterface* DataPinValueSupplier)
{
	FNebulaFlowCoreDelegates::OnNewPageShowed.RemoveAll(this);
	Super::StartFlow(DataPinValueSupplier);
}
