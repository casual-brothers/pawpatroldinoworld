

#include "ErrorsManagement/NebulaFlowBaseErrorInstance.h"
#include "NebulaFlow.h"
#include "UI/NebulaFlowDialog.h"
#include "Kismet/GameplayStatics.h"

UNebulaFlowBaseErrorInstance::UNebulaFlowBaseErrorInstance(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

}

bool UNebulaFlowBaseErrorInstance::IsErrorConditionValid()
{
	return bErrorConditionValid;
}

void UNebulaFlowBaseErrorInstance::SetErrorText(FText inErrorText)
{
	ErrorText = inErrorText;
	if (SystemDialog)
	{
		SystemDialog->SetDialogText(ErrorText);
	}
}

UWorld* UNebulaFlowBaseErrorInstance::GetWorld() const
{
	if (ErrorManagerRef)
	{
		return ErrorManagerRef->GetWorld();
	}
	return nullptr;
}

void UNebulaFlowBaseErrorInstance::TickErrorInstance(float DeltaTime)
{
	if (!bShouldTick)
	{
		return;
	}
	
	if (!IsErrorConditionValid() && ErrorManagerRef)
	{
		bShouldTick = false;
		ErrorManagerRef->ResolveError(this);
	}
}

void UNebulaFlowBaseErrorInstance::InitErrorInstance(UNebulaFlowErrorManager* InErrorManager, UNebulaFlowLocalPlayer* InLocalPlayer, FName inDialogID)
{
	ErrorManagerRef = InErrorManager;
	PlayerOwnerRef = InLocalPlayer;
	if (inDialogID != NAME_None)
	{
		SystemDialogID = inDialogID;
	}
}

void UNebulaFlowBaseErrorInstance::UninitErrorInstance()
{
	bShouldTick = false;
}

void UNebulaFlowBaseErrorInstance::StartManagingError()
{
	
}

