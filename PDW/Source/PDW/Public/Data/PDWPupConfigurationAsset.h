// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Gameplay/GAS/PDWGASAbility.h"
#include "StructUtils/InstancedStruct.h"
#include "PDWPupConfigurationAsset.generated.h"

 class APDWVehiclePawn;
 class APDWCharacter;
 class UNebulaGraphicsCustomizationComponent;

 USTRUCT(BlueprintType)
struct FPupCustomizationInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere,Category = "Customization", meta = (AllowedClasses = "/Script/NebulaGraphics.CustomizationDataAsset"))
	FSoftObjectPath CustomizationAsset = nullptr;

	UPROPERTY(Config, EditAnywhere,Category = "Customization")
	FGameplayTag CharacterCustomizationTag = FGameplayTag::EmptyTag;

	UPROPERTY(Config, EditAnywhere,Category = "Customization")
	FGameplayTag BackPackCustomizationTag = FGameplayTag::EmptyTag;

	UPROPERTY(Config, EditAnywhere,Category = "Customization")
	FGameplayTag HatCustomizationTag = FGameplayTag::EmptyTag;

	UPROPERTY(Config, EditAnywhere,Category = "Customization")
	FGameplayTag DefaultVehicleCustomizationTag = FGameplayTag::EmptyTag;
};

USTRUCT(BlueprintType)
struct FPupUIInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> PupImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText PupDisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FColor PupColor = FColor::White;
};

 USTRUCT(BlueprintType)
struct FEntity
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<APDWCharacter> PupClass = nullptr;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<APDWVehiclePawn> PupVehicle = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPDWGASAbility> PupBaseSkill = nullptr;

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer PupSkillType;

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer PupInteractions;

	UPROPERTY(EditAnywhere)
	FPupUIInfo PupUIInfo;

	UPROPERTY(EditAnywhere)
	FPupCustomizationInfo CustomizationInfo;
};

UCLASS(config = PupConfiguration)
class PDW_API UPDWPupConfigurationAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
	FEntity PupConfig;
};
