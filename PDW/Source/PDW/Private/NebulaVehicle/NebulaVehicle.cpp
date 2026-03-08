#include "NebulaVehicle/NebulaVehicle.h"

#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveFloat.h"
#include "Net/UnrealNetwork.h"
#include "NebulaVehicle/NebulaSuspensionComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"

DEFINE_LOG_CATEGORY(NEBULAVehicleLog);

ANebulaVehicle::ANebulaVehicle()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collision mesh
	PrimitiveComponent = CreateOptionalDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionComponent"));
	if (IsValid(PrimitiveComponent))
	{
		PrimitiveComponent->SetSimulatePhysics(true);
		RootComponent = PrimitiveComponent;
	}
}

void ANebulaVehicle::BeginPlay()
{
	Super::BeginPlay();

	AgentBodyInstance = PrimitiveComponent->GetBodyInstance();
	BodyMass = AgentBodyInstance->GetMassOverride();

	//TArray<UActorComponent*> comps = this->GetComponentsByClass(USuspensionComponent::StaticClass());
	TArray<UActorComponent*> comps;
	GetComponents(UNebulaSuspensionComponent::StaticClass(), comps);

	if (comps.Num() == 0) { return; }

	for (int i = 0; i < comps.Num(); i++)
	{
		UNebulaSuspensionComponent* suspComp = Cast<UNebulaSuspensionComponent>(comps[i]);
		SuspensionComponents.Add(suspComp);
		suspComp->SetVehicleOwner(this);
		suspComp->SetSteerOnAir(bSteerOnAir);
	}

	OriginalMaxSpeed = MaxSpeed;
}

void ANebulaVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PhysicsGameTick(DeltaTime);
}

void ANebulaVehicle::PhysicsGameTick(float DeltaTime)
{
	GameDeltaTime = DeltaTime;

	if (bAutoWakeRigidBodies)
	{
		PrimitiveComponent->WakeAllRigidBodies();
	}

	for (int i = 0; i < SuspensionComponents.Num(); i++)
	{
		SuspensionComponents[i]->UpdateTick(DeltaTime);
		SuspensionComponents[i]->SetHandbrakeInput(bHandbrakeInput);
	}

	// Track input time
#pragma region Input time

	if ((Brakes > 0) || bAutoBrakeActive)
	{
		TimePassedBreaking += DeltaTime;
		TimePassedBreakingScaled += DeltaTime * Brakes;
	}
	else
	{
		TimePassedBreakingScaled = 0.f;
		TimePassedBreaking = 0.f;
	}

	if (abs(SteeringInput) > 0.f)
	{
		TimePassedSteering += DeltaTime;
		TimePassedSteeringScaled += DeltaTime * abs(SteeringInput);
	}
	else
	{
		TimePassedSteering = 0.f;
		TimePassedSteeringScaled = 0.f;
	}

#pragma endregion
	
	if (GetVerticalSpeedKPH() > MaxAscensionSpeed && bOnAir)
	{
		SetLinearVelocity(FVector(GetLinearVelocity().X, GetLinearVelocity().Y, MaxAscensionSpeed * 100 / 3.6), false);
	}
	else if (abs(GetVerticalSpeedKPH()) >= MaxSpeed)
	{
		SetLinearVelocity(GetLinearVelocity().GetSafeNormal() * MaxSpeed * 100 / 3.6, false);
	}
}

void ANebulaVehicle::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);

	if(bStopCar)
	{
		bStopCar = false;
		StopCar();
	}
	if (bTeleportCar)
	{
		bTeleportCar = false;
		TeleportCar(TeleportLocation, TeleportRotation);
	}

	UpdatePhysicsCore(DeltaTime);
	for (int i = 0; i < SuspensionComponents.Num(); i++)
	{
		SuspensionComponents[i]->UpdatePhysics(DeltaTime);
	}
}

void ANebulaVehicle::UpdatePhysicsCore(float DeltaTime)
{	
	CalculateVehicleStateVariable(DeltaTime);
	CheckIsOnAir(DeltaTime);
	
	UpdateEngineArcade(DeltaTime);
	UpdateDrag(DeltaTime);
}

void ANebulaVehicle::UpdateEngineArcade(float DeltaTime)
{
	const float Max = FMath::Sign(GetSpeedKPH()) > 0 ? MaxSpeed : MaxSpeedInReverse;
	const float PCTSoftMaxSpeed = GetSpeedKPH() / (Max * (Throttle == 0 ? 1 : abs(Throttle)));
	if (!bOnAir || bAccelerateOnAir)
	{
		Throttle = FMath::Clamp(Throttle, -1, 1.f);

		CurrentTorque = EngineTorque;
		if (FMath::Sign(BodyLinearSpeed.X) == FMath::Sign(Throttle))
		{
			if (EngineTorqueCurve)
			{
				CurrentTorque *= EngineTorqueCurve->GetFloatValue(abs(PCTSoftMaxSpeed));
			}
			else
			{
				CurrentTorque *= (1 - FMath::Clamp(abs(PCTSoftMaxSpeed), 0.f, 1.f));
			}
		}
		const float WheelForce = CurrentTorque * FMath::Sign(Throttle) * AccelerationMultiplier;
		FVector WheelForceVector = WheelForce * 100.f * BodyForward;
		WheelForceVector.Z = 0.f;
		AddForce(WheelForceVector, "Engine");
	}

	bAutoBrakeActive = (Throttle == 0 || FMath::Sign(BodyLinearSpeed.X) != FMath::Sign(Throttle));
	float standardBrakeForce = AutoBrakeForce;
	if (BrakeForceCurveOverSpeed)
	{
		standardBrakeForce *= BrakeForceCurveOverSpeed->GetFloatValue(abs(PCTSoftMaxSpeed));
	}
	float BrakeForce = (bAutoBrakeActive ? 1 : 0) * standardBrakeForce * -FMath::Sign(BodyLinearSpeed.X);	

	FVector TotalForce = BrakeForce * 100.f * BodyForward;
	TotalForce.Z = 0.f;
	AddForce(TotalForce, "Engine");
}

void ANebulaVehicle::UpdateDrag(float DeltaTime)
{
	float Drag = 0.5f * AirDensity * abs(BodyLinearSpeed.X) * CrossSectionArea * CoefficientOfDrag * 100;
	AddForce(BodyForward * Drag * -FMath::Sign(BodyLinearSpeed.X), "Drag");
}

void ANebulaVehicle::CheckIsOnAir(float DeltaTime)
{
	OnAirWheel = 0;
	for (int i = 0; i < SuspensionComponents.Num(); i++)
	{
		if (SuspensionComponents[i]->IsInAir())
		{
			OnAirWheel++;
		}
	}
	SetIsOnAir(OnAirWheel >= OnAirWheelCount, DeltaTime);
}

void ANebulaVehicle::StopCarBP(FVector Location /*= FVector::ZeroVector*/, FRotator Rotation /*= FRotator::ZeroRotator*/, bool Immediatly /*= false*/)
{
	bStopCar = true;
	bStopCarImmediatly = Immediatly;

	if ((Location != FVector::ZeroVector) && (Rotation != FRotator::ZeroRotator))
	{
		TeleportLocation = Location;
		TeleportRotation = Rotation;
		bTeleportCar = true;
	}
}

void ANebulaVehicle::StopCar(bool Immediatly)
{
	//if (Immediatly)
	//{
	//	SetLinearVelocity(FVector::ZeroVector, false);
	//	SetAngularVelocityInDegrees(FVector::ZeroVector, false);
	//}

	SetThrottleInput(0);
	SetSteeringInput(0);
	SetBrakeInput(0);
}

void ANebulaVehicle::TeleportCar(FVector Location, FRotator Rotation)
{
	SetWorldLocationAndRotation(Location, Rotation);
}

void ANebulaVehicle::AnimateSteering()
{
	for (int i = 0; i < SuspensionComponents.Num(); i++)
	{
		SuspensionComponents[i]->SetSteeringInput(SteeringInput, SteeringSensitivity);
	}
}

void ANebulaVehicle::OverrideRPM(float newRPM)
{
	RPM = newRPM;
}

void ANebulaVehicle::RTWakeRigidBodies()
{
	PrimitiveComponent->WakeAllRigidBodies();
}

#pragma region Setters

void ANebulaVehicle::SetThrottleInput(float value)
{
	Throttle = IsUpshifting ? 0 : value;
}

void ANebulaVehicle::ApplyHandbrakeInput(bool value)
{
	bHandbrakeInput = value;
}

void ANebulaVehicle::SetBrakeInput(float value)
{
	Brakes = value;
	for (int i = 0; i < SuspensionComponents.Num(); i++)
	{
		SuspensionComponents[i]->SetBrakeInput(Brakes);
	}
}

float ANebulaVehicle::GetProcessedSteeringInput(float value)
{
	float processedInput = value;
	if (CurveSteeringInputClamp != nullptr)
	{
		processedInput *= CurveSteeringInputClamp->GetFloatValue(GetSpeedKPH() / MaxSpeed);
	}

	return processedInput;
}

void ANebulaVehicle::SetSteeringInput(float value)
{
	SteeringInput = FMath::FInterpTo(SteeringInput, GetProcessedSteeringInput(value), GameDeltaTime, SteeringSensitivity);
	if (value == 0)
	{
		SteeringInput = 0;
	}
	AnimateSteering();
}

void ANebulaVehicle::SetMaxSpeed(float Soft, float Hard, bool Reset /*= false*/)
{
	MaxSpeed = Reset ? OriginalMaxSpeed : Soft;
}

void ANebulaVehicle::SetIsOnAir(bool value, float DeltaTime)
{
	if (bOnAir != value)
	{
		bOnAir = value;
		if (bOnAir)
		{
			OnVehicleTakeOff.Broadcast();
		}
		else
		{
			OnVehicleLanding.Broadcast();
		}
	}
	OnAirTime = !bOnAir ? 0 : (OnAirTime + DeltaTime);
}

#pragma endregion

#pragma region Getters

float ANebulaVehicle::GetRPM()
{
	return RPM;
}

FTransform ANebulaVehicle::GetPhysicsTransform()
{
	return BodyTransform;
}

float ANebulaVehicle::GetSideSlipAngle()
{
	return FMath::RadiansToDegrees(FMath::Atan(UKismetMathLibrary::SafeDivide(GetVelocityVector().Y, GetVelocityVector().X)));
}

void ANebulaVehicle::SetAccelerationMultiplier(float value)
{
	AccelerationMultiplier = value; 
}

float ANebulaVehicle::GetSpeedKPH()
{
	return BodyLinearSpeed.X * 3.6f;
}

float ANebulaVehicle::GetVerticalSpeedKPH()
{
	return BodyLinearSpeed.Z * 3.6f;
}

FVector ANebulaVehicle::GetVelocityVector()
{
	return BodyLinearSpeed * 3.6f;
}

float ANebulaVehicle::GetThrottleInput()
{
	return Throttle;
}

float ANebulaVehicle::GetBrakesInput()
{
	return Brakes;
}

float ANebulaVehicle::GetSteeringInput()
{
	return SteeringInput;
}

bool ANebulaVehicle::GetHandbrakeInput()
{
	return bHandbrakeInput;
}

EPhysicalSurface ANebulaVehicle::GetWheelMaterial(int WheelIndex)
{
	if (SuspensionComponents.Num() <= 0)
	{
		return EPhysicalSurface();
	}
	if (SuspensionComponents[WheelIndex] != nullptr && SuspensionComponents[WheelIndex]->GetHitResult().PhysMaterial != nullptr)
	{
		return UPhysicalMaterial::DetermineSurfaceType(SuspensionComponents[WheelIndex]->GetHitResult().PhysMaterial.Get());
	}
	return EPhysicalSurface();
}

float ANebulaVehicle::GetEngineCurrentTorque()
{
	return CurrentTorque;
}

#pragma endregion

#pragma region To move to generic function library

void ANebulaVehicle::AddForce(FVector Force, FString DebugName /*= FString()*/)
{
	if (Force.SquaredLength() == 0)
		return;
	if (!BodyHandle)
		return;
	BodyHandle->AddForce(Force);
	UE_VLOG_ARROW(this, NEBULAVehicleLog, Verbose, CentreOfMass, CentreOfMass + Force / 1000, FColor::Red, TEXT("%s"), *DebugName);
}

void ANebulaVehicle::AddTorque(FVector Torque, bool bAccelChange, FString DebugName /*= FString()*/)
{
	if (Torque.SquaredLength() == 0)
		return;
	if (!BodyHandle)
		return;
	if (bAccelChange)
	{
		BodyHandle->AddTorque(Chaos::FParticleUtilitiesXR::GetWorldInertia(BodyHandle) * Torque, false);
	}
	else
	{
		BodyHandle->AddTorque(Torque, false);
	}
}

void ANebulaVehicle::AddForceAtLocation(FVector Force, FVector Location, FString DebugName /*= FString()*/)
{
	if (Force.SquaredLength() == 0)
		return;
	if (!BodyHandle)
		return;
	FVector WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(BodyHandle);
	FVector WorldTorque = Chaos::FVec3::CrossProduct(Location - WorldCOM, Force);
	BodyHandle->AddForce(Force, false);
	BodyHandle->AddTorque(WorldTorque, false);

	UE_VLOG_ARROW(this, NEBULAVehicleLog, Verbose, CentreOfMass, CentreOfMass + Force / 1000, FColor::Red, TEXT("%s"), *DebugName);
}

void ANebulaVehicle::AddImpulse(FVector Impulse, FString DebugName /*= FString()*/)
{
	if (BodyHandle)
	{
		BodyHandle->SetLinearImpulse(BodyHandle->LinearImpulse() + BodyHandle->M() * Impulse, false);
	}
}

void ANebulaVehicle::AddImpulseAtLocation(FVector Impulse, FVector Location, FString DebugName /*= FString()*/)
{
	if (BodyHandle)
	{
		const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(BodyHandle);
		const Chaos::FVec3 AngularImpulse = Chaos::FVec3::CrossProduct(Location - WorldCOM, Impulse);
		BodyHandle->SetLinearImpulse(BodyHandle->LinearImpulse() + Impulse, false);
		BodyHandle->SetAngularImpulse(BodyHandle->AngularImpulse() + AngularImpulse, false);
	}
}

FTransform ANebulaVehicle::GetPhysTransform()
{
	if (BodyHandle)
	{
		const Chaos::FRigidTransform3 WorldCOM = Chaos::FParticleUtilitiesGT::GetActorWorldTransform(BodyHandle);
		return WorldCOM;
	}
	return FTransform();
}

FVector ANebulaVehicle::GetLinearVelocity()
{
	if (BodyHandle)
	{
		return BodyHandle->V();
	}
	return FVector::ZeroVector;
}

FVector ANebulaVehicle::GetLinearVelocityAtPoint(FVector Point)
{
	if (BodyHandle)
	{
		if (ensure(BodyHandle->CanTreatAsKinematic()))
		{
			const bool bIsRigid = BodyHandle->CanTreatAsRigid();
			const Chaos::FVec3 COM = bIsRigid
											? Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(BodyHandle)
											: static_cast<Chaos::FVec3>(
												Chaos::FParticleUtilitiesGT::GetActorWorldTransform(BodyHandle).
												GetTranslation());
			const Chaos::FVec3 Diff = Point - COM;
			return BodyHandle->V() - Chaos::FVec3::CrossProduct(Diff, BodyHandle->W());
		}
	}
	return FVector::ZeroVector;
}

FVector ANebulaVehicle::GetAngularVelocity()
{
	if (BodyHandle)
	{
		return BodyHandle->W();
	}
	return FVector::ZeroVector;
}

FVector ANebulaVehicle::GetLinearAcceleration()
{
	return BodyLinearAcceleration;
}	

FVector ANebulaVehicle::GetCOMLocation()
{
	if (BodyHandle)
	{
		if (ensure(BodyHandle->CanTreatAsKinematic()))
		{
			const bool bIsRigid = BodyHandle->CanTreatAsRigid();
			return bIsRigid
				       ? Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(BodyHandle)
				       : static_cast<Chaos::FVec3>(Chaos::FParticleUtilitiesGT::GetActorWorldTransform(BodyHandle).
					       GetTranslation());
		}
	}
	return FVector::ZeroVector;
}

void ANebulaVehicle::SetLinearVelocity(FVector NewLinVelocity, bool bAddToCurrent)
{
	if (BodyHandle)
	{
		if (bAddToCurrent)
		{
			BodyHandle->SetV(BodyHandle->V() + NewLinVelocity);
		}
		else
		{
			BodyHandle->SetV(NewLinVelocity);
		}
	}
}

void ANebulaVehicle::SetAngularVelocityInRadians(FVector NewAngVelocity, bool bAddToCurrent)
{
	if (BodyHandle)
	{
		if (bAddToCurrent)
		{
			BodyHandle->SetW(BodyHandle->W() + NewAngVelocity);
		}
		else
		{
			BodyHandle->SetW(NewAngVelocity);
		}
	}
}

void ANebulaVehicle::SetAngularVelocityInDegrees(FVector NewAngVelocity, bool bAddToCurrent)
{
	SetAngularVelocityInRadians(FMath::DegreesToRadians(NewAngVelocity), bAddToCurrent);
}

void ANebulaVehicle::SetWorldLocationAndRotation(FVector Location, FRotator Rotation)
{
	if (BodyHandle)
	{
		const Chaos::FRotation3 Q = Rotation.Quaternion() /** RigidHandle->RotationOfMass().Inverse()*/;
		const Chaos::FVec3 P = Location /*- Q.RotateVector(RigidHandle->CenterOfMass())*/;
		BodyHandle->SetR(Q);
		BodyHandle->SetX(P);
	}
}

#pragma endregion

void ANebulaVehicle::CalculateVehicleStateVariable(float DeltaTime)
{
	if (!AgentBodyInstance && AgentBodyInstance->IsValidBodyInstance())
		return;
	if (!AgentBodyInstance->GetPhysicsActorHandle())
		return;
	BodyHandle = AgentBodyInstance->GetPhysicsActorHandle()->GetPhysicsThreadAPI();
	if (!BodyHandle)
		return;

	CentreOfMass = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(BodyHandle);
	
	BodyTransform = Chaos::FParticleUtilitiesGT::GetActorWorldTransform(BodyHandle);
	BodyForward = BodyTransform.GetUnitAxis(EAxis::X);
	BodyRight = BodyTransform.GetUnitAxis(EAxis::Y);
	BodyUp = BodyTransform.GetUnitAxis(EAxis::Z);

	BodyLocation = BodyTransform.GetLocation();
	BodyRotation = BodyTransform.GetRotation();

	BodyAngolarSpeed = BodyHandle->GetW();
	BodyAngolarSpeed = BodyRotation.UnrotateVector(BodyAngolarSpeed);

	FVector CandidateLinearSpeed = BodyHandle->GetV();
	CandidateLinearSpeed = BodyRotation.UnrotateVector(CandidateLinearSpeed) / 100;
	if (fabs(BodyLinearSpeed.X - CandidateLinearSpeed.X) > 1000.0f)
	{
		CandidateLinearSpeed.X = FMath::Clamp(CandidateLinearSpeed.X, -500.0f, 500.0f);
		CandidateLinearSpeed.Y = FMath::Clamp(CandidateLinearSpeed.Y, -500.0f, 500.0f);
		CandidateLinearSpeed.Z = FMath::Clamp(CandidateLinearSpeed.Z, -500.0f, 500.0f);
	}
	BodyLinearSpeed = CandidateLinearSpeed;
	BodyLinearAcceleration = (BodyLinearSpeed - PreviousBodyLinearSpeed) / DeltaTime;
	PreviousBodyLinearSpeed = BodyLinearSpeed;
}