// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/PDWUIBaseElement.h"
#include "PDWCollectionWidget.generated.h"

class UPDWCollectionElement;
class UTextBlock;

/**
 * 
 */
UCLASS()
class PDW_API UPDWCollectionWidget : public UPDWUIBaseElement
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> CollectionContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CollectionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UPDWCollectionElement> CollectionElementClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CustomActionOnElementSelected = "";
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SelectSoundID = FName();

	UPROPERTY()
	TArray<UPDWCollectionElement*> CurrentCollection;

	UPROPERTY()
	int32 CurrentIndex = 0;

	UFUNCTION(BlueprintImplementableEvent, meta =(DisplayName = "OnActiveElementChange"))
	void BP_OnActiveElementChange(UPDWCollectionElement* NewElement);
	
public:
	
	UFUNCTION(BlueprintCallable)
	void InitCollectionName(const FText& Name);

	UFUNCTION(BlueprintCallable)
	virtual void InitCollection(const FCollectionDataArray& CollectionList, FGameplayTag StartingElement = FGameplayTag(), FName InSelectSoundId = "");

	UFUNCTION(BlueprintCallable)
	virtual void MoveCollectionElements(int32 NumMove);

	UFUNCTION(BlueprintCallable)
	virtual void SelectCurrentElement();
};