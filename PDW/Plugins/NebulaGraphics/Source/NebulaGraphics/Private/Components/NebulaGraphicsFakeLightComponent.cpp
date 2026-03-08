#include "Components/NebulaGraphicsFakeLightComponent.h"

#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NebulaGraphics.h"

UNebulaGraphicsFakeLightComponent::UNebulaGraphicsFakeLightComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
}

void UNebulaGraphicsFakeLightComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR

	SetupFakeLight();

#endif
}

void UNebulaGraphicsFakeLightComponent::OnUnregister()
{
#if WITH_EDITOR

	if (FakeLightStaticMeshComponent)
	{
		FakeLightStaticMeshComponent->DestroyComponent();
		FakeLightStaticMeshComponent = nullptr;
	}

#endif

	Super::OnUnregister();
}

void UNebulaGraphicsFakeLightComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupFakeLight();
}

void UNebulaGraphicsFakeLightComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (FakeLightStaticMeshComponent)
	{
		FakeLightStaticMeshComponent->SetVisibility(LightComponent->Intensity > 0.0f);
	}
}

void UNebulaGraphicsFakeLightComponent::SetupFakeLight()
{
	UActorComponent* ActorComponent = GetOwner()->GetComponentByClass(ULightComponent::StaticClass());
	if (ActorComponent)
	{
		LightComponent = Cast<ULightComponent>(ActorComponent);
		check(LightComponent);

		// Create fake light  mesh component
		if (FakeLightStaticMeshComponent == nullptr)
		{
			FakeLightStaticMeshComponent = NewObject<UStaticMeshComponent>(this);
			FakeLightStaticMeshComponent->SetMobility(EComponentMobility::Movable);
			FakeLightStaticMeshComponent->SetRelativeTransform(FakeLightTransform);
			FakeLightStaticMeshComponent->SetupAttachment(GetOwner()->GetRootComponent());
			FakeLightStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			FakeLightStaticMeshComponent->SetCastShadow(false);
			FakeLightStaticMeshComponent->LDMaxDrawDistance = FakeLightDrawDistance;
			FakeLightStaticMeshComponent->SetCachedMaxDrawDistance(FakeLightDrawDistance);
			FakeLightStaticMeshComponent->RegisterComponent();
		}

		if (FakeLightStaticMeshComponent && FakeLightStaticMesh)
		{
			FakeLightStaticMeshComponent->SetStaticMesh(FakeLightStaticMesh);
			if (FakeLightMaterialInstance)
			{
				UMaterialInstanceDynamic* FakeLightMaterialInstanceDynamic = FakeLightStaticMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, FakeLightMaterialInstance);
				if (FakeLightMaterialInstanceDynamic)
				{
					// Multiply MaxDrawDistance with console command LightMaxDrawDistanceScale
					FakeLightMaterialInstanceDynamic->SetScalarParameterValue(LightMaxDrawDistanceParameterName, LightComponent->MaxDrawDistance * UKismetSystemLibrary::GetConsoleVariableFloatValue("r.LightMaxDrawDistanceScale"));

					// Light calculate draw distance from center of his bound.
					FakeLightMaterialInstanceDynamic->SetVectorParameterValue(LightCenterParameterName, LightComponent->GetBoundingSphere().Center);

					// Use same light color or temperature
					if (UseLightTemperature)
					{
						FakeLightMaterialInstanceDynamic->SetVectorParameterValue(LightColorMaterialParameterName, FLinearColor::MakeFromColorTemperature(LightComponent->Temperature));
					}
					else
					{
						FakeLightMaterialInstanceDynamic->SetVectorParameterValue(LightColorMaterialParameterName, LightComponent->GetLightColor());
					}

					// Apply fake light inner radius parameter
					FakeLightMaterialInstanceDynamic->SetScalarParameterValue(FakeLightSphereInnerRadiusParameterName, FakeLightSphereInnerRadius);

					// Apply fake light outer radius parameter
					FakeLightMaterialInstanceDynamic->SetScalarParameterValue(FakeLightSphereOuterRadiusParameterName, FakeLightSphereOuterRadius);
				}
			}
			else
			{
				UE_LOG(LogNebulaGraphics, Warning, TEXT("FakeLightMaterial not found."));
			}
		}
		else
		{
			UE_LOG(LogNebulaGraphics, Warning, TEXT("FakeLightStaticMesh not found."));
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("LightComponent not found."));
	}
}