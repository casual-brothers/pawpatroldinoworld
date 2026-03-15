// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TriggerVolume.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilities/Public/GameplayAbilitySpec.h"
#include "PDWAutoSwapArea.generated.h"

class UPDWGASAbility;
class APDWPlayerController;

UENUM(BlueprintType,meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EAutoSwapRule : uint8
{
	None =					0<<0,
	VehicleOnly =			1<<0,
	SkillOnly =				1<<1,
};
ENUM_CLASS_FLAGS(EAutoSwapRule);

USTRUCT(BlueprintType)
struct PDW_API FPDWAutoSwapPupConfiguration
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoSwapPupConfiguration")
	TSubclassOf<UPDWGASAbility> AbilityToTrigger{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoSwapPupConfiguration")
	TSubclassOf<UPDWGASAbility> AllowedAbility{ nullptr };

	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = "/Script/PDW.EAutoSwapRule"))
	uint8 AutoSwapRule{ 0 };

};

UCLASS()
class PDW_API APDWAutoSwapArea : public ATriggerVolume
{
	GENERATED_BODY()
	
	APDWAutoSwapArea();

public:

	const FGameplayTagContainer& GetAllowedPup() const;
	const FGameplayTagContainer& GetAllowedVehicle() const;
	const FGameplayTag& GetDefaultAllowedPup() const;

	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FGameplayTag DefaultAllowedPup{ FGameplayTag::EmptyTag };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FGameplayTagContainer AllowedPups{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FGameplayTagContainer AllowedVehicles{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration",meta = (ForceInlineRow))
	TMap<FGameplayTag, FPDWAutoSwapPupConfiguration> AutoSwapConfiguration{};

	FGameplayAbilitySpecHandle AbilitySpecHandle{};

	UFUNCTION()
	void DisableLogic();
	UFUNCTION()
	void EnableLogic(AActor* Actor);

	UFUNCTION()
	void OnCustomizationCompleted(APDWPlayerController* inController);

	UFUNCTION()
	void OnVehicleSwap(bool bInVehicleMode,APDWPlayerController* inController);

	bool bLogicDisabled = false;

	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
