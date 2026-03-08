// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PDWOverlapBehaviour.generated.h"

class UPDWInteractionReceiverComponent;
class UPDWInteractionComponent;

UENUM(BlueprintType,meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EOverlapBehaviourConfig : uint8
{
	None =					0<<0,
	EndOnSuccess =			1<<0,
	EndOnFail =				1<<1,
};
ENUM_CLASS_FLAGS(EOverlapBehaviourConfig);


USTRUCT(BlueprintType)
struct PDW_API FPDWOverlapBehaviourInizializzation
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	UPDWInteractionReceiverComponent* Receiver = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UPDWInteractionComponent* Sender = nullptr;
};

//# TODO - DM: With time refactor this and interaction behaviour to have 1 common class with basics functions
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class PDW_API UPDWOverlapBehaviour : public UObject
{
	GENERATED_BODY()

public:
	void InitBehaviour(FPDWOverlapBehaviourInizializzation Inizialization);
	void ExecuteBehaviour();
	void OnInteractionSuccessfull();
	void OnInteractionFailed();
	void EndBehaviour();
	void TickBehaviour(float inDeltaTime);
	UFUNCTION(BlueprintCallable)
	UPDWInteractionReceiverComponent* GetOwnerComponent();
	UFUNCTION(BlueprintCallable)
	UPDWInteractionComponent* GetSenderComponent();


protected:

	virtual void ExecuteBehaviour_Implementation() {}
	virtual void TickBehaviour_Implementation(float inDeltaTime) {}
	virtual void EndBehaviour_Implementation() {}
	virtual void OnInteractionSuccessfull_Implementation() {};
	virtual void OnInteractionFailed_Implementation() {};
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ExecuteBehaviour_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_TickBehaviour_Implementation(float inDeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_EndBehaviour_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnInteractionSuccessfull_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnInteractionFailed_Implementation();

	UPROPERTY()
	FPDWOverlapBehaviourInizializzation BehaviourInformation;

	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = EOverlapBehaviourConfig), Category = "Configuration")
	uint8 OverlapConfiguration;

};
