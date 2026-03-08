// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/NebulaFlowBasePage.h"
#include "GameplayTagContainer.h"
#include "PDWTeleportPage.generated.h"


class UPDWTeleportLocationWidget;

/**
 * Base class for the Teleport Menu page
 */
UCLASS(MinimalAPI, Abstract)
class UPDWTeleportPage : public UNebulaFlowBasePage
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

	/** Setup Properties */

	/** Widget Bindings */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWTeleportLocationWidget> BtnAreaDinoPlanes{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWTeleportLocationWidget> BtnAreaJungle{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWTeleportLocationWidget> BtnAreaShores{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPDWTeleportLocationWidget> BtnAreaVolcano{ nullptr };
};
