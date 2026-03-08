// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWDrillExcavationTargetingComp.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWDrillExcavationTargetingComp::BeginPlay()
{
	Super::BeginPlay();
	UPDWEventSubsytem::Get(this)->OnRockRemoved.AddUniqueDynamic(this,&UPDWDrillExcavationTargetingComp::OnRockRemovedEvent);
	CachedBlockingRocks = BlockingRocks;
}

void UPDWDrillExcavationTargetingComp::EndPlay(EEndPlayReason::Type Reason)
{
	UPDWEventSubsytem::Get(this)->OnRockRemoved.RemoveDynamic(this,&UPDWDrillExcavationTargetingComp::OnRockRemovedEvent);
	Super::EndPlay(Reason);
}

void UPDWDrillExcavationTargetingComp::Reset()
{
	BlockingRocks = CachedBlockingRocks;
}

void UPDWDrillExcavationTargetingComp::OnRockRemovedEvent(AActor* inRock)
{
	if (BlockingRocks.Contains(inRock))
	{

		BlockingRocks.Remove(inRock);
		RemovedRocks.AddUnique(inRock);
		BP_OnRockRemoved(inRock);

		if(BlockingRocks.Num()<=0)
			BP_OnAllRockRemoved();
	}
}
