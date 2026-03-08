// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWFlowNode_Checkpoint.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Data/PDWLocalPlayer.h"
#include "Data/PDWPersistentUser.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "FlowSave.h"
#include "FlowSubsystem.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDWFlowNode_Checkpoint)

UPDWFlowNode_Checkpoint::UPDWFlowNode_Checkpoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Graph");
#endif
}

void UPDWFlowNode_Checkpoint::ExecuteInput(const FName& PinName)
{
	UPDWDataFunctionLibrary::SaveGame(GetWorld());
	TriggerFirstOutput(true);
}

void UPDWFlowNode_Checkpoint::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	TriggerFirstOutput(true);
}
