
#include "Components/NebulaFlowFSMComponent.h"
#include "FSM/NebulaFlowBaseFSM.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowFSMManager.h"

UNebulaFlowFSMComponent::UNebulaFlowFSMComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true;
}

void UNebulaFlowFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);
	if (FSMRef)
	{
		FSMRef->UpdateFSM(DeltaTime);
	}
}

void UNebulaFlowFSMComponent::BeginPlay()
{
	Super::BeginPlay();
	FSMManagerRef = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowFSMManager(this);
	
	if(ensureMsgf(FSMClass, TEXT("No FSM Class defined")) && FSMManagerRef)
	{
		FSMRef = NewObject<UNebulaFlowBaseFSM>(this,FSMClass);
		if (FSMRef)
		{
			FSMRef->InitFSM();
			FSMManagerRef->RegisterFSM(FSMRef);
		}
	}
}

void UNebulaFlowFSMComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (FSMRef)
	{
		FSMRef->UninitFSM();
		FSMManagerRef->UnregisterFSM(FSMRef);
	}
	
	Super::EndPlay(EndPlayReason);
}
