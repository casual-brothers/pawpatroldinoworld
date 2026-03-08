#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Enums/NebulaGraphicsEnums.h"

#include "NebulaGraphicsMaterialAnimNotifyState.generated.h"

USTRUCT(BlueprintType)
struct FMaterialParameter
{
	GENERATED_BODY();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	bool bChangeWithTime = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FMaterialParameterInfo ParameterInfo = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	TArray<FName> SlotNameMaterials = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	ENebulaGraphicsMaterialParameterType MaterialParameterType = ENebulaGraphicsMaterialParameterType::Scalar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (EditCondition = "!bChangeWithTime && MaterialParameter == ENebulaGraphicsMaterialParameterType::Scalar", EditConditionHides))
	float Scalar = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (EditCondition = "MaterialParameter == ENebulaGraphicsMaterialParameterType::Vector", EditConditionHides))
	FLinearColor LinearColor = FLinearColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (EditCondition = "MaterialParameter == ENebulaGraphicsMaterialParameterType::Texture", EditConditionHides))
	UTexture* Texture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimNotify", meta = (EditCondition = "bChangeWithTime", EditConditionHides, ToolTip = "Name of the curve in this montage"))
	FName AnimationCurveName = NAME_None;

	float DefaultScalar = 0.0f;

	FLinearColor DefaultLinearColor = FLinearColor::Black;

	UTexture* DefaultTexture = nullptr;
};

UCLASS()
class NEBULAGRAPHICS_API UNebulaGraphicsMaterialAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	FName MeshTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	TArray<FMaterialParameter> MaterialParameterArray = {};

private:

	TMap<UMeshComponent*, TArray<UMaterialInstanceDynamic*>> CachedMaterialInstanceDynamic = {};

	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};