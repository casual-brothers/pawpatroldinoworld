// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWShowModalNode.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Managers/PDWUIManager.h"

UPDWShowModalNode::UPDWShowModalNode()
{
#if WITH_EDITOR
	Category = TEXT("PDW|Modal");
	NodeDisplayStyle = FlowNodeStyle::Node;
#endif
	OutputPins.Empty();
}
#if WITH_EDITOR

TArray<FName> UPDWShowModalNode::GetAllDTModalRows()
{
	TArray<FName> Result;
	UDataTable* ModalDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/UI/DT_Modal.DT_Modal"));
	if (ModalDataTable)
	{
		for (const FName& RowName : ModalDataTable->GetRowNames())
		{
			FDialogConfigTableRow* ModalRow = ModalDataTable->FindRow<FDialogConfigTableRow>(RowName, "");
			if (ModalRow)
			{
				if (ModalRow->DialogConfiguration.DialogType != EDialogType::ESystemDialog)
				{
					Result.Add(RowName);
				}
			}
		}
	}
	return Result;
}

TArray<FFlowPin> UPDWShowModalNode::GetContextOutputs() const
{
	TArray<FFlowPin> ContextOutputs = Super::GetContextOutputs();
	UDataTable* ModalDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/UI/DT_Modal.DT_Modal"));
	if (ModalDataTable)
	{
		FDialogConfigTableRow* ModalRow = ModalDataTable->FindRow<FDialogConfigTableRow>(ModalID, "");
		if (ModalRow)
		{
			if (ModalRow->DialogConfiguration.DialogType == EDialogType::EPopupDialog)
			{
				ContextOutputs.AddUnique(FFlowPin(DefaultOutputPin));
			}
			else
			{
				for (const FDialogButtonConfiguration& ButtonConfig : ModalRow->DialogConfiguration.ButtonConfigurations)
				{
					ContextOutputs.AddUnique(FFlowPin(ButtonConfig.ButtonResponse));
				}
			}
		}
	}
	return ContextOutputs;
}

bool UPDWShowModalNode::SupportsContextPins() const
{
	return true;
}

void UPDWShowModalNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnReconstructionRequested.ExecuteIfBound();
}
#endif

void UPDWShowModalNode::ExecuteInput(const FName& PinName)
{
	CurrentModal = UNebulaFlowUIFunctionLibrary::ShowDialog(this, ModalID, [&](FString Response){OnModalResponse(Response);});
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (UIManager->CurrentModalIsTypeOf(CurrentModal) == EDialogType::EPopupDialog)
	{
		TriggerOutput(DefaultOutputPin);
	}
}

void UPDWShowModalNode::OnModalResponse(FString Response)
{
	UNebulaFlowUIFunctionLibrary::HideDialog(this, CurrentModal);
	TriggerOutput(FName(Response));
}
