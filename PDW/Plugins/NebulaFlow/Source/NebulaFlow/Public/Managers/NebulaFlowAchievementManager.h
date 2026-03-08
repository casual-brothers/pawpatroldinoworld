#pragma once

#include "NebulaFlowManagerBase.h"
#include "UObject/Object.h"
#include "Engine/DataTable.h"
#include "NebulaFlowAchievementManager.generated.h"

class ULocalPlayer;
class ANebulaFlowPlayerController;

//************************************** ACHIEVEMENT OLD DB START  ********************************************************

UENUM(BlueprintType)
enum class EAchievement : uint8
{
	EA_Achievement_00	UMETA(DisplayName = "ACHIEVEMENT_00"),
	EA_Achievement_01	UMETA(DisplayName = "ACHIEVEMENT_01"),
	EA_Achievement_02	UMETA(DisplayName = "ACHIEVEMENT_02"),
	EA_Achievement_03	UMETA(DisplayName = "ACHIEVEMENT_03"),
	EA_Achievement_04	UMETA(DisplayName = "ACHIEVEMENT_04"),
	EA_Achievement_05	UMETA(DisplayName = "ACHIEVEMENT_05"),
	EA_Achievement_06	UMETA(DisplayName = "ACHIEVEMENT_06"),
	EA_Achievement_07	UMETA(DisplayName = "ACHIEVEMENT_07"),
	EA_Achievement_08	UMETA(DisplayName = "ACHIEVEMENT_08"),
	EA_Achievement_09	UMETA(DisplayName = "ACHIEVEMENT_09"),
	EA_Achievement_10	UMETA(DisplayName = "ACHIEVEMENT_10"),
	EA_Achievement_11	UMETA(DisplayName = "ACHIEVEMENT_11"),
	EA_Achievement_12	UMETA(DisplayName = "ACHIEVEMENT_12"),
	EA_Achievement_13	UMETA(DisplayName = "ACHIEVEMENT_13"),
	EA_Achievement_14	UMETA(DisplayName = "ACHIEVEMENT_14"),
	EA_Achievement_15	UMETA(DisplayName = "ACHIEVEMENT_15"),
	EA_Achievement_16	UMETA(DisplayName = "ACHIEVEMENT_16"),
	EA_Achievement_17	UMETA(DisplayName = "ACHIEVEMENT_17"),
	EA_Achievement_18	UMETA(DisplayName = "ACHIEVEMENT_18"),
	EA_Achievement_19	UMETA(DisplayName = "ACHIEVEMENT_19"),
	EA_Achievement_20	UMETA(DisplayName = "ACHIEVEMENT_20"),
	EA_Achievement_21	UMETA(DisplayName = "ACHIEVEMENT_21"),
	EA_Achievement_22	UMETA(DisplayName = "ACHIEVEMENT_22"),
	EA_Achievement_23	UMETA(DisplayName = "ACHIEVEMENT_23"),
	EA_Achievement_24	UMETA(DisplayName = "ACHIEVEMENT_24"),
	EA_Achievement_25	UMETA(DisplayName = "ACHIEVEMENT_25"),
	EA_Achievement_26	UMETA(DisplayName = "ACHIEVEMENT_26"),
	EA_Achievement_27	UMETA(DisplayName = "ACHIEVEMENT_27"),
	EA_Achievement_28	UMETA(DisplayName = "ACHIEVEMENT_28"),
	EA_Achievement_29	UMETA(DisplayName = "ACHIEVEMENT_29"),
	EA_Achievement_30	UMETA(DisplayName = "ACHIEVEMENT_30"),
	EA_Achievement_31	UMETA(DisplayName = "ACHIEVEMENT_31"),
	EA_Achievement_32	UMETA(DisplayName = "ACHIEVEMENT_32"),
	EA_Achievement_33	UMETA(DisplayName = "ACHIEVEMENT_33"),
	EA_Achievement_34	UMETA(DisplayName = "ACHIEVEMENT_34"),
	EA_Achievement_35	UMETA(DisplayName = "ACHIEVEMENT_35"),
	EA_Achievement_36	UMETA(DisplayName = "ACHIEVEMENT_36"),
	EA_Achievement_37	UMETA(DisplayName = "ACHIEVEMENT_37"),
	EA_Achievement_38	UMETA(DisplayName = "ACHIEVEMENT_38"),
	EA_Achievement_39	UMETA(DisplayName = "ACH_FAKE_ALL_UNLOCKED"),
	EA_Achievement_40	UMETA(DisplayName = "ACHIEVEMENT_40"),
	EA_Achievement_MAX  UMETA(Hidden)
};

//************************************** ACHIEVEMENT OLD DB END   ********************************************************

//************************************** ACHIEVEMENT CHECKER START   ********************************************************

UCLASS(Abstract)
class NEBULAFLOW_API UNebulaFlowAchievementCheckerBase : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Achievements")
	FName UnlockingAction;

	virtual bool IsUnlockConditionValid(ULocalPlayer* InPlayer){ return false; }

};

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced)
class NEBULAFLOW_API UNebulaFlowAchievementCheckerGreaterOf : public UNebulaFlowAchievementCheckerBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Achievements")
	int32 ComparisonValue;

	virtual bool IsUnlockConditionValid(ULocalPlayer* InPlayer)
	{ 
		return false; 
	}
};

USTRUCT(Blueprintable, BlueprintType)
struct NEBULAFLOW_API FAchievementConfigRow : public FTableRowBase
{	
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Achievements")
	TSubclassOf<UNebulaFlowAchievementCheckerBase> AchievementChecker = nullptr;
};

//************************************** ACHIEVEMENT CHECKER END   ********************************************************




UCLASS(Blueprintable)
class NEBULAFLOW_API UNebulaFlowAchievementManager: public UNebulaFlowManagerBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Achievements")
	UDataTable* AchievementsTable = nullptr;

	UPROPERTY(EditAnywhere, Category = "Achievements")
	bool bSyncronizeOnlineAchievement = true;

	TArray<FName> UnlockedServerAchievements;

	UNebulaFlowAchievementManager(const FObjectInitializer& ObjectInitializer);

	virtual void InitManager(UNebulaFlowGameInstance* InstanceOwner) override;

	virtual void TickManager(float DeltaTime) override;

	void QueryAchievement(ULocalPlayer* inPlayer);
	
	virtual void UnlockAchievement(ULocalPlayer* inPlayer, const FName& AchievementId);

	virtual void UpdateOfflineAchievements();

	void InitServerAchievements(ANebulaFlowPlayerController* PlayerController);

private:
	UPROPERTY()
	TMap<FName, UNebulaFlowAchievementCheckerBase*> AchievementCheckersMap{};

	void UnlockAchievement_Internal(ULocalPlayer* inPlayer, const FName& AchievementId);
};