#include "ErrorsManagement/NebulaFlowDefaultErrorInstance.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Core/NebulaFlowLocalPlayer.h"



const FName UNebulaFlowDefaultErrorInstance::DEFAULT_SYSTEM_DIALOG_ID = FName("DEFAULT_SYSTEM_DIALOG");


UNebulaFlowDefaultErrorInstance::UNebulaFlowDefaultErrorInstance(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
	
{
	bShouldTick = true;
	ErrorInstanceType = EErrorInstanceType::EDefault ;
	SystemDialogID = DEFAULT_SYSTEM_DIALOG_ID;
}


void UNebulaFlowDefaultErrorInstance::StartManagingError()
{
	Super::StartManagingError();
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

void UNebulaFlowDefaultErrorInstance::OnDialogResponse(FString Response)
{
	bErrorConditionValid = false;
	UNebulaFlowUIFunctionLibrary::HideDialog(this,SystemDialog);
	ErrorManagerRef->ResolveError(this);
}
