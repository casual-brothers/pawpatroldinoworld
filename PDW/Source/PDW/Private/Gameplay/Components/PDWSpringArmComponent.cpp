// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Components/PDWSpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WPHelperSubsystem.h"
#include "FunctionLibraries/GameOptionsFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PawnMovementComponent.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"

UPDWSpringArmComponent::UPDWSpringArmComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	bInheritRoll = false;
	bInheritPitch = false;
	bInheritYaw = true;
}

void UPDWSpringArmComponent::DestroyComponent(bool bPromoteChildren /*= false*/)
{
	if (IsValid(AttachedCamera))
		AttachedCamera->DestroyComponent();
	Super::DestroyComponent(bPromoteChildren);
}


void UPDWSpringArmComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		EventSubsystem->OnGameplayStateEnter.RemoveDynamic(this, &UPDWSpringArmComponent::OnGameplayStateEnter);
		EventSubsystem->OnMultiplayerStateChange.RemoveDynamic(this, &UPDWSpringArmComponent::OnMultiplayerStateChange);
	}
	Super::EndPlay(EndPlayReason);
}

void UPDWSpringArmComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UGameOptionsFunctionLibrary::GetManualCameraEnabled())
	{
		SetManualCameraActive();
	}
	else
	{
		SetAutomaticCameraActive();
	}

	bSplitScreenMode = UGameplayStatics::GetNumLocalPlayerControllers(this) > 1;
	SetPDWCharacter(Cast<APDWCharacter>(GetOwner()));
	SetAttachedCamera(Cast<UCameraComponent>(GetChildComponent(0)));
	Init();
	if (CurrentCameraParameters)
	{
		CameraLagLocationVectorToUse = CurrentCameraParameters->LinearLagVector;
	}
	UWPHelperSubsystem::Get(this)->OnTeleportStarted.AddUniqueDynamic(this, &UPDWSpringArmComponent::OnTeleportTriggered);
	UWPHelperSubsystem::Get(this)->OnTeleportCompleted.AddUniqueDynamic(this, &UPDWSpringArmComponent::OnTeleportCompleted);
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		EventSubsystem->OnGameplayStateEnter.AddUniqueDynamic(this, &UPDWSpringArmComponent::OnGameplayStateEnter);
		EventSubsystem->OnMultiplayerStateChange.AddUniqueDynamic(this, &UPDWSpringArmComponent::OnMultiplayerStateChange);
	
	}
}

void UPDWSpringArmComponent::OnGameplayStateEnter()
{
	if (UGameOptionsFunctionLibrary::GetManualCameraEnabled())
	{
		SetManualCameraActive();
	}
	else
	{
		SetAutomaticCameraActive();
	}
}


FVector UPDWSpringArmComponent::GetForwardMovementVector()
{
	return GetAttachedCamera()->GetForwardVector();
}

FVector UPDWSpringArmComponent::GetRightMovementVector()
{
	return GetAttachedCamera()->GetRightVector();
}


void UPDWSpringArmComponent::SetManualCameraActive()
{
	CurrentCameraParameters = ManualCameraParameters;
	bUsePawnControlRotation = true;
}

void UPDWSpringArmComponent::SetAutomaticCameraActive()
{
	CurrentCameraParameters = AutomaticCameraParameters;
	bUsePawnControlRotation = false;
}

void UPDWSpringArmComponent::SetPDWCharacter(APDWCharacter* Character)
{
	PDWCharacter = Character;
	if(PDWCharacter)
	{ 
		PDWCharacterMoveComp = PDWCharacter->GetMovementComponent();
	}
	SetRelativeRotation(CurrentCameraParameters->TransformRotation.Rotation());
	SetRelativeLocation(CurrentCameraParameters->TransformLocation);
	FVector ArmOrigin = GetComponentLocation() + TargetOffset;
	FRotator TargetWorldRotation1 = FRotator::MakeFromEuler(CurrentCameraParameters->TransformRotation);
	FQuat TargetWorldRotationQ1 = GetOwner()->GetActorRotation().Quaternion() * TargetWorldRotation1.Quaternion();
	FRotator DesiredRot = TargetWorldRotationQ1.Rotator();
	FTransform WorldCamTM(DesiredRot, ArmOrigin);
	// Convert to relative to component
	FTransform RelCamTM = WorldCamTM.GetRelativeTransform(GetComponentTransform());
	// Update socket location/rotation
	RelativeSocketLocation = RelCamTM.GetLocation();
	RelativeSocketRotation = RelCamTM.GetRotation();
	PreviousDesiredRot = DesiredRot;
	PreviousDesiredLoc = ArmOrigin;
}

void UPDWSpringArmComponent::SetAttachedCamera(UCameraComponent* Camera)
{
	AttachedCamera = Camera;
	if (IsValid(AttachedCamera))
	{
		AttachedCamera->FieldOfView = CurrentCameraParameters->DefaultFOV;
		RotateCamera(0.0f);
	}

}

void UPDWSpringArmComponent::Init()
{
	RotateCamera(0.0f);

	if (PDWCharacter)
	{
		PDWCharacter->OnMovementInput.AddUniqueDynamic(this, &ThisClass::OnMovementInputChanged);
		PDWCharacter->OnCameraInput.AddUniqueDynamic(this, &ThisClass::OnCameraInputChanged);
	}
}

void UPDWSpringArmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(CurrentCameraParameters) || !IsValid(GetOwner()) || !IsValid(AttachedCamera))
	{
		return;
	}

	RotateCamera(DeltaTime);
	UpdateFOV(DeltaTime);
	UpdateArmLength(DeltaTime);
	SetRelativeLocation(CurrentCameraParameters->TransformLocation);

	if (CurrentCameraParameters->bDebug)
	{
		DrawDebugLine(GetWorld(), GetComponentLocation(), AttachedCamera->GetComponentLocation(), FColor::Red, false, DeltaTime, 0, 1.0f);
		DrawDebugDirectionalArrow(GetWorld(), AttachedCamera->GetComponentLocation(), AttachedCamera->GetComponentLocation() + AttachedCamera->GetForwardVector() * 300.0f, 50.0f, FColor::Yellow, false, DeltaTime, 0, 1.0f);
	}
}



void UPDWSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	if (!IsValid(CurrentCameraParameters) || !IsValid(GetOwner()) || !IsValid(AttachedCamera))
	{
		Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
		return;
	}

	if (CurrentCameraParameters->ManualCamera)
	{	
		FRotator DesiredRot = GetTargetRotation();
		PreviousDesiredRot = DesiredRot;
		FVector ArmOrigin = GetComponentLocation() + TargetOffset;
		FVector DesiredLoc = ArmOrigin;

		float ZSpeed = CameraLagLocationVectorToUse.Z;
		if (CurrentCameraParameters->LinearLagZCurve && PDWCharacter && !PDWCharacterMoveComp->IsMovingOnGround())
		{
			ZSpeed = CurrentCameraParameters->LinearLagZCurve->GetFloatValue(DesiredLoc.Z - PreviousDesiredLoc.Z);
		}
		
		const FVector ArmMovementStep = (DesiredLoc - PreviousDesiredLoc) * (1.f / DeltaTime);
		FVector LerpTarget = PreviousDesiredLoc;
		DesiredLoc.Z = FMath::FInterpTo(PreviousDesiredLoc.Z, DesiredLoc.Z, DeltaTime, ZSpeed);
		PreviousDesiredLoc = DesiredLoc;

		PreviousArmOrigin = ArmOrigin;
		PreviousDesiredLoc = DesiredLoc;
		DesiredLoc -= DesiredRot.Vector() * TargetArmLength;
		DesiredLoc += FRotationMatrix(DesiredRot).TransformVector(SocketOffset);

		// Do a sweep to ensure we are not penetrating the world
		FVector ResultLoc;
		if (bDoTrace && (TargetArmLength != 0.0f))
		{
			bIsCameraFixed = true;
			FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetOwner());

			FHitResult Result;
			GetWorld()->SweepSingleByChannel(Result, ArmOrigin, DesiredLoc, FQuat::Identity, ProbeChannel, FCollisionShape::MakeSphere(ProbeSize), QueryParams);
		
			UnfixedCameraPosition = DesiredLoc;

			ResultLoc = BlendLocations(DesiredLoc, Result.Location, Result.bBlockingHit, DeltaTime);

			if (ResultLoc == DesiredLoc) 
			{	
				bIsCameraFixed = false;
			}
		}
		else
		{
			ResultLoc = DesiredLoc;
			bIsCameraFixed = false;
			UnfixedCameraPosition = ResultLoc;
		}

		// Form a transform for new world transform for camera
		FTransform WorldCamTM(DesiredRot, ResultLoc);
		// Convert to relative to component
		FTransform RelCamTM = WorldCamTM.GetRelativeTransform(GetComponentTransform());

		// Update socket location/rotation
		RelativeSocketLocation = RelCamTM.GetLocation();
		RelativeSocketRotation = RelCamTM.GetRotation();

		UpdateChildTransforms();
	}
	else
	{
		FRotator DesiredRot = GetOwner()->GetActorRotation();
		FRotator TargetWorldRotation = FRotator::MakeFromEuler(CurrentCameraParameters->TransformRotation);
		FQuat TargetWorldRotationQ = TargetWorldRotation.Quaternion();
		TargetWorldRotationQ = DesiredRot.Quaternion() * TargetWorldRotationQ;
		DesiredRot = TargetWorldRotationQ.Rotator();
		FVector RotSpeed = CurrentCameraParameters->RotationalSpeedVector;

		if (LastMovementInput.Y < -CurrentCameraParameters->BackMovementInputDeadZoneValue/* || (GetMovementComponent()->IsFalling() && bLockCameraRotationInAir)*/)
		{
			RotSpeed *= SMALL_NUMBER;
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("%1.1f  %1.1f %1.1f"), RotSpeed.X, RotSpeed.Y, RotSpeed.Z));
		}
		else
		{
			//Find Angle Between Pup and camera forward vectors
			FVector First = FVector::VectorPlaneProject(AttachedCamera->GetForwardVector(), FVector::UpVector).GetSafeNormal();
			FVector Second = FVector::VectorPlaneProject(GetOwner()->GetActorForwardVector(), FVector::UpVector).GetSafeNormal();
			float degAngle = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(First, Second))));

			if (CurrentCameraParameters->CameraLagRotationCurve)
			{
				RotSpeed *= CurrentCameraParameters->CameraLagRotationCurve->GetFloatValue(degAngle);
			}
			if (LastMovementInput.X == 0.f && LastMovementInput.Y == 0.f)
			{
				RotSpeed *= CurrentCameraParameters->StandingPupCameraSpeedMultiplier;
			}
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%1.1f  %1.1f %1.1f"), RotSpeed.X, RotSpeed.Y, RotSpeed.Z));
		}

		FRotator InterpedRot = PreviousDesiredRot;
		FRotator NewPitch = FMath::RInterpTo(FRotator(PreviousDesiredRot.Pitch, 0.f, 0.f), FRotator(DesiredRot.Pitch, 0.f, 0.f), DeltaTime, RotSpeed.Y);
		FRotator NewYaw = FMath::RInterpTo(FRotator(0.f, PreviousDesiredRot.Yaw, 0.f), FRotator(0.f, DesiredRot.Yaw, 0.f), DeltaTime, RotSpeed.Z);
		FRotator NewRoll = FMath::RInterpTo(FRotator(0.f, 0.f, PreviousDesiredRot.Roll), FRotator(0.f, 0.f, DesiredRot.Roll), DeltaTime, RotSpeed.X);
		InterpedRot = FRotator(NewPitch.Pitch, NewYaw.Yaw, NewRoll.Roll);
		PreviousDesiredRot = InterpedRot;
		// Get the spring arm 'origin', the target we want to look at
		FVector ArmOrigin = GetComponentLocation() + TargetOffset;
		// We lag the target, not the actual camera position, so rotating the camera around does not have lag
		FVector DesiredLoc = ArmOrigin;
		FVector LocalDesLoc = UKismetMathLibrary::InverseTransformLocation(GetOwner()->GetTransform(), DesiredLoc);
		FVector LocalPreviousDesired = UKismetMathLibrary::InverseTransformLocation(GetOwner()->GetTransform(), PreviousDesiredLoc);
	
		float ZSpeed = CameraLagLocationVectorToUse.Z;
		if (CurrentCameraParameters->LinearLagZCurve && PDWCharacter && !PDWCharacterMoveComp->IsMovingOnGround())
		{
			ZSpeed = CurrentCameraParameters->LinearLagZCurve->GetFloatValue(LocalDesLoc.Z - LocalPreviousDesired.Z);
		}

		if (IsValid(CurrentCameraParameters))
		{
			float NewX = FMath::FInterpTo(LocalPreviousDesired.X, LocalDesLoc.X, DeltaTime, CameraLagLocationVectorToUse.X);
			float NewY = FMath::FInterpTo(LocalPreviousDesired.Y, LocalDesLoc.Y, DeltaTime, CameraLagLocationVectorToUse.Y);
			float NewZ = FMath::FInterpTo(LocalPreviousDesired.Z, LocalDesLoc.Z, DeltaTime, ZSpeed);
			LocalDesLoc = FVector(NewX, NewY, NewZ);
		}
		float fallingFilterEffect = 0.0f;//FallingFilter.Evolve(PDWCharacter->GetVerticalSpeedKPH(), DeltaTime);
		DesiredLoc = UKismetMathLibrary::TransformLocation(GetOwner()->GetTransform(), LocalDesLoc + FVector::UpVector * fallingFilterEffect);
	#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (bDrawDebugLagMarkers)
		{
			DrawDebugPoint(GetWorld(), ArmOrigin, 30.f, FColor::Green, false, DeltaTime, 0);
			DrawDebugPoint(GetWorld(), DesiredLoc, 30.f, FColor::Yellow, false, DeltaTime, 0);

			const FVector ToOrigin = ArmOrigin - DesiredLoc;
			DrawDebugDirectionalArrow(GetWorld(), DesiredLoc, DesiredLoc + ToOrigin * 0.5f, 5.0f, FColor::Green, false, DeltaTime, 0, 2.0f);
			DrawDebugDirectionalArrow(GetWorld(), DesiredLoc + ToOrigin * 0.5f, ArmOrigin, 5.0f, FColor::Green, false, DeltaTime, 0, 2.0f);
		}
	#endif


		PreviousArmOrigin = ArmOrigin;
		PreviousDesiredLoc = DesiredLoc;

		// Now offset camera position back along our rotation
		DesiredLoc -= InterpedRot.Vector() * TargetArmLength;
		// Add socket offset in local space
		DesiredLoc += FRotationMatrix(InterpedRot).TransformVector(SocketOffset);

		// Do a sweep to ensure we are not penetrating the world
		FVector ResultLoc;
		if (TargetArmLength != 0.0f)
		{
			bIsCameraFixed = true;
			FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetOwner());

			FHitResult Result;
			GetWorld()->SweepSingleByChannel(Result, ArmOrigin, DesiredLoc, FQuat::Identity, CurrentCameraParameters->ProbeChannel, FCollisionShape::MakeSphere(ProbeSize), QueryParams);

			UnfixedCameraPosition = DesiredLoc;

			ResultLoc = BlendLocations(DesiredLoc, Result.Location, Result.bBlockingHit, DeltaTime);

			if (ResultLoc == DesiredLoc)
			{
				bIsCameraFixed = false;
			}
			if (CurrentCameraParameters->bDebugCollisions)
			{
				if (Result.bBlockingHit && IsValid(Result.GetComponent()))
				{
					GEngine->AddOnScreenDebugMessage(55553435, 1.0f, FColor::Yellow, FString::Printf(TEXT("Camera collision with %s"), *Result.GetComponent()->GetName()));
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(55553435, 1.0f, FColor::Yellow, FString::Printf(TEXT("No Camera collision detected")));
				}

			}
		}
		else
		{
			ResultLoc = DesiredLoc;
			bIsCameraFixed = false;
			UnfixedCameraPosition = ResultLoc;
		}

		// Form a transform for new world transform for camera
		FTransform WorldCamTM(InterpedRot, ResultLoc);
		// Convert to relative to component
		FTransform RelCamTM = WorldCamTM.GetRelativeTransform(GetComponentTransform());

		// Update socket location/rotation
		RelativeSocketLocation = RelCamTM.GetLocation();
		RelativeSocketRotation = RelCamTM.GetRotation();
		UpdateChildTransforms();
		}
}

void UPDWSpringArmComponent::UpdateArmLength(float DeltaTime)
{
	if (bSplitScreenMode)
	{
		TargetArmLength = CurrentCameraParameters->NormalArmLength * CurrentCameraParameters->SS_MultiplierArmLenght;
	}
	else
	{
		TargetArmLength = CurrentCameraParameters->NormalArmLength;
	}

}

void UPDWSpringArmComponent::RotateCamera(float DeltaTime)
{
	if (!IsValid(AttachedCamera) || CurrentCameraParameters->ManualCamera)
		return;
	FVector Target = CurrentCameraParameters->TargetDefaultOffset.X * GetOwner()->GetActorForwardVector()
		+ CurrentCameraParameters->TargetDefaultOffset.Y * GetOwner()->GetActorRightVector()
		+ CurrentCameraParameters->TargetDefaultOffset.Z * GetOwner()->GetActorUpVector()
		+ GetOwner()->GetActorLocation();
	if (DeltaTime > 0)
	{
		float ZSpeed = CurrentCameraParameters->LookTargetInterSpeed.Z;
		if (CurrentCameraParameters->LookTargetZCurve && PDWCharacter)
		{
			ZSpeed = CurrentCameraParameters->LookTargetZCurve->GetFloatValue(PDWCharacter->GetVelocity().Z);
		}

		LastTargetDefaultOffset.X = FMath::FInterpTo(LastTargetDefaultOffset.X, Target.X, DeltaTime, CurrentCameraParameters->LookTargetInterSpeed.X);
		LastTargetDefaultOffset.Y = FMath::FInterpTo(LastTargetDefaultOffset.Y, Target.Y, DeltaTime, CurrentCameraParameters->LookTargetInterSpeed.Y);
		LastTargetDefaultOffset.Z = FMath::FInterpTo(LastTargetDefaultOffset.Z, Target.Z, DeltaTime, ZSpeed);
	}
	else
	{
		LastTargetDefaultOffset = Target;
	}
	FRotator DesiredRot = UKismetMathLibrary::FindLookAtRotation(AttachedCamera->GetComponentLocation(), LastTargetDefaultOffset);
	DesiredRot.Roll = 0;
	if (PDWCharacter)
	{
		DesiredRot = FRotationMatrix::MakeFromXZ(Target - AttachedCamera->GetComponentLocation(), PDWCharacter->GetActorUpVector()).Rotator();
	}
	//if (PDWCharacter && !PDWCharacterMoveComp->IsMovingOnGround())
	//{
		DesiredRot.Pitch = CurrentCameraParameters->TransformRotation.Y;
	//}

	AttachedCamera->SetWorldRotation(DesiredRot);

	if (CurrentCameraParameters->bDebugLookAt)
	{
		DrawDebugPoint(GetWorld(), Target, 40.0f, FColor::Turquoise, false, DeltaTime, 5.0f);
		DrawDebugPoint(GetWorld(), LastTargetDefaultOffset, 40.0f, FColor::Green, false, DeltaTime, 10.0f);
	}
}

void UPDWSpringArmComponent::UpdateFOV(float DeltaTime)
{
	float PreviousFOV = AttachedCamera->FieldOfView;
	float TargetFOV = CurrentCameraParameters->DefaultFOV;
	if (GetOwner() && CurrentCameraParameters->AdditionalFOVOverSpeed)
	{
		FVector Velocity = GetOwner()->GetVelocity();
		FVector NoZVelocity = FVector(Velocity.X, Velocity.Y, 0.f);
		TargetFOV += CurrentCameraParameters->AdditionalFOVOverSpeed->GetFloatValue(NoZVelocity.Size() * 0.01f);
	}

	if (bSplitScreenMode)
	{
		TargetFOV *= CurrentCameraParameters->SS_MultiplierFOV;
	}
	AttachedCamera->FieldOfView = FMath::FInterpTo(PreviousFOV, TargetFOV, DeltaTime, 2.0);
}

void UPDWSpringArmComponent::OnTeleportTriggered()
{
	if (CurrentCameraParameters)
	{
		CameraLagLocationVectorToUse = CurrentCameraParameters->LinearTeleportLagLocation;
	}
}

void UPDWSpringArmComponent::OnTeleportCompleted()
{
	// #DEV <NOt really nice, but working> [#daniele.m, 22 October 2025, OnTeleportCompleted]
	GetWorld()->GetTimerManager().SetTimer(ResetCameraLagHandle, [=, this]()
		{
			if (CurrentCameraParameters)
			{
				CameraLagLocationVectorToUse = CurrentCameraParameters->LinearLagVector;
			}
		}, 0.5f, false);
}

void UPDWSpringArmComponent::OnMovementInputChanged(FVector2D NewInput)
{
	LastMovementInput = NewInput;
}

void UPDWSpringArmComponent::OnCameraInputChanged(FVector2D NewInput)
{
	LastCameraInput = NewInput.X;
	AccumulatedCameraInput = FMath::FInterpTo(AccumulatedCameraInput, LastCameraInput, GetWorld()->GetDeltaSeconds(), CurrentCameraParameters->ManualCameraInputSpeed);
	GetWorld()->GetTimerManager().ClearTimer(CameraResetHandle);
	GetWorld()->GetTimerManager().SetTimer(CameraResetHandle, this, &ThisClass::ResetManualCamera, 1.0f, false);
}


void UPDWSpringArmComponent::ResetManualCamera()
{
	AccumulatedCameraInput = 0;
}

void UPDWSpringArmComponent::OnMultiplayerStateChange()
{
	bSplitScreenMode = UPDWGameplayFunctionLibrary::IsMultiplayerOn(GetWorld());
}

