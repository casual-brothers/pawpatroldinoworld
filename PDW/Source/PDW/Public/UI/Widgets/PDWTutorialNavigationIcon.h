// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Widgets/NebulaFlowNavigationIcon.h"
#include "Managers/PDWUIManager.h"
#include "Data/GameOptionsEnums.h"
#include "PDWTutorialNavigationIcon.generated.h"

class UPanelWidget;
class UWidgetAnimation;

UENUM(BlueprintType)
enum class EControllerConfiguration : uint8
{
	None,
	Microsoft,
	PS4,
	PS5,
	Switch_Handheld,
	Switch_DualJoycon,
	Switch_JoyconLeft,
	Switch_JoyconRight,
	Switch_ProController,
	SteamDeck
};

UENUM(BlueprintType)
enum class EPadKeyNames : uint8
{
	ControllerBase,
	Gamepad_FaceButton_Top,
	Gamepad_FaceButton_Right,
	Gamepad_FaceButton_Bottom,
	Gamepad_FaceButton_Left,
	Gamepad_RightThumbstick,
	Gamepad_LeftThumbstick,
	Gamepad_DPad,
	Gamepad_Special_Left,
	Gamepad_Special_Right,
	Gamepad_RightShoulder,
	Gamepad_LeftShoulder,
	Gamepad_RightTrigger,
	Gamepad_LeftTrigger
};

USTRUCT(BlueprintType)
struct PDW_API FPDWPlatformIconConfiguration
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPadKeyNames KeyName{ EPadKeyNames::ControllerBase };

	UPROPERTY(VisibleAnywhere)
	FName KeyNameString = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key")
	TSoftObjectPtr<UTexture2D> KeyTexture{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key")
	FVector2D KeyImageSize = FVector2D(32.f, 32.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Key")
	FWidgetTransform KeyImageTransform = FWidgetTransform();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material")
	TSoftObjectPtr<UMaterialInstance> MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material")
	FVector2D MaterialSize = FVector2D(64.f, 64.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material")
	FWidgetTransform MaterialTransform = FWidgetTransform();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FUIElementData ElementData;
};

USTRUCT(BlueprintType)
struct PDW_API FPDWPlatformIcons
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (TitleProperty="KeyNameString"))
	TArray<FPDWPlatformIconConfiguration> Icons{};

	FPDWPlatformIconConfiguration* FindKeyName(EPadKeyNames KeyNameToFound)
	{
		for (FPDWPlatformIconConfiguration& Element : Icons)
		{
			if (Element.KeyName == KeyNameToFound)
			{
				return &Element;
			}
		}
		return nullptr;
	};
};


UCLASS()
class PDW_API UPDWPlatformIconData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ForceInlineRow))
	TMap<EControllerConfiguration, FPDWPlatformIcons> PlatformConfiguration;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

};


/**
 * 
 */
UCLASS(Abstract, MinimalAPI)
class UPDWTutorialNavigationIcon : public UNebulaFlowNavigationIcon
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DebugText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPDWPlatformIconData* PlatformConfigurationData;
	
	UPROPERTY(BlueprintReadOnly, meta =(BindWidget))
	TObjectPtr<UPanelWidget> PadPanel{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FName> KeyNamesAlias{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EControllerConfiguration DebugControllerConfiguration = EControllerConfiguration::Microsoft;

	UPROPERTY()
	int32 IconsToLoad = 0;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_InitAction(const FString& InKeyName);

	UFUNCTION(BlueprintCallable)
	UWidgetAnimation* GetAnimationByName(FName AnimationName) const;

	UFUNCTION()
	EControllerConfiguration CheckCurrentConfiguration();

	UFUNCTION()
	void InitKeys();

	void NativePreConstruct() override;

#if WITH_EDITOR
	UFUNCTION()
	void SaveWidgetsData();
#endif
	
	UFUNCTION()
	void CheckConfigurationJoycon(bool bIsRemaped);

	UFUNCTION()
	void OnGameOptionChanged(EGameOptionsId SettingsId, int32 CurrentValueIndex);
public:
	void UpdateImage() override;

	void NativeConstruct() override;

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void NativeDestruct() override;

	UFUNCTION()
	void OnSingleKeyIconLoad();
};
