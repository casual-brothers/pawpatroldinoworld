// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "UI/Widgets/NebulaFlowBaseWidget.h"
#include "GameOptionsCategoryWidget.generated.h"

class UTextBlock;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusedCategory, EGameOptionsCategory, Category);


UCLASS(MinimalAPI, Abstract)
class UGameOptionsCategoryWidget : public UNebulaFlowBaseWidget
{
	GENERATED_BODY()
	
public:
		
	void Setup(EGameOptionsCategory InCategory);	

	EGameOptionsCategory Category;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> GameOptionsText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> GameOptionsIcon;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
		
	UPROPERTY(BlueprintAssignable)
	FOnFocusedCategory OnFocusedCategory;

};
