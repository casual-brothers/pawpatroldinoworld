// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_ChangeItemQuantity.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

UPDWFlowNode_ChangeItemQuantity::UPDWFlowNode_ChangeItemQuantity(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("PDW|Quest");
#endif

	InputPins.Add(FFlowPin(TEXT("ItemTag"), EFlowPinType::GameplayTag));
}

void UPDWFlowNode_ChangeItemQuantity::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	if (InitWithInputPins)
	{
		FFlowDataPinResult_GameplayTag PinItemTag = UFlowNodeBase::TryResolveDataPinAsGameplayTag("ItemTag");
		Items.Empty();
		Items.Add(PinItemTag.Value, 1);
	}

	if (AddOrRemove == EChangeItemOperation::Add)
	{
		UPDWGameplayFunctionLibrary::AddItemsToInventory(this, Items, Notify);
	}
	else
	{
		for(TPair<FGameplayTag, int32> Item : Items)
		{
			UPDWDataFunctionLibrary::RemoveItemQuantityFromInventory(this, Item.Key, Item.Value);	
		}
	}	

	TriggerFirstOutput(true);
}
