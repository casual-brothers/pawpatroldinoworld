#pragma once

#include "GameFramework/Actor.h"

#include "NebulaToolPCGSpawnerActor.generated.h"

class UBillboardComponent;
class UBoxComponent;
class UFoliageType;
class UMaterialInterface;
class UPCGComponent;
class UStaticMesh;
struct FBodyInstance;

#pragma region ENUMS

UENUM(BlueprintType)
enum class EBlockerType : uint8
{
	Spline,
	Actor,
	Unknown
};

UENUM(BlueprintType)
enum class EMeshType : uint8
{
	Mesh,
	Foliage,
	Unknown
};

UENUM(BlueprintType)
enum class EScatteringType : uint8
{
	Landscape,
	Raycast,
	Unknown
};

#pragma endregion

#pragma region STRUCTS

USTRUCT(BlueprintType)
struct FInstanceSettings
{

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InstanceSettings")
	EMeshType MeshType = EMeshType::Mesh;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	UStaticMesh* StaticMesh = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Foliage", EditConditionHides))
	UFoliageType* FoliageType = nullptr;

	/** Mobility property to apply */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	TEnumAsByte<EComponentMobility::Type> Mobility = EComponentMobility::Movable;

	/**
	 * The distance where instances will begin to fade out if using a PerInstanceFadeAmount material node. 0 disables.
	 * When the entire cluster is beyond this distance, the cluster is completely culled and not rendered at all.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (ClampMin = 0.0f, EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	float CullDistance = 0.0f;

	/** Controls whether the instances should cast a shadow or not. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	bool bCastShadow = true;

	/** Controls whether the mesh should inject light into the Light Propagation Volume.  This flag is only used if bCastShadow is true. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "bCastShadow && MeshType == EMeshType::Mesh", EditConditionHides))
	bool bAffectDynamicIndirectLighting = true;

	/** Controls whether the mesh should affect dynamic distance field lighting methods.  This flag is only used if bCastShadow is true. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "bCastShadow && MeshType == EMeshType::Mesh", EditConditionHides))
	bool bAffectDistanceFieldLighting = true;

	/** Controls whether the mesh should cast shadows in the case of non precomputed shadowing.  This flag is only used if bCastShadow is true. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "bCastShadow && MeshType == EMeshType::Mesh", EditConditionHides))
	bool bCastDynamicShadow = true;

	/** Whether the mesh should cast a static shadow from shadow casting lights.  This flag is only used if bCastShadow is true. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "bCastShadow && MeshType == EMeshType::Mesh", EditConditionHides))
	bool bCastStaticShadow = true;

	/** Whether the mesh should cast contact shadows. This flag is only used if bCastShadow is true. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "bCastShadow && MeshType == EMeshType::Mesh", EditConditionHides))
	bool bCastContactShadow = false;

	/**
	 * Whether this component should create a per-object shadow that gives higher effective shadow resolution.
	 * Useful for cinematic character shadowing. Assumed to be enabled if bSelfShadowOnly is enabled.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "bCastShadow && MeshType == EMeshType::Mesh", EditConditionHides))
	bool bCastInsetShadow = false;

	/** Whether this mesh should cast dynamic shadows as if it were a two sided material. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "bCastDynamicShadow && MeshType == EMeshType::Mesh", EditConditionHides))
	bool bCastShadowAsTwoSided = false;

	/** Whether the mesh receives decals. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	bool bReceivesDecals = false;

	/** Whether to override the LightMap resolution defined in the static mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (InlineEditConditionToggle, EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	bool bOverrideLightMapRes = false;

	/**
	 * Whether to render the primitive in the depth only pass.
	 * This should generally be true for all objects, and let the renderer make decisions about whether to render objects in the depth only pass.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	bool bUseAsOccluder = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	bool bIncludeInHLOD = true;

	/**
	 * Whether to evaluate World Position Offset.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	bool bEvaluateWorldPositionOffset = true;

	/**
	 * Whether world position offset turns on velocity writes.
	 * If the WPO isn't static then setting false may give incorrect motion vectors.
	 * But if we know that the WPO is static then setting false may save performance.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	bool bWorldPositionOffsetWritesVelocity = true;

	/** Distance at which to disable World Position Offset for an entire instance (0 = Never disable WPO). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	int32 WorldPositionOffsetDisableDistance = 0;

	/** Custom collision for mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (HideObjectType = true, EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	FBodyInstance BodyInstance = {};

	/**
	 * Channels that this component should be in.  Lights with matching channels will affect the component.
	 * These channels only apply to opaque materials, direct lighting, and dynamic lighting and shadowing.
	 * Lighting channels are only supported on translucent materials using forward shading (i.e. when not using the translucency lighting volume).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	FLightingChannels LightingChannels = {};

	/** If true, instances will be rendered in the CustomDepth pass (usually used for outlines). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "MeshType == EMeshType::Mesh", EditConditionHides))
	bool bRenderCustomDepthPass = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "false", EditConditionHides))
	bool bEnableDensityScaling = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "false", EditConditionHides))
	bool bEnableDiscardOnLoad = false;

	/** Material overrides. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "InstanceSettings")
	bool bOverrideMaterials = false;

	UPROPERTY()
	TArray<UMaterialInterface*> FoliageOverrideMaterialArray = {};

	/** Instance material overrides. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings", meta = (EditCondition = "bOverrideMaterials", EditConditionHides))
	TMap<FName, UMaterialInterface*> OverrideMaterialMap = {};

	/** "Defines how often a mesh should appear relative to the others. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings")
	float Weight = 1.0f;

	/** Transform applied to instances. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSettings")
	FTransform Transform = FTransform::Identity;
};

USTRUCT(BlueprintType)
struct FLandscapeLayerFilter
{

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName LayerName = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Threshold = 1.0f;
};

USTRUCT(BlueprintType)
struct FTagFilter
{

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName Tag = FName("PCGBlocker");

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector ExtentModifier = FVector(500.0f, 500.0f, 500.0f);

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EBlockerType BlockerType = EBlockerType::Actor;
};

#pragma endregion

UCLASS()
class NEBULATOOL_API ANebulaToolPCGSpawnerActor : public AActor
{
	GENERATED_BODY()
	
public:	

	ANebulaToolPCGSpawnerActor();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Destroyed() override;

	UFUNCTION()
	void SetOverrides(UPCGComponent* InPCGComponent);

#if WITH_EDITOR

	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;

#endif

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup")
	EScatteringType ScatteringType = EScatteringType::Raycast;

	/** Density applied to point distribution. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup")
	float Density = 100.0f;

	// #TODO Replace with generic object PCG settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup")
	TArray<FInstanceSettings> InstanceSettingsArray = {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Placement")
	FVector PointsBoundsScale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Placement")
	FVector TranslationMin = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Placement")
	FVector TranslationMax = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Placement")
	bool bAbsoluteTranslation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Placement")
	FRotator RotationMin = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Placement")
	FRotator RotationMax = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Placement")
	bool bAbsoluteRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Placement")
	FVector ScaleMin = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Placement")
	FVector ScaleMax = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Placement")
	bool bAbsoluteScale = false;

	/** Landscape layers to exclude from spawning. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Filtering", meta = (EditCondition = "ScatteringType == EScatteringType::Landscape", EditConditionHides))
	TArray<FLandscapeLayerFilter> LandscapeLayerFilterArray = {};

	/** Tag of objects to exclude from spawning. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Filtering")
	TArray<FTagFilter> TagFilterArray = {};

	/** Whether delete generated random points. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Filtering")
	bool bRandomPointDelete = false;

	/** Percentage of points to delete. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|Filtering", meta = (ClampMin = 0.0f, EditCondition = "bRandomPointDelete", EditConditionHides))
	float PointsDeletePercentage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PCGSpawnerSetup|DEBUG")
	bool bVisualizeBlockerVolumes = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	UBoxComponent* BoxVolumeComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	UPCGComponent* PCGComponent = nullptr;

#if WITH_EDITORONLY_DATA

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* BillboardComponent = nullptr;

#endif
};