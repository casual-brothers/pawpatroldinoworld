// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWTutorialNavigationIcon.h"
#include "Components/PanelWidget.h"
#include "Components/Overlay.h"
#include "Misc/Paths.h"
#include "Animation/WidgetAnimation.h"
#include "FunctionLibraries/PDWUIFunctionLibrary.h"
#include "UI/Widgets/PDWTutorialSingleKey.h"
#include "Components/TextBlock.h"
#include "Data/GameOptionsEnums.h"
#include "Managers/PDWEventSubsytem.h"
#if PLATFORM_SWITCH
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Engine/LocalPlayer.h"
#endif
#pragma region IconData

#if WITH_EDITOR
void UPDWPlatformIconData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UPDWPlatformIconData, PlatformConfiguration))
	{
		for (auto& Pair : PlatformConfiguration)
		{
			FPDWPlatformIcons& PlatformIcons = Pair.Value;

			if (PlatformIcons.Icons.Num())
			{
				continue;
			}

			for (int32 i = 0; i < StaticEnum<EPadKeyNames>()->NumEnums() - 1; ++i)
			{
				EPadKeyNames KeyName = static_cast<EPadKeyNames>(i);
				FPDWPlatformIconConfiguration Config;
				Config.KeyName = KeyName;
				FName OutName = StaticEnum<EPadKeyNames>()->GetValueAsName(KeyName);
				FString CleanName;
				OutName.ToString().Split(TEXT("::"), nullptr, &CleanName);
				Config.KeyNameString = FName(CleanName);
				PlatformIcons.Icons.Add(Config);
			}
		}
	}
}
#endif

#pragma endregion

#if WITH_EDITOR
void UPDWTutorialNavigationIcon::SaveWidgetsData()
{
	for (UWidget* Child : PadPanel->GetAllChildren())
	{
		if (UPDWTutorialSingleKey* SingleKey = Cast<UPDWTutorialSingleKey>(Child))
		{
			if (PlatformConfigurationData->PlatformConfiguration.Contains(CheckCurrentConfiguration()))
			{
				if(FPDWPlatformIconConfiguration* IconConfig = PlatformConfigurationData->PlatformConfiguration[CheckCurrentConfiguration()].FindKeyName(SingleKey->GetCurrentKeyName()))
				{
					IconConfig->ElementData = SingleKey->GetNewWidgetData();
				}
			}
		}
	}

	PlatformConfigurationData->MarkPackageDirty();
}
#endif

void UPDWTutorialNavigationIcon::NativePreConstruct()
{
	Super::NativePreConstruct();
#if WITH_EDITOR
	if (IsDesignTime())
	{
		InitKeys();
		if (DebugText)
		{
			FName OutName = StaticEnum<EControllerConfiguration>()->GetValueAsName(DebugControllerConfiguration);
			FString CleanName;
			OutName.ToString().Split(TEXT("::"), nullptr, &CleanName);
			DebugText->SetText(FText::FromString(CleanName));
		}

		for (UWidget* Child : PadPanel->GetAllChildren())
		{
			if (UPDWTutorialSingleKey* SingleKey = Cast<UPDWTutorialSingleKey>(Child))
			{
				SingleKey->OnPostEditChangeTrigger.AddUniqueDynamic(this, &ThisClass::SaveWidgetsData);
			}
		}
	}
#endif
}

void UPDWTutorialNavigationIcon::NativeConstruct()
{
	Super::NativeConstruct();
	InitKeys();
#if PLATFORM_SWITCH
	FCoreUObjectDelegates::OnRemapJoyconsUIClosed.AddUObject(this, &ThisClass::CheckConfigurationJoycon);
#endif
	UPDWEventSubsytem::Get(this)->OnGameOptionsChanged.AddUniqueDynamic(this, &ThisClass::OnGameOptionChanged);
}

void UPDWTutorialNavigationIcon::NativeDestruct()
{
#if PLATFORM_SWITCH
	FCoreUObjectDelegates::OnRemapJoyconsUIClosed.RemoveAll(this);
#endif
	UPDWEventSubsytem::Get(this)->OnGameOptionsChanged.RemoveDynamic(this, &ThisClass::OnGameOptionChanged);
	Super::NativeDestruct();
}

void UPDWTutorialNavigationIcon::OnSingleKeyIconLoad()
{
	IconsToLoad--;
	if (IconsToLoad == 0)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

#if WITH_EDITOR
void UPDWTutorialNavigationIcon::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == "DebugControllerConfiguration")
	{
		InitKeys();
		if (DebugText)
		{
			FName OutName = StaticEnum<EControllerConfiguration>()->GetValueAsName(DebugControllerConfiguration);
			FString CleanName;
			OutName.ToString().Split(TEXT("::"), nullptr, &CleanName);
			DebugText->SetText(FText::FromString(CleanName));
		}
	}
}
#endif

void UPDWTutorialNavigationIcon::UpdateImage()
{
	if (bIsUsingGamePad)
	{
		NavigationOverlay->SetVisibility(ESlateVisibility::Collapsed);
		PadPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		FString KeyName = FString("");
		CheckOverriddenIcon(ActionName);
		FString IconPath = GetIconPath(KeyName);
		KeyName = FPaths::GetCleanFilename(IconPath);
		int32 DotIndex;
		if (KeyName.FindLastChar('.', DotIndex))
		{
			FString SecondName = KeyName.RightChop(DotIndex + 1);
			KeyName = SecondName;
		}
		if (KeyNamesAlias.Contains(FName(KeyName)))
		{
			KeyName = KeyNamesAlias[FName(KeyName)].ToString();
		}
		UWidget* Widget = UPDWUIFunctionLibrary::GetWidgetByName(this, FName(KeyName));
		if (Widget)
		{
			UPDWTutorialSingleKey* SingleKey = Cast<UPDWTutorialSingleKey>(Widget);
			if (SingleKey)
			{
				SingleKey->BP_PlayKeyAnimation();
			}
		}
		BP_InitAction(KeyName);
	}
	else
	{
		NavigationOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		PadPanel->SetVisibility(ESlateVisibility::Collapsed);
		Super::UpdateImage();
	}
}

UWidgetAnimation* UPDWTutorialNavigationIcon::GetAnimationByName(FName AnimationName) const
{
    for (TFieldIterator<FObjectProperty> Prop(GetClass()); Prop; ++Prop)
    {
        FObjectProperty* ObjectProp = *Prop;

        if (ObjectProp->PropertyClass == UWidgetAnimation::StaticClass())
        {
            UWidgetAnimation* Anim = Cast<UWidgetAnimation>(ObjectProp->GetObjectPropertyValue_InContainer(this));

            if (Anim)
            {
				if (Anim->GetName().Contains(AnimationName.ToString(), ESearchCase::IgnoreCase))
				{
					return Anim;
				}
            }
        }
    }

    return nullptr;
}

EControllerConfiguration UPDWTutorialNavigationIcon::CheckCurrentConfiguration()
{
#if PLATFORM_WINDOWS || PLATFORM_XSX || PLATFORM_XBOXONE
	#if WITH_EDITOR
		if(!GetWorld()->IsGameWorld())
		{
			return DebugControllerConfiguration;
		}
	#endif
	//bool bIsSteamDeck = FPlatformMisc::GetEnvironmentVariable(TEXT("SteamDeck")).Equals(FString(TEXT("1")));
	return EControllerConfiguration::Microsoft;
#elif PLATFORM_PS4 
	return EControllerConfiguration::PS4;
#elif PLATFORM_PS5
	return EControllerConfiguration::PS5;
#elif PLATFORM_SWITCH
	APDWPlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	int32 PlayerIndex = PC->GetLocalPlayer()->GetLocalPlayerIndex();
	if (UNebulaFlowConsoleFunctionLibrary::IsJoyconDualGrip(PlayerIndex))
	{
		return EControllerConfiguration::Switch_DualJoycon;
	}
	if (UNebulaFlowConsoleFunctionLibrary::IsJoyconHandheld(PlayerIndex))
	{
		return EControllerConfiguration::Switch_Handheld;
	}
	if (UNebulaFlowConsoleFunctionLibrary::IsJoyconLeft(PlayerIndex))
	{
		return EControllerConfiguration::Switch_JoyconLeft;
	}
	if (UNebulaFlowConsoleFunctionLibrary::IsJoyconRight(PlayerIndex))
	{
		return EControllerConfiguration::Switch_JoyconRight;
	}
	if (UNebulaFlowConsoleFunctionLibrary::IsSwitchProController(PlayerIndex))
	{
		return EControllerConfiguration::Switch_ProController;
	}
	return EControllerConfiguration::Switch_Handheld;
#endif

	return EControllerConfiguration::Microsoft;
}

void UPDWTutorialNavigationIcon::InitKeys()
{
	SetVisibility(ESlateVisibility::Hidden);
	if (PlatformConfigurationData)
	{
		UPDWPlatformIconData* PlatformIconData = Cast<UPDWPlatformIconData>(PlatformConfigurationData);
		if (PlatformIconData)
		{
			EControllerConfiguration CurrentController = CheckCurrentConfiguration();
			if (PlatformIconData->PlatformConfiguration.Contains(CurrentController))
			{
				TArray<FPDWPlatformIconConfiguration>& Icons = PlatformIconData->PlatformConfiguration[CurrentController].Icons;
				if (!Icons.Num())
				{
					return;
				}

				IconsToLoad = Icons.Num()-1;
				for (int32 i = Icons.Num() - 1; i >= 0; --i)
				{
					FPDWPlatformIconConfiguration& IconConfig = Icons[i];

					FName OutName = StaticEnum<EPadKeyNames>()->GetValueAsName(IconConfig.KeyName);
					FString CleanName;
					OutName.ToString().Split(TEXT("::"), nullptr, &CleanName);

					UWidget* Widget = UPDWUIFunctionLibrary::GetWidgetByName(this, FName(CleanName));
					if (Widget)
					{
						UPDWTutorialSingleKey* SingleKey = Cast<UPDWTutorialSingleKey>(Widget);
						if (SingleKey)
						{
							SingleKey->OnIconLoad.AddUniqueDynamic(this, &ThisClass::OnSingleKeyIconLoad);
							SingleKey->InitKey(IconConfig);
						}
					}
				}
			}
		}
	}
}

void UPDWTutorialNavigationIcon::CheckConfigurationJoycon(bool bIsRemaped)
{
	if (bIsRemaped)
	{
		InitKeys();
	}
}

void UPDWTutorialNavigationIcon::OnGameOptionChanged(EGameOptionsId SettingsId, int32 CurrentValueIndex)
{
	if (SettingsId == EGameOptionsId::SwapJoystick)
	{
		InitKeys();
	}
}
