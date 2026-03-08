// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "Blueprint/UserWidget.h"
#include "Core/NebulaFlowGameInstance.h"
#include "NebulaFlowSaveIcon.generated.h"

class UNebulaFlowGameInstance;

UCLASS()
class NEBULAFLOW_API UNebulaFlowSaveIcon : public UUserWidget
{
	
	GENERATED_BODY()

public:
	
	friend class UNebulaFlowGameInstance;

	UNebulaFlowSaveIcon(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly,Category = "Flow|UI")
	float TimeOnScreen = 4.f;

protected:
	
	void IniatializeSaveIcon(UNebulaFlowGameInstance* GInstance);

	void UpdateSaveIcon(float deltaTime);

private:

	UPROPERTY()
	UNebulaFlowGameInstance* GInstanceRef = nullptr;

	float SaveTimer= 0.f;

};