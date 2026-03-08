//Copyright 2024 P.Kallisto SKG

#include "NebulaVehicle/NebulaSuspensionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/StaticMesh.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Modules/ModuleInterface.h"
#include "PhysicsInterfaceDeclaresCore.h"

UNebulaSuspensionComponent::UNebulaSuspensionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNebulaSuspensionComponent::BeginPlay()
{
	Super::BeginPlay();

	VehiclePrimitiveComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	if (VehiclePrimitiveComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("FTSuspension: No valid primitive root component! in vehicle found"));
	}
	
	WheelMeshComponent = NewObject<UStaticMeshComponent>(GetOwner());
	if (WheelMeshComponent != nullptr)
	{
		WheelMeshComponent->RegisterComponent();
		WheelMeshComponent->SetWorldLocation(this->GetComponentLocation());
		WheelMeshComponent->SetWorldRotation(this->GetComponentRotation());
		if (WheelMesh != nullptr)
		{
			WheelMeshComponent->SetStaticMesh(WheelMesh);
			for (int i = 0; i < WheelMaterials.Num() - 1; i++)
			{
				if (WheelMaterials[i])
				{
					WheelMeshComponent->SetMaterial(i, WheelMaterials[i]);
				}
			}
			WheelRadius = WheelMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z / 2.f;
		}
		else
		{
			WheelRadius = Radius;
		}
		WheelMeshComponent->SetRenderCustomDepth(bRenderCustomDepthPass);
		WheelMeshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
		WheelMeshComponent->SetIsReplicated(false);
		WheelMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (bRotateWheel)
		{
			WheelMeshComponent->SetWorldScale3D(this->GetComponentScale());
			WheelMeshComponent->SetRelativeRotation((FRotator(0.f, 180.f, 0.f)));
		}
	}

	SweepShape = FCollisionShape::MakeSphere(WheelRadius);

	QueryParam = FCollisionQueryParams::DefaultQueryParam;
	QueryParam.TraceTag = "Trace";
	QueryParam.AddIgnoredActor(GetOwner());
	QueryParam.AddIgnoredComponent(WheelMeshComponent);
	QueryParam.bReturnPhysicalMaterial = true;
	
}

void UNebulaSuspensionComponent::DebugSuspension()
{
	if (!bDebugSuspension)
		return;
	if (!Hit.bBlockingHit)
		return;

	DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor::Red, false, 2.f);
}

void UNebulaSuspensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UNebulaSuspensionComponent, ServerSteeringRotation);
}

void UNebulaSuspensionComponent::UpdateSteeringGeometry(float DeltaTime)
{
	float r = 0.f;
	if (bIsSteeringWheel)
	{
		float steeringSpeed = SteeringInput == 0 ? SteeringReleaseSpeed : SensitvityInput;
		r = FMath::FInterpTo(GetRelativeRotation().Yaw, SteeringInput * MaxSteeringAngle, GameTime, steeringSpeed);
		SteeringRotation = r;
	}
	if (GetOwner()->HasAuthority())
	{
		ServerSteeringRotation = r;
		FRotator rotator = FRotator(0.f, r, 0.f);
		this->SetRelativeRotation(rotator);
	}
	else
	{
		ClientTimeSinceUpdate += DeltaTime;

		float lerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdates;
		float smooth_q = FMath::LerpStable(ClientStartSteeringRotation, ServerSteeringRotation, lerpRatio);

		FRotator qs = FRotator(0.f, ServerSteeringRotation, 0.f);
		this->SetRelativeRotation(qs);
	}
}

void UNebulaSuspensionComponent::OnRep_Steering()
{
	ClientTimeBetweenLastUpdates = ClientTimeSinceUpdate;
	ClientTimeSinceUpdate = 0;

	if (bIsSteeringWheel)
	{
		float steeringSpeed = SteeringInput == 0 ? SteeringReleaseSpeed : SensitvityInput;
		float r = FMath::FInterpTo(GetRelativeRotation().Yaw, SteeringInput * MaxSteeringAngle, GameTime, steeringSpeed);
		//float r = SteeringInput * MaxSteeringAngle;
		ClientStartSteeringRotation = r;
		FRotator qs = FRotator(0.f,ClientStartSteeringRotation, 0.f);
	}
	//this->SetRelativeRotation(qs);
	//Maybe update relative rotation?
}

void UNebulaSuspensionComponent::UpdateTick(float DeltaTime)
{	
	GameTime = DeltaTime;

	float Rad = bSingleRaycast ? WheelRadius : 0;
	WheelMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -CurrentLength + Rad));
	WheelRelativeOffset = -CurrentLength + Rad;

	UpdateSteeringGeometry(DeltaTime);

	float angularVelocity = (LinearVelocity.X * 100.f) / WheelRadius * -1.f;

	//If the frame rate is higher than 60, scale the rotation accordingly to compensate these extra rotations added every frame
	float timeScalar = 60.f / FMath::Pow(DeltaTime, -1.f);

	FRotator angularRotator = FRotator(angularVelocity * timeScalar, 0.f, 0.f);

	int d = bRotateWheel ? -1 : 1;
	if (bHandbrakeInput && bHandbrakeWheel)
	{
		WheelMeshComponent->AddLocalRotation(FRotator(0.f));
		WheelLocalRotation = FRotator::ZeroRotator;
	}
	else
	{
		WheelLocalRotation = angularRotator * WheelAngularVelocityMultiplier * d;
		WheelMeshComponent->AddLocalRotation(angularRotator * WheelAngularVelocityMultiplier * d);
	}

	DebugSuspension();
}

void UNebulaSuspensionComponent::RecalculateWheelRadius()
{
	if (WheelMeshComponent->GetStaticMesh() != nullptr)
	{
		WheelRadius = WheelMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z / 2.f;
	}
	else
	{
		WheelRadius = Radius;
	}

	SweepShape = FCollisionShape::MakeSphere(WheelRadius);
}

void UNebulaSuspensionComponent::UpdatePhysics(float DeltaTime)
{
	physicsTransform = GetRelativeTransform() * VehicleOwner->BodyTransform;
	Start = physicsTransform.GetLocation();
	
	up = physicsTransform.GetRotation().GetUpVector();
	right = physicsTransform.GetRotation().GetRightVector();

	// m/s
	LinearVelocity = VehicleOwner->GetLinearVelocityAtPoint(Start);
	LinearVelocity = UKismetMathLibrary::InverseTransformDirection(physicsTransform, LinearVelocity) * 0.01f;

	Acceleration = VehicleOwner->GetLinearAcceleration();

#pragma region Trace

	GetWorld()->SweepSingleByChannel(Hit, Start, Start + (-up * ExtensionLength), FQuat::Identity, CollisionChannel, SweepShape, QueryParam);

	FVector worldDirection = (physicsTransform.GetLocation() - Hit.ImpactPoint);
	worldDirection.Normalize();
	FVector ImpactNormal = UKismetMathLibrary::InverseTransformDirection(physicsTransform, worldDirection);

	FVector localImpactPoint = UKismetMathLibrary::InverseTransformLocation(VehicleOwner->BodyTransform, Hit.ImpactPoint);
		
	bool safteyCondition = FMath::Abs(ImpactNormal.Y) <= ImpactNormalTolerance && localImpactPoint.Z <= ImpactHeightTolerance;

	bSingleRaycast = false;

	GroundDistance = (Start - Hit.Location).Size();
	float length = ExtensionLength;
	if (Hit.bBlockingHit && safteyCondition && bHybridRaycast)
	{
		CurrentLength = (Start - Hit.Location).Size();
	}
	else
	{
		bSingleRaycast = true;
		length += WheelRadius/* + (0.125f * WheelRadius)*/;
		GetWorld()->LineTraceSingleByChannel(Hit, Start, Start - (up * 10000), CollisionChannel, QueryParam);
		GroundDistance = (Start - Hit.Location).Size();
		CurrentLength = FMath::Clamp(GroundDistance, 0.f, length);

		if (GroundDistance > length)
		{
			CurrentLength = length;
		}
	}

#pragma endregion

	const float compression = 1 - (CurrentLength / length);
	float velocity = (compression - CurrentCompression) / DeltaTime;
	if (CurrentCompression == -1)
	{
		velocity = 0.f;
	}
	const float force = compression * Stiffness * 100;
	const float damp = FMath::Clamp(Damping * velocity * 100, -force, force);
	CurrentCompression = compression;

	if (Hit.bBlockingHit)
	{
		VehicleOwner->AddForceAtLocation((force + damp) * 100.f * Hit.Normal, physicsTransform.GetLocation(), "Suspension");
	}
	if (bSteerOnAir || !IsTooSteep())
	{
		UpdateLateralForce(DeltaTime);
	}
}

void UNebulaSuspensionComponent::UpdateLateralForce(float DeltaTime)
{
	// Lateral velocity
	CurrentLateralForce = -LinearVelocity.Y * LateralForceScalar;

	// Clamp
	if (MaxLateralForce != -1)
	{
		CurrentLateralForce = FMath::Clamp(CurrentLateralForce, -MaxLateralForce, MaxLateralForce);
	}

	UpdateLateralForceCut(DeltaTime);

	CurrentLateralForce *= CurrentLateralForceCut * 100;

	FVector com = VehicleOwner->CentreOfMass;
	FVector z1 = FVector(0.f, 0.f, UKismetMathLibrary::InverseTransformLocation(physicsTransform, Start).Z);
	FVector z2 = FVector(0.f, 0.f, UKismetMathLibrary::InverseTransformLocation(physicsTransform, com).Z);
	float comDistOffset = FVector::Dist(z1, z2);

	const FVector Loc = Start + (up * comDistOffset) + (-up * ContactForceOffsetLocation);
	FVector CurrentLaterForceVector = CurrentLateralForce * right;
	CurrentLaterForceVector.Z = 0.f;
	VehicleOwner->AddForceAtLocation(CurrentLaterForceVector, Loc, "LateralForce");

	//UE_VLOG_ARROW(VehicleOwner, NEBULASuspensionLog, Verbose, Loc, Loc + right * CurrentLateralForce / 1000, FColor::Red, TEXT("Lateral force"));
}

#pragma region Setters

bool UNebulaSuspensionComponent::IsSteeringWheel()
{
	return bIsSteeringWheel;
}

void UNebulaSuspensionComponent::SetStiffness(float inStiffness)
{
	Stiffness = inStiffness;
}

void UNebulaSuspensionComponent::SetDamping(float inDamping)
{
	Damping = inDamping;
}

void UNebulaSuspensionComponent::SetExtensionLength(float inLength)
{
	ExtensionLength = inLength;
}

void UNebulaSuspensionComponent::SetWheelScale(float inScale)
{
	if (WheelMeshComponent->GetStaticMesh() != nullptr)
	{
		WheelRadius = WheelMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z / 2.f;
	}
	else
	{
		WheelRadius = Radius;
	}

	SweepShape = FCollisionShape::MakeSphere(WheelRadius * inScale);
	WheelMeshComponent->SetRelativeScale3D(FVector(inScale));
}

void UNebulaSuspensionComponent::SetSteeringInput(float inSteering, float sensitivity)
{
	SteeringInput = inSteering;
	SensitvityInput = sensitivity;
}

void UNebulaSuspensionComponent::SetHandbrakeInput(bool bInHandbrakeOn)
{
	bHandbrakeInput = bInHandbrakeOn;
}

#pragma endregion

#pragma region Getters

UStaticMeshComponent* UNebulaSuspensionComponent::GetWheelMeshComponent()
{
	return WheelMeshComponent;
}

bool UNebulaSuspensionComponent::IsInAir()
{
	const FVector Normal = GetHitResult().ImpactNormal;
	const float UpDot = Normal | FVector::UpVector;	
	
	return (GroundDistance > ExtensionLength) || (UpDot < 0.9);
}

bool UNebulaSuspensionComponent::IsTooSteep()
{
	if (!GetHitResult().bBlockingHit) return false;

	const FVector Normal = GetHitResult().ImpactNormal;
	const float UpDot = Normal | FVector::UpVector;	
	
	return UpDot < 0.9;
}

float UNebulaSuspensionComponent::GetCurrentLength()
{
	return CurrentLength;
}

FHitResult UNebulaSuspensionComponent::GetHitResult()
{
	return Hit;
}

float UNebulaSuspensionComponent::GetCompression()
{
	return CurrentCompression;
}

bool UNebulaSuspensionComponent::IsHandbrakeWheel()
{
	return bHandbrakeWheel;
}

float UNebulaSuspensionComponent::GetSteeringRotation()
{
	return SteeringRotation;
}

float UNebulaSuspensionComponent::GetWheelRelativeOffset()
{
	return WheelRelativeOffset;
}

FRotator UNebulaSuspensionComponent::GetWheelLocalRotation()
{
	return WheelLocalRotation;
}

#pragma endregion