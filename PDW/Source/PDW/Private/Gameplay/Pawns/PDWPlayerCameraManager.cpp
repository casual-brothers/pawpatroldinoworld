// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Pawns/PDWPlayerCameraManager.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/Pawns/PDWPlayerController.h"

void APDWPlayerCameraManager::SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	Super::SetViewTarget(NewViewTarget, TransitionParams);
	if (NewViewTarget == PCOwner)
	{
		const APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
		if (GameMode && PCOwner == GetWorld()->GetFirstPlayerController())
		{
			SetManualCameraFade(1.0f, FLinearColor::Black, false);
		}
	}
}

void APDWPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();

	UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	ensure(GameInstance);
	GameInstance->OnPageLoaded.AddUniqueDynamic(this, &APDWPlayerCameraManager::RemoveBlackScreen);
}

void APDWPlayerCameraManager::RemoveBlackScreen()
{
	SetManualCameraFade(0.0f, FLinearColor::Black, false);
}
