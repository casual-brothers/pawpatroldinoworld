// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "NebulaFlowNavbarDataStructures.generated.h"

class UNebulaFlowNavbarButton;
class UInputAction;

USTRUCT(Blueprintable, BlueprintType)
struct NEBULAFLOW_API FNavbarButtonData
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category ="UI")
	FName ButtonAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	UInputAction* ButtonInputAction=nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	FText ButtonText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	FColor ButtonTextColor = FColor::White;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	bool bListenAction = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI", meta = (EditCondition = "bListenAction"))
	bool bListenAnyPlayer = false;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = "UI")
	FName UISound = NAME_None;

	//Deprecated
	UPROPERTY()
	bool bConsumeInput = true;

	UPROPERTY()
	bool bIsAxis = false;

	UPROPERTY()
	float AxisScale = 0.f;
};

USTRUCT(Blueprintable, BlueprintType)
struct NEBULAFLOW_API FNavbarButtonOverrideInfo 
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "UI")
		FString KeyboardIconName{};

	UPROPERTY(EditAnywhere, Category = "UI")
		FString PadIconName{};

};

USTRUCT(Blueprintable, BlueprintType)
struct NEBULAFLOW_API FNavbarButtonDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UNebulaFlowNavbarButton> NavbarButtonClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	FNavbarButtonData NavbarButtonData;

};

UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowNavigationIconsAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UNebulaFlowNavigationIconsAsset(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FString MultiIconPrefix = FString("MULTI_");

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TMap<FName, FNavbarButtonOverrideInfo> OverrideInfosMap{};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		FString PS4Path {};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		FString PS5Path {};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		FString SwitchPath {};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		FString SwitchHorizontalLeftPath {};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		FString SwitchHorizontalRightPath {};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		FString XboxOnePath {};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		FString XSXPath {};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FString KeyboardPath;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TArray<TSoftObjectPtr<UInputMappingContext>> GameMappings;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TArray<TSoftObjectPtr<UInputMappingContext>> SwitchGameMappings;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TArray<TSoftObjectPtr<UInputMappingContext>> SwitchGameMappingsSingleJoycon;
};