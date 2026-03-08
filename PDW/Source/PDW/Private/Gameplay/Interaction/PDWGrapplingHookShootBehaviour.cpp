// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/PDWGrapplingHookShootBehaviour.h"
#include "Managers/PDWEventSubsytem.h"
#include "Gameplay/Components/PDWTargetingComponent.h"
#include "Data/PDWGameSettings.h"
#include "Kismet/KismetArrayLibrary.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Algo/RandomShuffle.h"
//#include "EnhancedInputComponent.h"

UWorld* UPDWGrapplingHookShootBehaviour::GetWorld() const
{
	if (GetControllerOwner())
	{
		return GetControllerOwner()->GetWorld();
	}
	return nullptr;
}

void UPDWGrapplingHookShootBehaviour::InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp)
{
	Super::InitializeBehaviour(inController,inMiniGameComp);
	CurrentIndex = 0;
	ShuffledArray.Empty();
	//TArray<FInstancedStruct> Actions;
	//GetInputActionsInfo(Actions);
	//const FGrapplingHookConfiguration* HookConfig = Actions[0].GetPtr<FGrapplingHookConfiguration>();
	//ShuffledArray = HookConfig->ButtonSequenceConfig;
	UPDWEventSubsytem::Get(this)->OnGrapplingHookTargetAquired.AddUniqueDynamic(this,&UPDWGrapplingHookShootBehaviour::OnTargetAquired);
}

void UPDWGrapplingHookShootBehaviour::OnTargetAquired(const TArray<AActor*>& Targets, APDWPlayerController* ControllerSender)
{
	if (ControllerSender == GetControllerOwner())
	{
		CurrentTarget = Targets;
		if (Targets.Num() && Targets[0])
		{
			UPDWTargetingComponent* Comp = Targets[0]->FindComponentByClass<UPDWTargetingComponent>();
			if(Comp)
			{
				Comp->SetIsTargeted(true);
			}
			bIsValidTarget = true;
		}
		else
		{
			bIsValidTarget = false;
			if(CurrentTarget.Num()>0 && CurrentTarget[0])
			{
				UPDWTargetingComponent* Comp = CurrentTarget[0]->FindComponentByClass<UPDWTargetingComponent>();
				if(Comp)
				{
					Comp->SetIsTargeted(false);
				}
			}
		}
		//CurrentTarget.AddUnique(Targets[0]);
	}
	BP_OnTargetAquired(Targets,ControllerSender);
}

void UPDWGrapplingHookShootBehaviour::UpdateBind()
{
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(Owner->InputComponent);
	if (Input)
	{
		TArray<FInstancedStruct> Actions;
		GetInputActionsInfo(Actions);
		const FGrapplingHookConfiguration* HookConfig = Actions[0].GetPtr<FGrapplingHookConfiguration>();
		if (HookConfig)
		{
			TArray<UInputAction*> ActionsToRemove;
			for (const FGameplayTag& ActionTag : HookConfig->ActionToUnbind)
			{
				UInputAction* InputAction = UPDWGameSettings::GetInputActionByTag(ActionTag);
				if (InputAction)
				{
					ActionsToRemove.AddUnique(InputAction);
				}
			}
			UInputAction* CurrentInputAction = UPDWGameSettings::GetInputActionByTag(HookConfig->InputActionsToUse);
			ActionsToRemove.AddUnique(CurrentInputAction);
			for (const TUniquePtr<FEnhancedInputActionEventBinding>& Binding : Input->GetActionEventBindings())
			{
				for (UInputAction* Action : ActionsToRemove)
				{
					if(Binding->GetAction() == Action)
					{
						BindingHandleToRemove.AddUnique(Binding->GetHandle());
					}
				}
			}

			for (const int32& BindingHandle: BindingHandleToRemove)
			{
				UPDWEventSubsytem::Get(this)->NotifyUnBindEvent(BindingHandle, GetControllerOwner());
			}

			if (!HookConfig->ButtonSequenceConfig.IsValidIndex(CurrentIndex))
			{
				ShuffledArray.Empty();
				BindInputAction();//Binding original input of the behaviour.
				UPDWEventSubsytem::Get(this)->NotifyBindEvent(HookConfig->ActionToUnbind.First(),GetControllerOwner());//Trigger event to force behaviour to bind again.
				UPDWHUDSubsystem::Get(this)->RestoreSight(GetControllerOwner());
				BP_OnComplete();
				CurrentIndex = 0;
			}
			else
			{
				FInstancedStruct Test;
				if (HookConfig->bShuffleSequence)
				{
					Test =  FInstancedStruct::Make<FButtonSequenceConfiguration>(ShuffledArray[CurrentIndex]);
				}
				else
				{
					Test = FInstancedStruct::Make<FButtonSequenceConfiguration>(HookConfig->ButtonSequenceConfig[CurrentIndex]);
				}
				FNavButtonTriggerCallback TriggerCallback;
				TriggerCallback.BindLambda([&, this](const FInputActionInstance& inInputInstance)
					{
						ExecuteBehaviour(inInputInstance);
					});

				FNavButtonProgressCallback ProgressCallback;
				ProgressCallback.BindLambda([&, this](const FInputActionInstance& inInputInstance, float PreviousTime, float CurrentTime)
					{
						OnProgressUpdate(inInputInstance, PreviousTime, CurrentTime);
					});
				UPDWHUDSubsystem::Get(this)->RequestSightInputActionIcon(GetControllerOwner(), Test, TriggerCallback, ProgressCallback);
				CurrentIndex++;
			}
		}
	}
}

void UPDWGrapplingHookShootBehaviour::NotifySuccess()
{
	TArray<FInstancedStruct> Actions;
	GetInputActionsInfo(Actions);
	const FGrapplingHookConfiguration* HookConfig = Actions[0].GetPtr<FGrapplingHookConfiguration>();
	if (HookConfig)
	{
		FInteractionEventSignature InteractionEvent;
		InteractionEvent.EventTag = HookConfig->EventID;
		InteractionEvent.Interacter = GetControllerOwner();
		UPDWEventSubsytem::Get(GetControllerOwner())->TriggerInteractionEvent(HookConfig->TargetIDToNotify, InteractionEvent);
	}
}

void UPDWGrapplingHookShootBehaviour::ShuffleButtonSequence()
{
	TArray<FInstancedStruct> Actions;
	GetInputActionsInfo(Actions);
	FGrapplingHookConfiguration* HookConfig = Actions[0].GetMutablePtr<FGrapplingHookConfiguration>();
	ShuffledArray = HookConfig->ButtonSequenceConfig;
	Algo::RandomShuffle(ShuffledArray);
}
