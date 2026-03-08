// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWPuzzlePieceComponent.h"
#include "Gameplay/MiniGames/PDWMovingObjectBehaviour.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWPuzzlePieceComponent::UnpossesComponent(bool bUnInitializingBehaviour/* = false*/,APDWPlayerController* inController /*= nullptr*/)
{
	bPossessed = false;
	SetComponentTickEnabled(false);
	SetCustomDepthStencilValue(0);
	if (!bUnInitializingBehaviour)
	{
		for (UPDWMovingObjectBehaviour* Behaviour : CurrentMovingObjectBehaviours)
		{
			if(Behaviour)
			{
				Behaviour->RequestSceneComp(Behaviour->GetControllerOwner());
			}
		}
	}
	UPDWEventSubsytem::Get(this)->OnPuzzlePossesEvent(false,this);
	CurrentMovingObjectBehaviours.Empty();
}

void UPDWPuzzlePieceComponent::RotatePiece(float inValue)
{
	FRotator CurrentRotation = GetComponentRotation();
	const FRotator NewRotation = CurrentRotation.Add(0,inValue,0);
	SetWorldRotation(NewRotation);
	OnOrientationChanged.Broadcast(this);
}

void UPDWPuzzlePieceComponent::PossesComponent(APDWPlayerController* inController, UPDWMovingObjectBehaviour* MovingBehaviour /*= nullptr*/)
{
	if (IsValid(this))
	{
		SetCustomDepthStencilValue(1);
		SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		UPDWEventSubsytem::Get(this)->OnPuzzlePossesEvent(true,this);
		Super::PossesComponent(inController,MovingBehaviour);
	}
}
