// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWDinoPenCollectionElement.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWDinoPenCollectionElement : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName="ActiveElement"))
	void BP_ActiveElement(bool Active);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName="InitElement"))
	void BP_InitElement(FGameplayTag ElementType);
};
