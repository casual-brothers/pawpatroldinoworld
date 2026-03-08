// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PDWAttributeSet.generated.h"

#define GAMEPLAYATTRIBUTE_VALUE_GETTERMAX(PropertyName,MaxValue) \
	FORCEINLINE float GetMax##PropertyName() const \
	{ \
		return MaxValue; \
	}

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName, MaxValue)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTERMAX(PropertyName,MaxValue)

UCLASS()
class PDW_API UPDWAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Character")
	FGameplayAttributeData CharacterAcceleration;
	ATTRIBUTE_ACCESSORS(UPDWAttributeSet, CharacterAcceleration, 5000.0f);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Character")
	FGameplayAttributeData CharacterSpeed;
	ATTRIBUTE_ACCESSORS(UPDWAttributeSet, CharacterSpeed, 100.0f);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Character")
	FGameplayAttributeData CharacterTurning;
	ATTRIBUTE_ACCESSORS(UPDWAttributeSet, CharacterTurning, 1000.0f);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vehicle")
	FGameplayAttributeData VehicleAcceleration;
	ATTRIBUTE_ACCESSORS(UPDWAttributeSet, VehicleAcceleration, 100.0f);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vehicle")
	FGameplayAttributeData VehicleSpeed;
	ATTRIBUTE_ACCESSORS(UPDWAttributeSet, VehicleSpeed, 100.0f);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vehicle")
	FGameplayAttributeData VehicleTurning;
	ATTRIBUTE_ACCESSORS(UPDWAttributeSet, VehicleTurning, 100.0f);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData GravityMultiplier;
	ATTRIBUTE_ACCESSORS(UPDWAttributeSet, GravityMultiplier, 100.0f);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;

};
