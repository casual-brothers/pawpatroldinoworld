// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "UI/Widgets/PDWRemappingWidget.h"
#include "Components/ScrollBox.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Data/PDWGameplayStructures.h"
#include "UI/Widgets/PDWRemappingActionButton.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Core/NebulaFlowPlayerInput.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "Data/GameOptionsDeveloperSettings.h"
#include "Data/PDWInputsData.h"
#include "Managers/PDWEventSubsytem.h"
#include "PlayerMappableKeySettings.h"
#include "UI/Widgets/PDWRemappingCategoryTitle.h"
#include "Components/Spacer.h"


void UPDWRemappingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	if (EventSubsystem)
	{
		EventSubsystem->OnOverlayCloseAnimationFinished.AddUniqueDynamic(this, &UPDWRemappingWidget::OnOverlayClosed);
		EventSubsystem->OnOverlayOpenAnimationFinished.AddUniqueDynamic(this, &UPDWRemappingWidget::OnOverlayOpened);
	}
	Setup();
}

void UPDWRemappingWidget::NativeDestruct()
{
	Empty();
	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	if (EventSubsystem)
	{
		EventSubsystem->OnOverlayCloseAnimationFinished.AddUniqueDynamic(this, &UPDWRemappingWidget::OnOverlayClosed);
		EventSubsystem->OnOverlayOpenAnimationFinished.AddUniqueDynamic(this, &UPDWRemappingWidget::OnOverlayOpened);
	}
	Super::NativeDestruct();
}

FReply UPDWRemappingWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!bCanSelectKey)
	{
		return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
	}
	else
	{
		FKey Key = InKeyEvent.GetKey();
		FInputChord InputChord{};
		InputChord.Key = Key;

		OnRemappingButtonNewKeyChosen(RemappingActionButton, InputChord);

		//if (Key.IsGamepadKey() != RemappingActionButton->bIsKeyboardButton)
		//{
		//	OnRemappingButtonNewKeyChosen(RemappingActionButton, InputChord);
		//}
		//else
		//{			
		//	bIsSelectingKey = false;
		//	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(this);
		//	EventSubsystem->OnRemappingButtonNewKeyChosen.Broadcast(nullptr);
		//	bIsUsingGamePad = Key.IsGamepadKey();
		//	bForceUpdate = true;
		//	// #DEV_PDW <add invalid messagebefore closing the overlay?> [#michele.b, 12 November 2025, NativeOnPreviewKeyDown]
		//}
		return FReply::Unhandled();
	}
}

// When clicking to remap an action, We activate the overlay to listen the click (not needed)
void UPDWRemappingWidget::OnRemappingButtonClick(UPDWRemappingActionButton* RemappingButton)
{
	RemappingActionButton = RemappingButton;
	ActionMappingName = RemappingActionButton->StoredKeyMapping->begin()->GetMappingName();
	UNebulaFlowUIFunctionLibrary::SetUserFocus(GetWorld(), ListenKeyButton, GetOwningPlayer());
}

// When chosen the new key we close the overlay
void UPDWRemappingWidget::OnRemappingButtonNewKeyChosen(UPDWRemappingActionButton* RemappingButton, FInputChord InputChord)
{
	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(this);
	bool KeyCanBeSelected = OnKeySelected(InputChord);
	if (KeyCanBeSelected)
	{
		bCanSelectKey = false;
		ChangeBinding(InputChord.Key);

		EventSubsystem->OnRemappingButtonNewKeyChosen.Broadcast(RemappingButton, InputChord.Key);
	}
	else
	{
		EventSubsystem->OnRemappingButtonInvalidNewKeyChosen.Broadcast(RemappingButton);
	}
}

void UPDWRemappingWidget::SetFocusOnRemapButtons()
{
	SetupFocusOnInputTypeChanged(bIsUsingGamePad);
}

void UPDWRemappingWidget::Setup()
{
	if (!GetWorld())
	{
		return;
	}
	if (!ensureMsgf(RemapWidgetClass, TEXT("RemapWidgetClass not defined!")))
	{
		return;
	}
	InputsData = GetDefault<UGameOptionsDeveloperSettings>()->DefaultInputData.LoadSynchronous();
	if (!ensureMsgf(InputsData, TEXT("Couldn't retrieve InputsData while remapping keys!")))
	{
		return;
	}

	Empty();

	GenerateRemapWidgets();

	if (TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		EnhancedInput = Cast<UNebulaFlowEnhancedPlayerInput>(PlayerController->PlayerInput);
	}

	if (!ensureMsgf(EnhancedInput, TEXT("Couldn't retrieve EnhancedInput from PlayerController!")))
	{
		return;
	}

	bIsUsingGamePad = EnhancedInput->IsPlayerUsingGamepad();

	InputHandler = FNebulaFlowCoreDelegates::OnAnyInputReleased.AddUObject(this, &ThisClass::OnAnyKeyPressed);

	SetFocusOnRemapButtons();
}

void UPDWRemappingWidget::Empty()
{
	KeyboardMapping->ClearChildren();
	ControllerMapping->ClearChildren();

	ButtonListKeyboard.Empty();
	ButtonListGamepad.Empty();

	FNebulaFlowCoreDelegates::OnAnyInputReleased.Remove(InputHandler);

	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(this);
	if (EventSubsystem)
	{
		EventSubsystem->OnRemappingButtonClick.RemoveDynamic(this, &ThisClass::OnRemappingButtonClick);
	}
}

void UPDWRemappingWidget::ResetInputs()
{
	bool KeyFound = false;

	// Apply to all user settings
	for (TObjectPtr<UEnhancedInputUserSettings> SingleUserSettings : AllActiveUserSettings)
	{
		for (const TPair<FString, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair : SingleUserSettings->GetAllAvailableKeyProfiles())
		{
			//const FString& ProfileName = ProfilePair.Key;
			const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;
			for (const TPair<FName, FKeyMappingRow>& RowPair : Profile->GetPlayerMappingRows())
			{
				if (RowPair.Value.HasAnyMappings())
				{
					for (auto it = RowPair.Value.Mappings.begin(); it != RowPair.Value.Mappings.end(); ++it)
					{
						KeyFound = true;

						FMapPlayerKeyArgs Args = {};
						Args.MappingName = it->GetMappingName();

						//UE_LOG(LogClass, Log, TEXT("****a %s, key: %s"), *it->GetMappingName().ToString(), *it->GetCurrentKey().ToString());

						FGameplayTagContainer FailureReason;
						SingleUserSettings->ResetAllPlayerKeysInRow(Args, FailureReason);
						SingleUserSettings->ResetKeyProfileIdToDefault(Args.ProfileIdString, FailureReason);
					}
				}
			}
		}
	}


	if (KeyFound)
	{
		for (TObjectPtr<UEnhancedInputUserSettings> SingleUserSettings : AllActiveUserSettings)
		{
			SingleUserSettings->AsyncSaveSettings();
		}
	}

	Setup();

	SetFocusOnRemapButtons();
}

void UPDWRemappingWidget::GenerateRemapWidgets()
{
	LocalPlayer = UGameplayStatics::GetGameInstance(this)->GetFirstGamePlayer();

	AllActiveUserSettings = TArray<TObjectPtr<UEnhancedInputUserSettings>>();

	ensure(LocalPlayer);
	if (LocalPlayer)
	{
		EISubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

		ensure(EISubsystem);
		if (EISubsystem)
		{
			UserSettings = EISubsystem->GetUserSettings();

			ensure(UserSettings);
			if (UserSettings)
			{
				GenerateRemapWidgets(true);
				GenerateRemapWidgets(false);
			}
		}
	}

	TArray<ULocalPlayer*> SingleLocalPlayers = UGameplayStatics::GetGameInstance(this)->GetLocalPlayers();
	for (ULocalPlayer* SingleLocalPlayer : SingleLocalPlayers)
	{
		UEnhancedInputLocalPlayerSubsystem* SingleEISubsystem = SingleLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

		if (SingleEISubsystem)
		{
			AllActiveUserSettings.Add(SingleEISubsystem->GetUserSettings());
		}
	}
}

void UPDWRemappingWidget::GenerateRemapWidgets(bool bIsKeyboard)
{
	for (const TPair<FString, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair : UserSettings->GetAllAvailableKeyProfiles())
	{
		//const FString& ProfileName = ProfilePair.Key;
		const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;

		TArray<FPDWInputMapsToUpdate> MapsInfo = bIsKeyboard ? InputsData->ContextKeyboardMapsInfo : InputsData->ContextConsoleMapsInfo;
		for (int32 MapIndex = 0; MapIndex < MapsInfo.Num(); ++MapIndex)
		{
			if (MapIndex != 0)
			{
				USpacer* Space = NewObject<USpacer>(this, USpacer::StaticClass());
				(bIsKeyboard ? KeyboardMapping : ControllerMapping)->AddChild(Space);
				Space->SetSize(FVector2D(SpaceBetweenCategories));
			}
			FPDWInputMapsToUpdate& MapToUpdate = MapsInfo[MapIndex];
			UPDWRemappingCategoryTitle* RemapWidgetTitle = CreateWidget<UPDWRemappingCategoryTitle>(this, RemapWidgetCategoryTitleClass);
			(bIsKeyboard ? KeyboardMapping : ControllerMapping)->AddChild(RemapWidgetTitle);
			RemapWidgetTitle->InitializeWidget(MapToUpdate.Title);


			for (auto RemappableAction : MapToUpdate.RemappableActions) // Cycle remappable actions
			{
				for (const TPair<FName, FKeyMappingRow>& RowPair : Profile->GetPlayerMappingRows()) // will be only 1 for most of our games
				{
					// Create a setting row for anything with valid mappings and that we haven't created yet
					if (RowPair.Value.HasAnyMappings())
					{
						bool KeyFound = false;

						for (auto it = RowPair.Value.Mappings.begin(); it != RowPair.Value.Mappings.end(); ++it)
						{
							const FKey& Key = it->GetCurrentKey();

							if (it->GetAssociatedInputAction()->GetFName() == RemappableAction->GetFName())
							{
								//RemappableActionsPerContext
								if (!Key.IsGamepadKey() && bIsKeyboard)
								{
									KeyFound = true;
								}
								else if (Key.IsGamepadKey() && !bIsKeyboard)
								{
									KeyFound = true;
								}


								if (KeyFound)
								{
									UPDWRemappingActionButton* RemapWidget = CreateWidget<UPDWRemappingActionButton>(this, RemapWidgetClass);

									(bIsKeyboard ? ButtonListKeyboard : ButtonListGamepad).Add(RemapWidget);

									(bIsKeyboard ? KeyboardMapping : ControllerMapping)->AddChild(RemapWidget);

									RemapWidget->InitializeWidget(RowPair.Value.Mappings, bIsKeyboard, it->GetSlot(), MapToUpdate.Map);
									RemapWidget->SetNavigationRuleBase(EUINavigation::Left, EUINavigationRule::Stop);
									RemapWidget->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Stop);

									RemapWidget->SetPadding(GameOptionsPadding);

									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

bool UPDWRemappingWidget::OnKeySelected(FInputChord InputChord)
{
	// we unable they key select before changing tab
	if ((InputChord.Key.IsGamepadKey() && RemappingActionButton->bIsKeyboardButton) || (!RemappingActionButton->bIsKeyboardButton && !InputChord.Key.IsGamepadKey()))
	{
		return false;
	}

#if PLATFORM_SWITCH
	if (InputsData->RemappableKeysSwitch.Contains(InputChord.Key))
	{
		return true;
	}
#else
	// the key selected is a key that cannot be rebindable
	if (InputsData->RemappableKeys.Contains(InputChord.Key))
	{
		return true;
	}
#endif
	return false;
}

FString UPDWRemappingWidget::FindActionDisplayName(const FName& ActionName)
{
	for (auto Widget : ButtonListKeyboard)
	{
		for (auto it3 = Widget->StoredKeyMapping->begin(); it3 != Widget->StoredKeyMapping->end(); ++it3)
		{
			if (it3->GetMappingName() == ActionName)
			{
				return it3->GetDisplayName().ToString();
			}
		}
	}
	return FString();
}

void UPDWRemappingWidget::SetupFocusOnInputTypeChanged(bool bIsGamepad)
{
	TArray<UWidget*> Children{};

	if (bIsGamepad)
	{
		ControllerMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		KeyboardMapping->SetVisibility(ESlateVisibility::Collapsed);
		Children = ControllerMapping->GetAllChildren();
		ControllerMapping->ScrollToStart();
		KeyboardMapping->ScrollToStart();
	}
	else
	{
		ControllerMapping->SetVisibility(ESlateVisibility::Collapsed);
		KeyboardMapping->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Children = KeyboardMapping->GetAllChildren();
		ControllerMapping->ScrollToStart();
		KeyboardMapping->ScrollToStart();
	}
	for (auto Child : Children)
	{
		if (Cast<UPDWRemappingActionButton>(Child))
		{
			UNebulaFlowUIFunctionLibrary::SetUserFocus(GetWorld(), Child, GetOwningPlayer(), false);
			break;
		}
	}

	bIsUsingGamePad = bIsGamepad;

	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(this);
	if (EventSubsystem)
	{
		EventSubsystem->OnRemappingButtonClick.AddUniqueDynamic(this, &ThisClass::OnRemappingButtonClick);
	}
}

void UPDWRemappingWidget::OnAnyKeyPressed(FKey Key, bool bIsGamepad, APlayerController* Sender)
{
	if (Sender && Sender == GetOwningPlayer())
	{
		if (bIsUsingGamePad != bIsGamepad)
		{
			SetupFocusOnInputTypeChanged(bIsGamepad);
		}
	}
}


void UPDWRemappingWidget::ChangeBinding(FKey InKey)
{
	FKey NewKey = InKey;

	const TSet<FPlayerKeyMapping>* StoredKeyMappingsPtr = RemappingActionButton->StoredKeyMapping;
	FKey OldKey = FKey(); // Initialize to an empty key

	EPlayerMappableKeySlot KeySlot = RemappingActionButton->Slot;
	if (StoredKeyMappingsPtr)
	{
		for (const FPlayerKeyMapping& Mapping : *StoredKeyMappingsPtr)
		{
			if (Mapping.GetSlot() == KeySlot)
			{
				OldKey = Mapping.GetCurrentKey();
				break;
			}
		}
	}

	//ActionsAlreadyMappedWithNewKey.Empty();
	//ActionsAlreadyMappedWithOldKey.Empty();

	ActionsAlreadyMappedWithNewKey.Empty();
	ActionsAlreadyMappedWithOldKey.Empty();
	// Apply to default user settings
	UEnhancedPlayerMappableKeyProfile* DefaultProfile = UserSettings->GetActiveKeyProfile();
	DefaultProfile->GetMappingNamesForKey(NewKey, ActionsAlreadyMappedWithNewKey); // should be always 1 profile if the game is not super complex
	DefaultProfile->GetMappingNamesForKey(OldKey, ActionsAlreadyMappedWithOldKey); // should be always 1 profile if the game is not super complex

	if (RemappingActionButton && RemappingActionButton->IMC)
	{
		FilterOutActionsNotInPairedIMCs(ActionsAlreadyMappedWithNewKey, RemappingActionButton->IMC);
		FilterOutActionsNotInPairedIMCs(ActionsAlreadyMappedWithOldKey, RemappingActionButton->IMC);
	}

	//UserSettings->AsyncSaveSettings();
	for (TObjectPtr<UEnhancedInputUserSettings> ActiveUserSettings : AllActiveUserSettings)
	{
		MapKeys(ActionsAlreadyMappedWithNewKey, OldKey, KeySlot, ActiveUserSettings);
		MapKeys(ActionsAlreadyMappedWithOldKey, NewKey, KeySlot, ActiveUserSettings);

	}

	if (ULocalPlayer* LP = UserSettings->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->RequestRebuildControlMappings();
		}
	}
	UserSettings->AsyncSaveSettings();


	FNebulaFlowCoreDelegates::OnActionRemapped.Broadcast();
}

void UPDWRemappingWidget::OnOverlayClosed()
{
	RemappingActionButton->bIsRemapping = false;

	UNebulaFlowUIFunctionLibrary::SetUserFocus(GetWorld(), RemappingActionButton, GetOwningPlayer());
}

void UPDWRemappingWidget::OnOverlayOpened()
{
	bCanSelectKey = true;
}


void UPDWRemappingWidget::MapKeys(TArray<FName>& Actions, FKey NewKey, EPlayerMappableKeySlot KeySlot, UEnhancedInputUserSettings* EIUS)
{
	for (auto Action : Actions)
	{
		FMapPlayerKeyArgs NewArgs = {};
		NewArgs.MappingName = Action;
		NewArgs.Slot = KeySlot;
		NewArgs.NewKey = NewKey;

		for (auto Button : NewArgs.NewKey.IsGamepadKey() ? ButtonListGamepad : ButtonListKeyboard) // we cycle all items to find the slot where they were holding that key
		{
			bool ActionFound = false;
			for (auto it2 = Button->StoredKeyMapping->begin(); it2 != Button->StoredKeyMapping->end(); ++it2)
			{
				if (it2->GetCurrentKey() == ContendedKey && it2->GetMappingName() == Action)
				{
					NewArgs.Slot = it2->GetSlot();
					ActionFound = true;
					break;
				}
			}
			if (ActionFound)
			{
				break;
			}
		}

		FGameplayTagContainer FailureReason;

		// Apply to currently active user settings
		//for (TObjectPtr<UEnhancedInputUserSettings> ActiveUserSettings : AllActiveUserSettings)
		//{
		//	ActiveUserSettings->MapPlayerKey(NewArgs, FailureReason);
		//}
		EIUS->MapPlayerKey(NewArgs, FailureReason);
		//UserSettings->MapPlayerKey(NewArgs, FailureReason);
	}
}

void UPDWRemappingWidget::FilterOutActionsNotInPairedIMCs(TArray<FName>& Actions, UInputMappingContext* IMC)
{
	TArray<FName> FilteredActions;
	TArray<UInputMappingContext*> IMCs{};
	for (FPDWPairingRemappingIMcs& PairingIMC : InputsData->PairingIMCs)
	{
		if (PairingIMC.IMCs.Contains(IMC))
		{
			IMCs = PairingIMC.IMCs;
			break;
		}
	}
	if (IMCs.IsEmpty())
	{
		IMCs.Add(IMC);
	}

	for (UInputMappingContext* CurrentIMC : IMCs)
	{
		const TArray<FEnhancedActionKeyMapping>& IMCMappings = CurrentIMC->GetMappings();
		for (FName ActionName : Actions)
		{
			bool bFoundInThisIMC = false;

			for (const FEnhancedActionKeyMapping& Mapping : IMCMappings)
			{
				if (Mapping.Action && Mapping.Action->GetPlayerMappableKeySettings())
				{
					auto IMCAction = Mapping.Action->GetPlayerMappableKeySettings();
					if (IMCAction->GetMappingName() == ActionName)
					{
						bFoundInThisIMC = true;
						break;
					}
				}
			}

			if (bFoundInThisIMC)
			{
				if (!FilteredActions.Contains(ActionName))
				{
					FilteredActions.Add(ActionName);
				}
			}
		}

	}

	Actions = FilteredActions;
}
