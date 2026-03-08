#include "Animation/Notify/NebulaGraphicsMaterialAnimNotifyState.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UNebulaGraphicsMaterialAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	TArray<FName> SlotNamesMaterials = {};

	for (const FMaterialParameter& MaterialParameter : MaterialParameterArray)
	{
		for (const FName& SlotName : MaterialParameter.SlotNameMaterials)
		{
			SlotNamesMaterials.AddUnique(SlotName);
		}
	}

	TArray<UActorComponent*> ActorComponentList = MeshComp->GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(), MeshTag);
	for (UActorComponent* ActorComponent : ActorComponentList)
	{
		UMeshComponent* MeshToCustomizeComponent = Cast<UMeshComponent>(ActorComponent);
		for (const FName& MaterialSlotName : SlotNamesMaterials)
		{
			const int32 MaterialIndex = MeshToCustomizeComponent->GetMaterialIndex(MaterialSlotName);
			if (MaterialIndex != INDEX_NONE)
			{
				UMaterialInterface* MaterialInterface = MeshToCustomizeComponent->GetMaterial(MaterialIndex);
				if (MaterialInterface)
				{
					UMaterialInstanceDynamic* MaterialInstanceDynamic = MeshToCustomizeComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndex, MeshToCustomizeComponent->GetMaterial(MaterialIndex));
					if (MaterialInstanceDynamic)
					{
						TArray<UMaterialInstanceDynamic*>& MaterialInstanceDynamicArray = CachedMaterialInstanceDynamic.FindOrAdd(MeshComp);
						MaterialInstanceDynamicArray.Add(MaterialInstanceDynamic);
					}
				}
			}
		}
	}

	for (FMaterialParameter& MaterialParameter : MaterialParameterArray)
	{
		if (MaterialParameter.ParameterInfo.Name != NAME_None)
		{
			if (!MaterialParameter.bChangeWithTime && CachedMaterialInstanceDynamic.Contains(MeshComp))
			{
				for (UMaterialInstanceDynamic* MaterialInstanceDynamic : CachedMaterialInstanceDynamic[MeshComp])
				{
					if (MaterialInstanceDynamic)
					{
						if (MaterialParameter.MaterialParameterType == ENebulaGraphicsMaterialParameterType::Scalar)
						{
							MaterialInstanceDynamic->GetScalarParameterValue(MaterialParameter.ParameterInfo, MaterialParameter.DefaultScalar);
							MaterialInstanceDynamic->SetScalarParameterValueByInfo(MaterialParameter.ParameterInfo, MaterialParameter.Scalar);
						}
						else if (MaterialParameter.MaterialParameterType == ENebulaGraphicsMaterialParameterType::Vector)
						{
							MaterialInstanceDynamic->GetVectorParameterValue(MaterialParameter.ParameterInfo, MaterialParameter.DefaultLinearColor);
							MaterialInstanceDynamic->SetVectorParameterValueByInfo(MaterialParameter.ParameterInfo, MaterialParameter.LinearColor);
						}
						else if (MaterialParameter.MaterialParameterType == ENebulaGraphicsMaterialParameterType::Texture)
						{
							MaterialInstanceDynamic->GetTextureParameterValue(MaterialParameter.ParameterInfo, MaterialParameter.DefaultTexture);
							MaterialInstanceDynamic->SetTextureParameterValueByInfo(MaterialParameter.ParameterInfo, MaterialParameter.Texture);
						}
					}
				}
			}
		}
	}
}

void UNebulaGraphicsMaterialAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp && IsValid(MeshComp->GetOwner()))
	{
		if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
		{
			for (const FMaterialParameter& MaterialParameter : MaterialParameterArray)
			{
				if (MaterialParameter.AnimationCurveName != NAME_None && MaterialParameter.ParameterInfo.Name != NAME_None &&
					MaterialParameter.bChangeWithTime && CachedMaterialInstanceDynamic.Contains(MeshComp))
				{
					float ScalarCurveValue = 0.0f;
					if (AnimInst->GetCurveValue(MaterialParameter.AnimationCurveName, ScalarCurveValue))
					{
						for (UMaterialInstanceDynamic* MaterialInstanceDynamic : CachedMaterialInstanceDynamic[MeshComp])
						{
							if (MaterialInstanceDynamic && MaterialParameter.MaterialParameterType == ENebulaGraphicsMaterialParameterType::Scalar)
							{
								MaterialInstanceDynamic->SetScalarParameterValueByInfo(MaterialParameter.ParameterInfo, ScalarCurveValue);
							}
						}
					}
				}
			}
		}
	}
}

void UNebulaGraphicsMaterialAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
	{
		for (const FMaterialParameter& MaterialParameter : MaterialParameterArray)
		{
			if (CachedMaterialInstanceDynamic.Contains(MeshComp))
			{
				for (UMaterialInstanceDynamic* MaterialInstanceDynamic : CachedMaterialInstanceDynamic[MeshComp])
				{
					if (MaterialInstanceDynamic && MaterialParameter.MaterialParameterType == ENebulaGraphicsMaterialParameterType::Scalar)
					{
						MaterialInstanceDynamic->SetScalarParameterValueByInfo(MaterialParameter.ParameterInfo, MaterialParameter.DefaultScalar);
					}
					else if (MaterialInstanceDynamic && MaterialParameter.MaterialParameterType == ENebulaGraphicsMaterialParameterType::Vector)
					{
						MaterialInstanceDynamic->SetVectorParameterValueByInfo(MaterialParameter.ParameterInfo, MaterialParameter.DefaultLinearColor);
					}
					else if(MaterialInstanceDynamic && MaterialParameter.MaterialParameterType == ENebulaGraphicsMaterialParameterType::Texture)
					{
						MaterialInstanceDynamic->SetTextureParameterValueByInfo(MaterialParameter.ParameterInfo, MaterialParameter.DefaultTexture);
					}
				}
			}
		}
	}
	CachedMaterialInstanceDynamic.Remove(MeshComp);

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}