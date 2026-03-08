// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Data/PDWGameplayStructures.h"
#include "GameplayTagContainer.h"
#include "PDWGraphicCustomizationComponent.h"
#include "PDWPaleoCenterSettings.generated.h"

class UPDWDinoPen;

USTRUCT(BlueprintType)
struct FDinoNeedConversationEntry
{
    GENERATED_BODY()
public:

    UPROPERTY(EditAnywhere)
    FGameplayTagContainer Tags;

    UPROPERTY(EditAnywhere)
    FConversation Conversation;
};


UCLASS()
class PDW_API UPDWDinoNeedConversationData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(TitleProperty="{Tags}"))
		TArray<FDinoNeedConversationEntry> DinoNeedConversation;

	FConversation* GetConversationByTagContainer(const FGameplayTagContainer& TagContainer)
    {
        for (FDinoNeedConversationEntry& Entry : DinoNeedConversation)
        {
            if (Entry.Tags.HasAllExact(TagContainer))
            {
                return &Entry.Conversation;
            }
        }

        for (FDinoNeedConversationEntry& Entry : DinoNeedConversation)
        {
            if (Entry.Tags.HasAnyExact(TagContainer))
            {
                return &Entry.Conversation;
            }
        }

        return nullptr;
    }
};


UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "PaleoCenter"))
class PDW_API UPDWPaleoCenterSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()
	
public:
	static UPDWPaleoCenterSettings* Get();

	UPROPERTY(Config, EditAnywhere)
	FSoftObjectPath PaleoCenterConfig;

	UPROPERTY(Config, EditAnywhere)
	FSoftObjectPath DinoNeedConversationConfig;

	UPROPERTY(Config, EditAnywhere)
	FConversation ConversationOnPickUpEggIfDinoIsBlocked;

    UPROPERTY(Config, EditAnywhere, meta = (ForceInlineRow))
    TMap<FGameplayTag, FSoftObjectPath> DinoCustomizationAsset;

    UPROPERTY(Config, EditAnywhere, meta = (ForceInlineRow))
    FGameplayTag DefaultDinoCustomizationTag = {};
};