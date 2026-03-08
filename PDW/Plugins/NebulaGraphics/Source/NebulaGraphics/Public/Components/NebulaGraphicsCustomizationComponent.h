#pragma once

#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/StreamableManager.h"
#include "GameplayTagContainer.h"
#include "Enums/NebulaGraphicsEnums.h"

#include "NebulaGraphicsCustomizationComponent.generated.h"

class UAnimInstance;
class UGroomAsset;
class UGroomBindingAsset;
class USkeletalMesh;
class UStaticMesh;
struct FPropertyChangedEvent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustomizationStarted, const ENebulaGraphicsCustomizationType, InCustomizationType, const FGameplayTag&, CustomizedComponentTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustomizationCompleted, const ENebulaGraphicsCustomizationType, InCustomizationType, const FGameplayTag&, CustomizedComponentTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllCustomizationCompleted);


USTRUCT(BlueprintType)
struct FNebulaGraphicsMeshToAttachData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	EAttachmentRule AttachmentRule = EAttachmentRule::KeepRelative;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "AttachmentRule == EAttachmentRule::SnapToTarget", EditConditionHides))
	FGameplayTag SocketName = {};

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "AttachmentRule == EAttachmentRule::KeepRelative || AttachmentRule == EAttachmentRule::KeepWorld", EditConditionHides))
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
	FVector Scale = FVector::OneVector;

	UPROPERTY(EditAnywhere)
	FGameplayTag AttachedMeshComponentTag = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsStaticMeshToAttach : public FNebulaGraphicsMeshToAttachData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> StaticMesh = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsSkeletalMeshToAttach : public FNebulaGraphicsMeshToAttachData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FGameplayTag ComponentToCustomizeTag = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsStaticMeshToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> StaticMesh = {};

	UPROPERTY(EditAnywhere)
	bool bEditStaticMeshTransform = false; 
	
	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bEditStaticMeshTransform", EditConditionHides))
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bEditStaticMeshTransform", EditConditionHides))
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bEditStaticMeshTransform", EditConditionHides))
	FVector Scale = FVector::OneVector;

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsStaticMeshToAttach> StaticMeshToAttachList = {};

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsSkeletalMeshToAttach> SkeletalMeshToAttachList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsSkeletalMeshToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh = {};

	UPROPERTY(EditAnywhere)
	bool bEditSkeletalMeshTransform = false;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bEditSkeletalMeshTransform", EditConditionHides))
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bEditSkeletalMeshTransform", EditConditionHides))
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bEditSkeletalMeshTransform", EditConditionHides))
	FVector Scale = FVector::OneVector;

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsStaticMeshToAttach> StaticMeshToAttachList = {};

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsSkeletalMeshToAttach> SkeletalMeshToAttachList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsAnimationToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UAnimInstance> AnimationInstanceClass;

	UPROPERTY(EditAnywhere)
	FGameplayTag LeaderPoseCoponentTag = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsMaterialToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FGameplayTag SlotName = {};

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UMaterialInstance> MaterialInstance = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsScalarToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FGameplayTag SlotName = {};

	UPROPERTY(EditAnywhere)
	FMaterialParameterInfo MaterialParameterInfo = {};

	UPROPERTY(EditAnywhere)
	float Scalar = 0.0f;
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsVectorToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FGameplayTag SlotName = {};

	UPROPERTY(EditAnywhere)
	FMaterialParameterInfo MaterialParameterInfo = {};

	UPROPERTY(EditAnywhere)
	FLinearColor LinearColor = FLinearColor::Black;
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsTextureToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FGameplayTag SlotName = {};

	UPROPERTY(EditAnywhere)
	FMaterialParameterInfo MaterialParameterInfo = {};

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture> Texture = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsDecalMaterialToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UMaterialInstance> MaterialInstance = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsDecalScalarToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FGameplayTag SlotName = {};

	UPROPERTY(EditAnywhere)
	FMaterialParameterInfo MaterialParameterInfo = {};

	UPROPERTY(EditAnywhere)
	float Scalar = 0.0f;
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsDecalVectorToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FGameplayTag SlotName = {};

	UPROPERTY(EditAnywhere)
	FMaterialParameterInfo MaterialParameterInfo = {};

	UPROPERTY(EditAnywhere)
	FLinearColor LinearColor = FLinearColor::Black;
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsDecalTextureToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FMaterialParameterInfo MaterialParameterInfo = {};

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture> Texture = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsGroomToCustomize : public FNebulaGraphicsBaseObjectToCustomizeData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UGroomAsset> GroomAsset = {};

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UGroomBindingAsset> BindingAsset = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, meta = (DisplayPriority = 1))
	FGameplayTag ID = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsStaticMeshCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsStaticMeshToCustomize> StaticMeshToCustomizeList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsSkeletalMeshCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsSkeletalMeshToCustomize> SkeletalMeshToCustomizeList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsAnimationCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsAnimationToCustomize> AnimationToCustomizeList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsMaterialCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsMaterialToCustomize> MaterialToCustomizeList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsScalarCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsScalarToCustomize> ScalarToCustomizeList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsVectorCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsVectorToCustomize> VectorToCustomizeList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsTextureCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsTextureToCustomize> TextureToCustomizeList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsDecalMaterialCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsDecalMaterialToCustomize> DecalMaterialToCustomizeList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsDecalScalarCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsDecalScalarToCustomize> DecalScalarToCustomizeList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsDecalVectorCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsDecalVectorToCustomize> DecalVectorToCustomizeList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsDecalTextureCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsDecalTextureToCustomize> DecalTextureToCustomizeList = {};
};

USTRUCT(BlueprintType)
struct FNebulaGraphicsGroomCustomization : public FNebulaGraphicsBaseObjectCustomizationData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FNebulaGraphicsGroomToCustomize> GroomToCustomizeList = {};
};

UCLASS()
class NEBULAGRAPHICS_API UCustomizationDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsStaticMeshCustomization> StaticMeshCustomizationList = {};

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsSkeletalMeshCustomization> SkeletalMeshCustomizationList = {};

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsAnimationCustomization> AnimationCustomizationList = {};

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsMaterialCustomization> MaterialCustomizationList = {};

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsScalarCustomization> ScalarCustomizationList = {};

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsVectorCustomization> VectorCustomizationList = {};

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsTextureCustomization> TextureCustomizationList = {};

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsDecalMaterialCustomization> DecalMaterialCustomizationList = {};

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsDecalScalarCustomization> DecalScalarCustomizationList = {};

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsDecalVectorCustomization> DecalVectorCustomizationList = {};

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsDecalTextureCustomization> DecalTextureCustomizationList = {};

	UPROPERTY(EditAnywhere, meta = (TitleProperty = "ID"))
	TArray<FNebulaGraphicsGroomCustomization> GroomCustomizationList = {};
};

UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class NEBULAGRAPHICS_API UNebulaGraphicsCustomizationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	static FName NebulaGraphicsAttachedComponentTag;

	UPROPERTY(BlueprintAssignable)
	FOnAllCustomizationCompleted OnAllCustomizationCompleted = {};

	UPROPERTY(BlueprintAssignable)
	FOnCustomizationStarted OnCustomizationStarted = {};

	UPROPERTY(BlueprintAssignable)
	FOnCustomizationCompleted OnCustomizationCompleted = {};

	UNebulaGraphicsCustomizationComponent(const FObjectInitializer& ObjectInitializer);

	void CustomizeStaticMesh(const FGameplayTagContainer& IDList);

	void CustomizeSkeletalMesh(const FGameplayTagContainer& IDList);

	void CustomizeAnimation(const FGameplayTagContainer& IDList);

	void CustomizeMaterial(const FGameplayTagContainer& IDList);

	void CustomizeScalar(const FGameplayTagContainer& IDList);

	void CustomizeVector(const FGameplayTagContainer& IDList);

	void CustomizeTexture(const FGameplayTagContainer& IDList);

	void CustomizeDecalMaterial(const FGameplayTagContainer& IDList);

	void CustomizeDecalScalar(const FGameplayTagContainer& IDList);

	void CustomizeDecalVector(const FGameplayTagContainer& IDList);

	void CustomizeDecalTexture(const FGameplayTagContainer& IDList);

	void CustomizeGroom(const FGameplayTagContainer& IDList);

	void SetCustomizationDataAsset(UCustomizationDataAsset* InCustomizationDataAsset);

	virtual void OnRegister() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UCustomizationDataAsset* GetCustomizationDataAsset() const;

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer StaticMeshIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer SkeletalMeshIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer AnimationIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer MaterialIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer ScalarIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer VectorIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer TextureIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer DecalMaterialIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer DecalScalarIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer DecalVectorIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer DecalTextureIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Preview")
	FGameplayTagContainer GroomIDList = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Customization")
	UCustomizationDataAsset* CustomizationDataAsset = nullptr;

	void DestroyAllAttachedComponents();

	void DestroyAttachedComponents(const UMeshComponent* InMeshComponent);

private:

	int32 CustomizationStarted = 0;

	int32 CustomizationCompeted = 0;

	UFUNCTION()
	void CustomizationStart(const ENebulaGraphicsCustomizationType InCustomizationType, const FGameplayTag& InCustomizedComponentTag);

	UFUNCTION()
	void CustomizationComplete(const ENebulaGraphicsCustomizationType InCustomizationType, const FGameplayTag& InCustomizedComponentTag);
};