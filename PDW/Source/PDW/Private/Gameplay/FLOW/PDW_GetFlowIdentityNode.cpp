// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDW_GetFlowIdentityNode.h"
#include "Gameplay/FLOW/PDWMinigameFlowAsset.h"

void UPDW_GetFlowIdentityNode::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	UPDWMinigameFlowAsset* Asset = Cast<UPDWMinigameFlowAsset>(GetFlowAsset());
	OutIdentityTag.Value = Asset->GetMinigameTag().GetSingleTagContainer();
	TriggerFirstOutput(true);
}

FFlowDataPinResult_GameplayTagContainer UPDW_GetFlowIdentityNode::TrySupplyDataPinAsGameplayTagContainer_Implementation(const FName& PinName) const
{
	//if (FlowDataPinValueSupplierInterface)
	//{
	//	FFlowDataPinResult_GameplayTag SuppliedResult = IFlowDataPinValueSupplierInterface::Execute_TrySupplyDataPinAsGameplayTag(FlowDataPinValueSupplierInterface.GetObject(), PinName);

	//	if (SuppliedResult.Result == EFlowDataPinResolveResult::Success)
	//	{
	//		return SuppliedResult;
	//	}
	//}

	return Super::TrySupplyDataPinAsGameplayTagContainer_Implementation(PinName);
}
