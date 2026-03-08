#include "ErrorsManagement/NebulaFlowProfileChangedErrorInstance.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "ErrorsManagement/NebulaFlowBaseErrorInstance.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"



const FName UNebulaFlowProfileChangedErrorInstance::PROFILE_CHANGED_DIALOG_ID = FName("PROFILE_CHANGED_DIALOG");


const FName UNebulaFlowProfileChangedErrorInstance::FROM_PROFILE_CHANGED_LABEL = FName("FromProfileChanged") ;

UNebulaFlowProfileChangedErrorInstance::UNebulaFlowProfileChangedErrorInstance(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
	
{
	bShouldTick = true;
	ErrorInstanceType = EErrorInstanceType::EPlayerProfileChanged;
	SystemDialogID = PROFILE_CHANGED_DIALOG_ID;
}


void UNebulaFlowProfileChangedErrorInstance::StartManagingError()
{
	Super::StartManagingError();
	UGameplayStatics::SetGamePaused(this,true);
	SystemDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this,SystemDialogID,
		[=, this](FString inResponse)
	{
		OnDialogResponse(inResponse);
	}
	,PlayerOwnerRef->PlayerController);
	if (SystemDialog && !ErrorText.IsEmpty())
	{
		SystemDialog->SetDialogText(ErrorText);
	}
	
}

void UNebulaFlowProfileChangedErrorInstance::OnDialogResponse(FString Response)
{
	bErrorConditionValid = false;
	UNebulaFlowUIFunctionLibrary::HideDialog(this,SystemDialog);
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	ensureMsgf(GInstance && GInstance->WelcomeScreenMapName!=NAME_None,TEXT("WelcomeScreenMap Name not defined in GameInstance!"));
	UGameplayStatics::SetGamePaused(this, false);
	if(GInstance && GInstance->WelcomeScreenMapName!=NAME_None)
	{ 
		UNebulaFlowFSMManager* FSMManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowFSMManager(this);
		if (FSMManager)
		{
			FSMManager->SetCurrentFSMLabel(FROM_PROFILE_CHANGED_LABEL);
		}
		UNebulaFlowCoreFunctionLibrary::LoadLevelByName(this,GInstance->WelcomeScreenMapName);
	}
	ErrorManagerRef->ResolveError(this);
}
