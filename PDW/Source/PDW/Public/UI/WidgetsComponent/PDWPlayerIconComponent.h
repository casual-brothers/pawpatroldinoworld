// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetsComponent/PDWIconComponent.h"
#include "PDWPlayerIconComponent.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWPlayerIconComponent : public UPDWIconComponent
{
	GENERATED_BODY()
	
protected:

	void HandleMultiplayerVisibility() override;

};
