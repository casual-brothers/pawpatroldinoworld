#include "K2Node/K2Node_FSMEntryPoint.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "FSM/NebulaFlowBaseFSM.h"
#include "FunctionLibraries/NebulaFlowK2NodeFunctionLibrary.h"
#include "K2Node/K2Node_FSMState.h"
#include "NebulaFlow_UncookedOnly.h"

#define LOCTEXT_NAMESPACE "K2Node_FSMEntryPoint"

const FName UK2Node_FSMEntryPoint::DefaultOutputPinName(TEXT("Default"));

/* Begin UObject interface */

void UK2Node_FSMEntryPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == TEXT("PinNames"))
	{
		// If DefaultOutputPinName exists in PinNames it must be the first
		const int32 FoundedDefaultOutputPinIndex = PinNames.Find(DefaultOutputPinName);
		if (FoundedDefaultOutputPinIndex > 0)
		{
			PinNames.RemoveAt(FoundedDefaultOutputPinIndex);
			PinNames.Shrink();
			PinNames.Insert(DefaultOutputPinName, 0);
		}

		// Duplicates are not allowed
		const int32 AddedElementIndex = PropertyChangedEvent.GetArrayIndex(PropertyName.ToString());
		if (AddedElementIndex >= 0 && AddedElementIndex < PinNames.Num())
		{
			const int32 FoundedIndex = PinNames.Find(PinNames[AddedElementIndex]);
			const int32 FoundedLastIndex = PinNames.FindLast(PinNames[AddedElementIndex]);
			if (FoundedIndex != AddedElementIndex || FoundedLastIndex != AddedElementIndex)
			{
				UE_LOG(LogNebulaFlowK2Node, Log, TEXT("Pin %s already exist."), *(PinNames[AddedElementIndex].ToString()));
				PinNames[AddedElementIndex] = UNebulaFlowK2NodeFunctionLibrary::GetUniqueName(PinNames, DefaultOutputPinName);
			}
		}

		// Removes unused connection from LabelToEntryPointMap
		UNebulaFlowBaseFSM* FSMOwner = UNebulaFlowK2NodeFunctionLibrary::GetFSMOwner(this);
		if (FSMOwner)
		{
			TArray<FName> KeyList;
			FSMOwner->LabelToEntryPointMap.GetKeys(KeyList);
			for (FName Key : KeyList)
			{
				if (!PinNames.Contains(Key))
				{
					FSMOwner->LabelToEntryPointMap.Remove(Key);
				}
			}

			FSMOwner->LabelToEntryPointMap.Compact();
			FSMOwner->LabelToEntryPointMap.Shrink();
		}

		ReconstructNode();
		UpdateDefaultLabel();
		GetGraph()->NotifyGraphChanged();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UK2Node_FSMEntryPoint::PostEditUndo()
{
	Super::PostEditUndo();

	for (UEdGraphPin* Pin : Pins)
	{
		UpdateTransitions(Pin);
	}
}

/* End of UObject interface */

/* Begin UEdGraphNode Interface */

void UK2Node_FSMEntryPoint::AllocateDefaultPins()
{
	// If PinNames is empty adds a DefaultOutputPinName
	if (PinNames.Num() == 0)
	{
		PinNames.Add(DefaultOutputPinName);
	}

	for (FName& PinName : PinNames) 
	{
		// Label with Name_NONE is not allowed
		if (PinName == NAME_None)
		{
			PinName = UNebulaFlowK2NodeFunctionLibrary::GetUniqueName(PinNames, DefaultOutputPinName);
		}
		
		// Creates output pins
		if (!FindPin(PinName, EGPD_Output))
		{
			CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinName);
		}
	}

	UNebulaFlowBaseFSM* FSMOwner = UNebulaFlowK2NodeFunctionLibrary::GetFSMOwner(this);
	if (FSMOwner)
	{
		// Sort LabelToEntryPointMap
		UNebulaFlowK2NodeFunctionLibrary::SortFSMStructureMapAsOutputPinList(PinNames, FSMOwner->LabelToEntryPointMap);
	}

	Super::AllocateDefaultPins();
}

void UK2Node_FSMEntryPoint::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	UpdateTransitions(Pin);
}

bool UK2Node_FSMEntryPoint::CanDuplicateNode() const
{
	return false;
}

FText UK2Node_FSMEntryPoint::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle", "FSM Entry Point");
}

FText UK2Node_FSMEntryPoint::GetTooltipText() const
{
	return LOCTEXT("NodeTooltip", "Nebula FSM Entry Point");
}

FLinearColor UK2Node_FSMEntryPoint::GetNodeTitleColor() const
{
	return FLinearColor(0.0f, 0.5f, 1.0f);
}

/* End UEdGraphNode Interface */

/* Begin UK2Node Interface */

void UK2Node_FSMEntryPoint::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* NodeClass = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(NodeClass))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);
		ActionRegistrar.AddBlueprintAction(NodeClass, NodeSpawner);
	}
}

bool UK2Node_FSMEntryPoint::ShouldShowNodeProperties() const
{
	return true;
}

FText UK2Node_FSMEntryPoint::GetMenuCategory() const
{
	return LOCTEXT("NodeCategory", "Nebula Flow Plugin");
}

/* End UK2Node Interface */

/* Begin IK2Node_AddPinInterface Interface */

void UK2Node_FSMEntryPoint::AddInputPin()
{
	Modify();
	const FName PinName = UNebulaFlowK2NodeFunctionLibrary::GetUniqueName(PinNames, DefaultOutputPinName);
	PinNames.Add(PinName);

	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, PinName);
}

/* End IK2Node_AddPinInterface Interface */

void UK2Node_FSMEntryPoint::UpdateDefaultLabel()
{
	UNebulaFlowBaseFSM* FSMOwner = UNebulaFlowK2NodeFunctionLibrary::GetFSMOwner(this);
	if (FSMOwner)
	{
		FName NodePinName = NAME_None;
		if (FSMOwner->LabelToEntryPointMap.Num() > 0)
		{
			TArray<FName> KeyList;
			FSMOwner->LabelToEntryPointMap.GetKeys(KeyList);
			NodePinName = KeyList[0];
		}
		FSMOwner->DefaultLabelName = (NodePinName != NAME_None) ? NodePinName : DefaultOutputPinName;
	}
}

void UK2Node_FSMEntryPoint::UpdateTransitions(UEdGraphPin* Pin)
{
	UNebulaFlowBaseFSM* FSMOwner = UNebulaFlowK2NodeFunctionLibrary::GetFSMOwner(this);
	if (FSMOwner)
	{
		// If edited output pin is connected adds transition, else remove transition
		if (Pin->LinkedTo.Num() > 0)
		{
			if (!FSMOwner->LabelToEntryPointMap.Contains(Pin->PinName))
			{
				// Adds transition
				const UK2Node_FSMState* FSMStateNode = Cast<UK2Node_FSMState>(Pin->LinkedTo[0]->GetOwningNode());
				if (FSMStateNode)
				{
					const UEdGraphPin* FSMStateLabelPin = FSMStateNode->FindPin(UK2Node_FSMState::FSMStateLabelPinName, EGPD_Input);
					if (FSMStateLabelPin)
					{
						FName FSMStateLabel = FName(*FSMStateLabelPin->DefaultValue);
						FSMOwner->LabelToEntryPointMap.Add(Pin->PinName, FSMStateLabel);
					}
				}
			}

			// Sort LabelToEntryPointMap
			UNebulaFlowK2NodeFunctionLibrary::SortFSMStructureMapAsOutputPinList(PinNames, FSMOwner->LabelToEntryPointMap);
		}
		else
		{
			// Removes transition
			if (FSMOwner->LabelToEntryPointMap.Contains(Pin->PinName))
			{
				FSMOwner->LabelToEntryPointMap.Remove(Pin->PinName);
				FSMOwner->LabelToEntryPointMap.Compact();
				FSMOwner->LabelToEntryPointMap.Shrink();
			}
		}

		UpdateDefaultLabel();
	}
}

#undef LOCTEXT_NAMESPACE