#include "Actors/PCG/NebulaToolPCGSpawnerActor.h"

#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "FoliageType.h"
#include "FoliageType_InstancedStaticMesh.h"
#include "NebulaTool.h"
#include "PCGComponent.h"

ANebulaToolPCGSpawnerActor::ANebulaToolPCGSpawnerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxVolumeComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxVolumeComponent"));
	check(BoxVolumeComponent);
	BoxVolumeComponent->SetBoxExtent({ 500.0f, 500.0f, 500.0f });
	BoxVolumeComponent->SetCanEverAffectNavigation(false);
	BoxVolumeComponent->SetEnableGravity(false);
	BoxVolumeComponent->bApplyImpulseOnDamage = false;
	BoxVolumeComponent->SetGenerateOverlapEvents(false);
	BoxVolumeComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	BoxVolumeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxVolumeComponent->SetCastShadow(false);
	BoxVolumeComponent->SetAffectDynamicIndirectLighting(false);
	BoxVolumeComponent->SetAffectDistanceFieldLighting(false);
	BoxVolumeComponent->bCastDynamicShadow = false;
	BoxVolumeComponent->bCastStaticShadow = false;
	BoxVolumeComponent->SetCastContactShadow(false);
	BoxVolumeComponent->SetReceivesDecals(false);
	BoxVolumeComponent->bEnableAutoLODGeneration = false;
	BoxVolumeComponent->bReceiveMobileCSMShadows = false;
	BoxVolumeComponent->SetupAttachment(RootComponent);

	PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCGComponent"));
	check(PCGComponent);

	bEnableAutoLODGeneration = false;
	SetCanBeDamaged(false);

#if WITH_EDITORONLY_DATA

	BillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	if (BillboardComponent)
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> TriggerTextureObject;
			FName ID_Triggers;
			FText NAME_Triggers;
			FConstructorStatics()
				: TriggerTextureObject(TEXT("/Engine/EditorResources/S_Trigger"))
				, ID_Triggers(TEXT("Triggers"))
				, NAME_Triggers(NSLOCTEXT("SpriteCategory", "Triggers", "Triggers"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		BillboardComponent->Sprite = ConstructorStatics.TriggerTextureObject.Get();
		BillboardComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		BillboardComponent->bHiddenInGame = false;
		BillboardComponent->bIsScreenSizeScaled = true;
	}
#endif
}

void ANebulaToolPCGSpawnerActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	PCGComponent->OnPCGGraphGeneratedExternal.AddUniqueDynamic(this, &ThisClass::SetOverrides);
}

void ANebulaToolPCGSpawnerActor::Destroyed()
{
	PCGComponent->OnPCGGraphGeneratedExternal.RemoveDynamic(this, &ThisClass::SetOverrides);

	Super::Destroyed();
}

void ANebulaToolPCGSpawnerActor::SetOverrides(UPCGComponent* InPCGComponent)
{
	if (InPCGComponent && !InstanceSettingsArray.IsEmpty())
	{
		TArray<UInstancedStaticMeshComponent*> InstancedStaticMeshComponentArray = {};
		GetComponents<UInstancedStaticMeshComponent>(InstancedStaticMeshComponentArray);
		for (UInstancedStaticMeshComponent* InstancedStaticMeshComponent : InstancedStaticMeshComponentArray)
		{
			const UStaticMesh* StaticMesh = InstancedStaticMeshComponent->GetStaticMesh();
			if (StaticMesh)
			{
				for (const FInstanceSettings& InstanceSettings : InstanceSettingsArray)
				{
					if (InstanceSettings.StaticMesh == StaticMesh)
					{
						// Set OverrideMaterials
						if (InstanceSettings.bOverrideMaterials && !InstanceSettings.OverrideMaterialMap.IsEmpty())
						{
							for (auto& MaterialOverrideInfo : InstanceSettings.OverrideMaterialMap)
							{
								if (MaterialOverrideInfo.Value)
								{
									InstancedStaticMeshComponent->SetMaterialByName(MaterialOverrideInfo.Key, MaterialOverrideInfo.Value);
								}
							}
						}
						else if (!InstanceSettings.bOverrideMaterials && InstanceSettings.MeshType == EMeshType::Foliage)
						{
							const UFoliageType_InstancedStaticMesh* FoliageTypeInstancedStaticMesh = Cast<UFoliageType_InstancedStaticMesh>(InstanceSettings.FoliageType);
							if (FoliageTypeInstancedStaticMesh)
							{
								for (int32 Index = 0; Index < FoliageTypeInstancedStaticMesh->OverrideMaterials.Num(); ++Index)
								{
									InstancedStaticMeshComponent->SetMaterial(Index, FoliageTypeInstancedStaticMesh->OverrideMaterials[Index]);
								}
							}
						}
						else
						{
							for (const auto& OriginalMaterialInfo : InstanceSettings.StaticMesh->GetStaticMaterials())
							{
								if (OriginalMaterialInfo.MaterialInterface)
								{
									InstancedStaticMeshComponent->SetMaterialByName(OriginalMaterialInfo.MaterialSlotName, OriginalMaterialInfo.MaterialInterface);
								}
							}
						}
					}

					if (InstanceSettings.MeshType == EMeshType::Mesh)
					{
						InstancedStaticMeshComponent->SetMobility(InstanceSettings.Mobility);
						InstancedStaticMeshComponent->SetCullDistance(InstanceSettings.CullDistance);
						InstancedStaticMeshComponent->SetCastShadow(InstanceSettings.bCastShadow);
						InstancedStaticMeshComponent->SetAffectDynamicIndirectLighting(InstanceSettings.bAffectDynamicIndirectLighting);
						InstancedStaticMeshComponent->SetAffectDistanceFieldLighting(InstanceSettings.bAffectDistanceFieldLighting);
						InstancedStaticMeshComponent->SetCastContactShadow(InstanceSettings.bCastContactShadow);
						InstancedStaticMeshComponent->bCastDynamicShadow = InstanceSettings.bCastDynamicShadow;
						InstancedStaticMeshComponent->bCastStaticShadow = InstanceSettings.bCastStaticShadow;
						InstancedStaticMeshComponent->SetCastInsetShadow(InstanceSettings.bCastInsetShadow);
						InstancedStaticMeshComponent->bCastShadowAsTwoSided = InstanceSettings.bCastShadowAsTwoSided;
						InstancedStaticMeshComponent->SetReceivesDecals(InstanceSettings.bReceivesDecals);
						InstancedStaticMeshComponent->bOverrideLightMapRes = InstanceSettings.bOverrideLightMapRes;
						InstancedStaticMeshComponent->bUseAsOccluder = InstanceSettings.bUseAsOccluder;
						InstancedStaticMeshComponent->bEnableAutoLODGeneration = InstanceSettings.bIncludeInHLOD;
						InstancedStaticMeshComponent->SetEvaluateWorldPositionOffset(InstanceSettings.bEvaluateWorldPositionOffset);
						InstancedStaticMeshComponent->bWorldPositionOffsetWritesVelocity = InstanceSettings.bWorldPositionOffsetWritesVelocity;
						InstancedStaticMeshComponent->SetWorldPositionOffsetDisableDistance(InstanceSettings.WorldPositionOffsetDisableDistance);
						InstancedStaticMeshComponent->BodyInstance = InstanceSettings.BodyInstance;
						InstancedStaticMeshComponent->SetLightingChannels(InstanceSettings.LightingChannels.bChannel0, InstanceSettings.LightingChannels.bChannel1, InstanceSettings.LightingChannels.bChannel2);
					}
					else if (InstanceSettings.MeshType == EMeshType::Foliage)
					{
						const UFoliageType_InstancedStaticMesh* FoliageTypeInstancedStaticMesh = Cast<UFoliageType_InstancedStaticMesh>(InstanceSettings.FoliageType);
						if (FoliageTypeInstancedStaticMesh)
						{
							InstancedStaticMeshComponent->SetMobility(FoliageTypeInstancedStaticMesh->Mobility);
							InstancedStaticMeshComponent->SetCullDistance(FoliageTypeInstancedStaticMesh->CullDistance.Max);
							InstancedStaticMeshComponent->SetCastShadow(FoliageTypeInstancedStaticMesh->CastShadow);
							InstancedStaticMeshComponent->SetAffectDynamicIndirectLighting(FoliageTypeInstancedStaticMesh->bAffectDynamicIndirectLighting);
							InstancedStaticMeshComponent->SetAffectDistanceFieldLighting(FoliageTypeInstancedStaticMesh->bAffectDistanceFieldLighting);
							InstancedStaticMeshComponent->SetCastContactShadow(FoliageTypeInstancedStaticMesh->bCastContactShadow);
							InstancedStaticMeshComponent->bCastDynamicShadow = FoliageTypeInstancedStaticMesh->bCastDynamicShadow;
							InstancedStaticMeshComponent->bCastStaticShadow = FoliageTypeInstancedStaticMesh->bCastStaticShadow;
							InstancedStaticMeshComponent->bCastShadowAsTwoSided = FoliageTypeInstancedStaticMesh->bCastShadowAsTwoSided;
							InstancedStaticMeshComponent->SetReceivesDecals(FoliageTypeInstancedStaticMesh->bReceivesDecals);
							InstancedStaticMeshComponent->bOverrideLightMapRes = FoliageTypeInstancedStaticMesh->bOverrideLightMapRes;
							InstancedStaticMeshComponent->bUseAsOccluder = FoliageTypeInstancedStaticMesh->bUseAsOccluder;
#if WITH_EDITORONLY_DATA
							InstancedStaticMeshComponent->bEnableAutoLODGeneration = FoliageTypeInstancedStaticMesh->bIncludeInHLOD;
#endif
							InstancedStaticMeshComponent->SetEvaluateWorldPositionOffset(FoliageTypeInstancedStaticMesh->bEvaluateWorldPositionOffset);
							InstancedStaticMeshComponent->SetWorldPositionOffsetDisableDistance(FoliageTypeInstancedStaticMesh->WorldPositionOffsetDisableDistance);
							InstancedStaticMeshComponent->BodyInstance = FoliageTypeInstancedStaticMesh->BodyInstance;
							InstancedStaticMeshComponent->SetLightingChannels(FoliageTypeInstancedStaticMesh->LightingChannels.bChannel0, FoliageTypeInstancedStaticMesh->LightingChannels.bChannel1, FoliageTypeInstancedStaticMesh->LightingChannels.bChannel2);
						}
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaTool, Warning, TEXT("InstanceSettingsArray is empty."));
	}
}

#if WITH_EDITOR

void ANebulaToolPCGSpawnerActor::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Mesh
	if (PropertyName == GET_MEMBER_NAME_CHECKED(FInstanceSettings, MeshType))
	{
		const int32 ArrayIndex = PropertyChangedEvent.GetArrayIndex(TEXT("InstanceSettingsArray"));

		InstanceSettingsArray[ArrayIndex].bOverrideMaterials = false;
		InstanceSettingsArray[ArrayIndex].StaticMesh = nullptr;
		InstanceSettingsArray[ArrayIndex].FoliageType = nullptr;
		
		InstanceSettingsArray[ArrayIndex].OverrideMaterialMap = {};
		InstanceSettingsArray[ArrayIndex].FoliageOverrideMaterialArray = {};
	}

	// Foliage Type
	if (PropertyName == GET_MEMBER_NAME_CHECKED(FInstanceSettings, FoliageType))
	{
		const int32 ArrayIndex = PropertyChangedEvent.GetArrayIndex(TEXT("InstanceSettingsArray"));
		if (InstanceSettingsArray.IsValidIndex(ArrayIndex))
		{
			UE_LOG(LogNebulaTool, VeryVerbose, TEXT("InstanceSettingsArray[%d].FoliageType changed to: %s"), ArrayIndex, InstanceSettingsArray[ArrayIndex].FoliageType ? *(InstanceSettingsArray[ArrayIndex].FoliageType)->GetName() : TEXT("None"));

			UObject* SourceObject = InstanceSettingsArray[ArrayIndex].FoliageType->GetSource();
			UStaticMesh* SourceMesh = Cast<UStaticMesh>(SourceObject);
			if (SourceMesh)
			{
				UE_LOG(LogNebulaTool, VeryVerbose, TEXT("Found static mesh: %s"), *SourceMesh->GetName());
				InstanceSettingsArray[ArrayIndex].StaticMesh = SourceMesh;
				if (!InstanceSettingsArray[ArrayIndex].bOverrideMaterials)
				{
					const UFoliageType_InstancedStaticMesh* FoliageTypeInstancedStaticMesh = Cast<UFoliageType_InstancedStaticMesh>(InstanceSettingsArray[ArrayIndex].FoliageType);
					if (FoliageTypeInstancedStaticMesh)
					{
						InstanceSettingsArray[ArrayIndex].FoliageOverrideMaterialArray = FoliageTypeInstancedStaticMesh->OverrideMaterials;
					}
				}
				else
				{
					InstanceSettingsArray[ArrayIndex].FoliageOverrideMaterialArray = {};
				}
				InstanceSettingsArray[ArrayIndex].bEnableDensityScaling = InstanceSettingsArray[ArrayIndex].FoliageType->bEnableDensityScaling;
				InstanceSettingsArray[ArrayIndex].bEnableDiscardOnLoad = InstanceSettingsArray[ArrayIndex].FoliageType->bEnableDiscardOnLoad;
			}
		}
	}

	// Override Foliage Materials
	if (PropertyName == GET_MEMBER_NAME_CHECKED(FInstanceSettings, bOverrideMaterials))
	{
		const int32 ArrayIndex = PropertyChangedEvent.GetArrayIndex(TEXT("InstanceSettingsArray"));
		if (InstanceSettingsArray.IsValidIndex(ArrayIndex) && InstanceSettingsArray[ArrayIndex].MeshType == EMeshType::Foliage)
		{
			if (!InstanceSettingsArray[ArrayIndex].bOverrideMaterials)
			{
				const UFoliageType_InstancedStaticMesh* FoliageTypeInstancedStaticMesh = Cast<UFoliageType_InstancedStaticMesh>(InstanceSettingsArray[ArrayIndex].FoliageType);
				if (FoliageTypeInstancedStaticMesh)
				{
					InstanceSettingsArray[ArrayIndex].FoliageOverrideMaterialArray = FoliageTypeInstancedStaticMesh->OverrideMaterials;
				}
			}
			else
			{
				InstanceSettingsArray[ArrayIndex].FoliageOverrideMaterialArray = {};
			}
		}
	}	
}

#endif