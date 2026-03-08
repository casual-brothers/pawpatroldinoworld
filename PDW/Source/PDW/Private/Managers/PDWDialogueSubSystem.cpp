// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PDWDialogueSubSystem.h"
#include "FlowSubsystem.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "PDWGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Animation/PDWAnimInstance.h"
#include "Managers/PDWHUDSubsystem.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Data/PDWGameSettings.h"
#include "Data/FlowDeveloperSettings.h"

UPDWDialogueSubSystem::FOnConversationCompleted UPDWDialogueSubSystem::OnConversationCompleted;

UPDWDialogueSubSystem* UPDWDialogueSubSystem::Get(UObject* WorldContextObject)
{
    return WorldContextObject->GetWorld()->GetSubsystem<UPDWDialogueSubSystem>();
}

void UPDWDialogueSubSystem::TriggerSubtitle(UObject* WorldContext, FDialogueLine inDialogueLine)
{
	FConversation NewConversation = FConversation();
	NewConversation.ConversationType = EConversationType::BlockingDialogue;
	NewConversation.DialogueLines.Add(inDialogueLine);
	TriggerConversation(WorldContext, NewConversation);
}

void UPDWDialogueSubSystem::SkipSubtitle(UObject* WorldContext)
{
	UNebulaFlowCoreFunctionLibrary::TriggerAction(WorldContext, UPDWGameSettings::GetUIActionConfirm().ToString(), "");
}

void UPDWDialogueSubSystem::TriggerConversation(UObject* WorldContext, const FConversation& inConversationConfig)
{
	if(!WorldContext)
		return;

	UPDWDialogueSubSystem::Get(WorldContext)->ShowConversation(inConversationConfig);
}

void UPDWDialogueSubSystem::TriggerDialogueLineAnimations(UObject* WorldContext, const FDialogueLine& inDialogConfig)
{
	if (inDialogConfig.FramedCharacter.IsValid())
	{
		UPDWDialogueSubSystem* SubSystem = UPDWDialogueSubSystem::Get(WorldContext);
		if (inDialogConfig.FramedCharacter != SubSystem->LastSpeakerID)
		{
			SubSystem->LastSpeakerID = inDialogConfig.FramedCharacter;
			if (const UFlowSubsystem* FlowSubsystem = WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>())
			{
				if (SubSystem->LastSpeakerActor)
				{
					SubSystem->ResetAnimation(SubSystem->LastSpeakerActor);
				}
				TArray<AActor*> Actors = FlowSubsystem->GetFlowActorsByTags(inDialogConfig.FramedCharacter, EGameplayContainerMatchType::All,AActor::StaticClass()).Array();
				AActor* SpeakerActor = Actors.IsEmpty() ? nullptr : Actors.Last();
				SubSystem->LastSpeakerActor = SpeakerActor;
				if (SpeakerActor)
				{
					SubSystem->TriggerCamera(SpeakerActor, inDialogConfig);
					SubSystem->TriggerAnimation(SpeakerActor, inDialogConfig);
				}
			}
		}
		else
		{
			if(inDialogConfig.DialogueAnimationConfig.AnimationMood != SubSystem->LastAnimationMood)
			{
				SubSystem->LastAnimationMood = inDialogConfig.DialogueAnimationConfig.AnimationMood;
				SubSystem->TriggerAnimation(SubSystem->LastSpeakerActor, inDialogConfig);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UPDWDialogueSubSystem - Missing Speaker Name for Dialogue!"));
	}
}

void UPDWDialogueSubSystem::TriggerCamera(AActor* inSpeakerActor,const FDialogueLine& inDialogConfig)
{
	APlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(GetWorld());
	P1->SetViewTargetWithBlend(inSpeakerActor, inDialogConfig.DialogueCameraConfig.CameraBlendTime, EViewTargetBlendFunction::VTBlend_EaseInOut,2.0f);
}

void UPDWDialogueSubSystem::TriggerAnimation(AActor* inSpeakerActor,const FDialogueLine& inDialogConfig)
{
	//N.B -> IF NPC HAVE MORE THAN 1 SKELETAL MESH SEARCH WITH TAG
	if (!inSpeakerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPDWDialogueSubSystem - Missing Speaker Actor for Dialogue Animation!"));
		return;
	}
	USkeletalMeshComponent* Skeletal = inSpeakerActor->FindComponentByClass<USkeletalMeshComponent>();
	if (Skeletal && Skeletal->GetAnimInstance())
	{
		UPDWAnimInstance* Animinstance = Cast<UPDWAnimInstance>(Skeletal->GetAnimInstance());
		if (Animinstance)
		{
			Animinstance->bIsTalking = true;
			Animinstance->EmotionType = inDialogConfig.DialogueAnimationConfig.AnimationMood;
		}
	}
}

void UPDWDialogueSubSystem::ResetAnimation(AActor* inSpeakerActor)
{
	if(!inSpeakerActor)
		return;

	USkeletalMeshComponent* Skeletal = inSpeakerActor->FindComponentByClass<USkeletalMeshComponent>();
	if (Skeletal && Skeletal->GetAnimInstance())
	{
		UPDWAnimInstance* Animinstance = Cast<UPDWAnimInstance>(Skeletal->GetAnimInstance());
		if (Animinstance)
		{
			Animinstance->bIsTalking = false;
			Animinstance->EmotionType = EEmotionType::Idle;
		}
	}
}

void UPDWDialogueSubSystem::CloseConversation(UObject* WorldContext)
{
	if(!WorldContext)
		return;

	UPDWDialogueSubSystem* SubSystem = UPDWDialogueSubSystem::Get(WorldContext);
	SubSystem->ResetAnimation(SubSystem->LastSpeakerActor);
	SubSystem->LastSpeakerActor = nullptr;
	SubSystem->LastSpeakerID = FGameplayTagContainer();
	SubSystem->LastAnimationMood = EEmotionType::Idle;

	UPDWDialogueSubSystem::OnConversationCompleted.Broadcast();

	UPDWDialogueSubSystem::Get(WorldContext)->CurrentConversation = FConversation();
}

void UPDWDialogueSubSystem::ShowConversation(const FConversation& inConversationConfig)
{
	if (inConversationConfig.ConversationType == EConversationType::None)
	{
		return;
	}

	if (inConversationConfig.ConversationType == EConversationType::HUDComment)
	{
		CurrentConversation = FConversation();
		UPDWHUDSubsystem::Get(this)->RequestHUDDialogue(inConversationConfig);
	}
	else if(inConversationConfig.ConversationType == EConversationType::BlockingDialogue)
	{
		UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UFlowDeveloperSettings::GetDialogueTag().ToString(), "");
		CurrentConversation = inConversationConfig;
		UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UPDWGameSettings::GetStartConversationAction().ToString(), "");
	}
	else
	{
		CurrentConversation = inConversationConfig;
		UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UPDWGameSettings::GetStartConversationAction().ToString(), "");
	}
}