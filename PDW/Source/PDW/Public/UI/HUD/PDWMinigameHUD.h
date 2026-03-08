// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/PDWBaseHUD.h"
#include "Managers/PDWEventSubsytem.h"
#include "GameplayTagContainer.h"
#include "PDWMinigameHUD.generated.h"

class UPDWMinigamePlayersNavButton;
class UPDWMinigameConfigData;
class UPanelWidget;
class UPDWProgressBar;
class UPDWPlayerIndicator;
class UPDWMinigameMovingMesh;

UENUM(BlueprintType)
enum class EPlayerInput : uint8
{
	BothPlayers = 0,
	Player1 = 1,
	Player2 = 2,
};

USTRUCT(BlueprintType)
struct PDW_API FPDWMinigameInputData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FNavbarButtonData ButtonData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlayerInput InputPlayer = EPlayerInput::BothPlayers;
};

USTRUCT(BlueprintType)
struct PDW_API FPDWMinigameButtonInputData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "ID.Minigame"))
	FGameplayTag MinigameId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPDWMinigameInputData> InputsData;
};

USTRUCT(BlueprintType)
struct PDW_API FPDWMinigameHintWidgets
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY()
	USceneComponent* MinigameComp = nullptr;

	UPROPERTY()
	UUserWidget* HintWidget = nullptr;

	UPROPERTY()
	float CurrentLifeTime = 0.0f;

	UPROPERTY()
	FGameplayTag MinigameId {};

	bool operator==(const FPDWMinigameHintWidgets& Other) const
	{
		return MinigameComp == Other.MinigameComp;
	}
};

USTRUCT(BlueprintType)
struct PDW_API FPDWMinigameSmellTarget
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> TargetTexture = nullptr;
};

USTRUCT(BlueprintType)
struct PDW_API FPDWPlayerIndicatorOffsetData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EMiniGamePlayerType, FVector2D> OffsetByPlayer {};
};


/**
 * 
 */
UCLASS()
class PDW_API UPDWMinigameHUD : public UPDWBaseHUD
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitWithMinigameData(const UPDWMinigameConfigData* MinigameData);

	UFUNCTION(BlueprintCallable)
	const FGameplayTag GetCurentMinigameTag() {return CurrentMinigameTag;};

	UFUNCTION()
	void OnNotifySuccessFromMinigameActor(const FInteractionEventSignature& inEvent);

	UFUNCTION(BlueprintCallable)
	void ShowSmellTarget(FGameplayTagContainer MinigameID);

	void ManageTriggerDialogue(const FConversation& Conversation) override;
protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UPanelWidget* ActionContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UPanelWidget* MinigameContainer;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UPDWDialogueBaseWidget* TutorialComment;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPDWProgressBar* MinigameProgress;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> SmellTargetImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UPanelWidget* SmellTargetPanel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceInlineRow), Category = "Actions")
	TArray<FPDWMinigameButtonInputData> MinigameInputsData;

	UPROPERTY(EditAnywhere, Category = "Actions")
	TSubclassOf<UPDWMinigamePlayersNavButton> PlayersNavButton;

	UPROPERTY(EditAnywhere, Category = "Hint")
	TSubclassOf<UUserWidget> HintWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Hint")
	float HintTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Hint")
	TMap<FGameplayTag, FVector2D> HintOffset{};

	UPROPERTY(EditAnywhere, Category = "Hint")
	TArray<FGameplayTag> MinigameSequenceTags{};
	
	UPROPERTY(EditAnywhere, Category = "Hint|Dialogue")
	float LoopTime = 10.f;

	UPROPERTY(EditAnywhere, Category = "Hint|Dialogue", meta = (ForceInlineRow))
	TMap<FGameplayTag, FConversation> DialoguesHintConfiguration{};

	UPROPERTY(EditAnywhere, Category = "Minigame|Minigame Smell", meta = (ForceInlineRow))
	TMap<FGameplayTag, FPDWMinigameSmellTarget> MinigameSmellTargetConfiguration{};

	UPROPERTY(EditAnywhere, Category = "Minigame|Minigame Smell")
	FName SmellTextureParameterName = "Color_Alpha";

	UPROPERTY(EditAnywhere, Category = "Minigame|Minigame Smell")
	FName SmellProgressionParameterName = "ErosionValue";

	UPROPERTY(EditAnywhere, Category = "Minigame|PlayerIndicator", meta = (ForceInlineRow))
	TArray<FGameplayTag> MinigamePlayerIndicatorExpection{};

	UPROPERTY(EditAnywhere, Category = "Minigame|PlayerIndicator", meta = (ForceInlineRow))
	TMap<FGameplayTag, FPDWPlayerIndicatorOffsetData> PlayerIndicatorOffset{};

	UPROPERTY(EditAnywhere, Category = "Audio")
	FName StepSuccesSound = "MinigameStepSucces";

	UPROPERTY()
	bool bStartCount;

	UPROPERTY()
	float CurrentTime = 0.f;

	UPROPERTY()
	FGameplayTag CurrentMinigameTag{};

	UPROPERTY()
	TArray<FPDWMinigameHintWidgets> ActiveHintWidgets {};
	
	UPROPERTY(BlueprintReadOnly)
	uint8 TargetQuantity = 0;
	
	UPROPERTY(BlueprintReadOnly)
	uint8 CurrentQuantity = 0;

	//Minigame Moving Object
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPDWPlayerIndicator> PlayerIndicatorWidgetClass;

	UPROPERTY()
	TMap<AActor*, UPDWPlayerIndicator*> ActorPossesedObjects;
	
	UPROPERTY()
	TMap<UPDWMinigameMovingMesh*, UPDWPlayerIndicator*> MovingMeshPossesedObjects;
	
	UPROPERTY()
	bool bStartMoveInidcator = false;


	void NativeDestruct() override;

	UFUNCTION()
	void OnShowHint(USceneComponent* Owner,const FGameplayTag& inMinigameID);

	UFUNCTION()
	void OnTargetDeactivation(AActor* ActorDeactivated);

	UFUNCTION()
	void OnHintRequest(const FGameplayTag& inMinigameID);

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void SetHintWidgetTranslation(UUserWidget* WidgetToSet, const FVector3d& WorldPos, const FGameplayTag& inMinigameID);

	UFUNCTION()
	void StartTutorialDialogue(FGameplayTag MinigameId);

	UFUNCTION()
	void OnEndTutorialDialogue();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "InitWithMinigameData"))
	void BP_InitWithMinigameData(const UPDWMinigameConfigData* MinigameData);

	UFUNCTION()
	void MovePlayerIndicator(FVector WorldTransform, UPDWPlayerIndicator* WidgetToMove);

	UFUNCTION()
	void OnMovingMeshChange(UPDWMinigameMovingMesh* OldMovingMesh, UPDWMinigameMovingMesh* NewMovingMesh);
};
