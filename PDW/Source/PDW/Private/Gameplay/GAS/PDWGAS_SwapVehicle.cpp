// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GAS/PDWGAS_SwapVehicle.h"
#include "Data/PDWPlayerState.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Gameplay/Interfaces/PDWPlayerInterface.h"

void UPDWGAS_SwapVehicle::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	APawn* BlendingTarget = nullptr;

	bool bMaintainVehicle = TriggerEventData->EventTag.IsValid();

	if (PlayerState->GetIsOnVehicle())
	{	
		if (!bMaintainVehicle)
		{

			//swap back to same pup
			BlendingTarget = PlayerController->GetPawn();
			IPDWPlayerInterface* PlayerInterface = Cast<IPDWPlayerInterface>(PlayerController->GetPawn());
			BlendingTarget->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			BlendingTarget->SetActorScale3D(FVector::OneVector);
			PlayerController->GetVehicleInstance()->Destroy();
			PlayerController->SetVehicleInstance(nullptr);
			PlayerInterface->SetActivation(true);
			PlayerState->SetIsOnVehicle(false);
		}
		else
		{
			//we are a pup on the car and we want to swap to a new car
			//PERDONAME POR THIS SHIT HOPEFULLY WE WOULD CUSTOMIZE ALSO THE CAR
			auto PreVelocity = PlayerController->GetVehicleInstance()->GetVelocity();
			PlayerController->GetVehicleInstance()->Destroy();
			PlayerController->SetVehicleInstance(nullptr);
			TSubclassOf<APawn> VehicleClass = PlayerController->GetPupClassForSwap(EPupRequestType::Current, true);
			FActorSpawnParameters Params;
			Params.Instigator = PlayerController->GetPawn();
			Params.Owner = PlayerController->GetPawn();
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			
			// Find terrain location
			FTransform SpawnTransform = PlayerController->GetPawn()->GetActorTransform();
			SpawnTransform.SetScale3D(FVector::OneVector);
			FindNewLocation(SpawnTransform);

			APDWVehiclePawn* Vehicle = PlayerController->GetPawn()->GetWorld()->SpawnActor<APDWVehiclePawn>(VehicleClass, SpawnTransform, Params);

			PlayerController->SetVehicleInstance(Vehicle);
			BlendingTarget = PlayerController->GetVehicleInstance(); 
			UMeshComponent* VehicleMesh = Vehicle->FindComponentByTag<UMeshComponent>(MeshToAttachTag.GetTagName());
			APawn* Pawn = PlayerController->GetPawn();
			//Pawn->SetActorScale3D(FVector::OneVector);
			Pawn->AttachToComponent(VehicleMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketToUse.GetTagName());
			PlayerState->SetIsOnVehicle(true);
			Vehicle->InitAttributes(PlayerController->GetCurrentPup());

			Vehicle->SetInitialVelocity(FVector(PreVelocity.X, PreVelocity.Y, Vehicle->GetSkipSwapGravity() ? 0.f : PreVelocity.Z));
		}
		//#TODO DM check if need delay like in bp.
	}
	else
	{
		auto PreVelocity = PlayerController->GetPawn()->GetVelocity();
		
		//swap to vehicle using the current pup
		//probably not needed to customize again.
		IPDWPlayerInterface* PlayerInterface = Cast<IPDWPlayerInterface>(PlayerController->GetPawn());
		PlayerInterface->SetActivation(false);
		//PlayerController->GetPawn()->SetActorEnableCollision(false);
		//SwapCharacterCustomization(true,EPupRequestType::Current);

		TSubclassOf<APawn> VehicleClass = PlayerController->GetPupClassForSwap(EPupRequestType::Current,true);
		FActorSpawnParameters Params;
		Params.Instigator = PlayerController->GetPawn();
		Params.Owner = PlayerController->GetPawn();
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		// Find terrain location
		FTransform SpawnTransform = PlayerController->GetPawn()->GetActorTransform();
		FindNewLocation(SpawnTransform);

		APDWVehiclePawn* Vehicle =PlayerController->GetPawn()->GetWorld()->SpawnActor<APDWVehiclePawn>(VehicleClass, SpawnTransform, Params);
		
		PlayerController->SetVehicleInstance(Vehicle);
		PlayerState->SetIsOnVehicle(true);
		BlendingTarget = PlayerController->GetVehicleInstance(); 
		UMeshComponent* VehicleMesh = BlendingTarget->FindComponentByTag<UMeshComponent>(MeshToAttachTag.GetTagName());
		APawn* Pawn = PlayerController->GetPawn();
		Pawn->AttachToComponent(VehicleMesh, FAttachmentTransformRules::SnapToTargetIncludingScale,SocketToUse.GetTagName());
		Vehicle->InitAttributes(PlayerController->GetCurrentPup());
		Vehicle->SetInitialVelocity(FVector(PreVelocity.X, PreVelocity.Y, Vehicle->GetSkipSwapGravity() ? 0.f : PreVelocity.Z));
	}
	if(PlayerController && !PlayerController->UsingMinigameCamera)
	{
		PlayerController->SetViewTargetWithBlend(BlendingTarget,TriggerEventData->EventMagnitude > 0 ? BlendTime : 0,BlendFunction,BlendExp);
	}
	CompleteCharacterSwap();
}

void UPDWGAS_SwapVehicle::FindNewLocation(FTransform& Transform)
{
	auto QueryParam = FCollisionQueryParams::DefaultQueryParam;
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, Transform.GetLocation(), Transform.GetLocation() - (FVector::UpVector * 300), ECollisionChannel::ECC_Vehicle, QueryParam);
	if (Hit.bBlockingHit)
	{
		Transform.SetLocation(Hit.Location);
	}
}
