// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GAS/PDWGASAbility.h"
#include "Gameplay/GAS/PDWAbilityTask_OnTick.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Gameplay/Components/PDWInteractionComponent.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "GameplayTagContainer.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Gameplay/Animation/PDWAnimInstance.h"

UPDWGASAbility::UPDWGASAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	auto ImplementedInBlueprint = [](const UFunction* Func) -> bool
		{
			return Func && ensure(Func->GetOuter())
				&& Func->GetOuter()->IsA(UBlueprintGeneratedClass::StaticClass());
		};

	{
		static FName FuncName = FName(TEXT("K2_Tick"));
		UFunction* ShouldRespondFunction = GetClass()->FindFunctionByName(FuncName);
		bHasBlueprintTickEnabled = ImplementedInBlueprint(ShouldRespondFunction);
	}
}

void UPDWGASAbility::TriggerAbilityLogic_Implementation()
{

}

void UPDWGASAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bInputReleased = false;
	if (bTickAbility || bHasBlueprintTickEnabled)
	{
		TickTask = UPDWAbilityTask_OnTick::AbilityTaskOnTick(this, NAME_None);
		TickTask->OnTick.AddUniqueDynamic(this, &ThisClass::Tick);
		TickTask->ReadyForActivation();
	}
	PlayerController = Cast<APDWPlayerController>(GetAvatarActorFromActorInfo());
	if(!IsValid(PlayerController))
		return;
	if (UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld()) == PlayerController)
	{
		PlayerState = UPDWGameplayFunctionLibrary::GetPlayerStateOne(GetWorld());
	}
	else if (UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(GetWorld()) == PlayerController)
	{
		PlayerState = UPDWGameplayFunctionLibrary::GetPlayerStateTwo(GetWorld());
	}
	SetUpInputBehavior(AbilityInputConfiguration);
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	OverrideMontageToPlaySettings();
	if (MontageSettings.MontageToPlay)
	{
		MeshComp = PlayerController->GetPawn()->FindComponentByTag<USkeletalMeshComponent>(MontageSettings.PupBackPackTag.GetTagName());
		if(MeshComp)
		{
			PupBackPackAnimInstance = MeshComp->GetAnimInstance();
			PupBackPackAnimInstance->Montage_Play(MontageSettings.MontageToPlay);
			if (MontageSettings.bEndSkillWithMontageEnd)
			{
				MontageEndedDelegate.BindUObject(this,&ThisClass::OnMontageEnded);
				PupBackPackAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageSettings.MontageToPlay);
			}
			PupBackPackAnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this,&UPDWGASAbility::OnNotify);
		}
	}
	if (MontageSettings.bActivateSkillState)
	{
		if (PlayerController && PlayerController->GetPawn())
		{
			const FGameplayTag GameplayTag = FGameplayTag::RequestGameplayTag("ComponentTag.CharacterMesh");
			if (GameplayTag.IsValid())
			{
				const FName Tag = FGameplayTag::RequestGameplayTag("ComponentTag.CharacterMesh").GetTagName();
				USkeletalMeshComponent* PupMeshComp = PlayerController->GetPawn()->FindComponentByTag<USkeletalMeshComponent>(Tag);
				if (PupMeshComp)
				{
					PupAnimInstance = Cast<UPDWAnimInstance>(PupMeshComp->GetAnimInstance());
					if (PupAnimInstance)
					{
						PupAnimInstance->SetAnimationState(EAnimationState::Skill);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp,Warning,TEXT("GASAbility - Activate ability: Missing MeshTag"));
			}
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("GASAbility - Activate ability: Missin controller or pawn"));
		}
	}
}

void UPDWGASAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (MontageSettings.bActivateSkillState)
	{
		if (PupAnimInstance)
		{
			PupAnimInstance->SetAnimationState(EAnimationState::Walking);
		}
	}
	if (bTickAbility || bHasBlueprintTickEnabled)
	{
		if (TickTask)
		{
			TickTask->OnTick.RemoveAll(this);
			TickTask->EndTask();
			TickTask = nullptr;
		}
	}
	if (PupBackPackAnimInstance && MontageSettings.MontageToPlay)
	{
		MontageEndedDelegate.Unbind();
		PupBackPackAnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this,&UPDWGASAbility::OnNotify);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPDWGASAbility::OnReleaseInput(FGameplayEventData inPayLoad)
{
	bInputReleased = true;
	BP_OnReleaseInput();
}

void UPDWGASAbility::OnRestartAnimation(FGameplayEventData inPayLoad)
{
	bInputReleased = false;
	if (PupBackPackAnimInstance)
	{
		PupBackPackAnimInstance->Montage_SetNextSection(MontageSettings.EndingSection,MontageSettings.StartingSection);
	}
}

void UPDWGASAbility::OnLoopRequest(FGameplayEventData inPayLoad)
{
	bInputReleased = false;
	if (PupBackPackAnimInstance)
	{
		PupBackPackAnimInstance->Montage_SetNextSection(MontageSettings.SectionToLoop,MontageSettings.SectionToLoop);
	}
}

void UPDWGASAbility::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void UPDWGASAbility::OnNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == MontageSettings.NotifyName)
	{
		TriggerAbilityLogic();
	}
	else if (NotifyName == MontageSettings.NotifyNameForInputCheck)
	{
		if (!bInputReleased && (uint8)EAbilityInputBehavior::ShouldLoopOnHold)
		{
			OnLoopRequest(FGameplayEventData());
		}
	}
}

void UPDWGASAbility::OnCancelAbility()
{

}

UAnimInstance* UPDWGASAbility::GetAnimInstance()
{
	if (!PupBackPackAnimInstance)
	{
		MeshComp = PlayerController->GetPawn()->FindComponentByTag<USkeletalMeshComponent>(MontageSettings.PupBackPackTag.GetTagName());
		if (MeshComp)
		{
			PupBackPackAnimInstance = MeshComp->GetAnimInstance();
		}
	}
	return PupBackPackAnimInstance;
}

void UPDWGASAbility::OverrideMontageToPlaySettings()
{

}

void UPDWGASAbility::Tick(float DeltaTime)
{
	K2_Tick(DeltaTime);
}

void UPDWGASAbility::SetUpInputBehavior(const uint8 inInputBehavior)
{
	if (inInputBehavior & (uint8)EAbilityInputBehavior::ShouldLoopOnInput)
	{
		WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,UPDWGameplayTagSettings::GetInputPressedTag());
		WaitEventTask->EventReceived.AddUniqueDynamic(this, &ThisClass::OnLoopRequest);
		WaitEventTask->ReadyForActivation();
	}
	if (inInputBehavior & (uint8)EAbilityInputBehavior::ShouldRestartOnInput)
	{
		WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,UPDWGameplayTagSettings::GetInputPressedTag());
		WaitEventTask->EventReceived.AddUniqueDynamic(this, &ThisClass::OnRestartAnimation);
		WaitEventTask->ReadyForActivation();
	}
	if (inInputBehavior & (uint8)EAbilityInputBehavior::ShouldInterruptOnRelease)
	{
		//#TODO : THINK WITH DESIGNERS IF NEEDED, SHOULD BE NICE TO HAVE
	}

	WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UPDWGameplayTagSettings::GetInputReleasedTag());
	WaitEventTask->EventReceived.AddUniqueDynamic(this, &ThisClass::OnReleaseInput);
	WaitEventTask->ReadyForActivation();
}
// #TODO DM: think if need this,shouldn't since the skill is triggered by the tag. 
UPDWInteractionReceiverComponent* UPDWGASAbility::GetBestInteractionForTag(/*FGameplayTag FilterTag*/)
{
	APDWCharacter* OwnerCharacter = Cast<APDWCharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(OwnerCharacter))
	{
		return nullptr;
	}
	UPDWInteractionComponent* InteractionComponent = OwnerCharacter->GetInteractionComponent();
	UPDWInteractionReceiverComponent* InteractReceiverComponent = InteractionComponent->GetCurrentInteractionReceiver();
	return InteractReceiverComponent;
}

void UPDWGASAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	//if (PupBackPackAnimInstance)
	//{
	//	PupBackPackAnimInstance->StopAllMontages(0);
	//}
	OnCancelAbility();
	BP_OnCancelAbility();
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
