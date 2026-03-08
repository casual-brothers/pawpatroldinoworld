// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "InputTriggers.h"
#include "StructUtils/InstancedStruct.h"

#include "PDWMiniGameInputBehaviour.generated.h"



class UInputAction;
class UInputMappingContext;
class UPDWMinigameConfigComponent;

USTRUCT(BlueprintType)
struct PDW_API FInputInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool BoolInfo = false;
	
	UPROPERTY(BlueprintReadWrite)
	float Axis1D = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	FVector2D Axis2D = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FVector Axis3D = FVector::ZeroVector;
};

UENUM(BlueprintType)
enum class EInputActionDisplayType : uint8
{
	None,
	TransformOnScreen
};

USTRUCT(BlueprintType)
struct PDW_API FInputActionInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Configuration",meta = (Categories = "Input.Action.Minigame"))
	FGameplayTag InputActionsToUse {};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Configuration")
	TArray<ETriggerEvent> InputMode {};

	UPROPERTY(EditAnywhere,Category = "Configuration")
	bool bNotifySuccess = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "bNotifySuccess",EditConditionHides))
	FGameplayTag TargetIDToNotify = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "bNotifySuccess",EditConditionHides))
	FGameplayTag EventID = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere,Category = "Configuration")
	bool bNotifyMinigameGraph = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (EditCondition = "bNotifyMinigameGraph",EditConditionHides))
	FGameplayTag FlowNotifyEvent = FGameplayTag::EmptyTag;

};
UCLASS(Abstract,Blueprintable, EditInlineNew,DefaultToInstanced)
class PDW_API UPDWMiniGameInputBehaviour : public UObject
{
	GENERATED_BODY()
	
public:

	virtual void InitializeBehaviour(APDWPlayerController* inController,UPDWMinigameConfigComponent* inMiniGameComp);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UninitializeBehaviour();
	
	virtual void UninitializeBehaviour();

	virtual void ExecuteBehaviour(const FInputActionInstance& inInputInstance);

	UFUNCTION(Blueprintcallable)
	APDWPlayerController* GetControllerOwner() const { return Owner; }

	UFUNCTION(Blueprintcallable)
	UPDWMinigameConfigComponent* GetMiniGameOwner() const { return MiniGameOwner; }

	virtual void CustomTick(const float inDeltaTime);

	UFUNCTION(BlueprintPure)
	void GetInputActionsInfo(TArray<FInstancedStruct>& outInfo) {outInfo = InputActionsInfo;};


	UWorld* GetWorld() const override;

protected:

	virtual void ExecuteSkipBehaviour(const FInputActionInstance& inInputInstance,FGameplayTag inNotifyTag);
	virtual void ExecuteExitBehaviour(const FInputActionInstance& inInputInstance,FGameplayTag inNotifyTag);

	UPROPERTY(EditAnywhere,Category = "Configuration",meta = (BaseStruct = "/Script/PDW.InputActionInfo"))
	TArray<FInstancedStruct> InputActionsInfo;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ExecuteBehaviour_Implementation(const FInputActionInstance& inInputInstance);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_CustomTick_Implementation(const float inDeltaTime);
	
	virtual void OnProgressUpdate(const FInputActionInstance& inInputInstance, float PreviousTime, float CurrentTime);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ButtonPressProgress(const FInputActionInstance& inInputInstance, float PreviousTime, float CurrentTime);

	virtual void BindInputAction();
	virtual void BindTriggerInputActionGivenTag(const FGameplayTag& inTag);

	UFUNCTION()
	virtual void ClearBindingByHandle(int32 BindingHandle,APDWPlayerController* InController);

	void BindExitAndPauseAction();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnInit();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnUninit();

	UPROPERTY()
	UPDWMinigameConfigComponent* MiniGameOwner = nullptr;
	UPROPERTY()
	APDWPlayerController* Owner = nullptr;


private:

};
