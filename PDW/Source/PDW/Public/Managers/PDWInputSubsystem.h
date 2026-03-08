// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "InputMappingContext.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "PDWInputSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWInputSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	static const int32 DEFAULT_GAMEPLAY;
	static const int32 INTERACTION;
	static const int32 DEFAULT_MINIGAME;
	static const int32 DEFAULT_UI;

	UFUNCTION(BlueprintCallable)
	static UPDWInputSubsystem* Get(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static void ApplyMappingContextByTag(const FGameplayTag MapTag, const APlayerController* PlayerToApply);

	UFUNCTION(BlueprintCallable)
	static void RemoveInputs(const APlayerController* PlayerToApply);

	UFUNCTION(BlueprintCallable)
	static void ToggleInteractMap(bool Add, const APlayerController* PlayerToApply);

	UFUNCTION(BlueprintCallable)
	static void ToggleUIMap(bool Add, const APlayerController* PlayerToApply);
	
	UFUNCTION(BlueprintCallable)
	static void RestoreLastConfiguration(const APlayerController* PlayerToApply);


	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION()
	void UpdateInputSwitch(bool bSingleJoycon);


	void Deinitialize() override;

protected:
	
	UPROPERTY()
	FGameplayTagContainer Player1LastConfig{};

	UPROPERTY()
	FGameplayTagContainer Player2LastConfig{};

	UPROPERTY()
	bool bPlayer1CanInteract = false;

	UPROPERTY()
	bool bPlayer2CanInteract = false;
};
