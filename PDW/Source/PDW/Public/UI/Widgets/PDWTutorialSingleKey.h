// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "PDWTutorialNavigationIcon.h"
#include "PDWTutorialSingleKey.generated.h"

#if WITH_EDITOR
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPostEditChangeTrigger);
#endif
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIconLoad);

/**
 * 
 */
UCLASS(Abstract, MinimalAPI)
class UPDWTutorialSingleKey : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (Bindwidget))
	TObjectPtr<UImage> Material;

	UPROPERTY(BlueprintReadOnly, meta = (Bindwidget))
	TObjectPtr<UImage> KeyImage;

	UPROPERTY()
	FPDWPlatformIconConfiguration CurrentIconConfiguration;
	
	UFUNCTION()
	void SetNewWidgetData(const FUIElementData& NewData);

public:

#if WITH_EDITOR
	UFUNCTION()
	FUIElementData GetNewWidgetData();

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	FOnPostEditChangeTrigger OnPostEditChangeTrigger;
#endif
	
	UPROPERTY(BlueprintAssignable)
	FOnIconLoad OnIconLoad;

	UFUNCTION(BlueprintCallable)
	void InitKey(FPDWPlatformIconConfiguration& IconConfiguration);

	UFUNCTION(BlueprintPure)
	const FPDWPlatformIconConfiguration& GetCurrentIconConfiguration() {return CurrentIconConfiguration;};

	UFUNCTION(BlueprintPure)
	const EPadKeyNames& GetCurrentKeyName() { return CurrentIconConfiguration.KeyName; };

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void BP_PlayKeyAnimation();

};
