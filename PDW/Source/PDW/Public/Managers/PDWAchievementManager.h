// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Managers/NebulaFlowAchievementManager.h"
#include "GameplayTagContainer.h"
#include "PDWAchievementManager.generated.h"

static TAutoConsoleVariable<bool> CVarShowAchievementsDebugData(
	TEXT("ShowAchievementsDebugData"),
	0,
	TEXT("Show Achievements DebugData\n"),
	ECVF_Default);

UENUM(BlueprintType)
enum class EAchievementType : uint8
{
	EA_Eggstralongneckbaby	UMETA(DisplayName = "Eggs-tra long neck baby"),
	EA_Eggsitter			UMETA(DisplayName = "Egg-sitter"),
	EA_Eggcellent			UMETA(DisplayName = "Egg-cellent"),
	EA_Eggperiencednanny	UMETA(DisplayName = "Egg-perienced nanny"),
	EA_Pupswardrobe			UMETA(DisplayName = "Pups wardrobe"),
	EA_Suchatrendypup		UMETA(DisplayName = "Such a trendy pup"),
	EA_Dinowardrobe			UMETA(DisplayName = "Dino wardrobe"),
	EA_Dinofashion			UMETA(DisplayName = "Dino fashion"),
	EA_Acozydinohome		UMETA(DisplayName = "A cozy dino home"),
	EA_Nodinoistoobig		UMETA(DisplayName = "No dino is too big!"),
	EA_Letsdinodothis		UMETA(DisplayName = "Let's dino do this"),
	EA_Letsroll				UMETA(DisplayName = "Let's roll!"),
	EA_Treattime			UMETA(DisplayName = "Treat time"),
	EA_Bellyfull			UMETA(DisplayName = "Bellyfull"),
	EA_WelcometoDinoIsland	UMETA(DisplayName = "Welcome to Dino Island"),
	EA_PupssaveBrachioDaddy	UMETA(DisplayName = "Pups save Brachio Daddy"),
	EA_PupssaveMommaTops	UMETA(DisplayName = "Pups save Momma Tops"),
	EA_PupssaveAuntieSteg	UMETA(DisplayName = "Pups save Auntie Steg"),
	EA_PupssaveMahakaBrah	UMETA(DisplayName = "Pups save Mahaka Brah"),
	EA_PupssaveDinoIsland	UMETA(DisplayName = "Pups save Dino Island"),
	EA_Eggstraordinary		UMETA(DisplayName = "Eggs-traordinary!"),
	EA_Treasurehunters		UMETA(DisplayName = "Treasure hunters")
};

UENUM(BlueprintType)
enum class EAchievementCondition : uint8
{
	EA_Execute,
	EA_ByValue,
	EA_ByTag
};

USTRUCT(BlueprintType)
struct PDW_API FAchievementDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAchievementCondition Condition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAchievementType AchievementType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName AchievementID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Condition == EAchievementCondition::EA_ByValue", EditConditionHides))
	int QuantityValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Condition == EAchievementCondition::EA_ByTag", EditConditionHides))
	FGameplayTag TagValue;

	FAchievementDataTableRow()
	{
		QuantityValue = 0;
	}
};

UCLASS()
class PDW_API UPDWAchievementManager : public UNebulaFlowAchievementManager
{
	GENERATED_BODY()
	
public:
	UPDWAchievementManager(const FObjectInitializer& ObjectInitializer);

	virtual void InitManager(UNebulaFlowGameInstance* InstanceOwner) override;
	virtual void UninitManager() override;

	virtual void UnlockAchievement(ULocalPlayer* inPlayer, const FName& AchievementId) override;
	virtual void UnlockAchievement(const FName& AchievementId);
	
	UPROPERTY(EditAnywhere, Category = "Achievements")
	TSubclassOf<AActor> ChestClass = nullptr;

protected:

	UPROPERTY()
	TMap<EAchievementType, FAchievementDataTableRow> AchievementsMap{};

	UFUNCTION()
	void OnItemCollected(FGameplayTag ItemTag);

	UFUNCTION()
	void OnPupCustomize();

	UFUNCTION()
	void OnDinoCustomize();

	UFUNCTION()
	void OnDinoNeedCompleted();

	UFUNCTION()
	void OnPupTreatCollected();

	UFUNCTION()
	void OnQuestCompleted(const FGameplayTag& QuestID);

	UFUNCTION()
	void OnEggHatch(FGameplayTag NestTag);

	UFUNCTION()
	void OnInteractionComplete(UPDWInteractionReceiverComponent* Interaction);

	UFUNCTION()
	void OnDinoPenUnlocked(const FGameplayTag&  DinoPenTag);
};
