#include "Flow/States/PDWStartFSMState.h"

#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"
#include "FunctionLibraries/NebulaGraphicsVideoSettingsFunctionLibrary.h"
#include "GameFramework/GameUserSettings.h"



void UPDWStartFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	UNebulaFlowConsoleFunctionLibrary::SetEnableSingleJoycon(bEnableSingleJoycon);
	
	UNebulaFlowConsoleFunctionLibrary::SetSwitchMaxControllers(MaxSwitchPlayers);
	
	FIntPoint DesktopResolution = {};
	UNebulaGraphicsVideoSettingsFunctionLibrary::GetDesktopResolution(DesktopResolution);
	UNebulaGraphicsVideoSettingsFunctionLibrary::ApplyScreenResolution(DesktopResolution.X, DesktopResolution.Y);

	TriggerTransition(UFlowDeveloperSettings::GetProceedTag().GetTagName());
}
