#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"

void UNebulaFlowCoreFunctionLibrary::TriggerAction(const UObject* WorldContextObject, const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	APlayerController* Sender = ControllerSender != nullptr ? ControllerSender : UGameplayStatics::GetPlayerController(WorldContextObject,0);
	
	FNebulaFlowCoreDelegates::OnActionTriggered.Broadcast(Action, Parameter, Sender);
}

void UNebulaFlowCoreFunctionLibrary::PlayerControllerGetLocalPlayer(APlayerController* PlayerController, bool& Success, ULocalPlayer*& LocalPlayer)
{
	Success = false;
	LocalPlayer = NULL;
	if (PlayerController == nullptr)
	{
		return;
	}
	if (PlayerController->Player == nullptr)
	{
		return;
	}
	if (!PlayerController->Player->GetClass()->IsChildOf(ULocalPlayer::StaticClass()))
	{
		return;
	}
	LocalPlayer = (ULocalPlayer*)PlayerController->Player;
	if (LocalPlayer == nullptr)
	{
		return;
	}
	Success = true;
}

UNebulaFlowLocalPlayer* UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(const UObject* WorldContextObject)
{
	auto* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (GameInstance)
	{
		auto* UserSubSystem = GameInstance->GetSubsystem<UNebulaFlowUserSubSystem>();
		if (UserSubSystem)
		{
			return UserSubSystem->GetPlayerOwner();
		}
	}
	return nullptr;
}

AActor* UNebulaFlowCoreFunctionLibrary::GetFirstActorOfClassByTag(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FName Tag)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(WorldContextObject,ActorClass,Tag,FoundActors);
	if(FoundActors.Num()>0)
	{
		return FoundActors[0];
	}
	return nullptr;
}

void UNebulaFlowCoreFunctionLibrary::LoadLevelByName(const UObject* WorldContextObject, FName LevelName, bool bAbsolute /*= true*/, FString Options /*= FString(TEXT(""))*/)
{
	UNebulaFlowGameInstance* Ginstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (Ginstance && !Ginstance->bIsLoadingLevel)
	{
		FNebulaFlowCoreDelegates::OnLevelLoadStart.Broadcast(LevelName);
		UGameplayStatics::OpenLevel(Ginstance, LevelName, bAbsolute, Options);
	}
}

void UNebulaFlowCoreFunctionLibrary::SetSystemPaused(const UObject* WorldContextObject, bool bIsPaused)
{
	UNebulaFlowGameInstance* Ginstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (Ginstance)
	{
		FNebulaFlowCoreDelegates::OnSystemPaused.Broadcast(bIsPaused);
		return Ginstance->SetSystemPaused(bIsPaused);
	}
}

void UNebulaFlowCoreFunctionLibrary::SetGamePaused(const UObject* WorldContextObject, bool bIsPaused)
{
	UNebulaFlowGameInstance* Ginstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (Ginstance)
	{
		FNebulaFlowCoreDelegates::OnGamePaused.Broadcast(bIsPaused);
		return Ginstance->SetGamePaused(bIsPaused);
	}
}

void UNebulaFlowCoreFunctionLibrary::SetNewFSMEntryPointLabel(const UObject* WorldContextObject, FName inNewLable)
{
	UNebulaFlowFSMManager* Mgr =  UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowFSMManager(WorldContextObject);
	if (Mgr)
	{
		Mgr->SetCurrentFSMLabel(inNewLable);
	}
}

IOnlineSubsystem* UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem()
{
#if (PLATFORM_WINDOWS && !PLATFORM_WINGDK) || PLATFORM_SWITCH
	const auto onlineSub = IOnlineSubsystem::Get();
#else
	const auto onlineSub = IOnlineSubsystem::GetByPlatform(true);
#endif
	check(onlineSub);
	return onlineSub;
}

IOnlineIdentityPtr UNebulaFlowCoreFunctionLibrary::GetOnlineIdentity()
{
	const auto onlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
	const IOnlineIdentityPtr& identityInterface = onlineSub->GetIdentityInterface();
	check(identityInterface.IsValid());
	return identityInterface;
}

void UNebulaFlowCoreFunctionLibrary::Internal_LoadLevelByName(UObject* WorldContextObject, FName LevelName, bool bAbsolute /*= true*/, FString Options /*= FString(TEXT(""))*/)
{

}
