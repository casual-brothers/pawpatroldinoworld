// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

// Engine
#include "StructUtils/InstancedStruct.h"
#include "UI/NebulaFlowBasePage.h"
#include "Data/GameOptionsEnums.h"
#include "UI/NebulaFlowWidgetDataStructures.h"

#include "GameOptionsData.generated.h"

class UNebulaFlowBaseWidget;
class UGameOptionsSelectorWidget;
class UGameOptionsCategoryWidget;


USTRUCT()
struct FGameOptionsBaseData
{
	GENERATED_USTRUCT_BODY()

	friend class USettingsDefinitionDataAsset;

public:

	virtual ~FGameOptionsBaseData() {};

	// to override in every child
	virtual int32 GetNumberOfValues() const { return 0; }
	virtual FText GetCurrentValueText() const { return FText(); }
	virtual void SetToDefault() {}


	// general getters
	float GetCurrentPercent() const;
	int32 GetCurrentValueIndex() const { return CurrentValueIndex; }
	const EGameOptionsId GetId() { return ID; }
	FText GetDisplayName() const { return DisplayName; }
	bool GetIsAvailableInCurrentPlatform() const;
	TSubclassOf<UGameOptionsSelectorWidget> GetOverrideSelector() const { return OverrideSelector; }
	bool GetCycle() const { return bCycle; }

	// general setter
	void SetCurrentValueIndex(int32 InValueIndex);

protected:


	UPROPERTY(Config)
	int32 CurrentValueIndex{ 0 };

	UPROPERTY(Config, EditAnywhere, Category = "ID")
	EGameOptionsId ID{ EGameOptionsId::E_None };

	UPROPERTY(Config, EditAnywhere, Category = "UI")
	FText DisplayName{};

	UPROPERTY(Config, EditDefaultsOnly, Category = "UI")
	bool bCycle{ false };
	
	UPROPERTY(Config, EditAnywhere, Category = "UI")
	TSubclassOf<UGameOptionsSelectorWidget> OverrideSelector{ nullptr };
		
	UPROPERTY(Config, EditAnywhere, Category = "Settings", Meta = (Bitmask, BitmaskEnum = "EPlatformAvailability"))
	int32 PlatformAvailability = 63 + 64;

};



USTRUCT(BlueprintType)
struct FNumericGameOptionsData : public FGameOptionsBaseData
{
	GENERATED_USTRUCT_BODY()

public:

	virtual ~FNumericGameOptionsData() override {};

	virtual int32 GetNumberOfValues() const override;
	virtual FText GetCurrentValueText() const override;
	virtual void SetToDefault();

	float GetCurrentValue() const;
	float GetDefaultValue() const;
		
	UPROPERTY(Config, EditAnywhere, meta = (ClampMin = 0), Category = "ZNumeric")
	int32 DefaultValueIndex{ 0 };

protected:

	UPROPERTY(Config, EditAnywhere, Category = "ZNumeric")
	float MinValue{ 0.f };

	UPROPERTY(Config, EditAnywhere, Category = "ZNumeric")
	float MaxValue{ 1.f };

	UPROPERTY(Config, EditAnywhere, Category = "ZNumeric")
	int32 NumberOfValues{ 11 };


};

USTRUCT(BlueprintType)
struct FBoolGameOptionsData : public FGameOptionsBaseData
{
	GENERATED_USTRUCT_BODY()

public:

	virtual ~FBoolGameOptionsData() override {};

	virtual int32 GetNumberOfValues() const override;
	virtual FText GetCurrentValueText() const override;
	virtual void SetToDefault() override;

	bool GetCurrentValue() const;
	void SetCurrentValue(bool bValue);

protected:

	UPROPERTY(Config, EditAnywhere)
	bool DefaultValue{ false };

};

USTRUCT(BlueprintType)
struct FTextGameOptionsData : public FGameOptionsBaseData
{
	GENERATED_USTRUCT_BODY()

public:

	FTextGameOptionsData() { bCycle = true; }

	virtual ~FTextGameOptionsData() override {};

	virtual int32 GetNumberOfValues() const override;
	virtual FText GetCurrentValueText() const override;
	virtual void SetToDefault() override;

	FName GetCurrentKey() const;
	FText GetCurrentValue() const;
	FText GetDefaultValue() const;


	UPROPERTY(Config, EditAnywhere)
	TMap<FName, FText> Values{};

	UPROPERTY(Config, EditAnywhere)
	FName DefaultValue{ NAME_None };

};

USTRUCT(BlueprintType)
struct FQualityGameOptionsData : public FGameOptionsBaseData
{
	GENERATED_USTRUCT_BODY()

public:

	virtual ~FQualityGameOptionsData() override {};

	virtual int32 GetNumberOfValues() const override;
	virtual FText GetCurrentValueText() const override;
	virtual void SetToDefault() override;

	void SetCurrentValue(EQualityGameOptionsValue Value);

	EQualityGameOptionsValue GetCurrentValue() const;


protected:

	UPROPERTY(Config, EditAnywhere)
	EQualityGameOptionsValue DefaultValue{ EQualityGameOptionsValue::Medium };

	UPROPERTY(Config, EditAnywhere, Meta = (Bitmask, BitmaskEnum = "EQualityGameOptionsValue"))
	int32 QualityAvailability{ 15 };

};

USTRUCT(BlueprintType)
struct FAggregatedQualityGameOptionsData : public FQualityGameOptionsData
{
	GENERATED_USTRUCT_BODY()

public:

	virtual ~FAggregatedQualityGameOptionsData() override {};

	UPROPERTY(Config, EditAnywhere)
	TArray<EGameOptionsId> AffectedQualities {};

};


USTRUCT(BlueprintType)
struct FModeNavbarAction
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere)
	ENavElementPosition NavbarPosition{ ENavElementPosition::LEFT };

	UPROPERTY(Config, EditAnywhere)
	FName ActionName{ NAME_None };
};

USTRUCT()
struct FGameOptionsCategory
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(Config, EditAnywhere)
	FText GameOptionsCategoryName{};

	UPROPERTY(Config, EditAnywhere)
	FUIAudioEvents UIAudioEvents;

	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UTexture2D> CategoryImage{ nullptr };

	UPROPERTY(Config, EditAnywhere, meta = (ExcludeBaseStruct, BaseStruct = "/Script/PDW.GameOptionsBaseData"))
	TArray<FInstancedStruct> GameOptions{};
};

