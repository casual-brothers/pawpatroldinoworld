// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PDWDinoPenCollection.generated.h"

class UPDWDinoPenCollectionElement;
class UPanelWidget;
class UTextBlock;

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UPDWDinoPenCollection : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DinoText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> CollectionContainer{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UPDWDinoPenCollectionElement> ColectionElementClass{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DinoSentence;

	UPROPERTY(BlueprintReadWrite)
	bool IsBabyDinoBorn = false;
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName="InitElement"))
	void BP_InitElement(int32 MaxValue, int32 CurrentValue, FGameplayTag ElementType);

	UFUNCTION()
	void InitElement(int32 MaxValue, int32 CurrentValue, FGameplayTag ElementType);

public:
	
	UFUNCTION(BlueprintCallable)
	void SetDinoName(FText DinoName);

	UFUNCTION(BlueprintCallable)
	void InitDinoPenByEggs(FPDWDinoPenEggs EggsInfo);

	UFUNCTION(BlueprintCallable)
	void InitDinoPenByDecorations(FPDWDinoPenDecorations DecorationInfo);
};
