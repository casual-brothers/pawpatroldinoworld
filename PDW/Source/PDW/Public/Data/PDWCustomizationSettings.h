// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "PDWCustomizationSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Customization Settings"))
class PDW_API UPDWCustomizationSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	FGameplayTag PupCustomizationTag;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	FGameplayTag VehicleCustomizationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	FGameplayTag DinoCustomizationTag;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "CustomizationLocations")
	FName PupP1CustomizationLocationTag = "PupP1CustomizationLocationTag";

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "CustomizationLocations")
	FName PupP1MultiplayerCustomizationLocationTag = "PupP1MultiplayerCustomizationLocationTag";

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "CustomizationLocations")
	FName PupP2CustomizationLocationTag = "PupP2CustomizationLocationTag";

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "CustomizationLocations")
	FName VehicleP1CustomizationLocationTag = "VehicleP1CustomizationLocationTag";

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "CustomizationLocations")
	FName VehicleP1MultiplayerCustomizationLocationTag = "VehicleP1MultiplayerCustomizationLocationTag";

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "CustomizationLocations")
	FName VehicleP2CustomizationLocationTag = "VehicleP2CustomizationLocationTag";

public:
	
	static UPDWCustomizationSettings* Get();

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetPupCustomizationTag() {return Get()->PupCustomizationTag;};

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetVehicleCustomizationTag() {return Get()->VehicleCustomizationTag;};

	UFUNCTION(BlueprintPure)
	static const FGameplayTag GetDinoCustomizationTag() { return Get()->DinoCustomizationTag; };

	UFUNCTION(BlueprintPure)
	static const FName GetPupP1CustomizationLocationTag() { return Get()->PupP1CustomizationLocationTag; };
	
	UFUNCTION(BlueprintPure)
	static const FName GetPupP1MultiplayerCustomizationLocationTag() { return Get()->PupP1MultiplayerCustomizationLocationTag; };

	UFUNCTION(BlueprintPure)
	static const FName GetPupP2CustomizationLocationTag() { return Get()->PupP2CustomizationLocationTag; };

	UFUNCTION(BlueprintPure)
	static const FName GetVehicleP1CustomizationLocationTag() { return Get()->VehicleP1CustomizationLocationTag; };

	UFUNCTION(BlueprintPure)
	static const FName GetVehicleP1MultiplayerCustomizationLocationTag() { return Get()->VehicleP1MultiplayerCustomizationLocationTag; };

	UFUNCTION(BlueprintPure)
	static const FName GetVehicleP2CustomizationLocationTag() { return Get()->VehicleP2CustomizationLocationTag; };
};
