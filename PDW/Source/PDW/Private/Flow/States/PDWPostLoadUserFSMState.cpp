// Fill out your copyright notice in the Description page of Project Settings.


#include "Flow/States/PDWPostLoadUserFSMState.h"

#include "Engine/LocalPlayer.h"
#include "EnhancedInputPlatformSettings.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "FunctionLibraries/NebulaFlowLocaleFunctionLibrary.h"
#include "InputMappingContext.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "PDWGameInstance.h"
#include "UI/NebulaFlowUIConstants.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Data/FlowDeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "Managers/PDWActivitiesManager.h"



void UPDWPostLoadUserFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	ULocalPlayer* LocalPlayer = UGameplayStatics::GetGameInstance(this)->GetFirstGamePlayer();
	if (!ensureMsgf(LocalPlayer, TEXT("Couldn't retrieve LocalPlayer during LoadUser!")))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* EISubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!ensureMsgf(EISubsystem, TEXT("Couldn't retrieve EnhancedInputLocalPlayerSubsystem during LoadUser!")))
	{
		return;
	}

	UEnhancedInputUserSettings* UserSettings = EISubsystem->GetUserSettings();
	UNebulaFlowUIConstants* UIConstants = (UNebulaFlowUIFunctionLibrary::GetUIConstants(GetWorld()));
	if (!ensureMsgf(UIConstants, TEXT("Couldn't retrieve UIConstants during LoadUser!")))
	{
		return;
	}

	const UNebulaFlowNavigationIconsAsset* NavigationDataAsset = UIConstants->GetNavigationIconDataAssets();
	if (!ensureMsgf(NavigationDataAsset, TEXT("Couldn't retrieve NavigationDataAsset during LoadUser!")))
	{
		return;
	}

#if PLATFORM_SWITCH
	UEnhancedInputPlatformData* InputPlatformData = Cast<UEnhancedInputPlatformData>(SwitchData.LoadSynchronous()->GetDefaultObject());
	const TMap<TObjectPtr<const UInputMappingContext>, TObjectPtr<const UInputMappingContext>>& redirects = InputPlatformData->GetMappingContextRedirects();
	for (const TPair<TObjectPtr<const UInputMappingContext>, TObjectPtr<const UInputMappingContext>>& Pair : redirects)
	{
		UserSettings->RegisterInputMappingContext(Pair.Value.Get());
	}
#else
	for (TSoftObjectPtr<UInputMappingContext> GameMapping : NavigationDataAsset->GameMappings)
	{
		UserSettings->RegisterInputMappingContext(GameMapping.LoadSynchronous());
	}
#endif

#if !PLATFORM_WINDOWS && !WITH_EDITOR
	// disable mouse use on start
	UNebulaFlowInputFunctionLibrary::SetMouseInputEnabled(this, false);
	//APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	//if (PlayerController)
	//{
	//	PlayerController->SetInputMode(FInputModeGameOnly());
	//}
#endif

	if (UPDWActivitiesManager::IsActivitiesAvailableOnCurrentPlatform())
	{
		UPDWGameInstance* GameInstance = Cast<UPDWGameInstance>(UGameplayStatics::GetGameInstance(this));
		if (GameInstance)
		{
			GameInstance->InitActivityManager();
		}
	}

	TriggerTransition(UFlowDeveloperSettings::GetProceedTag().GetTagName());
}