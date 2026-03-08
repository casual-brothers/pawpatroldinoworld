// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PDWCollectionWidget.h"
#include "PDWCollectionElement.generated.h"

struct FGameplayTag;

/**
 * 
 */
UCLASS()
class PDW_API UPDWCollectionElement : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	bool bIsActive = false;

	FGameplayTag IDElementTag {};

	bool bIsUnlocked = true;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="InitCollectionElement"))
	void BP_InitCollectionElement(const FCollectionData& CollectionData);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="ActiveCollectionElement"))
	void BP_ActiveCollectionElement(bool Active);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnElementSelected"))
	void BP_OnElementSelected();

public:
	
	UFUNCTION()
	virtual void InitCollectionElement(const FCollectionData& CollectionData);

	UFUNCTION()
	virtual void ActiveCollectionElement(bool Active);
	
	UFUNCTION()
	virtual void OnElementSelected();

	UFUNCTION(BlueprintPure)
	const bool GetIsActive() { return bIsActive; };

	UFUNCTION(BlueprintPure)
	const bool GetIsUnlocked() { return bIsUnlocked; };

	UFUNCTION(BlueprintPure)
	FGameplayTag GetElementID() { return IDElementTag; };
};
