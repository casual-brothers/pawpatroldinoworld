#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/UnrealType.h"

#include "NebulaReflectionFunctionLibrary.generated.h"

static const FString PropertySeparator(".");

USTRUCT()
struct FNebulaReflectionPropertyFilter
{
	GENERATED_USTRUCT_BODY()

public:

	EFieldIteratorFlags::SuperClassFlags SuperClassFlags = EFieldIteratorFlags::IncludeSuper;
	EFieldIteratorFlags::DeprecatedPropertyFlags DeprecatedPropertyFlags = EFieldIteratorFlags::ExcludeDeprecated;
	EFieldIteratorFlags::InterfaceClassFlags InterfaceClassFlags = EFieldIteratorFlags::IncludeInterfaces;
	
	bool bExcludeNumericProperty = true;
	bool bExcludeBoolProperty = true;
	bool bExcludeNameProperty = true;
	bool bExcludeTextProperty = true;
	bool bExcludeStringProperty = true;
	bool bExcludeEnumProperty = true;

	bool bExcludeStructProperty = true;

	bool bExcludeVectorStruct = true;
	bool bExcludeRotatorStruct = true;
	bool bExcludeTransformStruct = true;
	bool bExcludeColorStruct = true;
	
	bool bExcludeMemberFromVectorStruct = true;
	bool bExcludeMemberFromRotatorStruct = true;
	bool bExcludeMemberFromTransformStruct = true;
	bool bExcludeMemberFromColorStruct = true;
};

USTRUCT()
struct FNebulaReflectionPropertyData
{
	GENERATED_USTRUCT_BODY()

public:

	FProperty* Property = nullptr;

	void* PropertyAddress = nullptr;
};

UCLASS()
class NEBULATOOL_API UNebulaReflectionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static bool GetPropertyByName(UObject* InObject, FString InPropertyName, FNebulaReflectionPropertyData& OutPropertyData, const FNebulaReflectionPropertyFilter& InPropertyFilter = {});

	static bool GetStructPropertyByName(FStructProperty* InStructProperty, FString InPropertyName, FNebulaReflectionPropertyData& OutPropertyData, const FNebulaReflectionPropertyFilter& InPropertyFilter = {});

	static void GetPropertyByPath(UObject* InObject, const FString& InPropertyPath, FNebulaReflectionPropertyData& OutPropertyData);
	
	static TArray<FString> GetObjectPropertyList(UClass* InClass, const FNebulaReflectionPropertyFilter& InPropertyFilter = {});

	static TArray<FString> GetStructPropertyList(const UScriptStruct* InStruct, const FString& InPropertyContainerName, const FNebulaReflectionPropertyFilter& InPropertyFilter = {});
};