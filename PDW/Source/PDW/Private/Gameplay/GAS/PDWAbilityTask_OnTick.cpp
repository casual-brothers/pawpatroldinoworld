// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GAS/PDWAbilityTask_OnTick.h"


UPDWAbilityTask_OnTick::UPDWAbilityTask_OnTick(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

UPDWAbilityTask_OnTick* UPDWAbilityTask_OnTick::AbilityTaskOnTick(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UPDWAbilityTask_OnTick* MyObj = NewAbilityTask<UPDWAbilityTask_OnTick>(OwningAbility);
	return MyObj;
}

void UPDWAbilityTask_OnTick::Activate()
{
	Super::Activate();
}

void UPDWAbilityTask_OnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}