// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Interaction/PDWTalkingBehaviour.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "QuestSettings.h"
#include "UI/WidgetsComponent/PDWIconComponent.h"
#include "FlowComponent.h"
#include "GameFramework/PlayerController.h"

void UPDWTalkingBehaviour::ExecuteBehaviour_Implementation()
{
	//TODO: Replace this check with a missing configuration check when the non-quest dialogue is implemented
	FlowComp = GetOwnerComponent()->GetOwner()->FindComponentByClass<UFlowComponent>();
	if (FlowComp && FlowComp->RootFlow)
	{
		APlayerController* TalkingPlayer = {};
		if (APawn* PlayerPawn = Cast<APawn>(GetExecutioner()))
		{
			TalkingPlayer = Cast<APlayerController>(PlayerPawn->GetController());
		}
		UPDWDialogueSubSystem::Get(this)->PlayerDoingDialogue = TalkingPlayer;
		UNebulaFlowCoreFunctionLibrary::TriggerAction(GetOwnerComponent(), UFlowDeveloperSettings::GetDialogueTag().ToString(), "");
		OnStateTrigger();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing Talking Configuration %s"), *GetOwnerComponent()->GetOwner()->GetName());
		StopBehaviour();
	}
}

void UPDWTalkingBehaviour::OnStateTrigger()
{
	// #DEV <comment> [#daniele.m, 4 September 2025, OnStateTrigger]
	//I WOULD HAVE DONE A BASE NPC CHARACTER CLASS WITH A SYSTEM OF TAG CONTAINER AND EVENT OF TAGS ADDED THAT ICONCOMPONENT AND OTHER COMPONENT WOULD QUERY AND BIND TO CHECK TAGS INSTEAD OF GIBING TAGS DIRECTLY TO COMPONENT
	//TAGS CAN BE USED ALSO FROM OTHER COMPONENTS!

	//perdoname por mi vida loca
	UPDWIconComponent* IconComp = GetOwnerComponent()->GetOwner()->FindComponentByClass<UPDWIconComponent>();
	if (IconComp && IconComp->GetIconTag().IsValid())
	{
		UPDWGameplayFunctionLibrary::TriggerQuestTalkingEvent(GetOwnerComponent());
	}
	else
	{
		// #DEV <TO IMPLEMENT> [#daniele.m, 4 September 2025, OnStateTrigger]
		FlowComp->StartRootFlow();
	}

	//can't implement this way, step node does stuff that should be done via simple tags with the toggle quest giver(renamed toggle quest target)
	//if(GetOwnerComponent()->GetOwner()->Tags.Contains(UQuestSettings::GetQuestGiverTag().GetTagName()))
	//{
	//	UPDWGameplayFunctionLibrary::TriggerQuestTalkingEvent(GetOwnerComponent());
	//}
	//else
	//{
	//	// #DEV <implement flowgraphcall> [#daniele.m, 4 September 2025, OnStateTrigger]
	//}
	StopBehaviour();
}

void UPDWTalkingBehaviour::StopBehaviour_Implementation()
{
	UPDWDialogueSubSystem::Get(this)->PlayerDoingDialogue = nullptr;
	Super::StopBehaviour_Implementation();
}
