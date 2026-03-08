#include "FunctionLibraries/NebulaReflectionFunctionLibrary.h"

#include "Components/SceneComponent.h"
#include "NebulaTool.h"
#include "UObject/Field.h"

bool UNebulaReflectionFunctionLibrary::GetPropertyByName(UObject* InObject, FString InPropertyName, FNebulaReflectionPropertyData& OutPropertyData, const FNebulaReflectionPropertyFilter& InPropertyFilter /*= {}*/)
{
	for (TFieldIterator<FProperty> It(InObject->GetClass(), InPropertyFilter.SuperClassFlags, InPropertyFilter.DeprecatedPropertyFlags, InPropertyFilter.InterfaceClassFlags); It; ++It)
	{
		OutPropertyData.Property = *It;
		OutPropertyData.PropertyAddress = OutPropertyData.Property->ContainerPtrToValuePtr<void>(InObject);

		FString PropertyName = It->GetFName().ToString();
		PropertyName.RemoveSpacesInline();
		InPropertyName.RemoveSpacesInline();

		if (InPropertyName == PropertyName)
		{
			return true;
		}
		else if (FStructProperty* StructProperty = CastField<FStructProperty>(*It))
		{
			if (GetStructPropertyByName(StructProperty, InPropertyName, OutPropertyData))
			{
				return true;
			}
		}
	}

	UE_LOG(LogNebulaTool, Warning, TEXT("Property %s not found in %s."), *InPropertyName, *InObject->GetFName().ToString());

	return false;
}

bool UNebulaReflectionFunctionLibrary::GetStructPropertyByName(FStructProperty* InStructProperty, FString InPropertyName, FNebulaReflectionPropertyData& OutPropertyData, const FNebulaReflectionPropertyFilter& InPropertyFilter /*= {}*/)
{
	for (TFieldIterator<FProperty> It(InStructProperty->Struct, InPropertyFilter.SuperClassFlags, InPropertyFilter.DeprecatedPropertyFlags, InPropertyFilter.InterfaceClassFlags); It; ++It)
	{
		if (FStructProperty* StructProperty = CastField<FStructProperty>(*It))
		{
			return GetStructPropertyByName(StructProperty, InPropertyName, OutPropertyData);
		}
		else
		{
			FString PropertyName = It->GetFName().ToString();
			PropertyName.RemoveSpacesInline();
			InPropertyName.RemoveSpacesInline();
			if (InPropertyName == PropertyName)
			{
				OutPropertyData.Property = *It;
				OutPropertyData.PropertyAddress = OutPropertyData.Property->ContainerPtrToValuePtr<void>(OutPropertyData.PropertyAddress);

				return true;
			}
		}
	}

	UE_LOG(LogNebulaTool, Warning, TEXT("Property %s not found in %s."), *InPropertyName, *InStructProperty->GetFName().ToString());

	return false;
}

void UNebulaReflectionFunctionLibrary::GetPropertyByPath(UObject* InObject, const FString& InPropertyPath, FNebulaReflectionPropertyData& OutPropertyData)
{
	TArray<FString > PropertyList = {};
	InPropertyPath.ParseIntoArray(PropertyList, *PropertySeparator, false);

	if (PropertyList.Num() > 0)
	{
		OutPropertyData.Property = InObject->GetClass()->FindPropertyByName(*PropertyList[0]);
		OutPropertyData.PropertyAddress = OutPropertyData.Property->ContainerPtrToValuePtr<void>(InObject);
		for (int32 Index = 1; Index < PropertyList.Num(); ++Index)
		{
			if (FStructProperty* StructProperty = CastField<FStructProperty>(OutPropertyData.Property))
			{
				OutPropertyData.Property = StructProperty->Struct->FindPropertyByName(*PropertyList[Index]);
				OutPropertyData.PropertyAddress = OutPropertyData.Property->ContainerPtrToValuePtr<void>(OutPropertyData.PropertyAddress);
			}
		}
	}
}

TArray<FString> UNebulaReflectionFunctionLibrary::GetObjectPropertyList(UClass* InClass, const FNebulaReflectionPropertyFilter& InPropertyFilter /*= {}*/)
{
	TArray<FString> PropertyNameList = {};

	for (TFieldIterator<FProperty> It(InClass, InPropertyFilter.SuperClassFlags, InPropertyFilter.DeprecatedPropertyFlags, InPropertyFilter.InterfaceClassFlags); It; ++It)
	{
		if ((!InPropertyFilter.bExcludeNumericProperty && CastField<FNumericProperty>(*It)) ||
			(!InPropertyFilter.bExcludeBoolProperty && CastField<FBoolProperty>(*It)) ||
			(!InPropertyFilter.bExcludeNameProperty && CastField<FNameProperty>(*It)) ||
			(!InPropertyFilter.bExcludeTextProperty && CastField<FTextProperty>(*It)) ||
			(!InPropertyFilter.bExcludeStringProperty && CastField<FStrProperty>(*It)) ||
			(!InPropertyFilter.bExcludeEnumProperty && CastField<FEnumProperty>(*It)))
		{
			PropertyNameList.AddUnique(It->GetName());
		}
		else if (!InPropertyFilter.bExcludeStructProperty)
		{
			if (FStructProperty* StructProperty = CastField<FStructProperty>(*It))
			{
				if ((InPropertyFilter.bExcludeMemberFromVectorStruct && (TBaseStructure<FVector2D>::Get() == StructProperty->Struct || TBaseStructure<FVector>::Get() == StructProperty->Struct || TBaseStructure<FVector4>::Get() == StructProperty->Struct)) ||
					(InPropertyFilter.bExcludeMemberFromColorStruct && (TBaseStructure<FColor>::Get() == StructProperty->Struct || TBaseStructure<FLinearColor>::Get() == StructProperty->Struct)) ||
					(InPropertyFilter.bExcludeMemberFromRotatorStruct && TBaseStructure<FRotator>::Get() == StructProperty->Struct) ||
					(InPropertyFilter.bExcludeMemberFromTransformStruct && TBaseStructure<FTransform>::Get() == StructProperty->Struct))
				{
					PropertyNameList.AddUnique(It->GetName());
				}
				else
				{
					PropertyNameList.Append(GetStructPropertyList(StructProperty->Struct, It->GetName()));
				}
			}
		}
	}

	return PropertyNameList;
}

TArray<FString> UNebulaReflectionFunctionLibrary::GetStructPropertyList(const UScriptStruct* InStruct, const FString& InPropertyContainerName, const FNebulaReflectionPropertyFilter& InPropertyFilter /*= {}*/)
{
	TArray<FString> PropertyNameList = {};

	for (TFieldIterator<FProperty> It(InStruct, InPropertyFilter.SuperClassFlags, InPropertyFilter.DeprecatedPropertyFlags, InPropertyFilter.InterfaceClassFlags); It; ++It)
	{
		if ((!InPropertyFilter.bExcludeNumericProperty && CastField<FNumericProperty>(*It)) ||
			(!InPropertyFilter.bExcludeBoolProperty && CastField<FBoolProperty>(*It)) ||
			(!InPropertyFilter.bExcludeNameProperty && CastField<FNameProperty>(*It)) ||
			(!InPropertyFilter.bExcludeTextProperty && CastField<FTextProperty>(*It)) ||
			(!InPropertyFilter.bExcludeStringProperty && CastField<FStrProperty>(*It)) ||
			(!InPropertyFilter.bExcludeEnumProperty && CastField<FEnumProperty>(*It)))
		{
			PropertyNameList.AddUnique(InPropertyContainerName + PropertySeparator + It->GetName());
		}
		else if (!InPropertyFilter.bExcludeStructProperty)
		{
			if (FStructProperty* StructProperty = CastField<FStructProperty>(*It))
			{
				if (!InPropertyFilter.bExcludeVectorStruct && (TBaseStructure<FVector2D>::Get() == StructProperty->Struct || TBaseStructure<FVector>::Get() == StructProperty->Struct || TBaseStructure<FVector4>::Get() == StructProperty->Struct))
				{
					PropertyNameList.AddUnique(InPropertyContainerName + PropertySeparator + It->GetName());
					if (!InPropertyFilter.bExcludeMemberFromVectorStruct)
					{
						PropertyNameList.Append(GetStructPropertyList(StructProperty->Struct, InPropertyContainerName + PropertySeparator + It->GetName()));
					}
				}
				else if (!InPropertyFilter.bExcludeRotatorStruct && TBaseStructure<FRotator>::Get() == StructProperty->Struct)
				{
					PropertyNameList.AddUnique(InPropertyContainerName + PropertySeparator + It->GetName());
					if (!InPropertyFilter.bExcludeMemberFromRotatorStruct)
					{
						PropertyNameList.Append(GetStructPropertyList(StructProperty->Struct, InPropertyContainerName + PropertySeparator + It->GetName()));
					}
				}
				else if (!InPropertyFilter.bExcludeTransformStruct && TBaseStructure<FTransform>::Get() == StructProperty->Struct)
				{
					PropertyNameList.AddUnique(InPropertyContainerName + PropertySeparator + It->GetName());
					if (!InPropertyFilter.bExcludeMemberFromTransformStruct)
					{
						PropertyNameList.Append(GetStructPropertyList(StructProperty->Struct, InPropertyContainerName + PropertySeparator + It->GetName()));
					}
				}
				else if (!InPropertyFilter.bExcludeColorStruct && (TBaseStructure<FColor>::Get() == StructProperty->Struct || TBaseStructure<FLinearColor>::Get() == StructProperty->Struct))
				{
					PropertyNameList.AddUnique(InPropertyContainerName + PropertySeparator + It->GetName());
					if (!InPropertyFilter.bExcludeMemberFromColorStruct)
					{
						PropertyNameList.Append(GetStructPropertyList(StructProperty->Struct, InPropertyContainerName + PropertySeparator + It->GetName()));
					}
				}
			}
		}
	}
	return PropertyNameList;
}