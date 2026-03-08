// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GAS/PDWSpawnerAbility.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Actors/PDWAbilitySpawnedActor.h"
#include "Gameplay/Pawns/PDWCharacter.h"
// #TODO: EVALUATE IF USE POOLING SYSTEM OR SPAWN IS ENOUGH
void UPDWSpawnerAbility::SpawnActor(const FActorSpawnParameters inParams,FTransform inTransform)
{
	ensureMsgf(SpawnConfiguration.ActorToSpawn,TEXT("Missing Actor To spawn!!"));

	SpawnedActor = Cast<APDWAbilitySpawnedActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, SpawnConfiguration.ActorToSpawn, inTransform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn,inParams.Owner));
	if (!ensureMsgf(SpawnedActor, TEXT("Failed to spawn actor")))
	{
		return;
	}
	FAbilityInfo AbilityInfo;
	AbilityInfo.SkillActionType = SkillType;
	SpawnedActor->SendAbilityInfo(AbilityInfo);
	UGameplayStatics::FinishSpawningActor(SpawnedActor, inTransform);
	SpawnedActorsIndex++;
	if (SpawnConfiguration.SpawnType == ESpawnType::MultipleSpawn)
	{
		if (SpawnedActorsIndex >= SpawnConfiguration.SpawnAmount)
		{
			UWorld* World = GetWorld();
			World->GetTimerManager().ClearAllTimersForObject(this);
		}
	}

}

void UPDWSpawnerAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//#TODO : check if needed FOR ADJUST ROTATION TOWARDS THE INTERACTION ACTOR.
	//GetBestInteractionForTag();
}

void UPDWSpawnerAbility::TriggerAbilityLogic_Implementation()
{
	RequestSpawn();
}

void UPDWSpawnerAbility::RequestSpawn()
{
	APlayerController* Controller = Cast<APlayerController>(GetAvatarActorFromActorInfo());
	FActorSpawnParameters Params;
	Params.Owner = Controller->GetPawn();
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	const FTransform SpawnTransform = UPDWGameplayFunctionLibrary::GetSocketTransformFromSpecificMesh(Params.Owner,SpawnConfiguration.SpawnSocket,SpawnConfiguration.MeshToAttachTag.GetTagName());

	if (SpawnConfiguration.SpawnType == ESpawnType::SingleSpawn)
	{
		SpawnActor(Params,SpawnTransform);
	}
	else
	{
		UWorld* World = GetWorld();
		//Can't use const ref because create uobject request copy
		FTimerDelegate SpawnDelegate = FTimerDelegate::CreateUObject(this,&UPDWSpawnerAbility::SpawnActor,Params,SpawnTransform);
		World->GetTimerManager().SetTimer(SpawnTimerHandler,SpawnDelegate,SpawnConfiguration.SpawnInterval,true);
	}
}
