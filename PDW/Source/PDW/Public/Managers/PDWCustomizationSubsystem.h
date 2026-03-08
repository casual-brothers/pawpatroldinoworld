// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "StructUtils/InstancedStruct.h"
#include "GameplayTagContainer.h"
#include "PDWCustomizationSubsystem.generated.h"

class APDWCustomizationLocation;

USTRUCT(BlueprintType)
struct FCollectionData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ID{ FGameplayTag::EmptyTag };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInstancedStruct CustomData{};

	UPROPERTY(BlueprintReadWrite)
	bool bIsUnlocked = true;
};

USTRUCT(BlueprintType)
struct FCollectionDataArray
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCollectionData> CollectionData{};
};

USTRUCT(BlueprintType)
struct FCustomizationPupData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInstancedStruct PupUIData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCollectionDataArray PupCustomizationData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCollectionDataArray VehicleCustomizationData{};
};

USTRUCT(BlueprintType)
struct FCustomizationDinoData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCollectionDataArray DinoCustomizationData{};
};

USTRUCT(BlueprintType)
struct FCustomizationLocationDinoData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY()
	APDWCustomizationLocation* DinoLocation{ nullptr };

	UPROPERTY()
	FTransform SafeSpotP1{};

	UPROPERTY()
	FTransform SafeSpotP2{};
};

/**
 * 
 */
UCLASS()
class PDW_API UPDWCustomizationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	TMap<FGameplayTag, FCustomizationPupData> CustomizationPupDataMap{};
	
	UPROPERTY()
	TMap<FGameplayTag, FCustomizationDinoData> CustomizationDinoDataMap{};

	UFUNCTION()
	void FillCustomizationData();

	UFUNCTION()
	void FillCustomizationLocation(FName Tag, APDWCustomizationLocation*& CustomizationToFill);

	UFUNCTION()
	void FillDinoCustomizationLocations();

public:
	static UPDWCustomizationSubsystem* Get(UObject* WorldContextObject);
	
	void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION(BlueprintPure)
	const TMap<FGameplayTag, FCustomizationPupData>& GetCustomizationPupDataMap() {return CustomizationPupDataMap;};

	UFUNCTION(BlueprintPure)
	const TMap<FGameplayTag, FCustomizationDinoData>& GetCustomizationDinoDataMap() {return CustomizationDinoDataMap;};

	UFUNCTION(BlueprintPure)
	FCustomizationDinoData GetCustomizationDinoDataByTag(FGameplayTag TagToSearch);

	UPROPERTY()
	APDWCustomizationLocation* PupP1CustomizationLocation{ nullptr };

	UPROPERTY()
	APDWCustomizationLocation* PupP1MultiplayerCustomizationLocation{ nullptr };

	UPROPERTY()
	APDWCustomizationLocation* PupP2CustomizationLocation{ nullptr };

	UPROPERTY()
	APDWCustomizationLocation* VehicleP1CustomizationLocation{ nullptr };

	UPROPERTY()
	APDWCustomizationLocation* VehicleP1MultiplayerCustomizationLocation{ nullptr };

	UPROPERTY()
	APDWCustomizationLocation* VehicleP2CustomizationLocation{ nullptr };

	UPROPERTY()
	TMap<FGameplayTag, FCustomizationLocationDinoData> DinoLocations{};
};