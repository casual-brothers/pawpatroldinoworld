#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "NebulaFlowK2NodeFunctionLibrary.generated.h"

class UEdGraphNode;
class UNebulaFlowBaseFSM;

UCLASS()
class NEBULAFLOW_UNCOOKEDONLY_API UNebulaFlowK2NodeFunctionLibrary : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()

public:

	// Sort a given map as output pin
	static bool SortFSMStructureMapAsOutputPinList(const TArray<FName>& InOutputPinList, TMap<FName, FName>& OutFSMStructureMap);

	// Get a unique name
	static FName GetUniqueName(const TArray<FName>& InExistingsNameList, const FName InSuffix);

	// Get NebulaFlowFSMStructureDataAsset
	static UNebulaFlowBaseFSM* GetFSMOwner(const UEdGraphNode* InNode);
};