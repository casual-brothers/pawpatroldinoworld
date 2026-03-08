// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/PDWWaterHoseMoveBehaviour.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Gameplay/Components/PDWMinigameMovingMesh.h"

void UPDWWaterHoseMoveBehaviour::ExecuteBehaviour(const FInputActionInstance& inInputInstance)
{
	const FVector2D Value = inInputInstance.GetValue().Get<FVector2D>();
	const FMovingObjectBehaviourConfig* Config = MovingObjectBehaviourConfig.GetPtr<FMovingObjectBehaviourConfig>();
	if (Value.X || Value.Y)
	{
		const float DeltaTime = GetWorld()->GetDeltaSeconds();
		YawDelta   += Value.X * Config->Speed * DeltaTime;
		PitchDelta += Value.Y * Config->Speed * DeltaTime;

		FRotator NewRot = SceneComponentInstance->GetRelativeRotation();
		NewRot.Yaw   += YawDelta;
		NewRot.Pitch += PitchDelta;

		YawDelta   = FMath::ClampAngle(YawDelta, -MaxAngle, MaxAngle);
		PitchDelta = FMath::ClampAngle(PitchDelta, -MaxAngle, MaxAngle);

		NewRot.Yaw   = YawDelta;
		NewRot.Pitch = PitchDelta;

		SceneComponentInstance->SetRelativeRotation(NewRot);
	}
}

void UPDWWaterHoseMoveBehaviour::UninitializeBehaviour()
{
	SceneComponentInstance->SetRelativeRotation(FRotator::ZeroRotator);
	SceneComponentInstance->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Super::UninitializeBehaviour();
}

void UPDWWaterHoseMoveBehaviour::InitializeSceneComponentPosses(APDWPlayerController* inController)
{
	Super::InitializeSceneComponentPosses(inController);
	USkeletalMeshComponent* VehicleMesh = GetControllerOwner()->GetVehicleInstance()->FindComponentByTag<USkeletalMeshComponent>(MeshTag.GetTagName());
	SceneComponentInstance->AttachToComponent(VehicleMesh,FAttachmentTransformRules::SnapToTargetNotIncludingScale,SocketToAttach);
}
