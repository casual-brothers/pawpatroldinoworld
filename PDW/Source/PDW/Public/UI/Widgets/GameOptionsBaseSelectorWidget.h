// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "UI/Widgets/NebulaFlowBaseWidget.h"
#include "StructUtils/InstancedStruct.h"
#include "GameOptionsBaseSelectorWidget.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusedGameOptions, UGameOptionsBaseSelectorWidget*, GameOptionSelectorWidget);

UCLASS(MinimalAPI, Abstract)
class UGameOptionsBaseSelectorWidget : public UNebulaFlowBaseWidget
{
	GENERATED_BODY()
	
public:

	FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent);
	virtual void NativeConstruct() override;
	
	virtual void Setup(FInstancedStruct& InSettingsData) {}
	virtual void SetupChangeGameOptionsOnNavigation() {}

	virtual void ResetValue() {}
	virtual void ConfirmValue() {}

	UPROPERTY(BlueprintAssignable)
	FOnFocusedGameOptions OnFocusedGameOptions;

};

