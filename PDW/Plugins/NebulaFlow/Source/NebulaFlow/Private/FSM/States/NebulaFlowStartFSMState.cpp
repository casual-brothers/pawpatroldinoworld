#include "FSM/States/NebulaFlowStartFSMState.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Internationalization/Internationalization.h"
#include "FunctionLibraries/NebulaFlowAudioBanksFunctionLibrary.h"

#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"


UNebulaFlowStartFSMState::UNebulaFlowStartFSMState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNebulaFlowStartFSMState::OnFSMStateEnter_Implementation(const FString & InOption)
{
	Super::OnFSMStateEnter_Implementation(InOption);
	
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GInstance)
	{
		UNebulaFlowAudioBanksFunctionLibrary::LoadDefaultBanks(GInstance);
	}

	UNebulaFlowInputFunctionLibrary::SetupPS4JapaneseInputs(this);

	UNebulaFlowConsoleFunctionLibrary::SetEnableSingleJoycon(bEnableSingleJoycon);

	TriggerTransition(FName("Proceed"));
}

void UNebulaFlowStartFSMState::OnFSMStateExit_Implementation()
{
	Super::OnFSMStateExit_Implementation();	

	UNebulaFlowConsoleFunctionLibrary::SetIsJoyconHorizontal();
}
