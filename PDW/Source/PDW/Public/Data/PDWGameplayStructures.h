// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "Managers/PDWDialogueSubSystem.h"
#include "FlowTypes.h"
#include "PDWGameplayStructures.generated.h"

class ULevelSequence;
class UPDWDinoPen;
class UFileMediaSource;
class UBinkMediaPlayer;
class APDW_NavigationWaypoint;

UCLASS()
class PDW_API UPDWSequenceData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<ULevelSequence> CutsceneSequence{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsCinematic{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool WaitStartDialogueEvent{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool OnPlayer{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "OnPlayer == false"))
	FName FramedActorKey{ NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool UseTransformOrigin{ true };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool UseFadeIn{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool UseFadeTransition{ false };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool UseAsyncLoad{ true };
};

USTRUCT(BlueprintType)
struct PDW_API FPDWSequenceDataElement
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<ULevelSequence> CutsceneSequence{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsCinematic{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "IsCinematic == false", EditConditionHides))
	bool WaitStartDialogueEvent{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "IsCinematic == false", EditConditionHides))
	FConversation Conversation{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool OnPlayer{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool HidePlayer{ true };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "OnPlayer == false", EditConditionHides))
	FTransform CustomLocation{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "OnPlayer == false", EditConditionHides))
	FGameplayTagContainer FramedActorKey{};
};

USTRUCT(BlueprintType)
struct PDW_API FPDWSequenceCollection
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFMODEvent* CutsceneSoundtrack{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CutsceneSoundtrackID{ NAME_None };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsCinematicAndCutscene{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "IsCinematicAndCutscene == false"))
	bool IsCinematic{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "SequenceCollection"))
	TArray<FPDWSequenceDataElement> SequenceCollectionNew{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bMakeTransitionAtEndCustcene = false;
};

UENUM(BlueprintType)//CounterType
enum class ECounterType : uint8
{
	NoCounter,
	InteractionCounter,
	CollectCounter
};

UENUM(BlueprintType)//CounterType
enum class EPupTreatsEvent : uint8
{
	OnInteractionSuccess,
	OnStateChange
};

USTRUCT(BlueprintType)
struct PDW_API FPDWQuestTargetData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestTarget")
	bool bQuestIconActive{ true };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestTarget")
	bool bUsePaleoQuestIcon{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestTarget")
	bool bBreadCrumbsActive{ true };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestTarget")
	bool IsMain{ true };

	UPROPERTY()
	FGameplayTagContainer IdentityTag {};

	int32 TargetQuantity = 0;

	int32 CurrentQuantity = 0;

	UPROPERTY(SaveGame)
	TArray<int32> FakeTargetsUsed{};
};

USTRUCT(BlueprintType)
struct PDW_API FPDWStepDescriptionData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category = "StepDescription")
	FText QuestDescription {};
};

USTRUCT(BlueprintType)
struct PDW_API FPDWQuestStepData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECounterType CounterType{ ECounterType::NoCounter };

	UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = "Targets")
	FPDWQuestTargetData TargetsData{};

	UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = "Description")
	FPDWStepDescriptionData DescriptionData{};
};

UENUM(BlueprintType)
enum class EUnlockableType : uint8
{
	None UMETA(Hidden),
	DinoFound,
	PupCustomizable,
	DinoCustomizable,
	DinoEggs,
	DinDenDecoration,
};

UCLASS(BlueprintType)
class PDW_API UPDWUnlockableData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ID{ FGameplayTag::EmptyTag };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnlockableType UnlockableType{ EUnlockableType::None };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText UnlockableName {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText UnlockableDescription {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText UnlockableSecondaryDescription {};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> UnlockableTexture{ nullptr };
};

USTRUCT(BlueprintType)
struct PDW_API FPDWUnlockableContainer
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UPDWUnlockableData>> Unlocakbles{};
};

UCLASS()
class PDW_API UPDWLevelUpRewardsData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	int32 ExpToLevelUp{ 100 };

	UPROPERTY(EditAnywhere)
	TMap<int32, FPDWUnlockableContainer> Reward{};
};

USTRUCT(BlueprintType)
struct PDW_API FPDWQuestTargetActor
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer TargetTag{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> TargetActor{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector TargetLocation{ FVector::ZeroVector };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector BreadcrumbTargetOffset{ FVector::ZeroVector };
};

UENUM(BlueprintType)
enum class EQuestMessageType : uint8
{
	StartQuest = 0,
	StartStep = 1,
	EndStep = 2,
	EndQuest = 3,
	MinigameCompleted = 4
};

USTRUCT(BlueprintType)
struct PDW_API FPDWQuestMessageData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText QuestName{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EQuestMessageType QuestMessageType{ EQuestMessageType::StartQuest };
};

USTRUCT(BlueprintType)
struct PDW_API FPDWTriggerActionData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Action = "";
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Parameter = "";
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<APlayerController> ControllerSender{ nullptr };
};

UCLASS()
class PDW_API UPDWPaleoCenterConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, int32> MaxNeeds{};

	UPROPERTY(EditAnywhere)
	float UpdateDinoNeedsInterval{ 0.0f };

	UPROPERTY(EditAnywhere)
	float UpdateFoodAndWaterInterval{ 0.0f };

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UPDWDinoPen>> DinoPens{};
};

USTRUCT(BlueprintType)
struct PDW_API FPDWQuestTargetLocs
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer TagContainer{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<int32, FVector> Locations{};

	UPROPERTY(BlueprintReadOnly)
	EFlowTagContainerMatchType MatchType{ EFlowTagContainerMatchType::HasAll };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MinigameInfo")
	FText Name{};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MinigameInfo")
	FGameplayTag AllowedPup{ FGameplayTag::EmptyTag };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MinigameInfo")
	TMap<TObjectPtr<UDataLayerAsset>, bool> MinigameLayersInfo{};

};

UCLASS(BlueprintType)
class PDW_API UPDWQuestConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPDWQuestTargetLocs> TargetLocations{};
};

USTRUCT(BlueprintType)
struct PDW_API FPDWPenAreaBounds
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FVector PenAreaCenter{ FVector::ZeroVector };

	UPROPERTY(BlueprintReadOnly)
	float MaxMoveRadius{ 0.f };

	UPROPERTY(BlueprintReadOnly)
	FBox SOSearchBox{};
};

class PDW_API UPDWGameplayStructures : public UObject{};

USTRUCT(BlueprintType)
struct PDW_API FPDWDinoPenEggs
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag EggId{ FGameplayTag::EmptyTag };

	UPROPERTY(BlueprintReadOnly)
	int32 EggCollected{ 0 };

	UPROPERTY(BlueprintReadOnly)
	int32 MaxEggs{ 0 };

	UPROPERTY(BlueprintReadOnly)
	bool IsBabyDinoBorn{ false };
};

USTRUCT(BlueprintType)
struct PDW_API FPDWDinoPenDecorations
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag DinoId{ FGameplayTag::EmptyTag };

	UPROPERTY(BlueprintReadOnly)
	int32 DecorationsUnlocked{ 0 };

	UPROPERTY(BlueprintReadOnly)
	int32 MaxDecorations{ 0 };
};

USTRUCT(BlueprintType)
struct PDW_API FPDWDinoPenNeed
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag DinoNeed{ FGameplayTag::EmptyTag };

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag NeededItem{ FGameplayTag::EmptyTag };

	UPROPERTY(BlueprintReadOnly)
	bool bPlayerHaveTheItem{ false };

	UPROPERTY(BlueprintReadOnly)
	bool bDinoWaitingPlayerHelp{ false };
};

USTRUCT(BlueprintType)
struct PDW_API FPDWDinoPenInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag DinoPenTag{ FGameplayTag::EmptyTag };

	UPROPERTY(BlueprintReadOnly)
	FPDWDinoPenEggs DinoPenEggs{};
	
	UPROPERTY(BlueprintReadOnly)
	FPDWDinoPenDecorations DinoPenDecorations{};

	UPROPERTY(BlueprintReadOnly)
	FPDWDinoPenNeed DinoNeed{};

	UPROPERTY(BlueprintReadOnly)
	FText DinoDisplayName{};

	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> DinoDisplayIcon{ nullptr };

	UPROPERTY(BlueprintReadOnly)
	bool IsPenUnlocked{ false };
};

USTRUCT(BlueprintType)
struct PDW_API FPDWEggFlowConfig
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag PenTag{ FGameplayTag::EmptyTag };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag EggTag{ FGameplayTag::EmptyTag };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer NestTag{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag OutNestTeleportTag{ FGameplayTag::EmptyTag };
};

USTRUCT(BlueprintType)
struct PDW_API FPDWFoodFlowConfig
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer FoodPlantTag{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer FoodStationTag{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag FoodTypeTag{ FGameplayTag::EmptyTag };
};

USTRUCT(BlueprintType)
struct FPDWPairingRemappingIMcs
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DESIGN)
	TArray<UInputMappingContext*> IMCs{};	
};


USTRUCT(BlueprintType)
struct FPDWInputMapsToUpdate
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DESIGN)
	TObjectPtr<UInputMappingContext> Map{ nullptr };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DESIGN)
	FText Title{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DESIGN)
	TArray<UInputAction*> RemappableActions{};
};


USTRUCT(BlueprintType)
struct FPDWRemappingAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DESIGN)
	FText ActionName{}; // Set the display name on the remappable button 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DESIGN)
	UInputAction* Action{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DESIGN)
	TArray<UInputMappingContext*> MapsToUpdate{};
};

USTRUCT(BlueprintType)
struct PDW_API FQuestData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "SaveData")
	FGameplayTag QuestID{ FGameplayTag::EmptyTag };

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "SaveData")
	FGameplayTag AreaID{ FGameplayTag::EmptyTag };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveData")
	bool IsCompleted{ false };
};

USTRUCT(BlueprintType)
struct FPDWMenuVideo
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category = "Video")
	TSoftObjectPtr<UFileMediaSource> SwitchVideoToPlay = nullptr;

	UPROPERTY(EditAnywhere, Category = "Video")
	TSoftObjectPtr<UBinkMediaPlayer> BinkVideo{ nullptr };
};

USTRUCT(BlueprintType)
struct PDW_API FPDWWaypointData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<APDW_NavigationWaypoint*> Waypoints = {};
};
