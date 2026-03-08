// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/PDWMiniGameFunctionLibrary.h"
#include "Data/FlowDeveloperSettings.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"

bool UPDWMiniGameFunctionLibrary::OnMinigameComplete(const FMinigameFlowEventSignature& inEventSignature)
{
	UNebulaFlowCoreFunctionLibrary::TriggerAction(inEventSignature.MiniGameComponent,UFlowDeveloperSettings::GetMinigameSucceedTag().ToString(),"");
	inEventSignature.MiniGameComponent->MiniGameComplete();
	return true;
}

bool UPDWMiniGameFunctionLibrary::OnMiniGameReset(const FMinigameFlowEventSignature& inEventSignature)
{
	float a = 0.0f;
	return true;
}

bool UPDWMiniGameFunctionLibrary::OnMiniGameInitializationComplete(const FMinigameFlowEventSignature& inEventSignature)
{
	UNebulaFlowCoreFunctionLibrary::TriggerAction(inEventSignature.MiniGameComponent,UFlowDeveloperSettings::GetMinigameSucceedTag().ToString(),"");
	inEventSignature.MiniGameComponent->TriggerBehaviours();
	return true;
}

bool UPDWMiniGameFunctionLibrary::OnMinigameLeft(const FMinigameFlowEventSignature& inEventSignature)
{
	UNebulaFlowCoreFunctionLibrary::TriggerAction(inEventSignature.MiniGameComponent,UFlowDeveloperSettings::GetMinigameCompletedTag().ToString(),"");
	inEventSignature.MiniGameComponent->MiniGameLeft();
	return true;
}

bool UPDWMiniGameFunctionLibrary::OnMinigameSkip(const FMinigameFlowEventSignature& inEventSignature)
{
	//UNebulaFlowCoreFunctionLibrary::TriggerAction(inEventSignature.MiniGameComponent, UFlowDeveloperSettings::GetMinigameSucceedTag().ToString(), "");
	//inEventSignature.MiniGameComponent->MiniGameComplete();
	return true;
}
