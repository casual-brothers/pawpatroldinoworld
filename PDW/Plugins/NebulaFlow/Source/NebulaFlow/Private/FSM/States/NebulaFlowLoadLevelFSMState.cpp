#include "FSM/States/NebulaFlowLoadLevelFSMState.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"




UNebulaFlowLoadLevelFSMState::UNebulaFlowLoadLevelFSMState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
}

void UNebulaFlowLoadLevelFSMState::OnFSMStateEnter_Implementation(const FString & InOption)
{
	Super::OnFSMStateEnter_Implementation(InOption);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, *FString::Printf(TEXT("Level to load %s"), *LevelToLoadName.ToString()));
	
	if (LevelToLoadName.IsNone() && StateParameter.IsNone())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, *FString::Printf(TEXT("LevelToLoadName and Parameter are nullptr")));
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, *FString::Printf(TEXT("Level loaded"), *LevelToLoadName.ToString()));
	
	UNebulaFlowCoreFunctionLibrary::LoadLevelByName(this, LevelToLoadName.IsNone() ? StateParameter : LevelToLoadName, bIsabsolute, TravelOptions);
}

