// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/NebulaFlowNavButtonBase.h"
#include "Components/TextBlock.h"
#include "UI/Widgets/NebulaFlowNavigationIcon.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowPlayerController.h"
#include "Components/Button.h"
#include "Engine/LocalPlayer.h"
#include "Core/NebulaFlowPlayerInput.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"
#include "EnhancedInputComponent.h"
#include "Core/NebulaFlowEnhancedPlayerInput.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "InputTriggers.h"
#include "InputTriggers/InputTriggerHoldWithDecay.h"
#include "FunctionLibraries/NebulaFlowInputFunctionLibrary.h"
#include "GameFramework/GameModeBase.h"

void UNebulaFlowNavButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	#if WITH_EDITOR
	if (ButtonText)
	{
		ButtonText->SetText(ButtonDisplayText);
	}
	if (ProgressBarPanel)
	{
		ProgressBarPanel->SetVisibility(bShowProgressBarPanel ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	#endif
}

void UNebulaFlowNavButtonBase::NativeConstruct()
{
	Super::NativeConstruct();
	if (ProgressBarPanel)
	{
		ProgressBarPanel->SetVisibility(ESlateVisibility::Collapsed);
		if (CircularProgress)
		{
			if (ensureMsgf(CircularProgress->GetDynamicMaterial(), TEXT("Need Material for the Circular Progress Functionality")))
			{
				CircularProgress->GetDynamicMaterial()->SetScalarParameterValue(ValueParameterName, 0.f);
			}
		}
	}

	if (NavbarButtonData.ButtonInputAction || (!NavbarButtonData.ButtonAction.IsNone()))
	{
		InitializeButton(NavbarButtonData);
	}
	
	FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &UNebulaFlowNavButtonBase::OnPlayerLogin);
	FGameModeEvents::GameModeLogoutEvent.AddUObject(this, &UNebulaFlowNavButtonBase::OnPlayerLogout);
}

void UNebulaFlowNavButtonBase::InitializeButton(FNavbarButtonData NewButtonData)
{
	NavbarButtonData = NewButtonData;

	if (ButtonText)
	{
		ButtonText->SetText(NavbarButtonData.ButtonText);
		ButtonText->SetColorAndOpacity(NavbarButtonData.ButtonTextColor);
	}
	if (NavigationIcon)
	{
		NavigationIcon->SetOwningPlayer(GetOwningPlayer());
		if (UNebulaFlowInputFunctionLibrary::IsUsingEnhancedInput(this) && NavbarButtonData.ButtonInputAction)
		{
			NavigationIcon->InitNavigationIcon(NavbarButtonData.ButtonInputAction, NavbarButtonData.bIsAxis, NavbarButtonData.AxisScale);
		}
		else
		{ 
			NavigationIcon->InitNavigationIcon(NavbarButtonData.ButtonAction, NavbarButtonData.bIsAxis, NavbarButtonData.AxisScale);
		}
	}
	
	if (CircularProgress)
	{
		if (CircularProgress->GetDynamicMaterial())
		{
			CircularProgress->GetDynamicMaterial()->SetScalarParameterValue(ValueParameterName, 0.f);
		}
	}
	
	if (NavbarButtonData.bListenAction && NavbarButtonData.ButtonInputAction)
	{		
		if(GetOwningLocalPlayer() && GetWorld())
		{ 
			if(UNebulaFlowInputFunctionLibrary::IsUsingEnhancedInput(this))
			{
				bIsOneShot = false;
							
				//Trigger Time Base Supported are Hold and HoldWithDecay
				for (UInputTrigger* Trigger : NavbarButtonData.ButtonInputAction->Triggers)
				{
					if (UInputTriggerHoldWithDecay* HoldDecayTrigger = Cast<UInputTriggerHoldWithDecay>(Trigger))
					{
						bIsOneShot = HoldDecayTrigger->GetIsOneShot();
						bIsTimeTriggerEvent = true;
					}
					if (UInputTriggerHold* HoldTrigger = Cast<UInputTriggerHold>(Trigger))
					{
						bIsOneShot = HoldTrigger->bIsOneShot;
						bIsTimeTriggerEvent = true;
					}
					if (bIsTimeTriggerEvent)
					{
						if (ensureMsgf(ProgressBarPanel && CircularProgress && CircularProgress->GetDynamicMaterial(), TEXT("Is a time trigger event but Progress Bar is missing")))
						{
							ProgressBarPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
						}
						break;
					}
				}

				TArray<ANebulaFlowPlayerController*> Players {};
				if (!NavbarButtonData.bListenAnyPlayer)
				{
					Players.Add(Cast<ANebulaFlowPlayerController>(GetOwningLocalPlayer()->GetPlayerController(GetWorld())));
				}
				else
				{
					UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
					int32 NumOfPlayers = GI->GetNumLocalPlayers();
					for (int32 i = 0; i < NumOfPlayers; i++)
					{
						Players.Add(Cast<ANebulaFlowPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), i)));
					}
				}

				for (ANebulaFlowPlayerController* PController : Players)
				{
					UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PController->InputComponent);
					if(EnhancedInputComponent)
					{
						EnhancedInputComponent->ClearBindingsForObject(this);
						EnhancedInputComponent->BindAction(NavbarButtonData.ButtonInputAction, ETriggerEvent::Triggered, this, &UNebulaFlowNavButtonBase::OnInputTriggered, PController);
						EnhancedInputComponent->BindAction(NavbarButtonData.ButtonInputAction, ETriggerEvent::Started, this, &UNebulaFlowNavButtonBase::OnInputStarted, PController);
						EnhancedInputComponent->BindAction(NavbarButtonData.ButtonInputAction, ETriggerEvent::Completed, this, &UNebulaFlowNavButtonBase::OnInputCompleted, PController);
						EnhancedInputComponent->BindAction(NavbarButtonData.ButtonInputAction, ETriggerEvent::Canceled, this, &UNebulaFlowNavButtonBase::OnInputCancelled, PController);
						if(bIsTimeTriggerEvent) EnhancedInputComponent->BindAction(NavbarButtonData.ButtonInputAction, ETriggerEvent::Ongoing, this, &UNebulaFlowNavButtonBase::OnInputGoing, PController);
					}
				}
			}
			else   //DEPRECATED 
			{
				if (!NavbarButtonData.bListenAnyPlayer)
				{
					const ANebulaFlowPlayerController* PController = Cast<ANebulaFlowPlayerController>(GetOwningLocalPlayer()->GetPlayerController(GetWorld()));
					FInputActionBinding& BindPress = PController->InputComponent->BindAction(NavbarButtonData.ButtonAction,EInputEvent::IE_Pressed,this,&UNebulaFlowNavButtonBase::OnInputPressReceived);
					BindPress.bExecuteWhenPaused=true;		
					BindPress.bConsumeInput = NavbarButtonData.bConsumeInput;
					FInputActionBinding& BindRelease = PController->InputComponent->BindAction(NavbarButtonData.ButtonAction, EInputEvent::IE_Released, this, &UNebulaFlowNavButtonBase::OnInputReleaseReceived);
					BindRelease.bExecuteWhenPaused = true;
					BindRelease.bConsumeInput = NavbarButtonData.bConsumeInput;
					PressHandle = BindPress.GetHandle();
					ReleaseHandle = BindRelease.GetHandle();
				}
				else
				{
					if (GetWorld())
					{
						for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
						{
							APlayerController* currController = Cast<APlayerController>(*iter);
							if (currController)
							{
								PressedMap.Add(currController,false);
							}		
						}
					}
					InputReleasedActionHandler = FNebulaFlowCoreDelegates::OnAnyInputReleased.AddUObject(this, &UNebulaFlowNavButtonBase::OnAnyKeyReleased);
					InputPressActionHandler = FNebulaFlowCoreDelegates::OnAnyInputPressed.AddUObject(this,&UNebulaFlowNavButtonBase::OnAnyKeyPressed);
				}
			}

			if (ClickableButton)
			{
				if (!ClickableButton->OnClicked.Contains(this, FName("OnClick")))
				{
					ClickableButton->OnClicked.AddDynamic(this, &UNebulaFlowNavButtonBase::OnClick);
				}
			}
		}
	}
}

void UNebulaFlowNavButtonBase::UninitializeButton()
{
	if (ClickableButton)
	{
		ClickableButton->OnClicked.RemoveDynamic(this, &UNebulaFlowNavButtonBase::OnClick);
	}
	if (NavbarButtonData.bListenAction && NavbarButtonData.bListenAnyPlayer)
	{
		FNebulaFlowCoreDelegates::OnAnyInputReleased.Remove(InputReleasedActionHandler);
		FNebulaFlowCoreDelegates::OnAnyInputPressed.Remove(InputPressActionHandler);
	}
	if (!NavbarButtonData.bListenAnyPlayer)
	{
		const ANebulaFlowPlayerController* PController = Cast<ANebulaFlowPlayerController>(GetOwningLocalPlayer()->GetPlayerController(GetWorld()));
		if (PController)
		{
			UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PController->InputComponent);
			if (EnhancedInputComp)
			{
				EnhancedInputComp->ClearBindingsForObject(this);
			}

			//For Old System
			if(PressHandle) PController->InputComponent->RemoveActionBindingForHandle(PressHandle);
			if(ReleaseHandle) PController->InputComponent->RemoveActionBindingForHandle(ReleaseHandle);			
		}
	}
	FGameModeEvents::GameModePostLoginEvent.RemoveAll(this);
	FGameModeEvents::GameModeLogoutEvent.RemoveAll(this);
}

void UNebulaFlowNavButtonBase::NativeDestruct()
{
	UninitializeButton();
	Super::NativeDestruct();
}

void UNebulaFlowNavButtonBase::OnClick()
{
	ANebulaFlowPlayerController* PController = Cast<ANebulaFlowPlayerController>(GetOwningLocalPlayer()->GetPlayerController(GetWorld()));
	if (bIsNavbarButtonEnabled)
	{
		if (NavbarButtonData.UISound != NAME_None)
		{
			UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), NavbarButtonData.UISound);
		}
		OnButtonTriggered();
	}
	InputPressed = false;
}

void UNebulaFlowNavButtonBase::OnInputTriggered(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (!bIsNavbarButtonEnabled)
	{
		return;
	}
	
	InputTriggerSender = inPC;

	if (NavbarButtonData.UISound != NAME_None)
	{
		UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), NavbarButtonData.UISound);
	}

	if (bIsTimeTriggerEvent)
	{
		CircularProgress->GetDynamicMaterial()->SetScalarParameterValue(ValueParameterName, 1.f);
	}

	OnButtonTriggered();
}

void UNebulaFlowNavButtonBase::OnInputStarted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (bIsNavbarButtonEnabled && OnStartedAction && !bIsTimeTriggerEvent)
	{
		PlayAnimation(OnStartedAction);
	}
}

void UNebulaFlowNavButtonBase::OnInputGoing(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (bIsNavbarButtonEnabled && bIsTimeTriggerEvent)
	{
		UMaterialInstanceDynamic* MaterialInstance = CircularProgress->GetDynamicMaterial();
		
		float Duration = 0.f;
		float MaxHoldTimeInput = 0.f;

		for (UInputTrigger* Trigger : Instance.GetTriggers())
		{
			if (UInputTriggerHoldWithDecay* HoldDecayTrigger = Cast<UInputTriggerHoldWithDecay>(Trigger))
			{
				Duration = HoldDecayTrigger->GetProgression();
				MaxHoldTimeInput = HoldDecayTrigger->GetHoldTimeThreshold();
			}
			if (UInputTriggerHold* HoldTrigger = Cast<UInputTriggerHold>(Trigger))
			{
				Duration = Instance.GetElapsedTime();
				MaxHoldTimeInput = HoldTrigger->HoldTimeThreshold;
			}
		}

		CurrentTime = FMath::Clamp(Duration / MaxHoldTimeInput, 0.f, 1.f);

		MaterialInstance->SetScalarParameterValue(ValueParameterName, CurrentTime);
	}
}

void UNebulaFlowNavButtonBase::OnInputCompleted(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (!bIsNavbarButtonEnabled)
	{
		return;
	}

	if (!bIsOneShot && bIsTimeTriggerEvent)
	{
		UMaterialInstanceDynamic* MaterialInstance = CircularProgress->GetDynamicMaterial();
		MaterialInstance->SetScalarParameterValue(ValueParameterName, 0.f);
	}

	if (OnCompleteAction && !bIsTimeTriggerEvent)
	{
		PlayAnimation(OnCompleteAction);
	}
}

void UNebulaFlowNavButtonBase::OnInputCancelled(const FInputActionInstance& Instance, ANebulaFlowPlayerController* inPC)
{
	if (!bIsNavbarButtonEnabled)
	{
		return;
	}

	if (bIsTimeTriggerEvent)
	{
		UMaterialInstanceDynamic* MaterialInstance = CircularProgress->GetDynamicMaterial();
		MaterialInstance->SetScalarParameterValue(ValueParameterName, 0.f);
	}

	if (OnCompleteAction && !bIsTimeTriggerEvent)
	{
		PlayAnimation(OnCompleteAction);
	}
}

void UNebulaFlowNavButtonBase::OnPlayerLogin(AGameModeBase* InGameMode, APlayerController* InPlayerController)
{
	InitializeButton(NavbarButtonData);
}

void UNebulaFlowNavButtonBase::OnPlayerLogout(AGameModeBase* InGameMode, AController* InController)
{
	InitializeButton(NavbarButtonData);
}

void UNebulaFlowNavButtonBase::OnButtonTriggered()
{
	BP_OnButtonTrigger();
}

void UNebulaFlowNavButtonBase::ChangeButtonEnabled(bool bActivate)
{
	if (bIsNavbarButtonEnabled != bActivate)
	{
		bIsNavbarButtonEnabled = bActivate;
		BP_OnButtonEnabledChanged(bActivate);
	}
}

#pragma region Deprecated
void UNebulaFlowNavButtonBase::OnInputPressReceived()
{
	if (bIsNavbarButtonEnabled)
	{
		InputPressed = true;
	}
}

void UNebulaFlowNavButtonBase::OnInputReleaseReceived()
{
	ANebulaFlowPlayerController* PController = Cast<ANebulaFlowPlayerController>(GetOwningLocalPlayer()->GetPlayerController(GetWorld()));
	if (bIsNavbarButtonEnabled && InputPressed)
	{
		if (NavbarButtonData.UISound != NAME_None)
		{
			UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), NavbarButtonData.UISound);
		}
		OnButtonTriggered();
	}
	InputPressed = false;
}

void UNebulaFlowNavButtonBase::OnAnyKeyPressed(FKey Key, bool bIsGamepad, APlayerController* Sender)
{
	if (Sender && bIsNavbarButtonEnabled)
	{
		const UNebulaFlowPlayerInput* PInput = Cast<UNebulaFlowPlayerInput>(Sender->PlayerInput); //DEPRECATED
		const UNebulaFlowEnhancedPlayerInput* PEnhancedInput = Cast<UNebulaFlowEnhancedPlayerInput>(Sender->PlayerInput);
		if (((PInput && PInput->IsKeyBoundToAction(Key, NavbarButtonData.ButtonAction))|| (PEnhancedInput && PEnhancedInput->IsKeyBoundToAction(Key,NavbarButtonData.ButtonInputAction))) && PressedMap.Contains(Sender))
		{
			PressedMap[Sender] = true;
		}
	}
}

void UNebulaFlowNavButtonBase::OnAnyKeyReleased(FKey Key, bool bIsGamepad, APlayerController* Sender)
{
	if (Sender && bIsNavbarButtonEnabled && PressedMap.Contains(Sender))
	{
		const UNebulaFlowPlayerInput* PInput = Cast<UNebulaFlowPlayerInput>(Sender->PlayerInput);	//DEPRECATED
		const UNebulaFlowEnhancedPlayerInput* PEnhancedInput = Cast<UNebulaFlowEnhancedPlayerInput>(Sender->PlayerInput);
		if (((PInput && PInput->IsKeyBoundToAction(Key, NavbarButtonData.ButtonAction)) || (PEnhancedInput && PEnhancedInput->IsKeyBoundToAction(Key, NavbarButtonData.ButtonInputAction))) && PressedMap[Sender])
		{
			if (NavbarButtonData.UISound != NAME_None && Sender == GetOwningPlayer())
			{
				UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(),NavbarButtonData.UISound);
			}
			OnButtonTriggered();
		}
		PressedMap[Sender]=false;
	}
}
#pragma endregion
