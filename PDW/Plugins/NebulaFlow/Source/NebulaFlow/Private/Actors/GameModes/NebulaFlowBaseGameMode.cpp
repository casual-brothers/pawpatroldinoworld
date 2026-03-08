#include "Actors/GameModes/NebulaFlowBaseGameMode.h"
#include "Core/NebulaFlowGameState.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/NebulaGameplayFreeCamera.h"
#include "Core/NebulaFlowGameInstance.h"

ANebulaFlowBaseGameMode::ANebulaFlowBaseGameMode()
{
	bUseSeamlessTravel = true;
	GameStateClass = ANebulaFlowGameState::StaticClass();
}

void ANebulaFlowBaseGameMode::EnableFreeCamera()
{
    UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
    ensure(GameInstance);
    GameInstance->SetFreeCamera(true);
 	if (!FreeCameraPawn)
 	{
 		return;
 	}
 	// if there isn't, create the camera controller for the first player available (2 or 3)
 	if (FreeCameraController == nullptr)
 	{
 		CameraPlayerIndex = 1;
 		while (UGameplayStatics::GetPlayerController(GetWorld(), CameraPlayerIndex) != nullptr)
 		{
 			CameraPlayerIndex++;
 		}
 		FreeCameraController = Cast<APlayerController>(UGameplayStatics::CreatePlayer(this, CameraPlayerIndex, true));
 	}
 	else
 	{
 		CameraPlayerIndex = 1;
 		while (UGameplayStatics::GetPlayerController(GetWorld(), CameraPlayerIndex) != nullptr)
 		{
 			CameraPlayerIndex++;
 		}
 		CameraPlayerIndex--;
 		if (ANebulaGameplayFreeCamera* FreeCamera = Cast<ANebulaGameplayFreeCamera>(FreeCameraController->GetPawn()))
 		{
 			if (!FreeCamera->bSwitchCamera)
 			{
 				FreeCamera->SwitchCamera();
 				UGameplayStatics::RemovePlayer(UGameplayStatics::GetPlayerController(GetWorld(), CameraPlayerIndex), true);
 				FreeCameraController = Cast<APlayerController>(UGameplayStatics::CreatePlayer(this, CameraPlayerIndex, true));
 				FTimerHandle THandle;
 				FTimerDelegate Del = FTimerDelegate::CreateUObject(this, &ThisClass::EnableFreeCamera);
 				GetWorld()->GetTimerManager().SetTimer(THandle, Del, 0.5f, false);
 				return;
 			}
 			else
 			{
 				UGameplayStatics::RemovePlayer(UGameplayStatics::GetPlayerController(GetWorld(), CameraPlayerIndex), true);
 				FreeCameraController = Cast<APlayerController>(UGameplayStatics::CreatePlayer(this, CameraPlayerIndex, true));
 			}
 		}
 	}
 	UGameplayStatics::SetForceDisableSplitscreen(this, true);
 	if (FreeCameraController)
 	{
 		FActorSpawnParameters SpawnInfo;
 		SpawnInfo.Owner = FreeCameraController;
 		SpawnInfo.Instigator = GetInstigator();
 		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 		SpawnInfo.ObjectFlags |= RF_Transient;
 		SpawnInfo.bDeferConstruction = false;
 		APawn* PlayerPawn =  UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn();
 		FVector Location;
 		FRotator Rotation;
 		if (PlayerPawn)
 		{
 			Location = PlayerPawn->GetActorLocation();
 			Rotation = PlayerPawn->GetActorRotation();
 
 			// This can be overritten 
 			SetInitialFreeCameraLocationAndRotation(Location, Rotation);
 		}
 		ANebulaGameplayFreeCamera* FreeCamera = GetWorld()->SpawnActor<ANebulaGameplayFreeCamera>(FreeCameraPawn, Location, Rotation, SpawnInfo);
 		if (FreeCamera)
 		{
 			if (FreeCameraController->GetPawn())
 			{
 				FreeCameraController->GetPawn()->Destroy();
 			}
 			FreeCameraController->Possess(FreeCamera);
 			FreeCamera->InitializeFreeCamera(PlayerPawn, FreeCamera->GetTransform());
 			UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(FreeCamera);
 			if (UCameraComponent* PlayerCamera = Cast<UCameraComponent>(PlayerPawn->GetComponentByClass(UCameraComponent::StaticClass())))
 			{
 				FreeCamera->CameraComponent->FieldOfView = PlayerCamera->FieldOfView;
 			}
			FreeCamera->ToggleUI(false);

 		}
 	}
}


void ANebulaFlowBaseGameMode::UpdateTargetActorFreeCamera()
{
    if (!FreeCameraController || !FreeCameraController->IsValidLowLevel())
    {
        return;
    }

    APawn* Pawn = FreeCameraController->GetPawn();
	if (Pawn && Pawn->IsValidLowLevel())
	{
		ANebulaGameplayFreeCamera* FreeCam = Cast<ANebulaGameplayFreeCamera>(Pawn);
		if (FreeCam && FreeCam->IsValidLowLevel())
		{
			APawn* PlayerPawn = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn();
			FreeCam->UpdateFollowActor(PlayerPawn);
		}
	}
}

void ANebulaFlowBaseGameMode::DisableFreeCamera()
{
    UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
    ensure(GameInstance);
    if (ANebulaGameplayFreeCamera* FreeCamera = Cast<ANebulaGameplayFreeCamera>(FreeCameraController->GetPawn()))
    {    
	    FreeCamera->ToggleUI(true);
    }
    GameInstance->SetFreeCamera(false);
 	if (!FreeCameraPawn)
 	{
 		return;
 	}

    DestroyPlayerCamera();
    FreeCameraController->Destroy();
    
 	APawn* PlayerPawn =  UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn();
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(PlayerPawn);
}



void ANebulaFlowBaseGameMode::DestroyPlayerCamera()
{
    UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
    ensure(GameInstance);

    TArray<ULocalPlayer*> Players = GameInstance->GetLocalPlayers();
	if (CameraPlayerIndex >= 1 && Players.Num() > CameraPlayerIndex)
    {
        ULocalPlayer* CameraPlayer = Players[CameraPlayerIndex];

        if (CameraPlayer)
        {
            if (APlayerController* PC = CameraPlayer->PlayerController)
            {
                PC->Destroy();
            }

            GameInstance->RemoveLocalPlayer(CameraPlayer);
        }
    }
    CameraPlayerIndex = -1;
}

void ANebulaFlowBaseGameMode::SetInitialFreeCameraLocationAndRotation(FVector& OutLocation, FRotator& OutRotation)
{
	APawn* PlayerPawn =  UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn();
	if (PlayerPawn)
	{
		if (UCameraComponent* PlayerCamera = Cast<UCameraComponent>(PlayerPawn->GetComponentByClass(UCameraComponent::StaticClass())))
		{
			OutLocation = PlayerCamera->GetComponentLocation();
			OutRotation = PlayerCamera->GetComponentRotation();
		}

	}
}

void ANebulaFlowBaseGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{ 
    DestroyPlayerCamera();

    Super::EndPlay(EndPlayReason);
}

