// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Areas/PDWDialogueArea.h"
#include "Components/ShapeComponent.h"

APDWDialogueArea::APDWDialogueArea()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	AreaConversation.ConversationType = EConversationType::HUDComment;
}

void APDWDialogueArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!bTriggered)
	{
		UPDWDialogueSubSystem::TriggerConversation(this,AreaConversation);
		bTriggered = true;
		GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}