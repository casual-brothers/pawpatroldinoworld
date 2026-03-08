#include "FSM/States/NebulaFlowPreWelcomeScreenFSMState.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Internationalization/Internationalization.h"
#include "FunctionLibraries/NebulaFlowLocaleFunctionLibrary.h"

#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "FunctionLibraries/NebulaFlowAudioBanksFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowAudioManager.h"


UNebulaFlowPreWelcomeScreenFSMState::UNebulaFlowPreWelcomeScreenFSMState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNebulaFlowPreWelcomeScreenFSMState::OnFSMStateEnter_Implementation(const FString & InOption)
{
	Super::OnFSMStateEnter_Implementation(InOption);


	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GInstance)
	{
		UNebulaFlowAudioManager* AudioManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(GInstance);
		if (AudioManager)
		{
			UNebulaFlowLocaleFunctionLibrary::InitializeLocale(GInstance);

			UNebulaFlowAudioBanksFunctionLibrary::LoadBanks(AudioManager->FMODMenuVOBanks);
		}
	}
	
	// #DEV <keep english in editor> [#michele.b, 21 January 2026, OnFSMStateEnter_Implementation]
#if WITH_EDITOR
	if (bForceEnglishInEditor)
	{
		FInternationalization::Get().SetCurrentCulture("en");
	}
#endif //WITH_EDITOR


	TriggerTransition(ProceedTag.GetTagName());

}

void UNebulaFlowPreWelcomeScreenFSMState::OnFSMStateExit_Implementation()
{
	Super::OnFSMStateExit_Implementation();
}
