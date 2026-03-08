// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWStopSoundtrackNode.h"
#include "Managers/PDWAudioManager.h"


void UPDWStopSoundtrackNode::PlaySoundtrack()
{
	ResolvedNotifyContainer = ResolveNotifyContainer();
	if (ResolvedNotifyContainer.Num())
	{
		UPDWAudioManager::Get(this)->StopMiniGameSoundtrack(ResolvedNotifyContainer.First());
	}
}
