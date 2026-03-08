#include "Components/NebulaGameplaySurfaceControlComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "DrawDebugHelpers.h"
#include "Math/Rotator.h"
#include "Kismet/KismetMathLibrary.h"


UNebulaGameplaySurfaceControlComponent::UNebulaGameplaySurfaceControlComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UNebulaGameplaySurfaceControlComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = GetOwner();
	if (OwnerActor)
	{
		TargetParallelRotator = OwnerActor->GetActorRotation();
	}
}

void UNebulaGameplaySurfaceControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bIsSurfaceComponentActive || !OwnerActor)
	{
		return;
	}
	
	UpdateSurfaceComponent(DeltaTime);
}

void UNebulaGameplaySurfaceControlComponent::SetIsOnGround(bool bGrounded)
{
	if (bIsOnGround != bGrounded)
	{
		bIsOnGround = bGrounded;
		HandleOnGroundedChange();
	}
}

void UNebulaGameplaySurfaceControlComponent::UpdateSurfaceComponent(float deltatime)
{
	FCollisionQueryParams Parms;
	Parms.AddIgnoredActor(OwnerActor);
	Parms.bReturnPhysicalMaterial = true;
	FHitResult Hit;
	FVector StartPoint = OwnerActor->GetActorLocation() + StartPointOffset;
	FVector EndPoint = OwnerActor->GetActorLocation() - (FVector::ZAxisVector * CollisionTraceLength);
	GetWorld()->LineTraceSingleByChannel(Hit, StartPoint, EndPoint, CollisionChannel, Parms);
	SetIsOnGround(Hit.bBlockingHit);
	if (bIsOnGround)
	{
		ImpactZPoint = Hit.ImpactPoint.Z;
		CurrentSurface = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

		if (PreviousSurface != CurrentSurface)
		{
			PreviousSurface = CurrentSurface;
			HandleSurfaceChange();
		}
		//if (bForceActorParallelToGround && OwnerActor)
		//{			
		//	FRotator CurrentOwnerRotation = OwnerActor->GetActorRotation();
		//	StartPoint = OwnerActor->GetActorLocation() +(OwnerActor->GetActorForwardVector()*ActorForwardTraceDelta);
		//	StartPoint+= FVector::ZAxisVector * 200.f;
		//	EndPoint = StartPoint - (400.f * FVector::ZAxisVector);
		//	GetWorld()->LineTraceSingleByChannel(Hit, StartPoint, EndPoint, CollisionChannel, Parms);
		//	if (Hit.bBlockingHit)
		//	{
		//		bool SkipCollision = false;
		//		
		//		for (TSubclassOf<AActor> current : ActorsToIgnore)
		//		{
		//			if (Hit.GetActor()->IsA(current))
		//			{
		//				SkipCollision = true;
		//			}
		//		}
		//		if (!SkipCollision)
		//		{
		//			FRotator NewTarget = UKismetMathLibrary::MakeRotFromZX(Hit.Normal,OwnerActor->GetActorForwardVector());
		//			float NewRoll = FMath::Clamp(NewTarget.Roll, -MaxRoll, MaxRoll);
		//			float NewPitch = FMath::Clamp(NewTarget.Pitch, -MaxPitch, MaxPitch);
		//			TargetParallelRotator = FRotator(NewPitch,CurrentOwnerRotation.Yaw, NewRoll);
		//		}
		//	}
		//	OwnerActor->SetActorRotation(FMath::RInterpTo(CurrentOwnerRotation,TargetParallelRotator,deltatime,RotationSmoothVelocity));
		//}
	}
#if WITH_EDITOR
	if (bShowDebugInfos)
	{
		FString IsOnGroundString = bIsOnGround ? FString("Is On Ground") : FString("NOT On Ground");
		DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Cyan, false, -1.0f, 0.0f, 20.0f);
		GEngine->AddOnScreenDebugMessage(152, 2.f, FColor::Yellow, FString::Printf(TEXT("%s"), *IsOnGroundString));
		GEngine->AddOnScreenDebugMessage(153, 2.f, FColor::Red, FString::Printf(TEXT("%s"), *UEnum::GetDisplayValueAsText(CurrentSurface).ToString()));
		DrawDebugSphere(GetWorld(),Hit.ImpactPoint,30,32,FColor::Red);
	}
#endif
}


void UNebulaGameplaySurfaceControlComponent::HandleSurfaceChange()
{

}

void UNebulaGameplaySurfaceControlComponent::HandleOnGroundedChange()
{
}
