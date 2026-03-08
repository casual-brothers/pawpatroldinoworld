// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GAS/PDWMultipleMontageAbility.h"
#include "Data/PDWPlayerState.h"
#include "Gameplay/Animation/PDWAnimInstance.h"

void UPDWMultipleMontageAbility::OverrideMontageToPlaySettings()
{
	FGameplayTagContainer PlayerStatesContainer = PlayerState->GetPlayerStates();
	bool bMatchFound = false;
	for (const auto& [Tag, Setting] : ExtraMontages)
	{
		if (PlayerStatesContainer.HasTag(Tag))
		{
			MontageSettings = Setting;
			bMatchFound = true;
			break;
		}
	}
	if (!bMatchFound)
	{
		MontageSettings = ExtraMontages[DefaulMontageSettingsTag];
	}

	if (bSkip)
	{
		MontageSettings.MontageToPlay = nullptr;
	}
}

bool UPDWMultipleMontageAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	Super::CanActivateAbility(Handle, ActorInfo, nullptr, nullptr, nullptr);
	APlayerController* Controller = Cast<APlayerController>(ActorInfo->AvatarActor);
	if (Controller)
	{
		USkeletalMeshComponent* Mesh = Controller->GetPawn()->FindComponentByTag<USkeletalMeshComponent>(MontageSettings.PupBackPackTag.GetTagName());
		if (Mesh)
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				if (UPDWAnimInstance* CastedAnim = Cast<UPDWAnimInstance>(AnimInstance))
				{
					if(!CastedAnim->GetSwimmingState())
						return true;
				}
			}
			const FVector StartTrace = Mesh->GetSocketLocation(FName("spine_02_skin"));
			const FVector EndTrace = StartTrace + FVector(0,0,-Trace.TraceLength);
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Controller->GetPawn());
			
			FHitResult HitResult;
			Controller->GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, Trace.CollisionChannel,Params);
			if (Trace.bDebug)
			{
				DrawDebugLine(Controller->GetWorld(), StartTrace, EndTrace, FColor::Green, true, 10.0f, 0.0f,3);
			}
			if (HitResult.bBlockingHit)
			{
				float Distance = (HitResult.ImpactPoint - StartTrace).Length();
				return Distance>MinHeight;
			}
		}
	}
	return true;
}

