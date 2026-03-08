#include "Components/NebulaGraphicsBillboardComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NebulaGraphics.h"

UNebulaGraphicsBillboardComponent::UNebulaGraphicsBillboardComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bTickEvenWhenPaused = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;

	bApplyImpulseOnDamage = false;
	bReplicatePhysicsToAutonomousProxy = false;

	SetGenerateOverlapEvents(false);
	SetCollisionProfileName("UI");

	CastShadow = false;
	LightingChannels.bChannel0 = false;
	LightingChannels.bChannel1 = false;
	LightingChannels.bChannel2 = false;

	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"), nullptr, LOAD_None, nullptr);
	if (PlaneMesh)
	{
		SetStaticMesh(PlaneMesh);
	}
}

void UNebulaGraphicsBillboardComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetWorld()->GetFirstPlayerController();
}

void UNebulaGraphicsBillboardComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bUseCustomTarget && PlayerController && PlayerController->PlayerCameraManager)
	{
		PlayerController->PlayerCameraManager->GetCameraViewPoint(TargetLocation, TargetRotator);
	}

	FVector StartLocation = GetComponentLocation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
	SetWorldRotation(FRotator(0.0f, LookAtRotation.Yaw + 90.0f, LookAtRotation.Roll + 90.0f));
}

void UNebulaGraphicsBillboardComponent::UseCustomTarget(const bool bInUseCustomTarget)
{
	bUseCustomTarget = bInUseCustomTarget;
}

void UNebulaGraphicsBillboardComponent::SetCustomTarget(const FVector& InTarget)
{
	TargetLocation = InTarget;
	bUseCustomTarget = true;
}

void UNebulaGraphicsBillboardComponent::GetCustomTarget(FVector& OutTarget) const
{
	OutTarget = TargetLocation;
}