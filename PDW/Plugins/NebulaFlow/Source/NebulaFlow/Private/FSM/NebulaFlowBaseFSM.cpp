#include "FSM/NebulaFlowBaseFSM.h"
#include "FSM/NebulaFlowBaseFSMState.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/NebulaFlowFSMManager.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "NebulaFlow.h"
#include "Managers/NebulaFlowUIManager.h"
#include "UI/NebulaFlowBasePage.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "FSM/StateInitConditions/NebulaFlowBaseStateInitCondition.h"

//#pragma optimize("", off)

static TAutoConsoleVariable<int32> CVarShowFSMDebugData(
	TEXT("ShowFsmDebugData"),
	0,
	TEXT("Shows FSM useful infos\n"),	
	ECVF_Default);

UNebulaFlowBaseFSM::UNebulaFlowBaseFSM(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
}

void UNebulaFlowBaseFSM::InitFSM()
{
	bHasBeenInitialized = true;
	GoToEntryPoint();
	
}

void UNebulaFlowBaseFSM::UninitFSM()
{
	bCanUpdate = false;
}

void UNebulaFlowBaseFSM::UpdateFSM(float deltaTime)
{
#if !UE_BUILD_SHIPPING
	int32 debugVar = CVarShowFSMDebugData.GetValueOnAnyThread();
	if (debugVar > 0)
	{
		UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(this);
		FString Page=FString("");
		if (UIManager && UIManager->GetCurrentDisplayedPage())
		{
			Page = UIManager->GetCurrentDisplayedPage()->GetName();
		}
		if (GEngine && CurrentFsmState)
			GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Blue, FString::Printf(TEXT("CurrentState: %s \n CurrentPage: %s "), *CurrentFsmState->StateName.ToString(),*Page));
			
	}
#endif
	
	if (bCanUpdate && bHasBeenInitialized)
	{
		if(bIsWaitingInitConditions)
		{
			ResolveInitConditions(deltaTime);
			return;
		}
		
		if(FSMMode == EFsmMode::AsyncMode)
		{ 
			ManageTransition();
		}
		if (!bIsBetweenTransition && CurrentFsmState)
		{
			CurrentFsmState->OnFSMStateUpdate(deltaTime);
		}		
	}
}

void UNebulaFlowBaseFSM::SetCanUpdateFSM(bool bInCanUpdate)
{
	bCanUpdate = bInCanUpdate;
}

void UNebulaFlowBaseFSM::TriggerLabel(FName LabelName)
{
	UNebulaFlowFSMManager* FSMMAnager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowFSMManager(GetWorld());
	if (FSMMAnager)
	{
		FSMMAnager->SetCurrentFSMLabel(LabelName);
	}
}

bool UNebulaFlowBaseFSM::IsInBetweenTransition()
{
	return bIsBetweenTransition;
}

bool UNebulaFlowBaseFSM::TriggerTransition(const FName TransitionName, FName LabelName)
{
	if (!bIsBetweenTransition && PendingTransition == NAME_None)
	{
		if (CurrentFsmState &&
			FSMStructureMap.Contains(CurrentFsmState->StateName) &&
			FSMStructureMap[CurrentFsmState->StateName].TransitionToStateMap.Contains(TransitionName))
		{ 			
			PendingTransition = TransitionName;
			UE_LOG(LogNebulaFlowFSM, Log, TEXT("TriggeredTransition: %s"), *TransitionName.ToString());
			if (LabelName != NAME_None)
			{
				TriggerLabel(LabelName);
			}
			if (FSMMode == EFsmMode::SyncMode)
			{
				ManageTransition();
			}
			return true;
		}
	}
	else
	{
		UE_LOG(LogNebulaFlowFSM, Warning, TEXT("Transition Already In progress Or Invalid TransitionTriggered for CurrentState, Transition is ignored"));
	}
	return false;
}

bool UNebulaFlowBaseFSM::TriggerTransitionWithOption(const FName TransitionName, FString InOption, FName LabelName /*= NAME_None*/)
{
	if (FSMMode == EFsmMode::SyncMode)
	{
		PendingOption = InOption;
	}
	
	bool bSuccess = TriggerTransition(TransitionName,LabelName);
	
	if(bSuccess && FSMMode==EFsmMode::AsyncMode)
	{
		PendingOption = InOption;
	}
	return bSuccess;
}

bool UNebulaFlowBaseFSM::TriggerLabelTransition(const FName LabelName, FString InOption /*= FString("")*/)
{
	if(LabelToEntryPointMap.Contains(LabelName))
	{ 
		PendingOption = InOption;
		TriggerLabel(LabelName);
		GoToEntryPoint();
		return true;
	}
	return false;
}

UWorld* UNebulaFlowBaseFSM::GetWorld() const
{
	UActorComponent* OwnerComponent = Cast<UActorComponent>(GetOuter());
	if (OwnerComponent)
	{
		return GetOuter()->GetWorld();
	}
	return nullptr;
}

void UNebulaFlowBaseFSM::OnLoadLevelStarted(FName LevelName)
{
	UNebulaFlowUIFunctionLibrary::HideGameDialog(this);
	if (CurrentFsmState)
	{
		ManageBehaviors(CurrentFsmState,EStatePhase::EOnExit);
		CurrentFsmState->OnFSMStateExit();
		CurrentFsmState->UnInitState();
		CurrentFsmState = nullptr;
	}
}

void UNebulaFlowBaseFSM::OnLoadLevelEnded(FName LevelName)
{

}

FName UNebulaFlowBaseFSM::GetEntryPointStateLabel()
{
	FName CurrentLabelName;
	UNebulaFlowFSMManager* FSMMAnager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowFSMManager(GetWorld());
	if (FSMMAnager && FSMMAnager->GetCurrentFSMLabel(CurrentLabelName))
	{
		if (LabelToEntryPointMap.Contains(CurrentLabelName) &&
			FSMStructureMap.Contains(LabelToEntryPointMap[CurrentLabelName]))
		{
			return CurrentLabelName;
		}
	}
	return DefaultLabelName;
}

void UNebulaFlowBaseFSM::ManageTransition()
{
	if (bCanUpdate && bHasBeenInitialized)
	{

	if(bIsWaitingInitConditions)
	{
		return;
	}

#if !UE_BUILD_SHIPPING
		int32 debugVar = CVarShowFSMDebugData.GetValueOnAnyThread();
#endif
		
		if (!bIsBetweenTransition && PendingTransition != NAME_None)
		{
			bIsBetweenTransition = true;
			TSubclassOf<UNebulaFlowBaseFSMState> NewStateClass = nullptr;
			FName NewStateName = NAME_None;
			FName NewStateParameter = NAME_None;
			if (FSMStructureMap.Contains(CurrentFsmState->StateName) &&
				FSMStructureMap[CurrentFsmState->StateName].TransitionToStateMap.Contains(PendingTransition))
			{
				NewStateName = FSMStructureMap[CurrentFsmState->StateName].TransitionToStateMap[PendingTransition];
				NewStateClass = FSMStructureMap[NewStateName].FSMStateClass;
				NewStateParameter = FSMStructureMap[NewStateName].StateParameter;
			}
			else
			{
				UE_LOG(LogNebulaFlowFSM, Warning, TEXT("No transition with name %s found for state %s, Ignore Transition"), *CurrentFsmState->StateName.ToString(), *PendingTransition.ToString());

				PendingTransition = NAME_None;
				bIsBetweenTransition = false;
				return;
			}
			ensure(NewStateClass);
			if (NewStateClass == nullptr)
			{
				UE_LOG(LogNebulaFlowFSM, Warning, TEXT("No valid Class for new State found, Ignore Transition"))
					PendingTransition = NAME_None;
				bIsBetweenTransition = false;
				return;
			}
			//New State  transition Found , uninit and destroy old state create and init new one

			if (CurrentFsmState != nullptr)	//Uninit and destroy OldState
			{
				UE_LOG(LogNebulaFlowFSM, Log, TEXT("Exiting From Fsm State: %s"), *CurrentFsmState->StateName.ToString());

#if !UE_BUILD_SHIPPING
				if (debugVar > 0)
				{
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Exiting From FSM State %s"), *CurrentFsmState->StateName.ToString()));
					}
				}
#endif			
				EndCurrentState();
			}
			if (NewStateClass != nullptr)	// create and init new state
			{
				CurrentFsmState = NewObject<UNebulaFlowBaseFSMState>(this, NewStateClass, NewStateName);
				if (CurrentFsmState)
				{
					UE_LOG(LogNebulaFlowFSM, Log, TEXT("Entering in Fsm State: %s"), *NewStateName.ToString());
#if !UE_BUILD_SHIPPING
					if (debugVar > 0)
					{
						if (GEngine)
							GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Entering in FSM State %s"), *NewStateName.ToString()));
					}
#endif					
					PendingTransition = NAME_None;
					bIsBetweenTransition = false;
					if (!NewStateParameter.IsNone())
					{
						CurrentFsmState->StateParameter = NewStateParameter;
					}
					CurrentFsmState->InitState(NewStateName, this);					
					BeginCurrentState();
					return;
				}
			}

		}		
		bIsBetweenTransition = false;
	}
}

void UNebulaFlowBaseFSM::ManageBehaviors(UNebulaFlowBaseFSMState* CurrentState, EStatePhase CurrentPhase)
{
	if (CurrentState)
	{
		for (auto* currentBehavior : CurrentState->StateBehaviors)
		{
			switch (CurrentPhase)
			{
			case (EStatePhase::EBeforeOnEnter):
			{
				currentBehavior->BeforeOnEnter();
			}
			break;
			case (EStatePhase::EOnEnter):
			{
				currentBehavior->OnEnter();
			}
			break;
			case (EStatePhase::EOnExit):
			{
				currentBehavior->OnExit();
				currentBehavior->ConditionalBeginDestroy();
			}
			break;
			}
		}
	}
}



void UNebulaFlowBaseFSM::BeginCurrentState()
{
	if(CurrentFsmState && CurrentFsmState->HasPendingConditions())
	{
		bIsWaitingInitConditions = true;
	}
	
	else if(!bIsWaitingInitConditions)
	{ 
		FString currentPendingOption = PendingOption;
		PendingOption = FString("");
		ManageBehaviors(CurrentFsmState, EStatePhase::EBeforeOnEnter);
		CurrentFsmState->OnFSMStateEnter(currentPendingOption);
		ManageBehaviors(CurrentFsmState, EStatePhase::EOnEnter);
	}
}

void UNebulaFlowBaseFSM::ResolveInitConditions(float deltaTime)
{
	if(CurrentFsmState)
	{
		if(CurrentFsmState->_InternalConditionsObjects.Num()>0)
		{
			if(!CurrentFsmState->_InternalConditionsObjects[0]->IsConditionInitialized())
			{
				UE_LOG(LogNebulaFlowFSM, Log, TEXT("Initializing Init Condition: %s"), *(CurrentFsmState->_InternalConditionsObjects[0]->GetName()));
				CurrentFsmState->_InternalConditionsObjects[0]->Initialize(CurrentFsmState);
			}

#if !UE_BUILD_SHIPPING
			int32 debugVar = CVarShowFSMDebugData.GetValueOnAnyThread();
			if(debugVar>0)
			{
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Yellow, FString::Printf(TEXT("Resolving Init Condition %s for state %s"), *CurrentFsmState->_InternalConditionsObjects[0]->GetFName().ToString(),*(CurrentFsmState->StateName).ToString()));
			}
#endif
			if(CurrentFsmState->_InternalConditionsObjects[0]->CheckResolvedCondition(deltaTime))
			{
				UE_LOG(LogNebulaFlowFSM, Log, TEXT("Resolved Condition: %s"), *(CurrentFsmState->_InternalConditionsObjects[0]->GetName()));
				CurrentFsmState->_InternalConditionsObjects[0]->ConditionalBeginDestroy();
				CurrentFsmState->_InternalConditionsObjects.RemoveAt(0);
			}
		}
		else
		{
			bIsWaitingInitConditions=false;
			BeginCurrentState();
		}
	}
}

void UNebulaFlowBaseFSM::EndCurrentState()
{
	if(CurrentFsmState)
	{ 
		UNebulaFlowUIFunctionLibrary::HideGameDialog(this);		//Always close game dialogs exiting a state

		ManageBehaviors(CurrentFsmState, EStatePhase::EOnExit);
		CurrentFsmState->OnFSMStateExit();
		CurrentFsmState->UnInitState();
		CurrentFsmState->ConditionalBeginDestroy();
		CurrentFsmState = nullptr;

		GEngine->ForceGarbageCollection(true);
	}
}

void UNebulaFlowBaseFSM::GoToEntryPoint()
{
	FName CurrentLabel = GetEntryPointStateLabel();
	EndCurrentState();
	if (LabelToEntryPointMap.Contains(CurrentLabel))
	{
		const FName EntryPointState = LabelToEntryPointMap[CurrentLabel];
		if (FSMStructureMap.Contains(EntryPointState))
		{
			CurrentFsmState = NewObject<UNebulaFlowBaseFSMState>(this, FSMStructureMap[EntryPointState].FSMStateClass,EntryPointState);
			if (CurrentFsmState)
			{
				CurrentFsmState->InitState(EntryPointState,this);
				BeginCurrentState();
			}
		}
	}
	else
	{
		UE_LOG(LogNebulaFlowFSM, Warning, TEXT("No valid entry point found for FSM"));
	}
}