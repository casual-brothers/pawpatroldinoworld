#include "FSM/States/NebulaFlowSaveUserFSMState.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "NebulaFlow.h"
#include "Core/NebulaFlowCoreDelegates.h"


UNebulaFlowSaveUserFSMState::UNebulaFlowSaveUserFSMState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
}

void UNebulaFlowSaveUserFSMState::OnFSMStateEnter_Implementation(const FString & InOption)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	UNebulaFlowLocalPlayer* localPlayerOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this);
	if (localPlayerOwner)
	{
		bool bResult;
		localPlayerOwner->SavePersistentUser(bResult);
		UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
		if (GInstance)
		{
			switch (GInstance->SaveCondition)
			{
			case 0:
			{
				FNebulaFlowCoreDelegates::OnGameSaved.Broadcast(bResult);
				TriggerTransitionWithOption(bResult ? FName("SaveSucceded") : FName("SaveError"), InOption);
			}
				break;
			case 1:
				if (!bResult)
				{
					//Show dialog...
					//On dialog response continue playing:
					FNebulaFlowCoreDelegates::OnGameSaved.Broadcast(bResult);
					TriggerTransitionWithOption(FName("SaveSucceded"), InOption);
				}
				else
				{
					FNebulaFlowCoreDelegates::OnGameSaved.Broadcast(bResult);
					TriggerTransitionWithOption(FName("SaveSucceded"), InOption);
				}
				break;
			case 2:
				if (!bResult)
				{
					SaveLoop();
				}
				else
				{
					FNebulaFlowCoreDelegates::OnGameSaved.Broadcast(bResult);
					TriggerTransitionWithOption(FName("SaveSucceded"), InOption);
				}
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaFlowFSM, Warning, TEXT("PlayerController MUST BE a Subclass Of NebulaPlayerController"));
		TriggerTransitionWithOption(FName("SaveError"), InOption);
	}
}

void UNebulaFlowSaveUserFSMState::SaveLoop()
{
	FNebulaFlowCoreDelegates::OnSaveFailed.Broadcast();
	SaveGameHandle = FNebulaFlowCoreDelegates::OnGameSaved.AddUObject(this, &ThisClass::OnSaveCompleted);
}

void UNebulaFlowSaveUserFSMState::OnSaveCompleted(bool bResult)
{
	if (bResult)
	{
		FNebulaFlowCoreDelegates::OnGameSaved.Remove(SaveGameHandle);
		TriggerTransition(FName("SaveSucceded"));
	}	
}

