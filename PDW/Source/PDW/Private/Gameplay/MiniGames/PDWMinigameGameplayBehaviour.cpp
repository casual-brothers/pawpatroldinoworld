// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/PDWMinigameGameplayBehaviour.h"

void UPDWMinigameGameplayBehaviour::ExecuteBehaviour(AActor* inOwner,AActor* inInteracter,bool& bSuccessNotify)
{
	bSuccessNotify = bNotifySuccess;
	BP_ExecuteBehaviour(inOwner,inInteracter);
}
