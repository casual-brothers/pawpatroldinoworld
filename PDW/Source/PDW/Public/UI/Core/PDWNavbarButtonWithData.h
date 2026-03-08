// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/NebulaFlowNavbarButton.h"
#include "PDWNavbarButtonWithData.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWNavbarButtonWithData : public UNebulaFlowNavbarButton
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, Category = "Design")
	FNavbarButtonData Data;

protected:

	void NativeConstruct() override;
};
