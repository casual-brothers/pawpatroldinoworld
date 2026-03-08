#include "FunctionLibraries/NebulaFlowK2NodeFunctionLibrary.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Blueprint.h"
#include "FileHelpers.h"
#include "FSM/NebulaFlowBaseFSM.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Misc/PackageName.h"
#include "NebulaFlow_UncookedOnly.h"

bool UNebulaFlowK2NodeFunctionLibrary::SortFSMStructureMapAsOutputPinList(const TArray<FName>& InOutputPinList, TMap<FName, FName>& OutFSMStructureMap)
{
	bool Success = false;
	TMap <FName, FName> SortedMap = {};
	for (const FName& OutputPin : InOutputPinList)
	{
		if (OutFSMStructureMap.Contains(OutputPin))
		{
			SortedMap.Add(OutputPin, OutFSMStructureMap[OutputPin]);
			Success = true;
		}
	}
	if (Success)
	{
		OutFSMStructureMap.Reset();
		OutFSMStructureMap.Append(SortedMap);
	}

	return Success;
}

FName UNebulaFlowK2NodeFunctionLibrary::GetUniqueName(const TArray<FName>& InExistingsNameList, const FName InSuffix)
{
	FString PinName;
	int32 Index = 0;
	do
	{
		PinName = InSuffix.ToString() + FString("_") + FString::FromInt(Index);
		++Index;
	} 
	while (InExistingsNameList.Contains(FName(*PinName)));

	return FName(*PinName);
}

UNebulaFlowBaseFSM* UNebulaFlowK2NodeFunctionLibrary::GetFSMOwner(const UEdGraphNode* InNode)
{
	if (InNode)
	{
		UBlueprint* FSMBlueprint = FBlueprintEditorUtils::FindBlueprintForNode(InNode);
		if (FSMBlueprint)
		{
			UNebulaFlowBaseFSM* FSMOwner = Cast<UNebulaFlowBaseFSM>(FSMBlueprint->GeneratedClass.GetDefaultObject());
			if (FSMOwner)
			{
				return FSMOwner;
			}
		}
	}
	UE_LOG(LogNebulaFlowK2Node, Error, TEXT("FSM Owner Null"));

	return nullptr;
}