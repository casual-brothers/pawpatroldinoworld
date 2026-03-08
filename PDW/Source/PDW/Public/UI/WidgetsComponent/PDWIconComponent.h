// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/WidgetsComponent/PDWWidgetComponentBase.h"
#include "GameplayTagContainer.h"
#include "Engine/Texture2D.h"
#include "PDWIconComponent.generated.h"

class UPDWIconWidget;
class APDWPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIconUpdated, bool, IsActive);


USTRUCT(BlueprintType)
struct PDW_API FPDWIconData
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> IconTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D ImageSize {64.f, 64.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UPDWIconWidget> CustomWidgetIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCheckForNearDistance = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCheckForFarDistance = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCheckHeightFromWidgetToActor = false;
};


UCLASS()
class PDW_API UPDWIconComponent : public UPDWWidgetComponentBase
{
	GENERATED_BODY()
	
protected:

	UPDWIconComponent();
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Configuration|Debug")
	FGameplayTagContainer IconDebugTag;
#endif

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayTagContainer IconTags;

	UPROPERTY(BlueprintAssignable)
	FOnIconUpdated OnIconUpdated;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", meta = (ForceInlineRow))
	TMap<FGameplayTag, FPDWIconData> IconsConfiguration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration")
    bool IsPlayer2Icon = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration|Distance Check", meta=(ForceUnits="m"))
    float NearRange = 3000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration|Distance Check", meta=(ForceUnits="m"))
    float FarRange = 8000.0f;

	TMap<int32, bool> PlayerNearStates;

	TMap<int32, bool> PlayerFarStates;

	bool bIsInGameplayState = true;

	bool bCheckForNearDistance = true;
	
	bool bCheckForFarDistance = true;

	bool bCheckForDistanceHeight = false;

	UPROPERTY()
	TSubclassOf<UPDWIconWidget> DefaultWidgetClass {};

	UPROPERTY()
	bool StayHidden = false;

	UPROPERTY()
	APDWPlayerController* PlayerControllerOwner {};

	FTimerHandle StartTimerHandle;

    UFUNCTION()
    void OnPlayerNear(int32 PlayerIndex, bool bIsNear);

    UFUNCTION()
    void OnPlayerFar(int32 PlayerIndex, bool bIsFar);

    UFUNCTION(BlueprintImplementableEvent, Category="Distance Events", meta = (DisplayName = "OnPlayerNear"))
    void BP_OnPlayerNear(int32 PlayerIndex, bool bIsNear);

    UFUNCTION(BlueprintImplementableEvent, Category="Distance Events", meta = (DisplayName = "OnPlayerFar"))
    void BP_OnPlayerFar(int32 PlayerIndex, bool bIsFar);

	UFUNCTION()
	void UpdateIcon();

	UFUNCTION()
	void OnGameplayStateEnter();

	UFUNCTION()
	void OnGameplayStateExit();

	UFUNCTION(BlueprintCallable)
	void ChangeWidgetVisibility(bool Hide);

	UFUNCTION()
	bool GetIconDataByCurrentTags(FPDWIconData& StructToFill);

	UFUNCTION()
	bool HaveThisConfigurationTags(const FGameplayTagContainer StructToCheck);

	UFUNCTION()
	void OnMultiplayerStateChange();

	UFUNCTION()
	virtual void HandleMultiplayerVisibility();

public:

	FGameplayTagContainer GetIconTag() const;

	UFUNCTION(BlueprintCallable)
	void OnTagAdded(const FGameplayTagContainer& TagContainer,const FGameplayTag& AddedTag);

	UFUNCTION(BlueprintCallable)
	void OnTagRemoved(const FGameplayTagContainer& TagContainer,const FGameplayTag& RemovedTag);

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type Reason) override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};