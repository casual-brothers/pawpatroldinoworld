// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "Blueprint/UserWidget.h"
#include "PDWRemappingCategoryTitle.generated.h"


class UTextBlock;

UCLASS(meta=(BlueprintSpawnableComponent))
class PDW_API UPDWRemappingCategoryTitle : public UUserWidget
{
	GENERATED_BODY()
	
public:
		
	void InitializeWidget(FText& Title);

	UPROPERTY(BlueprintReadOnly, meta = (Bindwidget))
	TObjectPtr<UTextBlock> TitleText{ nullptr };
	

};
