// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/PDWMiniGameInputBehaviour.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "Data/PDWGameSettings.h"
#include "FlowComponent.h"
#include "Managers/PDWEventSubsytem.h"



void UPDWMiniGameInputBehaviour::InitializeBehaviour(APDWPlayerController* inController,UPDWMinigameConfigComponent* inMiniGameComp)
{
	Owner = inController;
	MiniGameOwner = inMiniGameComp;
	BindInputAction();
	UPDWEventSubsytem::Get(this)->OnNotifyUnBind.AddUniqueDynamic(this, &UPDWMiniGameInputBehaviour::ClearBindingByHandle);
	BP_OnInit();
}

void UPDWMiniGameInputBehaviour::UninitializeBehaviour()
{
	BP_UninitializeBehaviour();
	if (Owner)
	{
		UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(Owner->InputComponent);
		Input->ClearBindingsForObject(this);
	}
	UPDWEventSubsytem::Get(this)->OnNotifyUnBind.RemoveDynamic(this, &UPDWMiniGameInputBehaviour::ClearBindingByHandle);
	Owner = nullptr;
	MiniGameOwner = nullptr;
	BP_OnUninit();
}

void UPDWMiniGameInputBehaviour::ExecuteBehaviour(const FInputActionInstance& inInputInstance)
{
	BP_ExecuteBehaviour_Implementation(inInputInstance);
}

void UPDWMiniGameInputBehaviour::CustomTick(const float inDeltaTime)
{
	BP_CustomTick_Implementation(inDeltaTime);
}

UWorld* UPDWMiniGameInputBehaviour::GetWorld() const
{
	if (Owner)
	{
		return Owner->GetWorld();
	}
	return nullptr;
}

void UPDWMiniGameInputBehaviour::ExecuteSkipBehaviour(const FInputActionInstance& inInputInstance,FGameplayTag inNotifyTag)
{
	if(!MiniGameOwner)
		return;
	MiniGameOwner->GetMinigameFlowComponent()->NotifyOwnerGraph(inNotifyTag);
}

void UPDWMiniGameInputBehaviour::ExecuteExitBehaviour(const FInputActionInstance& inInputInstance,FGameplayTag inNotifyTag)
{
	if(!MiniGameOwner)
		return;
	MiniGameOwner->GetMinigameFlowComponent()->NotifyOwnerGraph(inNotifyTag);
}

void UPDWMiniGameInputBehaviour::OnProgressUpdate(const FInputActionInstance& inInputInstance, float PreviousTime, float CurrentTime)
{
	BP_ButtonPressProgress(inInputInstance,PreviousTime,CurrentTime);
}

void UPDWMiniGameInputBehaviour::BindInputAction()
{
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(Owner->InputComponent);
	if (ensureMsgf(InputActionsInfo.Num(), TEXT("Missing Inputs Actions")))
	{
		for (FInstancedStruct InstStruct : InputActionsInfo)
		{
			const UInputAction* InputAction = UPDWGameSettings::GetInputActionByTag(InstStruct.GetPtr<FInputActionInfo>()->InputActionsToUse);
			for (const auto& Mode : InstStruct.GetPtr<FInputActionInfo>()->InputMode)
			{
				Input->BindAction(InputAction, Mode, this, &UPDWMiniGameInputBehaviour::ExecuteBehaviour);
			}
		}
		//const UInputAction* InputAction = UPDWGameSettings::GetInputActionByTag(InputActionsInfo[0].GetPtr<FInputActionInfo>()->InputActionsToUse);	
		//for (const auto& Mode : InputActionsInfo[0].GetPtr<FInputActionInfo>()->InputMode)
		//{
		//	Input->BindAction(InputAction, Mode, this, &UPDWMiniGameInputBehaviour::ExecuteBehaviour);
		//}
	}
	BindExitAndPauseAction();
}

void UPDWMiniGameInputBehaviour::BindTriggerInputActionGivenTag(const FGameplayTag& inTag)
{
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(Owner->InputComponent);
	if (ensureMsgf(InputActionsInfo.Num(), TEXT("Missing Inputs Actions")))
	{
		const UInputAction* InputAction = UPDWGameSettings::GetInputActionByTag(inTag);
		Input->BindAction(InputAction, ETriggerEvent::Triggered, this, &UPDWMiniGameInputBehaviour::ExecuteBehaviour);
	}
}

void UPDWMiniGameInputBehaviour::ClearBindingByHandle(int32 BindingHandle, APDWPlayerController* InController)
{
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(Owner->InputComponent);
	Input->RemoveBindingByHandle(BindingHandle);
}

void UPDWMiniGameInputBehaviour::BindExitAndPauseAction()
{
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(Owner->InputComponent);
	const UInputAction* ExitAction = UPDWGameSettings::GetInputActionByTag(UPDWGameSettings::GetExitMinigameActionTag());
	Input->BindAction(ExitAction, ETriggerEvent::Triggered, this, &UPDWMiniGameInputBehaviour::ExecuteExitBehaviour, UPDWGameSettings::GetMinigameExitNotifyFlowTag());

	const UInputAction* SkipAction = UPDWGameSettings::GetInputActionByTag(UPDWGameSettings::GetSkipMinigameActionTag());
	Input->BindAction(SkipAction, ETriggerEvent::Triggered, this, &UPDWMiniGameInputBehaviour::ExecuteSkipBehaviour, UPDWGameSettings::GetMinigameSkipNotifyFlowTag());
}
