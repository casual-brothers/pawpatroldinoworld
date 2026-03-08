#include "Core/NebulaFlowGameState.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowCoreDelegates.h"


ANebulaFlowGameState::ANebulaFlowGameState(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void ANebulaFlowGameState::HandleMatchIsWaitingToStart()
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GInstance)
	{
		FName MapLoaded = FName(*UGameplayStatics::GetCurrentLevelName(this));
		FNebulaFlowCoreDelegates::OnLevelLoaded.Broadcast(MapLoaded);
	}
	Super::HandleMatchIsWaitingToStart();
}

