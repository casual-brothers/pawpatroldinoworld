// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PDWGASComponent.generated.h"

class UPDWGASAbility;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Category = "AbilityInput", meta = (BlueprintSpawnableComponent))
class PDW_API UPDWGASComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	
	UPDWGASAbility* GetCurrentAbility() const { return CurrentAbility; }
	void SetCurrentAbility(UPDWGASAbility* inAbility) { CurrentAbility = inAbility;};

protected:

	UPROPERTY()
	UPDWGASAbility* CurrentAbility = nullptr;
};
