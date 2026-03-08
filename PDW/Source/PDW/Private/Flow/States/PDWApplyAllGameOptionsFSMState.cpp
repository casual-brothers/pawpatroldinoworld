// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "Flow/States/PDWApplyAllGameOptionsFSMState.h"
#include "Data/FlowDeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "FunctionLibraries/GameOptionsFunctionLibrary.h"
//#include "FunctionLibraries/PDWOptionsFunctionLibrary.h"

void UPDWApplyAllGameOptionsFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	UGameOptionsFunctionLibrary::LoadAllGameOptions(GetWorld());
	TriggerTransition(UFlowDeveloperSettings::GetProceedTag().GetTagName());
}

