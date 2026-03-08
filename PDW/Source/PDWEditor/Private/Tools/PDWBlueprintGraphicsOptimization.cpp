#include "Tools/PDWBlueprintGraphicsOptimization.h"

#include "EditorUtilityLibrary.h"
#include "Engine/InheritableComponentHandler.h"
#include "Engine/SCS_Node.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/SavePackage.h"

void UPDWBlueprintGraphicsOptimization::BlueprintGraphicsOptimization(bool bHLOD, 
																	  bool bEvaluateWPO, 
																	  bool bWPOWritesVelocity, 
																	  bool bUseAsOccluder, 
																	  bool bAllowClothActors,
																	  bool bAffectDynamicIndirectLighting,
																	  bool bPerBoneMotionBlur,
																	  float DrawDistanceFactor /*= 5.0f*/)
{
	DrawDistanceFactor = FMath::Clamp(DrawDistanceFactor, 0.0f, 1000.0f);
	
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	for (UObject* SelectedAsset : SelectedAssets)
	{
		bool bSave = false;
		UBlueprint* Blueprint = Cast<UBlueprint>(SelectedAsset);
		if (Blueprint)
		{	
			AActor* CDO = Cast<AActor>(Blueprint->GeneratedClass->GetDefaultObject());
			if (CDO)
			{
				CDO->bEnableAutoLODGeneration = bHLOD;
				CDO->SetCanBeDamaged(false);

				// C++ COMPONENTS
				TArray<UActorComponent*> CDOActorComponents;
				CDO->GetComponents(CDOActorComponents);
				for (UActorComponent* CDOActorComponent : CDOActorComponents)
				{
					if (CDOActorComponent)
					{
						if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(CDOActorComponent))
						{
							if (UStaticMesh* Mesh = StaticMeshComponent->GetStaticMesh())
							{
								StaticMeshComponent->LDMaxDrawDistance = FMath::Floor(Mesh->ExtendedBounds.SphereRadius * DrawDistanceFactor);
								StaticMeshComponent->SetCachedMaxDrawDistance(StaticMeshComponent->LDMaxDrawDistance);
							}
							StaticMeshComponent->bEnableAutoLODGeneration = bHLOD;
							StaticMeshComponent->bEvaluateWorldPositionOffset = bEvaluateWPO;
							StaticMeshComponent->bWorldPositionOffsetWritesVelocity = bWPOWritesVelocity;
							StaticMeshComponent->bUseAsOccluder = bUseAsOccluder;

							bSave = true;
						}
						else if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(CDOActorComponent))
						{
							SkeletalMeshComponent->LDMaxDrawDistance = FMath::Floor(SkeletalMeshComponent->GetLocalBounds().SphereRadius * DrawDistanceFactor);
							SkeletalMeshComponent->SetCachedMaxDrawDistance(SkeletalMeshComponent->LDMaxDrawDistance);
							SkeletalMeshComponent->bEnableAutoLODGeneration = false;
							SkeletalMeshComponent->bAllowClothActors = bAllowClothActors;
							SkeletalMeshComponent->bAffectDynamicIndirectLighting = bAffectDynamicIndirectLighting;
							SkeletalMeshComponent->bPerBoneMotionBlur = bPerBoneMotionBlur;
							SkeletalMeshComponent->bUseAsOccluder = bUseAsOccluder;
							SkeletalMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

							bSave = true;
						}
					}
				}

				// BP COMPONENTS
				USimpleConstructionScript* SimpleConstructionScript = Blueprint->SimpleConstructionScript;
				if (SimpleConstructionScript)
				{
					TArray<USCS_Node*> Nodes = SimpleConstructionScript->GetAllNodes();
					if (Nodes.Num() > 0)
					{
						for (USCS_Node* Node : Nodes)
						{
							if (Node)
							{
								UClass* ComponentClass = Node->ComponentClass;
								if (ComponentClass)
								{
									UActorComponent* ActorComponent = Node->GetActualComponentTemplate(Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass));
									if (ActorComponent)
									{
										if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorComponent))
										{
											if (UStaticMesh* Mesh = StaticMeshComponent->GetStaticMesh())
											{
												StaticMeshComponent->LDMaxDrawDistance = FMath::Floor(Mesh->ExtendedBounds.SphereRadius * DrawDistanceFactor);
												StaticMeshComponent->SetCachedMaxDrawDistance(StaticMeshComponent->LDMaxDrawDistance);
											}
											StaticMeshComponent->bEnableAutoLODGeneration = bHLOD;
											StaticMeshComponent->bEvaluateWorldPositionOffset = bEvaluateWPO;
											StaticMeshComponent->bWorldPositionOffsetWritesVelocity = bWPOWritesVelocity;
											StaticMeshComponent->bUseAsOccluder = bUseAsOccluder;

											bSave = true;
										}
										else if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ActorComponent))
										{
											SkeletalMeshComponent->LDMaxDrawDistance = FMath::Floor(SkeletalMeshComponent->GetLocalBounds().SphereRadius * DrawDistanceFactor);
											SkeletalMeshComponent->SetCachedMaxDrawDistance(SkeletalMeshComponent->LDMaxDrawDistance);
											SkeletalMeshComponent->bEnableAutoLODGeneration = false;
											SkeletalMeshComponent->bAllowClothActors = bAllowClothActors;
											SkeletalMeshComponent->bAffectDynamicIndirectLighting = bAffectDynamicIndirectLighting;
											SkeletalMeshComponent->bPerBoneMotionBlur = bPerBoneMotionBlur;
											SkeletalMeshComponent->bUseAsOccluder = bUseAsOccluder;
											SkeletalMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

											bSave = true;
										}
									}
								}
							}
						}
					}
				}

				// BP INHERITABLE COMPONENTS
				UInheritableComponentHandler* InheritableComponentHandler = Blueprint->GetInheritableComponentHandler(false);
				if (InheritableComponentHandler)
				{
					TArray<UActorComponent*> ActorComponents = {};
					InheritableComponentHandler->GetAllTemplates(ActorComponents);
					for (UActorComponent* ActorComponent : ActorComponents)
					{
						if (ActorComponent)
						{
							if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorComponent))
							{
								if (UStaticMesh* Mesh = StaticMeshComponent->GetStaticMesh())
								{
									StaticMeshComponent->LDMaxDrawDistance = FMath::Floor(Mesh->ExtendedBounds.SphereRadius * DrawDistanceFactor);
									StaticMeshComponent->SetCachedMaxDrawDistance(StaticMeshComponent->LDMaxDrawDistance);
								}
								StaticMeshComponent->bEnableAutoLODGeneration = bHLOD;
								StaticMeshComponent->bEvaluateWorldPositionOffset = bEvaluateWPO;
								StaticMeshComponent->bWorldPositionOffsetWritesVelocity = bWPOWritesVelocity;
								StaticMeshComponent->bUseAsOccluder = bUseAsOccluder;

								bSave = true;
							}
							else if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ActorComponent))
							{
								SkeletalMeshComponent->LDMaxDrawDistance = FMath::Floor(SkeletalMeshComponent->GetLocalBounds().SphereRadius * DrawDistanceFactor);
								SkeletalMeshComponent->SetCachedMaxDrawDistance(SkeletalMeshComponent->LDMaxDrawDistance);
								SkeletalMeshComponent->bEnableAutoLODGeneration = false;
								SkeletalMeshComponent->bAllowClothActors = bAllowClothActors;
								SkeletalMeshComponent->bAffectDynamicIndirectLighting = bAffectDynamicIndirectLighting;
								SkeletalMeshComponent->bPerBoneMotionBlur = bPerBoneMotionBlur;
								SkeletalMeshComponent->bUseAsOccluder = bUseAsOccluder;
								SkeletalMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

								bSave = true;
							}
						}
					}
				}

				// SAVE
				if (bSave)
				{
					Blueprint->Modify();
					Blueprint->MarkPackageDirty();

					FKismetEditorUtilities::CompileBlueprint(Blueprint);

					UPackage* Package = Blueprint->GetOutermost();
					if (Package)
					{
						FSavePackageArgs Args;
						Args.TopLevelFlags = RF_Standalone;

						UPackage::SavePackage(
							Package,
							Blueprint,
							*FPackageName::LongPackageNameToFilename(
								Package->GetName(),
								FPackageName::GetAssetPackageExtension()
							),
							Args
						);
					}
				}
			}
		}
	}
}