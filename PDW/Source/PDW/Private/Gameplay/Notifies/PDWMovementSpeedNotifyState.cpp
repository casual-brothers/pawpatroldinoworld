// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Notifies/PDWMovementSpeedNotifyState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Gameplay/Pawns/PDWPlayerController.h"

void UPDWMovementSpeedNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if(!MeshComp)
		return;

	APDWCharacter* Owner = Cast<APDWCharacter>(MeshComp->GetOwner());
	if(!Owner)
		return;

	APDWPlayerController* Controller = Owner->GetPDWPlayerController();
	if(!Controller)
		return;
	Controller->SetSprintDisabled(true);

	UCharacterMovementComponent* MoveComp = Owner->FindComponentByClass<UCharacterMovementComponent>();
	if(!MoveComp)
		return;


	OriginalSpeed = MoveComp->GetMaxSpeed();
	MoveComp->MaxWalkSpeed = NewSpeed;
}

void UPDWMovementSpeedNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(!MeshComp)
		return;

	APDWCharacter* Owner = Cast<APDWCharacter>(MeshComp->GetOwner());
	if(!Owner)
		return;

	APDWPlayerController* Controller = Owner->GetPDWPlayerController();
	if(!Controller)
		return;
	Controller->SetSprintDisabled(false);
	
	UCharacterMovementComponent* MoveComp = Owner->FindComponentByClass<UCharacterMovementComponent>();
	if(!MoveComp)
		return;

	MoveComp->MaxWalkSpeed = OriginalSpeed;
}