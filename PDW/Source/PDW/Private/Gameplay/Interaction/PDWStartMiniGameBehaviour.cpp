// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/PDWStartMiniGameBehaviour.h"
#include "Data/PDWGameSettings.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Data/PDWPlayerState.h"

void UPDWStartMiniGameBehaviour::ExecuteBehaviour_Implementation()
{
	UPDWMinigameConfigComponent* MiniGameComponent = GetOwnerComponent()->GetOwner()->FindComponentByTag<UPDWMinigameConfigComponent>(UPDWGameSettings::GetMiniGameComponentTag());
	ensureMsgf(MiniGameComponent, TEXT("UPDWStartMiniGameBehaviour::ExecuteBehaviour_Implementation - MiniGameConfigComponent not found on owner actor %s"), *GetOwnerComponent()->GetOwner()->GetName());

	IPDWPlayerInterface* Player = Cast<IPDWPlayerInterface>(BehaviourExecutioner);
	if (Player && !bMinigameTriggered)
	{
		FSwapConfiguration SwapConfig;
		SwapConfig.AllowedPup = AllowedPup;
		SwapConfig.AutoSwapConfiguration = AutoSwapConfiguration;
		MiniGameComponent->InitializeMiniGame(Cast<APDWPlayerController>(Player->GetPDWPlayerController()),SwapConfig);
		StopBehaviour();
		bMinigameTriggered = true;
		UPDWEventSubsytem::Get(this)->OnJoinRequestRefused.AddUniqueDynamic(this, &UPDWStartMiniGameBehaviour::OnMinigameJoinRefused);
		MiniGameComponent->OnMinigameLeft.AddUniqueDynamic(this, &UPDWStartMiniGameBehaviour::OnMinigameLeft);
	}
	else
	{
		StopBehaviour();
		UE_LOG(LogTemp, Warning, TEXT("UPDWStartMiniGameBehaviour::ExecuteBehaviour_Implementation - Executioner is not a Pawn: %s"), *GetOwnerComponent()->GetOwner()->GetName());
	}
}

void UPDWStartMiniGameBehaviour::OnMinigameJoinRefused()
{
	bMinigameTriggered = false;
}

void UPDWStartMiniGameBehaviour::OnMinigameLeft(const FMiniGameEventSignature& inSignature)
{
	OnMinigameJoinRefused();
}

