// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "UI/NebulaFlowBasePage.h"
#include "PDWVideoPage.generated.h"

class UNebulaFlowGameInstance;
class UImage;

UCLASS(meta=(BlueprintSpawnableComponent))
class PDW_API UPDWVideoPage : public UNebulaFlowBasePage
{	
	GENERATED_BODY()

protected:

	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void NativeConstruct() override;
	//virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION()
	void KeyDown();

	
	UPROPERTY(BlueprintReadWrite , meta = (BindWidget))
	UImage* VideoImage;

protected:

	UPROPERTY()
	UNebulaFlowGameInstance* GameInstance = nullptr;
	
	UPROPERTY(EditAnywhere)
	UMaterial* SwitchMaterial = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterial* DefaultMaterial = nullptr;


};

