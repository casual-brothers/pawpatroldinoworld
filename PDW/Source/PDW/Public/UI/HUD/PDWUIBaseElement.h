// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Managers/PDWHUDSubsystem.h"
#include "Managers/PDWUIManager.h"
#include "PDWUIBaseElement.generated.h"

#if WITH_EDITOR
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentElementDataChange, FName, WidgetName, FUIElementData&, NewData);
#endif

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UPDWUIBaseElement : public UUserWidget
{
	GENERATED_BODY()
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EVisualizationType CurrentVisualization = EVisualizationType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ElementID;

	virtual void SaveWidgetData();

	UFUNCTION(BlueprintCallable)
	virtual void ApplyCurrentConfiguration(FUIElementData Configuration);

	UPROPERTY()
	ESlateVisibility ChancedVisibility = ESlateVisibility::Visible;

public:
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintAssignable)
	FOnCurrentElementDataChange OnCurrentElementDataChange;
#endif

	UFUNCTION(BlueprintPure)
	FName GetElementID() const {return ElementID;};
#if WITH_EDITORONLY_DATA

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation) override;
#endif

	UFUNCTION(BlueprintCallable)
	virtual void ChangeCurrentConfiguration(EVisualizationType NewType, FUIElementData NewConfiguration);
};
