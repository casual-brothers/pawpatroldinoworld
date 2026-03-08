// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "Data/GameOptionsData.h"
#include "Data/PDWGameSettings.h"
#include "GameOptionsDeveloperSettings.generated.h"

class UPDWInputsData;


/**
 *
 */
UCLASS(Config = GameOptionsSettings, DefaultConfig, meta = (DisplayName = "Game options"))
class PDW_API UGameOptionsDeveloperSettings : public UDeveloperSettings
{
    GENERATED_BODY()

	friend class UGameOptionsFunctionLibrary;

public:
    static const UGameOptionsDeveloperSettings* Get();

    virtual FName GetContainerName() const override { return TEXT("Project"); }
    virtual FName GetCategoryName() const override { return TEXT("Nebula"); }
    virtual FName GetSectionName() const override { return TEXT("GameOptions"); }

protected:
    UPROPERTY(Config, EditAnywhere, Category = "GameOptions")
	TMap<EGameOptionsCategory, FGameOptionsCategory	> GameOptionsData{};

public:

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "UI", meta = (GetKeyOptions = "GetDefaultSelectorsKeyOptions"))
	TMap<FName, TSubclassOf<UGameOptionsSelectorWidget>> DefaultSelectorWidgets;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "UI")
	TSubclassOf<UGameOptionsCategoryWidget> CategoryWidget;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "UI")
	TMap<EQualityGameOptionsValue, FText> QualityTexts;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "UI")
	TMap<bool, FText> BoolTexts;


#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;

	UFUNCTION()
	TArray<FName> GetDefaultSelectorsKeyOptions() const;

#endif

public:

    UPROPERTY(EditAnywhere, Config, Category="Input Data", meta=(AllowedClasses="PDWInputsData"))
    TSoftObjectPtr<UPDWInputsData> DefaultInputData;
	

};