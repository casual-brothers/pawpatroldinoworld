// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WPLoadingHandlerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEntityEnabledDelegate);

class UBehaviorTree;

UENUM(BlueprintType,meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ESetUpBehavior : uint8
{
	None =				0<<0,
	Gravity =			1<<0,
	AILogic =			1<<1,
};
ENUM_CLASS_FLAGS(ESetUpBehavior);

UENUM(BlueprintType)
enum class EEntityState : uint8
{
	Disabled = 0,
	Enabled = 1,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WORLDPARTITIONHELPER_API UWPLoadingHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UWPLoadingHandlerComponent();

	//this function should be called only from the subsystem - don't want to use friend class.
	virtual void OnLoadingComplete();
	const EEntityState GetEntityState()const  { return CurrentState; };
	const bool GetEntityAvailable() const { return CurrentState == EEntityState::Enabled; };

	UPROPERTY(BlueprintAssignable)
	FOnEntityEnabledDelegate OnEntityEnabledDelegate;

protected:

	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = ESetUpBehavior, ToolTip = "For Character use EnableAILogic instead of Gravity"))
	uint8 SetUpBehavior;

	virtual void BeginPlay() override;

	virtual void EnableGravity();
	virtual void EnableAILogic();

private:

	EEntityState CurrentState = EEntityState::Disabled;
	
	UPROPERTY()
	AController* Controller = nullptr;
};
