// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PDWInputSubsystem.h"
#include "Data/PDWGameSettings.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowInputFunctionLibrary.h"
#include "Modes/PDWGameplayGameMode.h"
#include "PDWGameInstance.h"

const int32 UPDWInputSubsystem::DEFAULT_GAMEPLAY = 0;
const int32 UPDWInputSubsystem::INTERACTION = 9;
const int32 UPDWInputSubsystem::DEFAULT_MINIGAME = 0;
const int32 UPDWInputSubsystem::DEFAULT_UI = 19;
UPDWInputSubsystem* UPDWInputSubsystem::Get(UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance->GetSubsystem<UPDWInputSubsystem>();
}

void UPDWInputSubsystem::ApplyMappingContextByTag(const FGameplayTag MapTag, const APlayerController* PlayerToApply)
{
	FGameplayTagContainer& TagsToModify = UPDWGameplayFunctionLibrary::IsSecondPlayer(PlayerToApply) ? Get(PlayerToApply->GetWorld())->Player2LastConfig : Get(PlayerToApply->GetWorld())->Player1LastConfig;

	if (!TagsToModify.HasTag(MapTag))
	{
		TagsToModify.Reset();
		TagsToModify.AddTag(MapTag);
	}

	RestoreLastConfiguration(PlayerToApply);
}

void UPDWInputSubsystem::RemoveInputs(const APlayerController* PlayerToApply)
{
	if (PlayerToApply)
	{
		if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerToApply->GetLocalPlayer()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				InputSystem->ClearAllMappings();
			}
		}
	}
}

void UPDWInputSubsystem::ToggleInteractMap(bool Add, const APlayerController* PlayerToApply)
{
	if (PlayerToApply)
	{
		if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerToApply->GetLocalPlayer()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (Add)
				{
					if (UPDWGameplayFunctionLibrary::IsSecondPlayer(PlayerToApply))
					{
						Get(LocalPlayer)->bPlayer2CanInteract = true;
					}
					else
					{
						Get(LocalPlayer)->bPlayer1CanInteract = true;
					}
					InputSystem->AddMappingContext(UPDWGameSettings::GetMappinContextByTag(UPDWGameSettings::GetInteractionTagMap()), INTERACTION);
				}
				else
				{
					if (UPDWGameplayFunctionLibrary::IsSecondPlayer(PlayerToApply))
					{
						Get(LocalPlayer)->bPlayer2CanInteract = false;
					}
					else
					{
						Get(LocalPlayer)->bPlayer1CanInteract = false;
					}
					InputSystem->RemoveMappingContext(UPDWGameSettings::GetMappinContextByTag(UPDWGameSettings::GetInteractionTagMap()));
				}
			}
		}
		UNebulaFlowInputFunctionLibrary::SetupPS4JapaneseInputs(PlayerToApply);
	}	
}

void UPDWInputSubsystem::ToggleUIMap(bool Add, const APlayerController* PlayerToApply)
{
	if (PlayerToApply)
	{
		if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerToApply->GetLocalPlayer()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				InputSystem->ClearAllMappings();
				if (Add)
				{
					InputSystem->AddMappingContext(UPDWGameSettings::GetMappinContextByTag(UPDWGameSettings::GetUIDefaultTagMap()), DEFAULT_UI);
				}
				else
				{
					RestoreLastConfiguration(PlayerToApply);
				}
			}
		}		
		UNebulaFlowInputFunctionLibrary::SetupPS4JapaneseInputs(PlayerToApply);
	}
}

void UPDWInputSubsystem::RestoreLastConfiguration(const APlayerController* PlayerToApply)
{
	if (PlayerToApply)
	{
		if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerToApply->GetLocalPlayer()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				FGameplayTagContainer& TagsToCheck = UPDWGameplayFunctionLibrary::IsSecondPlayer(PlayerToApply) ? Get(LocalPlayer)->Player2LastConfig : Get(LocalPlayer)->Player1LastConfig;

				InputSystem->ClearAllMappings();
				for (FGameplayTag InputTag : TagsToCheck)
				{
					if (UPDWGameSettings::GetDefaultGameplayTagMap() == InputTag)
					{
						InputSystem->AddMappingContext(UPDWGameSettings::GetMappinContextByTag(InputTag), DEFAULT_GAMEPLAY);
					}
					else if (UPDWGameSettings::GetDefaultMinigameTagMap() == InputTag)
					{
						InputSystem->AddMappingContext(UPDWGameSettings::GetMappinContextByTag(UPDWGameSettings::GetDefaultMinigameTagMap()), DEFAULT_MINIGAME);
					}
					else if (UPDWGameSettings::GetUIDefaultTagMap() == InputTag)
					{
						InputSystem->AddMappingContext(UPDWGameSettings::GetMappinContextByTag(UPDWGameSettings::GetUIDefaultTagMap()), DEFAULT_UI);
					}
				}

				if (TagsToCheck.HasTag(UPDWGameSettings::GetDefaultGameplayTagMap()))
				{
					if (UPDWGameplayFunctionLibrary::IsSecondPlayer(PlayerToApply))
					{
						if (Get(LocalPlayer)->bPlayer2CanInteract)
						{
							InputSystem->AddMappingContext(UPDWGameSettings::GetMappinContextByTag(UPDWGameSettings::GetInteractionTagMap()), INTERACTION);
						}
					}
					else
					{
						if (Get(LocalPlayer)->bPlayer1CanInteract)
						{
							InputSystem->AddMappingContext(UPDWGameSettings::GetMappinContextByTag(UPDWGameSettings::GetInteractionTagMap()), INTERACTION);
						}
					}
				}
			}
		}		
		UNebulaFlowInputFunctionLibrary::SetupPS4JapaneseInputs(PlayerToApply);
	}
}

void UPDWInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
#if PLATFORM_SWITCH
	UPDWGameInstance* GameInstance = UPDWGameInstance::Get(this);
	ensure(GameInstance);
	GameInstance->OnSwitchRemapJoyConClosed.AddUniqueDynamic(this, &UPDWInputSubsystem::UpdateInputSwitch);
#endif

}



void UPDWInputSubsystem::UpdateInputSwitch(bool bSingleJoycon)
{
#if PLATFORM_SWITCH
	const APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	ensure(GameMode);
	if (GameMode)
	{
		RestoreLastConfiguration(GameMode->GetPlayerControllerOne());
		RestoreLastConfiguration(GameMode->GetPlayerControllerTwo());
	}	
#endif
}

void UPDWInputSubsystem::Deinitialize()
{	
#if PLATFORM_SWITCH
	UPDWGameInstance* GameInstance = UPDWGameInstance::Get(this);
	ensure(GameInstance);
	GameInstance->OnSwitchRemapJoyConClosed.RemoveAll(this);
#endif
	Super::Deinitialize();
}
