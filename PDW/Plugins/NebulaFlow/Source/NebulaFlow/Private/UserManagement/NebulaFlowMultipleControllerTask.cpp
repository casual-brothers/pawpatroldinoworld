#include "UserManagement/NebulaFlowMultipleControllerTask.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"

void UNebulaFlowMultipleControllerTask::ExecutionImplementation()
{
	ShowDialog();
}

bool UNebulaFlowMultipleControllerTask::IsValidImplementation()
{
	auto* gameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (gameInstance)
	{
		auto currentGameContext = gameInstance->GetCurrentGameContext();
		if (currentGameContext == EGameContext::EIntro || currentGameContext == EGameContext::EWelcomeScreen)
		{
			return false;
		}
	}
	return NextUserId != INDEX_NONE;
}

void UNebulaFlowMultipleControllerTask::ShowDialog()
{
	Dialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, DialogId,
		[=, this](FString inResponse)
	{
	});

	UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GameInstance);

	GameInstance->GetTimerManager().SetTimer(TimeHandler, 
	[=, this](){
		UNebulaFlowUIFunctionLibrary::HideDialog(this, Dialog); 
		Terminate();
		}, 
	DialogDuration, 
	false);
	
}