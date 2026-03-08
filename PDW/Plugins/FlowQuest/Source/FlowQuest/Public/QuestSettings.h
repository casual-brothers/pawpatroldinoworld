#pragma once

#include "Engine/DeveloperSettings.h"
#include "Templates/SubclassOf.h"
#include "GameplayTagContainer.h"
#include "QuestSettings.generated.h"

class UUserWidget;

/**
 *
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Quest"))
class FLOWQUEST_API UQuestSettings final : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

	static UQuestSettings* Get() { return CastChecked<UQuestSettings>(StaticClass()->GetDefaultObject()); }

protected:
	UPROPERTY(Config, EditAnywhere)
	FGameplayTag QuestTargetTag;

	UPROPERTY(Config, EditAnywhere)
	FGameplayTag PaleoQuestTargetTag;

	UPROPERTY(Config, EditAnywhere)
	FGameplayTag QuestGiverTag;

	UPROPERTY(Config, EditAnywhere)
	FGameplayTag TutorialQuestTag;

public:

	UPROPERTY(Config, EditAnywhere)
	FSoftObjectPath QuestConfig;

	UFUNCTION(Blueprintpure)
	static FGameplayTag GetQuestTargetTag() { return Get()->QuestTargetTag; };

	UFUNCTION(Blueprintpure)
	static FGameplayTag GetPaleoQuestTargetTag() { return Get()->PaleoQuestTargetTag; };

	UFUNCTION(Blueprintpure)
	static FGameplayTag GetQuestGiverTag() { return Get()->QuestGiverTag; };

	UFUNCTION(Blueprintpure)
	static FGameplayTag GetTutorialQuestTag() { return Get()->TutorialQuestTag; };
};
