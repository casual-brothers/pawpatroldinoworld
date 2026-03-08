#include "Components/NebulaGraphicsCustomizationComponent.h"

#include "Components/DecalComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Engine/StreamableManager.h"
#include "GroomComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NebulaGraphics.h"

FName UNebulaGraphicsCustomizationComponent::NebulaGraphicsAttachedComponentTag("NebulaGraphicsAttachedComponentTag");

UNebulaGraphicsCustomizationComponent::UNebulaGraphicsCustomizationComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bTickEvenWhenPaused = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
}

void UNebulaGraphicsCustomizationComponent::CustomizeStaticMesh(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->StaticMeshCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsStaticMeshCustomization* StaticMeshCustomization = CustomizationDataAsset->StaticMeshCustomizationList.FindByPredicate([&](const FNebulaGraphicsStaticMeshCustomization& InItem)
				{ 
					return InItem.ID.MatchesTagExact(ID); 
				});
				if (StaticMeshCustomization)
				{
					for (const FNebulaGraphicsStaticMeshToCustomize& StaticMeshToCustomize : StaticMeshCustomization->StaticMeshToCustomizeList)
					{
						// Find component to customize
						if (StaticMeshToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(UStaticMeshComponent::StaticClass(), StaticMeshToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorComponent);
									if (StaticMeshComponent)
									{
										// Destroy old children components
										DestroyAttachedComponents(StaticMeshComponent);
										 
										// Customize static mesh
										if (StaticMeshToCustomize.StaticMesh.ToSoftObjectPath().IsValid())
										{
											OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::StaticMesh, StaticMeshToCustomize.ComponentToCustomizeTag);

											// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
											TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

											FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
											StreamableManager.RequestAsyncLoad(
												StaticMeshToCustomize.StaticMesh.ToSoftObjectPath(),
												[WeakThis, StaticMeshToCustomize, StaticMeshComponent]()
												{
													if (WeakThis.IsValid() && StaticMeshToCustomize.StaticMesh.ToSoftObjectPath().IsValid() && StaticMeshComponent)
													{
														StaticMeshComponent->SetStaticMesh(StaticMeshToCustomize.StaticMesh.Get());
														if (StaticMeshToCustomize.bEditStaticMeshTransform)
														{
															StaticMeshComponent->SetRelativeLocation(StaticMeshToCustomize.Location);
															StaticMeshComponent->SetRelativeRotation(StaticMeshToCustomize.Rotation);
															StaticMeshComponent->SetRelativeScale3D(StaticMeshToCustomize.Scale);
														}

														const TArray<FName>& MaterialSlotNameList = StaticMeshComponent->GetMaterialSlotNames();
														for (FName MaterialSlotName : MaterialSlotNameList)
														{
															const int32 MaterialIndex = StaticMeshComponent->GetMaterialIndex(MaterialSlotName);
															if (Cast<UMaterialInstanceDynamic>(StaticMeshComponent->GetMaterial(MaterialIndex)))
															{
																UMaterialInterface* StaticMeshMaterial = StaticMeshToCustomize.StaticMesh.Get()->GetMaterial(MaterialIndex);
																if (MaterialIndex != INDEX_NONE && StaticMeshMaterial)
																{
																	StaticMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndex, StaticMeshMaterial);
																}
															}
														}

														TArray<UTexture*> TextureList;
														StaticMeshComponent->GetUsedTextures(TextureList, EMaterialQualityLevel::Num);
														for (int32 TextureIndex = 0; TextureIndex < TextureList.Num(); ++TextureIndex)
														{
															UTexture2D* Texture = Cast<UTexture2D>(TextureList[TextureIndex]);
															if (Texture && !Texture->IsFullyStreamedIn() && !StaticMeshComponent->bForceMipStreaming)
															{
																Texture->SetForceMipLevelsToBeResident(5.0f);
																Texture->WaitForStreaming();
															}
														}

														WeakThis.Get()->OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::StaticMesh, StaticMeshToCustomize.ComponentToCustomizeTag);
													}
												},
												FStreamableManager::AsyncLoadHighPriority
											);
										}
										else
										{
											StaticMeshComponent->SetStaticMesh(nullptr);
										}

										// Attach static mesh
										for (const FNebulaGraphicsStaticMeshToAttach& StaticMeshToAttach : StaticMeshToCustomize.StaticMeshToAttachList)
										{
											if (StaticMeshToAttach.StaticMesh.ToSoftObjectPath().IsValid())
											{
												UStaticMeshComponent* AttachedStaticMeshComponent = NewObject<UStaticMeshComponent>(this);
												if (AttachedStaticMeshComponent)
												{
													AttachedStaticMeshComponent->CreationMethod = EComponentCreationMethod::Instance;
													AttachedStaticMeshComponent->SetMobility(EComponentMobility::Movable);
													AttachedStaticMeshComponent->AttachToComponent(StaticMeshComponent, FAttachmentTransformRules(StaticMeshToAttach.AttachmentRule, true), StaticMeshToAttach.SocketName.GetTagName());
													AttachedStaticMeshComponent->ComponentTags.Add(StaticMeshToAttach.AttachedMeshComponentTag.GetTagName());
													AttachedStaticMeshComponent->ComponentTags.Add(NebulaGraphicsAttachedComponentTag);
													AttachedStaticMeshComponent->ComponentTags.Add(ID.GetTagName());
													AttachedStaticMeshComponent->SetEnableGravity(false);
													AttachedStaticMeshComponent->SetCanEverAffectNavigation(false);
													AttachedStaticMeshComponent->SetGenerateOverlapEvents(false);
													AttachedStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
													AttachedStaticMeshComponent->SetCastShadow(StaticMeshComponent->CastShadow);
													AttachedStaticMeshComponent->SetAffectDynamicIndirectLighting(StaticMeshComponent->bAffectDynamicIndirectLighting);
													AttachedStaticMeshComponent->SetAffectDistanceFieldLighting(StaticMeshComponent->bAffectDistanceFieldLighting);
													AttachedStaticMeshComponent->bCastDynamicShadow = StaticMeshComponent->bCastDynamicShadow;
													AttachedStaticMeshComponent->bCastContactShadow = StaticMeshComponent->bCastContactShadow;
													AttachedStaticMeshComponent->SetLightingChannels(StaticMeshComponent->LightingChannels.bChannel0, StaticMeshComponent->LightingChannels.bChannel1, StaticMeshComponent->LightingChannels.bChannel2);
													AttachedStaticMeshComponent->bVisibleInReflectionCaptures = StaticMeshComponent->bVisibleInReflectionCaptures;
													AttachedStaticMeshComponent->SetReceivesDecals(StaticMeshComponent->bReceivesDecals);
													AttachedStaticMeshComponent->RegisterComponent();

													if (StaticMeshToAttach.AttachmentRule == EAttachmentRule::KeepRelative)
													{
														AttachedStaticMeshComponent->SetRelativeLocation(StaticMeshToAttach.Location);
														AttachedStaticMeshComponent->SetRelativeRotation(StaticMeshToAttach.Rotation);
														AttachedStaticMeshComponent->SetRelativeScale3D(StaticMeshToAttach.Scale);
													}
													else if (StaticMeshToAttach.AttachmentRule == EAttachmentRule::KeepWorld)
													{
														AttachedStaticMeshComponent->SetWorldLocation(StaticMeshToAttach.Location);
														AttachedStaticMeshComponent->SetWorldRotation(StaticMeshToAttach.Rotation);
														AttachedStaticMeshComponent->SetWorldScale3D(StaticMeshToAttach.Scale);
													}
													else
													{
														AttachedStaticMeshComponent->SetRelativeRotation(StaticMeshToAttach.Rotation);
														AttachedStaticMeshComponent->SetRelativeScale3D(StaticMeshToAttach.Scale);
													}

													// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
													TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

													FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
													StreamableManager.RequestAsyncLoad(
														StaticMeshToAttach.StaticMesh.ToSoftObjectPath(),
														[WeakThis, StaticMeshToAttach, AttachedStaticMeshComponent, StaticMeshComponent]()
														{
															if (WeakThis.IsValid() && StaticMeshToAttach.StaticMesh.ToSoftObjectPath().IsValid() && AttachedStaticMeshComponent)
															{
																AttachedStaticMeshComponent->SetStaticMesh(StaticMeshToAttach.StaticMesh.Get());

																TArray<UTexture*> TextureList;
																AttachedStaticMeshComponent->GetUsedTextures(TextureList, EMaterialQualityLevel::Num);
																for (int32 TextureIndex = 0; TextureIndex < TextureList.Num(); ++TextureIndex)
																{
																	UTexture2D* Texture = Cast<UTexture2D>(TextureList[TextureIndex]);
																	if (Texture && !Texture->IsFullyStreamedIn() && !StaticMeshComponent->bForceMipStreaming)
																	{
																		Texture->SetForceMipLevelsToBeResident(5.0f);
																		Texture->WaitForStreaming();
																	}
																}
															}
														},
														FStreamableManager::AsyncLoadHighPriority
													);
												}
												else
												{
													UE_LOG(LogNebulaGraphics, Verbose, TEXT("CustomizationStaticMeshComponent not spawned."));
												}
											}
										}

										// Attach skeletal mesh
										for (const FNebulaGraphicsSkeletalMeshToAttach& SkeletalMeshToAttach : StaticMeshToCustomize.SkeletalMeshToAttachList)
										{
											if (SkeletalMeshToAttach.SkeletalMesh.ToSoftObjectPath().IsValid())
											{
												USkeletalMeshComponent* AttachedSkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this);
												if (AttachedSkeletalMeshComponent)
												{
													AttachedSkeletalMeshComponent->CreationMethod = EComponentCreationMethod::Instance;
													AttachedSkeletalMeshComponent->SetMobility(EComponentMobility::Movable);
													AttachedSkeletalMeshComponent->AttachToComponent(StaticMeshComponent, FAttachmentTransformRules(SkeletalMeshToAttach.AttachmentRule, true), SkeletalMeshToAttach.SocketName.GetTagName());
													AttachedSkeletalMeshComponent->ComponentTags.Add(SkeletalMeshToAttach.AttachedMeshComponentTag.GetTagName());
													AttachedSkeletalMeshComponent->ComponentTags.Add(NebulaGraphicsAttachedComponentTag);
													AttachedSkeletalMeshComponent->ComponentTags.Add(ID.GetTagName());
													AttachedSkeletalMeshComponent->SetEnableGravity(false);
													AttachedSkeletalMeshComponent->SetCanEverAffectNavigation(false);
													AttachedSkeletalMeshComponent->SetGenerateOverlapEvents(false);
													AttachedSkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
													AttachedSkeletalMeshComponent->SetCastShadow(StaticMeshComponent->CastShadow);
													AttachedSkeletalMeshComponent->SetAffectDynamicIndirectLighting(StaticMeshComponent->bAffectDynamicIndirectLighting);
													AttachedSkeletalMeshComponent->SetAffectDistanceFieldLighting(StaticMeshComponent->bAffectDistanceFieldLighting);
													AttachedSkeletalMeshComponent->bCastDynamicShadow = StaticMeshComponent->bCastDynamicShadow;
													AttachedSkeletalMeshComponent->bCastContactShadow = StaticMeshComponent->bCastContactShadow;
													AttachedSkeletalMeshComponent->SetLightingChannels(StaticMeshComponent->LightingChannels.bChannel0, StaticMeshComponent->LightingChannels.bChannel1, StaticMeshComponent->LightingChannels.bChannel2);
													AttachedSkeletalMeshComponent->bVisibleInReflectionCaptures = StaticMeshComponent->bVisibleInReflectionCaptures;
													AttachedSkeletalMeshComponent->SetReceivesDecals(StaticMeshComponent->bReceivesDecals);
													AttachedSkeletalMeshComponent->RegisterComponent();

													if (SkeletalMeshToAttach.AttachmentRule == EAttachmentRule::KeepRelative)
													{
														AttachedSkeletalMeshComponent->SetRelativeLocation(SkeletalMeshToAttach.Location);
														AttachedSkeletalMeshComponent->SetRelativeRotation(SkeletalMeshToAttach.Rotation);
														AttachedSkeletalMeshComponent->SetRelativeScale3D(SkeletalMeshToAttach.Scale);
													}
													else if (SkeletalMeshToAttach.AttachmentRule == EAttachmentRule::KeepWorld)
													{
														AttachedSkeletalMeshComponent->SetWorldLocation(SkeletalMeshToAttach.Location);
														AttachedSkeletalMeshComponent->SetWorldRotation(SkeletalMeshToAttach.Rotation);
														AttachedSkeletalMeshComponent->SetWorldScale3D(SkeletalMeshToAttach.Scale);
													}
													else
													{
														AttachedSkeletalMeshComponent->SetRelativeRotation(SkeletalMeshToAttach.Rotation);
														AttachedSkeletalMeshComponent->SetRelativeScale3D(SkeletalMeshToAttach.Scale);
													}

													// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
													TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

													FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
													StreamableManager.RequestAsyncLoad(
														SkeletalMeshToAttach.SkeletalMesh.ToSoftObjectPath(),
														[WeakThis, SkeletalMeshToAttach, AttachedSkeletalMeshComponent, StaticMeshComponent]()
														{
															if (WeakThis.IsValid() && SkeletalMeshToAttach.SkeletalMesh.ToSoftObjectPath().IsValid() && AttachedSkeletalMeshComponent)
															{
																AttachedSkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshToAttach.SkeletalMesh.Get());

																TArray<UTexture*> TextureList;
																AttachedSkeletalMeshComponent->GetUsedTextures(TextureList, EMaterialQualityLevel::Num);
																for (int32 TextureIndex = 0; TextureIndex < TextureList.Num(); ++TextureIndex)
																{
																	UTexture2D* Texture = Cast<UTexture2D>(TextureList[TextureIndex]);
																	if (Texture && !Texture->IsFullyStreamedIn() && !StaticMeshComponent->bForceMipStreaming)
																	{
																		Texture->SetForceMipLevelsToBeResident(5.0f);
																		Texture->WaitForStreaming();
																	}
																}
															}
														},
														FStreamableManager::AsyncLoadHighPriority
													);

													CustomizeAnimation(ID.GetSingleTagContainer());
												}
												else
												{
													UE_LOG(LogNebulaGraphics, Warning, TEXT("SkeletalMeshToAttach not spawned."));
												}
											}
										}
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("StaticMeshComponent with tag %s not found."), *(StaticMeshToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("StaticMeshCustomizationList doesn't contains a StaticMesh with %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizeSkeletalMesh(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->SkeletalMeshCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsSkeletalMeshCustomization* SkeletalMeshCustomization = CustomizationDataAsset->SkeletalMeshCustomizationList.FindByPredicate([&](const FNebulaGraphicsSkeletalMeshCustomization& InItem)
				{
					return InItem.ID.MatchesTagExact(ID);
				}); 
				if (SkeletalMeshCustomization)
				{
					for (const FNebulaGraphicsSkeletalMeshToCustomize& SkeletalMeshToCustomize : SkeletalMeshCustomization->SkeletalMeshToCustomizeList)
					{
						// Find component to customize
						if (SkeletalMeshToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), SkeletalMeshToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								// Customize skeletal mesh
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ActorComponent);
									if (SkeletalMeshComponent)
									{
										// Destroy old children components
										DestroyAttachedComponents(SkeletalMeshComponent);

										if (SkeletalMeshToCustomize.SkeletalMesh.ToSoftObjectPath().IsValid())
										{
											OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::SkeletalMesh, SkeletalMeshToCustomize.ComponentToCustomizeTag);

											// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
											TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

											FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
											StreamableManager.RequestAsyncLoad(
												SkeletalMeshToCustomize.SkeletalMesh.ToSoftObjectPath(),
												[WeakThis, SkeletalMeshToCustomize, SkeletalMeshComponent]()
												{
													if (WeakThis.IsValid() && SkeletalMeshToCustomize.SkeletalMesh.ToSoftObjectPath().IsValid() && SkeletalMeshComponent)
													{
														SkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshToCustomize.SkeletalMesh.Get());
														if (SkeletalMeshToCustomize.bEditSkeletalMeshTransform)
														{
															SkeletalMeshComponent->SetRelativeLocation(SkeletalMeshToCustomize.Location);
															SkeletalMeshComponent->SetRelativeRotation(SkeletalMeshToCustomize.Rotation);
															SkeletalMeshComponent->SetRelativeScale3D(SkeletalMeshToCustomize.Scale);
														}

														if (SkeletalMeshComponent->OverrideMaterials.Num() > 0)
														{
															const TArray<FName>& MaterialSlotNameList = SkeletalMeshComponent->GetMaterialSlotNames();
															for (FName MaterialSlotName : MaterialSlotNameList)
															{
																const int32 MaterialIndex = SkeletalMeshComponent->GetMaterialIndex(MaterialSlotName);
																if (Cast<UMaterialInstanceDynamic>(SkeletalMeshComponent->GetMaterial(MaterialIndex)))
																{
																	const TArray<FSkeletalMaterial>& SkeletalMeshMaterialList = SkeletalMeshToCustomize.SkeletalMesh.Get()->GetMaterials();
																	if (MaterialIndex != INDEX_NONE && MaterialIndex < SkeletalMeshMaterialList.Num())
																	{
																		const FSkeletalMaterial& SkeletalMeshMaterial = SkeletalMeshMaterialList[MaterialIndex];
																		if (SkeletalMeshMaterial.MaterialSlotName == MaterialSlotName && SkeletalMeshMaterial.MaterialInterface)
																		{
																			SkeletalMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndex, SkeletalMeshMaterial.MaterialInterface);
																		}
																	}
																}
															}
														}

														TArray<UTexture*> TextureList;
														SkeletalMeshComponent->GetUsedTextures(TextureList, EMaterialQualityLevel::Num);
														for (int32 TextureIndex = 0; TextureIndex < TextureList.Num(); ++TextureIndex)
														{
															UTexture2D* Texture = Cast<UTexture2D>(TextureList[TextureIndex]);
															if (Texture && !Texture->IsFullyStreamedIn() && !SkeletalMeshComponent->bForceMipStreaming)
															{
																Texture->SetForceMipLevelsToBeResident(5.0f);
																Texture->WaitForStreaming();
															}
														}

														WeakThis.Get()->OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::SkeletalMesh, SkeletalMeshToCustomize.ComponentToCustomizeTag);
													}
												},
												FStreamableManager::AsyncLoadHighPriority
											);
										}
										else
										{
											SkeletalMeshComponent->SetSkeletalMesh(nullptr);
										}

										// Attach static mesh
										for (const FNebulaGraphicsStaticMeshToAttach& StaticMeshToAttach : SkeletalMeshToCustomize.StaticMeshToAttachList)
										{
											if (StaticMeshToAttach.StaticMesh.ToSoftObjectPath().IsValid())
											{
												UStaticMeshComponent* AttachedStaticMeshComponent = NewObject<UStaticMeshComponent>(this);
												if (AttachedStaticMeshComponent)
												{
													AttachedStaticMeshComponent->CreationMethod = EComponentCreationMethod::Instance;
													AttachedStaticMeshComponent->SetMobility(EComponentMobility::Movable);
													AttachedStaticMeshComponent->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules(StaticMeshToAttach.AttachmentRule, true), StaticMeshToAttach.SocketName.GetTagName());
													AttachedStaticMeshComponent->ComponentTags.Add(StaticMeshToAttach.AttachedMeshComponentTag.GetTagName());
													AttachedStaticMeshComponent->ComponentTags.Add(NebulaGraphicsAttachedComponentTag);
													AttachedStaticMeshComponent->ComponentTags.Add(ID.GetTagName());
													AttachedStaticMeshComponent->SetEnableGravity(false);
													AttachedStaticMeshComponent->SetCanEverAffectNavigation(false);
													AttachedStaticMeshComponent->SetGenerateOverlapEvents(false);
													AttachedStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
													AttachedStaticMeshComponent->SetCastShadow(SkeletalMeshComponent->CastShadow);
													AttachedStaticMeshComponent->SetAffectDynamicIndirectLighting(SkeletalMeshComponent->bAffectDynamicIndirectLighting);
													AttachedStaticMeshComponent->SetAffectDistanceFieldLighting(SkeletalMeshComponent->bAffectDistanceFieldLighting);
													AttachedStaticMeshComponent->bCastDynamicShadow = SkeletalMeshComponent->bCastDynamicShadow;
													AttachedStaticMeshComponent->bCastContactShadow = SkeletalMeshComponent->bCastContactShadow;
													AttachedStaticMeshComponent->SetLightingChannels(SkeletalMeshComponent->LightingChannels.bChannel0, SkeletalMeshComponent->LightingChannels.bChannel1, SkeletalMeshComponent->LightingChannels.bChannel2);
													AttachedStaticMeshComponent->bVisibleInReflectionCaptures = SkeletalMeshComponent->bVisibleInReflectionCaptures;
													AttachedStaticMeshComponent->SetReceivesDecals(SkeletalMeshComponent->bReceivesDecals);
													AttachedStaticMeshComponent->RegisterComponent();

													if (StaticMeshToAttach.AttachmentRule == EAttachmentRule::KeepRelative)
													{
														AttachedStaticMeshComponent->SetRelativeLocation(StaticMeshToAttach.Location);
														AttachedStaticMeshComponent->SetRelativeRotation(StaticMeshToAttach.Rotation);
														AttachedStaticMeshComponent->SetRelativeScale3D(StaticMeshToAttach.Scale);
													}
													else if (StaticMeshToAttach.AttachmentRule == EAttachmentRule::KeepWorld)
													{
														AttachedStaticMeshComponent->SetWorldLocation(StaticMeshToAttach.Location);
														AttachedStaticMeshComponent->SetWorldRotation(StaticMeshToAttach.Rotation);
														AttachedStaticMeshComponent->SetWorldScale3D(StaticMeshToAttach.Scale);
													}
													else
													{
														AttachedStaticMeshComponent->SetRelativeRotation(StaticMeshToAttach.Rotation);
														AttachedStaticMeshComponent->SetRelativeScale3D(StaticMeshToAttach.Scale);
													}

													// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
													TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

													FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
													StreamableManager.RequestAsyncLoad(
														StaticMeshToAttach.StaticMesh.ToSoftObjectPath(),
														[WeakThis, StaticMeshToAttach, AttachedStaticMeshComponent, SkeletalMeshComponent]()
														{
															if (WeakThis.IsValid() && StaticMeshToAttach.StaticMesh.ToSoftObjectPath().IsValid() && AttachedStaticMeshComponent)
															{
																AttachedStaticMeshComponent->SetStaticMesh(StaticMeshToAttach.StaticMesh.Get());

																TArray<UTexture*> TextureList;
																AttachedStaticMeshComponent->GetUsedTextures(TextureList, EMaterialQualityLevel::Num);
																for (int32 TextureIndex = 0; TextureIndex < TextureList.Num(); ++TextureIndex)
																{
																	UTexture2D* Texture = Cast<UTexture2D>(TextureList[TextureIndex]);
																	if (Texture && !Texture->IsFullyStreamedIn() && !SkeletalMeshComponent->bForceMipStreaming)
																	{
																		Texture->SetForceMipLevelsToBeResident(5.0f);
																		Texture->WaitForStreaming();
																	}
																}
															}
														},
														FStreamableManager::AsyncLoadHighPriority
													);
												}
												else
												{
													UE_LOG(LogNebulaGraphics, Verbose, TEXT("CustomizationStaticMeshComponent not spawned."));
												}
											}
										}

										// Attach skeletal mesh
										for (const FNebulaGraphicsSkeletalMeshToAttach& SkeletalMeshToAttach : SkeletalMeshToCustomize.SkeletalMeshToAttachList)
										{
											if (SkeletalMeshToAttach.SkeletalMesh.ToSoftObjectPath().IsValid())
											{
												USkeletalMeshComponent* AttachedSkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this);
												if (AttachedSkeletalMeshComponent)
												{
													AttachedSkeletalMeshComponent->CreationMethod = EComponentCreationMethod::Instance;
													AttachedSkeletalMeshComponent->SetMobility(EComponentMobility::Movable);
													AttachedSkeletalMeshComponent->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules(SkeletalMeshToAttach.AttachmentRule, true), SkeletalMeshToAttach.SocketName.GetTagName());
													AttachedSkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
													AttachedSkeletalMeshComponent->SetAnimInstanceClass(SkeletalMeshComponent->GetAnimClass());
													AttachedSkeletalMeshComponent->AnimScriptInstance = SkeletalMeshComponent->AnimScriptInstance;
													AttachedSkeletalMeshComponent->SetLeaderPoseComponent(SkeletalMeshComponent);
													AttachedSkeletalMeshComponent->ComponentTags.Add(SkeletalMeshToAttach.AttachedMeshComponentTag.GetTagName());
													AttachedSkeletalMeshComponent->ComponentTags.Add(NebulaGraphicsAttachedComponentTag);
													AttachedSkeletalMeshComponent->ComponentTags.Add(ID.GetTagName());
													AttachedSkeletalMeshComponent->SetEnableGravity(false);
													AttachedSkeletalMeshComponent->SetCanEverAffectNavigation(false);
													AttachedSkeletalMeshComponent->SetGenerateOverlapEvents(false);
													AttachedSkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
													AttachedSkeletalMeshComponent->SetCastShadow(SkeletalMeshComponent->CastShadow);
													AttachedSkeletalMeshComponent->SetAffectDynamicIndirectLighting(SkeletalMeshComponent->bAffectDynamicIndirectLighting);
													AttachedSkeletalMeshComponent->SetAffectDistanceFieldLighting(SkeletalMeshComponent->bAffectDistanceFieldLighting);
													AttachedSkeletalMeshComponent->bCastDynamicShadow = SkeletalMeshComponent->bCastDynamicShadow;
													AttachedSkeletalMeshComponent->bCastContactShadow = SkeletalMeshComponent->bCastContactShadow;
													AttachedSkeletalMeshComponent->SetLightingChannels(SkeletalMeshComponent->LightingChannels.bChannel0, SkeletalMeshComponent->LightingChannels.bChannel1, SkeletalMeshComponent->LightingChannels.bChannel2);
													AttachedSkeletalMeshComponent->bVisibleInReflectionCaptures = SkeletalMeshComponent->bVisibleInReflectionCaptures;
													AttachedSkeletalMeshComponent->SetReceivesDecals(SkeletalMeshComponent->bReceivesDecals);
													AttachedSkeletalMeshComponent->RegisterComponent();

													if (SkeletalMeshToAttach.AttachmentRule == EAttachmentRule::KeepRelative)
													{
														AttachedSkeletalMeshComponent->SetRelativeLocation(SkeletalMeshToAttach.Location);
														AttachedSkeletalMeshComponent->SetRelativeRotation(SkeletalMeshToAttach.Rotation);
														AttachedSkeletalMeshComponent->SetRelativeScale3D(SkeletalMeshToAttach.Scale);
													}
													else if (SkeletalMeshToAttach.AttachmentRule == EAttachmentRule::KeepWorld)
													{
														AttachedSkeletalMeshComponent->SetWorldLocation(SkeletalMeshToAttach.Location);
														AttachedSkeletalMeshComponent->SetWorldRotation(SkeletalMeshToAttach.Rotation);
														AttachedSkeletalMeshComponent->SetWorldScale3D(SkeletalMeshToAttach.Scale);
													}
													else
													{
														AttachedSkeletalMeshComponent->SetRelativeRotation(SkeletalMeshToAttach.Rotation);
														AttachedSkeletalMeshComponent->SetRelativeScale3D(SkeletalMeshToAttach.Scale);
													}

													// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
													TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

													FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
													StreamableManager.RequestAsyncLoad(
														SkeletalMeshToAttach.SkeletalMesh.ToSoftObjectPath(),
														[WeakThis, SkeletalMeshToAttach, AttachedSkeletalMeshComponent, SkeletalMeshComponent]()
														{
															if (WeakThis.IsValid() && SkeletalMeshToAttach.SkeletalMesh.ToSoftObjectPath().IsValid() && AttachedSkeletalMeshComponent)
															{
																AttachedSkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshToAttach.SkeletalMesh.Get());

																TArray<UTexture*> TextureList;
																AttachedSkeletalMeshComponent->GetUsedTextures(TextureList, EMaterialQualityLevel::Num);
																for (int32 TextureIndex = 0; TextureIndex < TextureList.Num(); ++TextureIndex)
																{
																	UTexture2D* Texture = Cast<UTexture2D>(TextureList[TextureIndex]);
																	if (Texture && !Texture->IsFullyStreamedIn() && !SkeletalMeshComponent->bForceMipStreaming)
																	{
																		Texture->SetForceMipLevelsToBeResident(5.0f);
																		Texture->WaitForStreaming();
																	}
																}
															}
														},
														FStreamableManager::AsyncLoadHighPriority
													);

													CustomizeAnimation(ID.GetSingleTagContainer());
												}
												else
												{
													UE_LOG(LogNebulaGraphics, Verbose, TEXT("CustomizationSkeletalMeshComponent not spawned."));
												}
											}
										}
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("SkeletalMeshComponent with tag %s not found."), *(SkeletalMeshToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("SkeletalMeshCustomizationList doesn't contains a SkeletalMesh with specified %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizeAnimation(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->AnimationCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsAnimationCustomization* AnimationCustomization = CustomizationDataAsset->AnimationCustomizationList.FindByPredicate([&](const FNebulaGraphicsAnimationCustomization& InItem)
				{
					return InItem.ID.MatchesTagExact(ID);
				});
				if (AnimationCustomization)
				{
					for (const FNebulaGraphicsAnimationToCustomize& AnimationToCustomize : AnimationCustomization->AnimationToCustomizeList)
					{
						if (AnimationToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							// Find component to customize
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), AnimationToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ActorComponent);
									if (SkeletalMeshComponent)
									{
										OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::Animation, AnimationToCustomize.ComponentToCustomizeTag);

										// Customize animation
										SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
										SkeletalMeshComponent->SetAnimInstanceClass(AnimationToCustomize.AnimationInstanceClass.LoadSynchronous());

										// Apply leader pose
										if (AnimationToCustomize.LeaderPoseCoponentTag != FGameplayTag::EmptyTag)
										{
											TArray<UActorComponent*> LeaderPoseActorComponentList = GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), AnimationToCustomize.LeaderPoseCoponentTag.GetTagName());
											if (LeaderPoseActorComponentList.Num() > 0)
											{
												USkeletalMeshComponent* LeaderPoseSkeletalMeshComponent = Cast<USkeletalMeshComponent>(LeaderPoseActorComponentList[0]);
												if (LeaderPoseSkeletalMeshComponent && SkeletalMeshComponent != LeaderPoseSkeletalMeshComponent)
												{
													SkeletalMeshComponent->AnimScriptInstance = LeaderPoseSkeletalMeshComponent->AnimScriptInstance;
													SkeletalMeshComponent->SetLeaderPoseComponent(LeaderPoseSkeletalMeshComponent);
												}
											}
										}

										OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::Animation, AnimationToCustomize.ComponentToCustomizeTag);
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("SkeletalMeshComponent with tag %s not found."), *(AnimationToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("AnimationCustomizationList doesn't contains an Animation with specified %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizeMaterial(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->MaterialCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsMaterialCustomization* MaterialCustomization = CustomizationDataAsset->MaterialCustomizationList.FindByPredicate([&](const FNebulaGraphicsMaterialCustomization& InItem)
				{
					return InItem.ID.MatchesTagExact(ID);
				});
				if (MaterialCustomization)
				{
					for (const FNebulaGraphicsMaterialToCustomize& MaterialToCustomize : MaterialCustomization->MaterialToCustomizeList)
					{
						// Find component to customize
						if (MaterialToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(), MaterialToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComponent);
									if (MeshComponent)
									{
										// Customize material
										if (MaterialToCustomize.MaterialInstance.ToSoftObjectPath().IsValid())
										{
											OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::MaterialInstance, MaterialToCustomize.ComponentToCustomizeTag);

											// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
											TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

											FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
											StreamableManager.RequestAsyncLoad(
												MaterialToCustomize.MaterialInstance.ToSoftObjectPath(),
												[WeakThis, MaterialToCustomize, MeshComponent]()
												{
													if (WeakThis.IsValid() && MaterialToCustomize.MaterialInstance.ToSoftObjectPath().IsValid() && MeshComponent)
													{
														MeshComponent->SetMaterialByName(MaterialToCustomize.SlotName.GetTagName(), MaterialToCustomize.MaterialInstance.Get());

														if (!MeshComponent->bForceMipStreaming)
														{
															MaterialToCustomize.MaterialInstance->SetForceMipLevelsToBeResident(false, true, 5.0f, 0, false, true);
														}

														WeakThis.Get()->OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::MaterialInstance, MaterialToCustomize.ComponentToCustomizeTag);
													}
												},
												FStreamableManager::AsyncLoadHighPriority
											);
										}
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("MeshComponent with tag %s not found."), *(MaterialToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("MaterialCustomizationList doesn't contains a MaterialInterface with specified %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizeScalar(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->ScalarCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsScalarCustomization* ScalarCustomization = CustomizationDataAsset->ScalarCustomizationList.FindByPredicate([&](const FNebulaGraphicsScalarCustomization& InItem)
				{
					return InItem.ID.MatchesTagExact(ID);
				});
				if (ScalarCustomization)
				{
					for (const FNebulaGraphicsScalarToCustomize& ScalarToCustomize : ScalarCustomization->ScalarToCustomizeList)
					{
						// Find component to customize
						if (ScalarToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(), ScalarToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComponent);
									if (MeshComponent)
									{
										// Customize Scalar
										const int32 MaterialIndex = MeshComponent->GetMaterialIndex(ScalarToCustomize.SlotName.GetTagName());
										if (MaterialIndex != INDEX_NONE)
										{
											UMaterialInstanceDynamic* MaterialInstanceDynamic = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndex, MeshComponent->GetMaterial(MaterialIndex));
											if (MaterialInstanceDynamic)
											{
												OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::MaterialInstanceDynamic, ScalarToCustomize.ComponentToCustomizeTag);

												MaterialInstanceDynamic->SetScalarParameterValueByInfo(ScalarToCustomize.MaterialParameterInfo, ScalarToCustomize.Scalar);

												OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::MaterialInstanceDynamic, ScalarToCustomize.ComponentToCustomizeTag);
											}
										}
										else
										{
											UE_LOG(LogNebulaGraphics, Verbose, TEXT("MaterialIndex not found."));
										}
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("MeshComponent with tag %s not found."), *(ScalarToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("ScalarCustomizationList doesn't contains a Scalar with specified %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizeVector(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->VectorCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsVectorCustomization* VectorCustomization = CustomizationDataAsset->VectorCustomizationList.FindByPredicate([&](const FNebulaGraphicsVectorCustomization& InItem)
				{
					return InItem.ID.MatchesTagExact(ID);
				});
				if (VectorCustomization)
				{
					for (const FNebulaGraphicsVectorToCustomize& VectorToCustomize : VectorCustomization->VectorToCustomizeList)
					{
						// Find component to customize
						if (VectorToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(), VectorToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComponent);
									if (MeshComponent)
									{
										// Customize Vector
										const int32 MaterialIndex = MeshComponent->GetMaterialIndex(VectorToCustomize.SlotName.GetTagName());
										if (MaterialIndex != INDEX_NONE)
										{
											UMaterialInstanceDynamic* MaterialInstanceDynamic = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndex, MeshComponent->GetMaterial(MaterialIndex));
											if (MaterialInstanceDynamic)
											{
												OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::MaterialInstanceDynamic, VectorToCustomize.ComponentToCustomizeTag);

												MaterialInstanceDynamic->SetVectorParameterValueByInfo(VectorToCustomize.MaterialParameterInfo, VectorToCustomize.LinearColor);

												OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::MaterialInstanceDynamic, VectorToCustomize.ComponentToCustomizeTag);
											}
										}
										else
										{
											UE_LOG(LogNebulaGraphics, Verbose, TEXT("MaterialIndex not found."));
										}
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("MeshComponent with tag %s not found."), *(VectorToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("VectorCustomizationList doesn't contains a Vector with specified %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizeTexture(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->TextureCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsTextureCustomization* TextureCustomization = CustomizationDataAsset->TextureCustomizationList.FindByPredicate([&](const FNebulaGraphicsTextureCustomization& InItem)
				{
					return InItem.ID.MatchesTagExact(ID);
				});
				if (TextureCustomization)
				{
					for (const FNebulaGraphicsTextureToCustomize& TextureToCustomize : TextureCustomization->TextureToCustomizeList)
					{
						// Find component to customize
						if (TextureToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(), TextureToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorComponent);
									if (MeshComponent)
									{
										// Customize texture
										const int32 MaterialIndex = MeshComponent->GetMaterialIndex(TextureToCustomize.SlotName.GetTagName());
										if (MaterialIndex != INDEX_NONE)
										{
											if (TextureToCustomize.Texture.ToSoftObjectPath().IsValid())
											{
												OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::MaterialInstanceDynamic, TextureToCustomize.ComponentToCustomizeTag);

												// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
												TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

												FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
												StreamableManager.RequestAsyncLoad(
													TextureToCustomize.Texture.ToSoftObjectPath(),
													[WeakThis, TextureToCustomize, MeshComponent, MaterialIndex]()
													{
														UMaterialInstanceDynamic* MaterialInstanceDynamic = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndex, MeshComponent->GetMaterial(MaterialIndex));
														if (WeakThis.IsValid() && TextureToCustomize.Texture.ToSoftObjectPath().IsValid() && MaterialInstanceDynamic)
														{
															if (!TextureToCustomize.Texture->IsFullyStreamedIn() && !MeshComponent->bForceMipStreaming)
															{
																TextureToCustomize.Texture->SetForceMipLevelsToBeResident(5.0f);
																TextureToCustomize.Texture->WaitForStreaming();
															}

															MaterialInstanceDynamic->SetTextureParameterValueByInfo(TextureToCustomize.MaterialParameterInfo, TextureToCustomize.Texture.Get());

															WeakThis.Get()->OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::MaterialInstanceDynamic, TextureToCustomize.ComponentToCustomizeTag);
														}
													},
													FStreamableManager::AsyncLoadHighPriority
												);
											}
										}
										else
										{
											UE_LOG(LogNebulaGraphics, Verbose, TEXT("MaterialIndex not found."));
										}
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("MeshComponent with tag %s not found."), *(TextureToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("TextureCustomizationList doesn't contains a Texture with specified %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizeDecalMaterial(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->DecalMaterialCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsDecalMaterialCustomization* DecalMaterialCustomization = CustomizationDataAsset->DecalMaterialCustomizationList.FindByPredicate([&](const FNebulaGraphicsDecalMaterialCustomization& InItem)
				{
					return InItem.ID.MatchesTagExact(ID);
				});
				if (DecalMaterialCustomization)
				{
					for (const FNebulaGraphicsDecalMaterialToCustomize& DecalMaterialToCustomize : DecalMaterialCustomization->DecalMaterialToCustomizeList)
					{
						// Find component to customize
						if (DecalMaterialToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(UDecalComponent::StaticClass(), DecalMaterialToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									UDecalComponent* DecalComponent = Cast<UDecalComponent>(ActorComponent);
									if (DecalComponent)
									{
										// Customize DecalMaterial
										if (DecalMaterialToCustomize.MaterialInstance.ToSoftObjectPath().IsValid())
										{
											OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::DecalMaterialInstance, DecalMaterialToCustomize.ComponentToCustomizeTag);

											// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
											TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

											FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
											StreamableManager.RequestAsyncLoad(
												DecalMaterialToCustomize.MaterialInstance.ToSoftObjectPath(),
												[WeakThis, DecalMaterialToCustomize, DecalComponent]()
												{
													if (WeakThis.IsValid() && DecalMaterialToCustomize.MaterialInstance.ToSoftObjectPath().IsValid() && DecalComponent)
													{
														DecalComponent->SetDecalMaterial(DecalMaterialToCustomize.MaterialInstance.Get());

														DecalMaterialToCustomize.MaterialInstance->SetForceMipLevelsToBeResident(false, true, 5.0f, 0, false, true);

														WeakThis.Get()->OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::DecalMaterialInstance, DecalMaterialToCustomize.ComponentToCustomizeTag);
													}
												},
												FStreamableManager::AsyncLoadHighPriority
											);
										}
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("DecalComponent with tag %s not found."), *(DecalMaterialToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("DecalMaterialCustomizationList doesn't contains a MaterialInterface with specified %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizeDecalScalar(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->DecalScalarCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsDecalScalarCustomization* DecalScalarCustomization = CustomizationDataAsset->DecalScalarCustomizationList.FindByPredicate([&](const FNebulaGraphicsDecalScalarCustomization& InItem)
				{
					return InItem.ID.MatchesTagExact(ID);
				});
				if (DecalScalarCustomization)
				{
					for (const FNebulaGraphicsDecalScalarToCustomize& DecalScalarToCustomize : DecalScalarCustomization->DecalScalarToCustomizeList)
					{
						// Find component to customize
						if (DecalScalarToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(UDecalComponent::StaticClass(), DecalScalarToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									UDecalComponent* DecalComponent = Cast<UDecalComponent>(ActorComponent);
									if (DecalComponent)
									{
										// Customize DecalScalar
										UMaterialInstanceDynamic* MaterialInstanceDynamic = Cast<UMaterialInstanceDynamic>(DecalComponent->GetDecalMaterial());
										if (MaterialInstanceDynamic == nullptr)
										{
											MaterialInstanceDynamic = DecalComponent->CreateDynamicMaterialInstance();
										}
										if (MaterialInstanceDynamic)
										{
											OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::DecalMaterialInstanceDynamic, DecalScalarToCustomize.ComponentToCustomizeTag);

											MaterialInstanceDynamic->SetScalarParameterValueByInfo(DecalScalarToCustomize.MaterialParameterInfo, DecalScalarToCustomize.Scalar);

											OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::DecalMaterialInstanceDynamic, DecalScalarToCustomize.ComponentToCustomizeTag);
										}
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("DecalComponent with tag %s not found."), *(DecalScalarToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("DecalScalarCustomizationList doesn't contains a MaterialInterface with specified %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizeDecalVector(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->DecalVectorCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsDecalVectorCustomization* DecalVectorCustomization = CustomizationDataAsset->DecalVectorCustomizationList.FindByPredicate([&](const FNebulaGraphicsDecalVectorCustomization& InItem)
				{
					return InItem.ID.MatchesTagExact(ID);
				});
				if (DecalVectorCustomization)
				{
					for (const FNebulaGraphicsDecalVectorToCustomize& DecalVectorToCustomize : DecalVectorCustomization->DecalVectorToCustomizeList)
					{
						// Find component to customize
						if (DecalVectorToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(UDecalComponent::StaticClass(), DecalVectorToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									UDecalComponent* DecalComponent = Cast<UDecalComponent>(ActorComponent);
									if (DecalComponent)
									{
										// Customize DecalVector
										UMaterialInstanceDynamic* MaterialInstanceDynamic = Cast<UMaterialInstanceDynamic>(DecalComponent->GetDecalMaterial());
										if (MaterialInstanceDynamic == nullptr)
										{
											MaterialInstanceDynamic = DecalComponent->CreateDynamicMaterialInstance();
										}
										if (MaterialInstanceDynamic)
										{
											OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::DecalMaterialInstanceDynamic, DecalVectorToCustomize.ComponentToCustomizeTag);

											MaterialInstanceDynamic->SetVectorParameterValueByInfo(DecalVectorToCustomize.MaterialParameterInfo, DecalVectorToCustomize.LinearColor);

											OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::DecalMaterialInstanceDynamic, DecalVectorToCustomize.ComponentToCustomizeTag);
										}
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("DecalComponent with tag %s not found."), *(DecalVectorToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("DecalVectorCustomizationList doesn't contains a MaterialInterface with specified %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizeDecalTexture(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->DecalTextureCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsDecalTextureCustomization* DecalTextureCustomization = CustomizationDataAsset->DecalTextureCustomizationList.FindByPredicate([&](const FNebulaGraphicsDecalTextureCustomization& InItem)
				{
					return InItem.ID.MatchesTagExact(ID);
				});
				if (DecalTextureCustomization)
				{
					for (const FNebulaGraphicsDecalTextureToCustomize& DecalTextureToCustomize : DecalTextureCustomization->DecalTextureToCustomizeList)
					{
						// Find component to customize
						if (DecalTextureToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(UDecalComponent::StaticClass(), DecalTextureToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									UDecalComponent* DecalComponent = Cast<UDecalComponent>(ActorComponent);
									if (DecalComponent)
									{
										// Customize DecalTexture
										if (DecalTextureToCustomize.Texture.ToSoftObjectPath().IsValid())
										{
											OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::DecalMaterialInstanceDynamic, DecalTextureToCustomize.ComponentToCustomizeTag);

											// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
											TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

											FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
											StreamableManager.RequestAsyncLoad(
												DecalTextureToCustomize.Texture.ToSoftObjectPath(),
												[WeakThis, DecalTextureToCustomize, DecalComponent]()
												{
													if (DecalComponent)
													{
														UMaterialInstanceDynamic* MaterialInstanceDynamic = Cast<UMaterialInstanceDynamic>(DecalComponent->GetDecalMaterial());
														if (MaterialInstanceDynamic == nullptr)
														{
															MaterialInstanceDynamic = DecalComponent->CreateDynamicMaterialInstance();
														}
														if (WeakThis.IsValid() && DecalTextureToCustomize.Texture.ToSoftObjectPath().IsValid() && MaterialInstanceDynamic)
														{
															if (!DecalTextureToCustomize.Texture->IsFullyStreamedIn())
															{
																DecalTextureToCustomize.Texture->SetForceMipLevelsToBeResident(5.0f);
																DecalTextureToCustomize.Texture->WaitForStreaming();
															}

															MaterialInstanceDynamic->SetTextureParameterValueByInfo(DecalTextureToCustomize.MaterialParameterInfo, DecalTextureToCustomize.Texture.Get());

															WeakThis.Get()->OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::DecalMaterialInstanceDynamic, DecalTextureToCustomize.ComponentToCustomizeTag);
														}
													}
												},
												FStreamableManager::AsyncLoadHighPriority
											);
										}
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("DecalComponent with tag %s not found."), *(DecalTextureToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("DecalTextureCustomizationList doesn't contains a MaterialInterface with specified %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizeGroom(const FGameplayTagContainer& IDList)
{
	if (CustomizationDataAsset)
	{
		if (CustomizationDataAsset->GroomCustomizationList.Num() > 0)
		{
			for (const FGameplayTag& ID : IDList)
			{
				// Find customization
				FNebulaGraphicsGroomCustomization* GroomCustomization = CustomizationDataAsset->GroomCustomizationList.FindByPredicate([&](const FNebulaGraphicsGroomCustomization& InItem)
				{
					return InItem.ID.MatchesTagExact(ID);
				});
				if (GroomCustomization)
				{
					for (const FNebulaGraphicsGroomToCustomize& GroomToCustomize : GroomCustomization->GroomToCustomizeList)
					{
						// Find component to customize
						if (GroomToCustomize.ComponentToCustomizeTag != FGameplayTag::EmptyTag)
						{
							TArray<UActorComponent*> ActorComponentList = GetOwner()->GetComponentsByTag(UGroomComponent::StaticClass(), GroomToCustomize.ComponentToCustomizeTag.GetTagName());
							if (ActorComponentList.Num() > 0)
							{
								for (UActorComponent* ActorComponent : ActorComponentList)
								{
									UGroomComponent* GroomComponent = Cast<UGroomComponent>(ActorComponent);
									if (GroomComponent)
									{
										// Customize Groom Asset
										if (GroomToCustomize.GroomAsset.ToSoftObjectPath().IsValid())
										{
											OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::Groom, GroomToCustomize.ComponentToCustomizeTag);

											// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
											TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

											FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
											StreamableManager.RequestAsyncLoad(
												GroomToCustomize.GroomAsset.ToSoftObjectPath(),
												[WeakThis, GroomToCustomize, GroomComponent]()
												{
													if (WeakThis.IsValid() && GroomToCustomize.GroomAsset.ToSoftObjectPath().IsValid() && GroomComponent)
													{
														GroomComponent->SetGroomAsset(GroomToCustomize.GroomAsset.Get());
														WeakThis.Get()->OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::Groom, GroomToCustomize.ComponentToCustomizeTag);
													}
												},
												FStreamableManager::AsyncLoadHighPriority
											);
										}

										// Customize Groom Asset
										if (GroomToCustomize.BindingAsset.ToSoftObjectPath().IsValid())
										{
											OnCustomizationStarted.Broadcast(ENebulaGraphicsCustomizationType::Groom, GroomToCustomize.ComponentToCustomizeTag);

											// Using weak ptr in case 'this' has gone out of scope by the time this lambda is called
											TWeakObjectPtr<UNebulaGraphicsCustomizationComponent> WeakThis(this);

											FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
											StreamableManager.RequestAsyncLoad(
												GroomToCustomize.BindingAsset.ToSoftObjectPath(),
												[WeakThis, GroomToCustomize, GroomComponent]()
												{
													if (WeakThis.IsValid() && GroomToCustomize.BindingAsset.ToSoftObjectPath().IsValid() && GroomComponent)
													{
														GroomComponent->SetBindingAsset(GroomToCustomize.BindingAsset.Get());

														WeakThis.Get()->OnCustomizationCompleted.Broadcast(ENebulaGraphicsCustomizationType::Groom, GroomToCustomize.ComponentToCustomizeTag);
													}
												},
												FStreamableManager::AsyncLoadHighPriority
											);
										}
									}
								}
							}
							else
							{
								UE_LOG(LogNebulaGraphics, Verbose, TEXT("GroomComponent with tag %s not found."), *(GroomToCustomize.ComponentToCustomizeTag.ToString()));
							}
						}
						else
						{
							UE_LOG(LogNebulaGraphics, Verbose, TEXT("ComponentToCustomizeTag not specified."));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaGraphics, Verbose, TEXT("GroomCustomizationList doesn't contains a MaterialInterface with specified %s."), *(ID.ToString()));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaGraphics, Warning, TEXT("CustomizationDataAsset not found."));
	}
}

void UNebulaGraphicsCustomizationComponent::SetCustomizationDataAsset(UCustomizationDataAsset* InCustomizationDataAsset)
{
	CustomizationDataAsset = InCustomizationDataAsset;
}

void UNebulaGraphicsCustomizationComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR

	if (StaticMeshIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeStaticMesh(StaticMeshIDList);
	}
	if (SkeletalMeshIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeSkeletalMesh(SkeletalMeshIDList);
	}
	if (AnimationIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeAnimation(AnimationIDList);
	}
	if (MaterialIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeMaterial(MaterialIDList);
	}
	if (ScalarIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeScalar(ScalarIDList);
	}
	if (VectorIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeVector(VectorIDList);
	}
	if (TextureIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeTexture(TextureIDList);
	}
	if (DecalMaterialIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeDecalMaterial(DecalMaterialIDList);
	}
	if (DecalScalarIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeDecalScalar(DecalScalarIDList);
	}
	if (DecalVectorIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeDecalVector(DecalVectorIDList);
	}
	if (DecalTextureIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeDecalTexture(DecalTextureIDList);
	}
	if (GroomIDList != FGameplayTagContainer::EmptyContainer)
	{
		CustomizeGroom(GroomIDList);
	}

#endif
}

void UNebulaGraphicsCustomizationComponent::BeginPlay()
{
	Super::BeginPlay();

	OnCustomizationStarted.AddUniqueDynamic(this, &ThisClass::CustomizationStart);
	OnCustomizationCompleted.AddUniqueDynamic(this, &ThisClass::CustomizationComplete);
}

void UNebulaGraphicsCustomizationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyAllAttachedComponents();

	OnCustomizationStarted.RemoveDynamic(this, &ThisClass::CustomizationStart);
	OnCustomizationCompleted.RemoveDynamic(this, &ThisClass::CustomizationComplete);

	Super::EndPlay(EndPlayReason);
}

UCustomizationDataAsset* UNebulaGraphicsCustomizationComponent::GetCustomizationDataAsset() const
{
	return CustomizationDataAsset;
}

void UNebulaGraphicsCustomizationComponent::DestroyAllAttachedComponents()
{
	if (GetOwner())
	{
		TArray<UActorComponent*> AttachedActorComponentList = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(), NebulaGraphicsAttachedComponentTag);
		for (UActorComponent* AttachedActorComponent : AttachedActorComponentList)
		{
			UMeshComponent* MeshComponent = Cast<UMeshComponent>(AttachedActorComponent);
			if (MeshComponent && MeshComponent->IsValidLowLevel())
			{
				MeshComponent->DestroyComponent();
				MeshComponent = nullptr;
			}
		}
	}
}

void UNebulaGraphicsCustomizationComponent::DestroyAttachedComponents(const UMeshComponent* InMeshComponent)
{
	if (GetOwner() && InMeshComponent)
	{
		TArray<USceneComponent*> ChildrenComponentList = {};
		InMeshComponent->GetChildrenComponents(false, ChildrenComponentList);
		for (USceneComponent* ChildrenComponent : ChildrenComponentList)
		{	
			if (ChildrenComponent && ChildrenComponent->IsValidLowLevel() && ChildrenComponent->ComponentTags.Contains(NebulaGraphicsAttachedComponentTag))
			{
				UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ChildrenComponent);
				if (StaticMeshComponent)
				{
					StaticMeshComponent->DestroyComponent();
					StaticMeshComponent = nullptr;
				}
				USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ChildrenComponent);
				if (SkeletalMeshComponent)
				{
					SkeletalMeshComponent->DestroyComponent();
					SkeletalMeshComponent = nullptr;
				}
			}
		}
	}
}

void UNebulaGraphicsCustomizationComponent::CustomizationStart(const ENebulaGraphicsCustomizationType InCustomizationType, const FGameplayTag& InCustomizedComponentTag)
{
	++CustomizationStarted;
}

void UNebulaGraphicsCustomizationComponent::CustomizationComplete(const ENebulaGraphicsCustomizationType InCustomizationType, const FGameplayTag& InCustomizedComponentTag)
{
	++CustomizationCompeted;

	if (CustomizationStarted == CustomizationCompeted)
	{
		CustomizationStarted = 0;
		CustomizationCompeted = 0;
		OnAllCustomizationCompleted.Broadcast();
	}
}