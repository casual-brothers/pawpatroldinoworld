#include "UserManagement/NebulaFlowAppResumeTask.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Online/CoreOnline.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "UI/NebulaFlowDialog.h"
#include "Engine/Player.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowFSMManager.h"

void UNebulaFlowAppResumeTask::InitializeTask()
{

}

void UNebulaFlowAppResumeTask::ExecutionImplementation()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	check (GameInstance);
	UNebulaFlowUserSubSystem* UserSubsystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
	check(UserSubsystem);
	for (int32 i = 0; i < GameInstance->GetNumLocalPlayers(); ++i)
	{
		if (GameInstance->GetLocalPlayerByIndex(i)->GetCachedUniqueNetId().IsValid() && 
		UserSubsystem->LocalPlayerOnlineStatus[i] == ELoginStatus::LoggedIn && !UserSubsystem->IsLocalPlayerOnline(GameInstance->GetLocalPlayerByIndex(i)))
		{
			bool bIsFirst = true;
			if (UserSubsystem->GetPlayerOwner() && UserSubsystem->GetPlayerOwner()->GetLocalPlayerIndex() != i)
			{
				bIsFirst = false;
			}
			ShowDialog(bIsFirst);
			return;
		}
	}
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
	Terminate();
}

bool UNebulaFlowAppResumeTask::IsValidImplementation()
{
	auto* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	check (GameInstance);
	auto CurrentGameContext = GameInstance->GetCurrentGameContext();
	return !(CurrentGameContext == EGameContext::EIntro || CurrentGameContext == EGameContext::EWelcomeScreen);
}

void UNebulaFlowAppResumeTask::ShowDialog(bool bIsFirstPlayer)
{
	UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	check(GameInstance);

	const FName& DefDialogId = bIsFirstPlayer ? DialogId : SecondPlayerDisconnectDialogId;

	SystemDialog = UNebulaFlowUIFunctionLibrary::ShowDialog(this, DefDialogId,
		[=, this](FString inResponse)
	{
		OnDialogResponse(inResponse);
	}
	, nullptr);

	if (SystemDialog)
	{		
		UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, true);
	}
}

void UNebulaFlowAppResumeTask::OnDialogResponse(const FString& inResponse)
{
	UNebulaFlowCoreFunctionLibrary::SetSystemPaused(this, false);
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	ensureMsgf(GInstance && GInstance->WelcomeScreenMapName != NAME_None, TEXT("WelcomeScreenMap Name not defined in GameInstance!"));
	if (GInstance && GInstance->WelcomeScreenMapName != NAME_None)
	{
		UNebulaFlowFSMManager* FSMManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowFSMManager(this);
		if (FSMManager)
		{
			FSMManager->SetCurrentFSMLabel(ReturnFsmLabel);
		}
		UNebulaFlowCoreFunctionLibrary::LoadLevelByName(this, GInstance->WelcomeScreenMapName);
	}
	Terminate();
}

