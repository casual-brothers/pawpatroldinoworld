// Copyright Epic Games, Inc. All Rights Reserved.
#include "UI/Widgets/NebulaFlowNavigationIcon.h"
#include "GameFramework/InputSettings.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Framework/Application/SlateApplication.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowPlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "NebulaFlow.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "UI/NebulaFlowNavbarDataStructures.h"
#include "UI/NebulaFlowUIConstants.h"
#include "EnhancedInputSubsystems.h"
#include "Core/NebulaFlowEnhancedPlayerInput.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "PlayerMappableKeySettings.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"

//#pragma optimize("",off)

UNebulaFlowNavigationIcon::UNebulaFlowNavigationIcon(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	
}



void UNebulaFlowNavigationIcon::InitNavigationIcon(FName InActionName, bool bInIsAxis /*= false*/, float InScale /*= 0.0f*/)
{
	if(!UIConstants)
	{
		UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(this);
	}

	ActionName = InActionName;
	bIsAxis = bInIsAxis;
	AxisScale = InScale;

	UpdateImage();
}

void UNebulaFlowNavigationIcon::InitNavigationIcon(const UInputAction* InAction, bool bInIsAxis /*= false*/, float InScale /*= 0.0f*/ , FName InOverridedActionName)
{
	OverridedActionName = InOverridedActionName;
	
	ensure(InAction);	
	if(InAction)
	{
		InitNavigationIcon(FName(InAction->GetName()),bInIsAxis,InScale);
	}
}

void UNebulaFlowNavigationIcon::NativeConstruct()
{
	if (!bFirstSynchronize)
	{
		bFirstSynchronize = true;

		if (NavigationText && NavigationImage && NavigationOverlay)
		{
			NavigationText->SetVisibility(ESlateVisibility::Collapsed);
			NavigationImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			NavigationOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

#if WITH_EDITOR
	//bIsUsingGamePad =false;
#endif
		Init_Internal();

	}
	Super::NativeConstruct();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningLocalPlayer());
	if (Subsystem)
	{
		Subsystem->ControlMappingsRebuiltDelegate.AddDynamic(this, &UNebulaFlowNavigationIcon::OnRebuildInputs);
	}
}

void UNebulaFlowNavigationIcon::NativeDestruct()
{
	FNebulaFlowCoreDelegates::OnAnyInputReleased.Remove(InputHandler);
	FNebulaFlowCoreDelegates::OnActionRemapped.Remove(RemapInputHandler);


	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningLocalPlayer());
	if (Subsystem)
	{
		Subsystem->ControlMappingsRebuiltDelegate.RemoveDynamic(this, &UNebulaFlowNavigationIcon::OnRebuildInputs);
	}

	Super::NativeDestruct();
}

void UNebulaFlowNavigationIcon::UpdateOwnerPlayerController(APlayerController* inController, bool bForceUpdate)
{
	if(inController && (GetOwningPlayer() != inController||bForceUpdate))
	{ 
		SetOwningPlayer(inController);
		UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));		
		if (GInstance)
		{
			if (UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this)->GetPlayerController(GInstance->GetWorld()) != GetOwningPlayer())	//not Main Player
			{
				bIsUsingGamePad = true;
				bIsFirstPlayerOnly = false;
			}
		}
		FNebulaFlowCoreDelegates::OnAnyInputReleased.Remove(InputHandler);
		Init_Internal();
	}
}

void UNebulaFlowNavigationIcon::UpdateImage()
{	
	CheckOverriddenIcon(ActionName);
	FString KeyName = FString("");
	if(bUseMulticonIfPresent)
	{
		CurrentTexture = Cast<UTexture2D>(LoadObject<UTexture2D>(nullptr, *GetIconPath(KeyName,true)));
	}
	if(!CurrentTexture || !bUseMulticonIfPresent)
	{
		CurrentTexture = Cast<UTexture2D>(LoadObject<UTexture2D>(nullptr,*GetIconPath(KeyName)));
	}
	
	if (!bShowTextOnKeyNotFound)
	{
		NavigationText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (CurrentTexture && NavigationImage)
	{
		NavigationText->SetVisibility(ESlateVisibility::Collapsed);
		SetNavigationImageTexture(CurrentTexture);
	}
#if PLATFORM_WINDOWS
	else if (!KeyName.IsEmpty())
	{
		// brutal and temporaneal
		if (KeyName == "None" || KeyName == "none")
		{
			NavigationImage->SetVisibility(ESlateVisibility::Collapsed);
			NavigationText->SetVisibility(ESlateVisibility::HitTestInvisible);
			NavigationText->SetText(FText::FromString(KeyName));
		}
		else
		{
			NavigationText->SetVisibility(ESlateVisibility::HitTestInvisible);
			NavigationText->SetText(FText::FromString(KeyName));
			if (BaseTexture && NavigationImage)
			{
				NavigationImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				NavigationImage->SetBrushFromSoftTexture(BaseTexture);
			}
		}

	}

	if (NavigationText->GetText().ToString().Contains("None") || NavigationText->GetText().ToString().Contains("none"))
	{
		NavigationImage->SetVisibility(ESlateVisibility::Collapsed);
		NavigationText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
#endif
}

FString UNebulaFlowNavigationIcon::GetIconPath(FString& OutKeyName,bool bSearchMultiIconPath)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	FString IconPath = FString("");
	FString IconName = FString("");
	if (!bIsOverriddenIcon && GInstance)
	{
		if (GInstance->IsEnhancedInputActive())
		{
			bool bActionFound = false;

			ULocalPlayer* LocalPlayer = UGameplayStatics::GetGameInstance(this)->GetFirstGamePlayer();

			// FIRST WE SEARCH IN SAVES TO SEE IF THE KEYS HAVE BEEN REMAPPED
			ensure(LocalPlayer);
			
			APlayerController* PC = GetOwningPlayer();
			ULocalPlayer* RealLocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
#if PLATFORM_SWITCH

			if (RealLocalPlayer && !UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(RealLocalPlayer->GetControllerId()))
#endif
			if (LocalPlayer)
			{
				UEnhancedInputLocalPlayerSubsystem* EISubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

				ensure(EISubsystem);
				if (EISubsystem)
				{
					UEnhancedInputUserSettings* UserSettings = EISubsystem->GetUserSettings();
					ensure(UserSettings); // Enhanced Input must be enabled under PlayerSettings->Enhanced Input->Enable user settings
					for (const TPair<FString, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair : UserSettings->GetAllAvailableKeyProfiles())
					{
						const FString& ProfileName = ProfilePair.Key;
						const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;
						
						for (const TPair<FName, FKeyMappingRow>& RowPair : Profile->GetPlayerMappingRows()) // will be only 1 for most of our games
						{
							if (RowPair.Value.HasAnyMappings())
							{

								bool KeyFound = false;
								bool IsGamepadKey = false;

								for (auto it = RowPair.Value.Mappings.begin(); it != RowPair.Value.Mappings.end(); ++it)
								{
									const FKey& Key = it->GetCurrentKey();
									
									if(it->GetAssociatedInputAction())
									{

										FName st = it->GetMappingName();
										if ((OverridedActionName == FName() && it->GetAssociatedInputAction()->GetName() == ActionName) || (OverridedActionName == it->GetMappingName()))
										{
											if (Key.IsGamepadKey() && bIsUsingGamePad)
											{
												OutKeyName = Key.GetDisplayName(false).ToString();
												IconName = Key.GetFName().ToString();
												bActionFound = true;
												break;
											}
											else if (!bIsUsingGamePad && !Key.IsGamepadKey())
											{
												OutKeyName = Key.GetDisplayName(false).ToString();
												IconName = Key.GetFName().ToString();
												bActionFound = true;
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
			// THEN WE SEARCH IN BETWEEN THE DEFAULT KEYS 


			FName TempOverridedActionName = OverridedActionName; // we use temporary variable because we may enter in this cycle on input type switch and dont want to change value of the OverridedActionName

			if (!bActionFound && UIConstants)
			{
				const UNebulaFlowNavigationIconsAsset* NavigationDataAsset = UIConstants->GetNavigationIconDataAssets();

				if (NavigationDataAsset)
				{
					TArray<TSoftObjectPtr<UInputMappingContext>> InputMappings = NavigationDataAsset->GameMappings;
					
#if PLATFORM_SWITCH
					if (UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(LocalPlayer->GetControllerId()))
					{
						InputMappings = NavigationDataAsset->SwitchGameMappingsSingleJoycon;
					}
					else
					{
						InputMappings = NavigationDataAsset->SwitchGameMappings;
					}
#endif


#if PLATFORM_PS4
					if (GInstance->GetIsPS_JP())
					{
						InputMappings = NavigationDataAsset->SwitchGameMappings;
					}
#endif
					for (int j = 0 ; j < InputMappings.Num() ; j++)
					{
						InputMappings[j].LoadSynchronous();
						TArray<FEnhancedActionKeyMapping> Mappings = InputMappings[j].Get()->GetMappings();
						for (int i = 0; i < Mappings.Num(); i++)
						{							
							UPlayerMappableKeySettings* MappingSetting = Mappings[i].GetPlayerMappableKeySettings();
							
							if(Mappings[i].Action)
							{

								if (Mappings[i].Action->GetName() == ActionName.ToString())
								{
									// if OverridedActionName is valid means the action is overriden in the IMC e.g. cases like movements
									if(TempOverridedActionName == FName() || (MappingSetting && TempOverridedActionName == MappingSetting->Name))
									{
										if (Mappings[i].Key.IsGamepadKey() && bIsUsingGamePad )
										{
											OutKeyName = Mappings[i].Key.GetDisplayName(false).ToString();
											IconName = Mappings[i].Key.GetFName().ToString();
											bActionFound = true;
											break;
										}
										else if (!(Mappings[i].Key.IsGamepadKey()) && !(bIsUsingGamePad))
										{
											OutKeyName = Mappings[i].Key.GetDisplayName(false).ToString();
											IconName = Mappings[i].Key.GetFName().ToString();
											bActionFound = true;
											break;
										}
									}

								}
							}
							// in the case an overrided name has been passed , but cannot find the correct action, we fall back on normal action
							
						}
						if (bActionFound)
						{
							break;
						}
						if (j == InputMappings.Num() - 1 && TempOverridedActionName != FName() && !bActionFound)
						{
							TempOverridedActionName = FName();
							j = 0;
						}
					}

				}
			}

		}
		else
		{
			UInputSettings* InputSettings = UInputSettings::GetInputSettings();
			if (InputSettings)
			{
				if (bIsAxis)
				{
 					TArray<FInputAxisKeyMapping> AxisMappings;
 					InputSettings->GetAxisMappingByName(ActionName, AxisMappings);
					FInputAxisKeyMapping* FoundAxisMapping = AxisMappings.FindByPredicate([=, this](const FInputAxisKeyMapping InMapping)
						{
							return ((bIsUsingGamePad == InMapping.Key.IsGamepadKey()) && AxisScale == InMapping.Scale);
						});
					if (FoundAxisMapping)
					{
						IconName = FoundAxisMapping->Key.ToString();
						if (!FoundAxisMapping->Key.IsGamepadKey())
						{
							if (!FoundAxisMapping->Key.GetDisplayName(false).IsEmpty())
							{
								IconName = FoundAxisMapping->Key.GetDisplayName(false).ToString();
							}
							else
							{
								IconName = FoundAxisMapping->Key.GetDisplayName(false).IsEmpty() ? IconName : FoundAxisMapping->Key.GetDisplayName(false).ToString();
							}
							OutKeyName = IconName;
						}
					}
				}
				else
				{
					TArray<FInputActionKeyMapping> ActionMappings;
					InputSettings->GetActionMappingByName(ActionName, ActionMappings);
					FInputActionKeyMapping* FoundActionMapping = ActionMappings.FindByPredicate([=, this](const FInputActionKeyMapping InMapping)
						{
							return (bIsUsingGamePad == InMapping.Key.IsGamepadKey());
						});
					if (FoundActionMapping)
					{
						IconName = FoundActionMapping->Key.GetFName().ToString();
						if (!FoundActionMapping->Key.IsGamepadKey())
						{
							if (!FoundActionMapping->Key.GetDisplayName(false).IsEmpty())
							{
								OutKeyName = FoundActionMapping->Key.GetDisplayName(false).ToString();
							}
							else
							{
								OutKeyName = FoundActionMapping->Key.GetDisplayName(false).IsEmpty() ? IconName : FoundActionMapping->Key.GetDisplayName(false).ToString();
							}
						}
					}
				}
			}
		}
	}
	else
	{
		//overridden Icon Logic
		IconName = bIsUsingGamePad ? OverriddenPadIcon : OverriddenKeyboardIcon;
	}

	if (!IconName.IsEmpty())
	{
		IconName.RemoveSpacesInline();

		FString Prefix = FString("");
		if(!UIConstants)
		{
			UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(GetWorld());
		}
		if (UIConstants && bSearchMultiIconPath)
		{
			const UNebulaFlowNavigationIconsAsset* NavigationIconsAsset = UIConstants->GetNavigationIconDataAssets();
			if (NavigationIconsAsset)
			{
				Prefix = NavigationIconsAsset->MultiIconPrefix;
			}
		}
		IconPath = UNebulaFlowUIFunctionLibrary::GetInputIconPath(this,Prefix+IconName,!bIsUsingGamePad , GetOwningPlayer());
			
		IconPath+="."+Prefix+IconName;
	}
	
	return IconPath;
}

void UNebulaFlowNavigationIcon::CheckOverriddenIcon(FName InActionName)
{
	if(!UIConstants)
	{
		UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(this);
	}
	//ensureMsgf(InActionName!=NAME_None,TEXT("No action Associated"));
	if (UIConstants && InActionName!=NAME_None)
	{
		const UNebulaFlowNavigationIconsAsset* NavigationIconsAsset = UIConstants->GetNavigationIconDataAssets();
		if (NavigationIconsAsset)
		{
			if (NavigationIconsAsset)
			{
				if (NavigationIconsAsset->OverrideInfosMap.Contains(InActionName))
				{
					UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
					if (GInstance->IsUsingGamepad)
					{
						if (!NavigationIconsAsset->OverrideInfosMap[InActionName].PadIconName.IsEmpty())
						{
							OverriddenPadIcon = NavigationIconsAsset->OverrideInfosMap[InActionName].PadIconName;
							bIsOverriddenIcon = true;
						}
						else
						{
							bIsOverriddenIcon = false;
						}
					}
					else
					{
						if (!NavigationIconsAsset->OverrideInfosMap[InActionName].KeyboardIconName.IsEmpty())
						{
							OverriddenKeyboardIcon = NavigationIconsAsset->OverrideInfosMap[InActionName].KeyboardIconName;
							bIsOverriddenIcon = true;
						}
						else
						{
							bIsOverriddenIcon = false;
						}
					}
				}
			}
		}
	}
}

void UNebulaFlowNavigationIcon::Init_Internal()
{
#if PLATFORM_WINDOWS

	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	bIsUsingGamePad = true;
	if (GInstance && (bIsFirstPlayerOnly || UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this)->GetPlayerController(GInstance->GetWorld()) == GetOwningPlayer()))
	{
		APlayerController*  PController = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this)->GetPlayerController(GInstance->GetWorld());
		if (PController)
		{
			if (GInstance->IsEnhancedInputActive())
			{
				UNebulaFlowEnhancedPlayerInput* PInput = Cast<UNebulaFlowEnhancedPlayerInput>(PController->PlayerInput);
				ensure(PInput);
				if (PInput)
				{
					bIsUsingGamePad = PInput->IsPlayerUsingGamepad();
				}
				else
				{
					UE_LOG(LogNebulaFlowUI, Warning, TEXT("PlayerInput MUST BE a Subclass Of NebulaEnhancedPlayerInput"));
				}
			}
			else
			{
				UNebulaFlowPlayerInput* PInput = Cast<UNebulaFlowPlayerInput>(PController->PlayerInput);
				ensure(PInput);
				if (PInput)
				{
					bIsUsingGamePad = PInput->IsPlayerUsingGamepad();
				}
				else
				{
					UE_LOG(LogNebulaFlowUI, Warning, TEXT("PlayerInput MUST BE a Subclass Of NebulaPlayerInput"));
				}
			}
		}
		InputHandler = FNebulaFlowCoreDelegates::OnAnyInputReleased.AddUObject(this, &UNebulaFlowNavigationIcon::OnAnyKeyPressed);
		RemapInputHandler = FNebulaFlowCoreDelegates::OnActionRemapped.AddUObject(this, &UNebulaFlowNavigationIcon::UpdateImage);

	}
	
#endif

	OverriddenKeyboardIcon = FString("");
	OverriddenPadIcon = FString("");
	if (ActionName != NAME_None)
	{
		UpdateImage();
	}
}

void UNebulaFlowNavigationIcon::OnInputTypeSwitched(const bool bInIsUsingGamepad)
{
	bIsUsingGamePad = bInIsUsingGamepad;

	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	GInstance->IsUsingGamepad = bInIsUsingGamepad;

	UpdateImage();
}

void UNebulaFlowNavigationIcon::OnAnyKeyPressed(FKey Key, bool bIsGamepad, APlayerController* Sender)
{
	if (Sender && Sender==GetOwningPlayer() && Sender->NetPlayerIndex == 0)
	{
		if (bIsUsingGamePad != bIsGamepad)
		{
			OnInputTypeSwitched(bIsGamepad);
		}
	}
}

void UNebulaFlowNavigationIcon::OnRebuildInputs()
{
	OnInputTypeSwitched(bIsUsingGamePad);
}

void UNebulaFlowNavigationIcon::SetNavigationImageTexture(UTexture2D* Texture)
{
	if (NavigationImage && Texture)
	{
		UMaterialInstanceDynamic* Material = NavigationImage->GetDynamicMaterial();
		if (Material)
		{
			Material->SetTextureParameterValue(MaterialTextureParameterName, Texture);
		}
		else
		{
			NavigationImage->SetBrushFromTexture(Texture);
		}
	}
}