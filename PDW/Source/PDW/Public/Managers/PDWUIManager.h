// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Managers/NebulaFlowUIManager.h"
#include "GameplayTagContainer.h"
#include "Components/CanvasPanelSlot.h"
#include "UI/Widgets/PDWTransitionWidget.h"
#include "PDWUIManager.generated.h"

#pragma region BaseWidgets Structures
UENUM(BlueprintType)
enum class EHudWidgetNames : uint8
{
	None = 0,
	PupTreats = 1,
	QuestLog = 2,
	Vehicle = 3,
	Skill = 4,
	Character = 5,
	PauseIcon = 6,
};

UENUM(BlueprintType)
enum class EVisualizationType : uint8
{
	None = 0 UMETA(Hidden),
	Singleplayer = 1,
	Multiplayer = 2,
	SingleplayerLarge = 3,
	MultiplayerLarge = 4,
};

USTRUCT(BlueprintType)
struct PDW_API FUIElementData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas Slot")
	FAnchorData LayoutData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas Slot", meta = (DisplayName = "Size To Content"))
	bool bAutoSize{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Canvas Slot")
	int32 ZOrder{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas Slot")
	FVector2D Position = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Overlay Slot, Horizontal or Vertical Box")
	FMargin Padding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Overlay Slot, Horizontal or Vertical Box")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Overlay Slot, Horizontal or Vertical Box")
	TEnumAsByte<EVerticalAlignment> VerticalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility")
	ESlateVisibility Visibility{ ESlateVisibility::HitTestInvisible };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility")
	float RenderOpacity{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Render Transform")
	FWidgetTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Render Transform")
	FVector2D Pivot{ FVector2D::ZeroVector };
};

USTRUCT(BlueprintType)
struct PDW_API FWidgetVisualizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVisualizationType VisualizationType{ EVisualizationType::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUIElementData UIElementData;
};

USTRUCT(BlueprintType)
struct PDW_API FWidgetVisualizationSet
{
	GENERATED_USTRUCT_BODY()

public:
	/** The unique name identifying the widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WidgetName{ NAME_None };

	/** Visualization data for each supported type (Singleplayer, Multiplayer, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(TitleProperty="{VisualizationType}"))
	TArray<FWidgetVisualizationData> Visualizations{};
};

UCLASS()
class PDW_API UPDWUIPageElementsData : public UDataAsset
{
	GENERATED_BODY()

public:
	/** List of all widget visualizations grouped by widget name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(TitleProperty="{WidgetName}"))
	TArray<FWidgetVisualizationSet> WidgetVisualizations{};

	UFUNCTION(BlueprintCallable)
	bool ContainsWidgetName(FName WidgetName)
	{
		for (FWidgetVisualizationSet& SingleSet : WidgetVisualizations)
		{
			if (SingleSet.WidgetName == WidgetName)
			{
				return true;
			}
		}
		return false;
	}

	UFUNCTION(BlueprintCallable)
	FUIElementData GetElementDataByName(FName WidgetName, EVisualizationType Visualization)
	{
		for (FWidgetVisualizationSet& SingleSet : WidgetVisualizations)
		{
			if (SingleSet.WidgetName == WidgetName)
			{
				bool bFound = false;
				for (FWidgetVisualizationData SingleVisualization : SingleSet.Visualizations)
				{
					if (SingleVisualization.VisualizationType == Visualization)
					{
						bFound = true;
						return SingleVisualization.UIElementData;
					}
				}
				if (!bFound)
				{
					if (Visualization == EVisualizationType::SingleplayerLarge)
					{
						return GetElementDataByName(WidgetName, EVisualizationType::Singleplayer);
					}
					else if (Visualization == EVisualizationType::MultiplayerLarge)
					{
						return GetElementDataByName(WidgetName, EVisualizationType::Multiplayer);
					}
				}
			}
		}
		return FUIElementData();
	}

	UFUNCTION(BlueprintCallable)
	void SetElementDataByName(FName WidgetName, EVisualizationType Visualization, FUIElementData& NewData)
	{
		for (FWidgetVisualizationSet& SingleSet : WidgetVisualizations)
		{
			if (SingleSet.WidgetName == WidgetName)
			{
				for (FWidgetVisualizationData& SingleVisualization : SingleSet.Visualizations)
				{
					if (SingleVisualization.VisualizationType == Visualization)
					{
						SingleVisualization.UIElementData = NewData;
					}
				}
			}
		}
	}
};
#pragma endregion

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllowedPupsChanged);

/**
 * 
 */
UCLASS(Blueprintable)
class PDW_API UPDWUIManager : public UNebulaFlowUIManager
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag UIMapTag{ FGameplayTag::EmptyTag };

	//This list follows the same order as the pups in the widget
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pups", meta = (Categories = "ID.Character"))
	TArray<FGameplayTag> DefaultAllowedPups{};

	UPROPERTY()
	TArray<FGameplayTag> CurrentAllowedPupsP1{};

	UPROPERTY()
	TArray<FGameplayTag> CurrentAllowedPupsP2{};

	UPROPERTY()
	TArray<FGameplayTag> CurrentHighlightedPups{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* CharacterInfoDT = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPDWTransitionWidget> TransitionWidgetClass{ nullptr };

	UPROPERTY()
	TArray<EHudWidgetNames> WidgetsHUDToHide{};

	UPROPERTY()
	TObjectPtr<UPDWTransitionWidget> CurrentTransitionWidget{ nullptr };

	UNebulaFlowDialog* ShowDialog(UGameInstance* GInstance, APlayerController* Owner, FName DialogID, TFunction<void(FString) > InCallBack, TArray<ANebulaFlowPlayerController*> SyncControllers ={}, TArray<FText> Params ={}, UTexture2D* Image = nullptr) override;
	bool HideDialog(UGameInstance* GInstance, UNebulaFlowDialog* DialogToClose) override;

	UFUNCTION()
	void OnPlayerAreaChange(const APDWPlayerController* Player, FGameplayTagContainer AllowedPups, bool EnterArea, const APDWAutoSwapArea* NewArea);

	UFUNCTION()
	void ChangeAllowedCharacterList(TArray<FGameplayTag>& ListToChange, FGameplayTagContainer NewAllowedPups);

public:
	void InitManager(UNebulaFlowGameInstance* InstanceOwner) override;

	void UninitManager() override;

	UFUNCTION(BlueprintCallable)
	void ChangeAllowedPups(FGameplayTagContainer NewAllowedPups, FGameplayTagContainer HighlightedPups = FGameplayTagContainer(), bool SaveIt = false);

	UFUNCTION(BlueprintCallable)
	void ResetAllowedPups(bool SaveIt = false);

	UFUNCTION(BlueprintPure)
	const TArray<FGameplayTag> GetDefaultAllowedPups() { return DefaultAllowedPups; };

	UFUNCTION(BlueprintPure)
	const TArray<FGameplayTag> GetCurrentAllowedPupsP1() { return CurrentAllowedPupsP1; };

	UFUNCTION(BlueprintPure)
	const TArray<FGameplayTag> GetCurrentAllowedPupsP2() { return CurrentAllowedPupsP2; };

	UFUNCTION(BlueprintPure)
	const TArray<FGameplayTag> GetCurrentHighlithedPups() { return CurrentHighlightedPups; };

	UFUNCTION(BlueprintPure)
	UDataTable* GetCharacterInfoDT() { return CharacterInfoDT; };

	UFUNCTION(BlueprintPure)
	const TArray<EHudWidgetNames> GetWidgetsHUDtoHide() {return WidgetsHUDToHide;};

	UFUNCTION(BlueprintCallable)
	void SetWidgetsHUDtoHide(EHudWidgetNames WidgetName, bool Add = true);

	const EDialogType CurrentModalIsTypeOf(const UNebulaFlowDialog* ModalToCheck);

	UPROPERTY(BlueprintAssignable)
	FOnAllowedPupsChanged OnAllowedPupsChanged;

	UFUNCTION(BlueprintCallable)
	void StartTransitionIn();
	
	UFUNCTION(BlueprintCallable)
	void ShowTransitionImmediatly();

	UFUNCTION(BlueprintCallable)
	void StartTransitionOut();
	
	UFUNCTION(BlueprintCallable)
	void RemoveTransition();
	
	UFUNCTION(BlueprintCallable)
	bool IsTransitionOn();
	
	UFUNCTION(BlueprintCallable)
	bool IsTransitionOnWithScreenBlack();
};
