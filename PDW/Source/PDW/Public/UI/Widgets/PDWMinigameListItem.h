// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Widgets/NebulaFlowBaseButton.h"
#include "Data/PDWSaveStructures.h"
#include "GameplayTagContainer.h"
#include "PDWMinigameListItem.generated.h"


class UImage;
class UTextBlock;

/**
 * Base class for a Minigame selection button, used in the Ryder Menu
 */
UCLASS(MinimalAPI, Abstract)
class UPDWMinigameListItem : public UNebulaFlowBaseButton
{
	GENERATED_BODY()

public:

	void InitMinigameData(const FPDWMinigameInfo& MinigameInfo);

	FGameplayTagContainer GetIdentityTags() const { return IdentityTags; }

protected:

	/** Widget Bindings */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ImagePup{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelAreaName{ nullptr };

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelMinigameName{ nullptr };

private:

	FGameplayTagContainer IdentityTags{};
};
