// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NebulaFlowProfileSwap.generated.h"

class UTextBlock;
class UPanelWidget;
class UNebulaFlowNavbarButton;

/**
 * 
 */
UCLASS()
class NEBULAFLOW_API UNebulaFlowProfileSwap : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelGamerTag{ nullptr };
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelTagSuffix{ nullptr };

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> PanelUser{ nullptr };

	virtual void NativeConstruct()override;

};
