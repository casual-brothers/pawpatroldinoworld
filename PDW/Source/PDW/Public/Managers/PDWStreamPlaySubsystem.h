// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "PDWStreamPlaySubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FPDWStreamPlaySessionChanged, bool);

UCLASS()
class PDW_API UPDWStreamPlaySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UPDWStreamPlaySubsystem* Get(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	bool IsAvailable() const;

	UFUNCTION(BlueprintCallable)
	bool IsSessionActive() const;

	UFUNCTION(BlueprintCallable)
	bool IsSessionPending() const;

	UFUNCTION(BlueprintCallable)
	bool StartGuestSession();

	UFUNCTION(BlueprintCallable)
	void StopGuestSession();

	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	FPDWStreamPlaySessionChanged OnSessionChanged;

private:
	void BindPlatformDelegates();
	void UnbindPlatformDelegates();
	void OnStreamPlayInitialized(const bool bStreamPlayInitialized);

	bool bIsGuestSessionActive = false;
	bool bIsGuestSessionPending = false;
	bool bHasBoundPlatformDelegates = false;
};
