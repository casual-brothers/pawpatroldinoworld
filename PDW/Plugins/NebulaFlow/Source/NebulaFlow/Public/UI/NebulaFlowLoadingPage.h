// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "Blueprint/UserWidget.h"
#include "NebulaFlowLoadingPage.generated.h"



UCLASS()
class NEBULAFLOW_API UNebulaFlowLoadingPage : public UUserWidget
{
	
	GENERATED_BODY()

public:

	UNebulaFlowLoadingPage(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeLoadingPage(FName inLoadingMap);


};