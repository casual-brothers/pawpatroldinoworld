#include "FSM/States/NebulaFlowLoadUserFSMState.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowLocalPlayer.h"


UNebulaFlowLoadUserFSMState::UNebulaFlowLoadUserFSMState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
}

void UNebulaFlowLoadUserFSMState::OnFSMStateEnter_Implementation(const FString & InOption)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GInstance)
	{
		GInstance->OnPersistentUserLoadedDelegate.AddDynamic(this,&UNebulaFlowLoadUserFSMState::OnUserLoaded);
		UNebulaFlowLocalPlayer* localPlayerOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this);
		if (localPlayerOwner)
		{
			localPlayerOwner->LoadPersistentUser();
		}
	}
}

void UNebulaFlowLoadUserFSMState::OnFSMStateExit_Implementation()
{
	Super::OnFSMStateExit_Implementation();
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GInstance)
	{
		GInstance->OnPersistentUserLoadedDelegate.RemoveDynamic(this, &UNebulaFlowLoadUserFSMState::OnUserLoaded);
	}
}

void UNebulaFlowLoadUserFSMState::OnUserLoaded(int ControllerId)
{
	TriggerTransition(FName("Proceed"));
}
