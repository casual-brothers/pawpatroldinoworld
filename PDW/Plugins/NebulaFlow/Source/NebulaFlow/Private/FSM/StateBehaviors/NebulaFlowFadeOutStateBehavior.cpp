#include "FSM/StateBehaviors/NebulaFlowFadeOutStateBehavior.h"
#include "GameFramework/PlayerController.h"


UNebulaFlowFadeOutStateBehavior::UNebulaFlowFadeOutStateBehavior(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
}

void UNebulaFlowFadeOutStateBehavior::BeforeOnEnter_Implementation()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* Controller = (Iterator->Get());
		if (Controller != nullptr)
		{
			Controller->PlayerCameraManager->StartCameraFade(1.f,0.f,FadeOutTime,FadeColor);
		}
	}
}

void UNebulaFlowFadeOutStateBehavior::OnExit_Implementation()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* Controller = (Iterator->Get());
		if (Controller != nullptr)
		{
			Controller->PlayerCameraManager->SetManualCameraFade(0.f,FadeColor,false);
		}
	}
	
}
