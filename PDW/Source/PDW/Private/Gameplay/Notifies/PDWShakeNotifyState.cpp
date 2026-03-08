// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Notifies/PDWShakeNotifyState.h"
#include "Kismet/GameplayStatics.h"
#include "PDWGameInstance.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Data/PDWGameSettings.h"
#include "Data/PDWGameplayTagSettings.h"

void UPDWShakeNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (!MeshComp->GetOwner())
	{
		return;
	}

	if (APawn* Player = Cast<APawn>(MeshComp->GetOwner()))
	{
		if (AController* Controller = Player->GetController())
		{
			if (APDWPlayerController* PlayerController = Cast<APDWPlayerController>(Controller))
			{
				UPDWGameplayFunctionLibrary::PlayForceFeedback({PlayerController}, ControllerShake, NAME_None, bLooping);
				UPDWGameplayFunctionLibrary::StartCameraShake(PlayerController,CameraShake);
			}
		}
	}
}

void UPDWShakeNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation,EventReference);

	if (APawn* Player = Cast<APawn>(MeshComp->GetOwner()))
	{
		if (AController* Controller = Player->GetController())
		{
			if (APDWPlayerController* PlayerController = Cast<APDWPlayerController>(Controller))
			{
				UPDWGameplayFunctionLibrary::StopForceFeedback({PlayerController});
				UPDWGameplayFunctionLibrary::StopCameraShake(PlayerController, CameraShake);
			}
		}
	}
}
