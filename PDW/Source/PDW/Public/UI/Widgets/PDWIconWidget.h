// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"
#include "PDWIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWIconWidget : public UUserWidget
{
	GENERATED_BODY()
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayTagContainer IconsTag;

public:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName ="UpdateIcon"))
	void BP_UpdateIcon(const TSoftObjectPtr<UTexture2D>& NewIconTexture, const FVector2D& ImageSize);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName ="OnPlayerNear"))
	void BP_OnPlayerNear(int32 PlayerIndex, bool bIsNear);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPlayerFar"))
	void BP_OnPlayerFar(int32 PlayerIndex, bool bIsFar);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnChangeVisibilityWidget"))
	void BP_OnChangeVisibilityWidget(bool Show);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DistanceFromCenterToWidget"))
	void BP_DistanceFromCenterToWidget(float DistanceDifference);

	UFUNCTION()
	void SetIconsTag(FGameplayTagContainer& CurrentTags) {IconsTag = CurrentTags;};
};
