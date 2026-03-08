// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/Text/TextLayout.h"
#include "NebulaFlowAutoResizableContainer.generated.h"


class UBorder;
class UTextBlock;

/**
 * 
 */
UCLASS()
class NEBULAFLOW_API UNebulaFlowAutoResizableContainer : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* BorderContainer {};

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* TextBlock {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FSlateBrush BorderBrush {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category ="NebulaFlowAutoResizableContainer|Appearance")
		FText Text {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NebulaFlowAutoResizableContainer|Appearance")
		FSlateColor InColorAndOpacity {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaFlowAutoResizableContainer|Appearance")
		FSlateFontInfo Font;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaFlowAutoResizableContainer|Appearance")
		FSlateBrush StrikeBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaFlowAutoResizableContainer|Appearance")
		FVector2D ShadowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaFlowAutoResizableContainer|Appearance", meta = (DisplayName = "Shadow Color"))
		FLinearColor ShadowColorAndOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaFlowAutoResizableContainer|Appearance")
		float MinDesiredWidth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaFlowAutoResizableContainer|Appearance", meta = (DisplayName = "Transform Policy"))
		ETextTransformPolicy TextTransformPolicy;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NebulaFlowAutoResizableContainer|Appearance")
	//	Type Justification;

	UFUNCTION(BlueprintCallable)
	void SetText(FText NewText);

protected:

	virtual void NativePreConstruct()override;

};
