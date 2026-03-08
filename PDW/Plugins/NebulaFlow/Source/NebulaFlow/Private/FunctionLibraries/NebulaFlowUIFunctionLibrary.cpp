#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "UI/NebulaFlowBasePage.h"
#include "FSM/NebulaFlowBaseFSMState.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NebulaFlowUIDelegates.h"
#include "Core/NebulaFlowGameInstance.h"
#include "UI/NebulaFlowUIConstants.h"
#include "UI/NebulaFlowNavbarDataStructures.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowUIManager.h"
#include "Core/NebulaFlowPlayerController.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "NebulaFlow.h"
#include "GameFramework/PlayerController.h"
#include "UI/Widgets/NebulaFlowBaseNavbar.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "UI/Widgets/NebulaFlowBaseWidget.h"




UNebulaFlowBasePage* UNebulaFlowUIFunctionLibrary::CreateNewPage(UObject* WorldContextObject, TSubclassOf<UNebulaFlowBasePage> PageClass, UNebulaFlowBaseFSMState* StateOwner, ANebulaFlowPlayerController* OwnerPlayerController /*= nullptr*/, EAddToScreenType AddToScreenType /*= EAddToScreenType::EAddToViewPort*/)
{
	UNebulaFlowBasePage* NewPage = CreateWidget<UNebulaFlowBasePage>(UGameplayStatics::GetGameInstance(WorldContextObject), PageClass);
	
	ANebulaFlowPlayerController* CurrentOwnerPlayerController = OwnerPlayerController != nullptr ? OwnerPlayerController : Cast<ANebulaFlowPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(WorldContextObject);

	if(UIManager)
	{ 
		if (UIManager->CurrentDisplayedPageRef)
		{
			RemovePage(WorldContextObject,UIManager->CurrentDisplayedPageRef);
		}
		if (NewPage && CurrentOwnerPlayerController)
		{
			NewPage->InitPage(CurrentOwnerPlayerController, StateOwner);
			NewPage->SetOwningPlayer(CurrentOwnerPlayerController);

			if (NewPage->GetPageNavbar())
			{
				NewPage->GetPageNavbar()->SetOwningPlayer(CurrentOwnerPlayerController);
			}

			if (AddToScreenType == EAddToScreenType::EAddToViewPort)
			{
				NewPage->AddToViewport();
			}
			else
			{
				NewPage->AddToPlayerScreen();
			}
			UIManager->CurrentDisplayedPageRef = NewPage;
		}
	}
	return NewPage;
}

void UNebulaFlowUIFunctionLibrary::RemovePage(UObject* WorldContextObject, UNebulaFlowBasePage* InPageToRemove)
{
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(WorldContextObject);
	if(InPageToRemove)
	{ 
		if(UIManager)
		{ 
			if (UIManager->CurrentDisplayedPageRef == InPageToRemove)
			{
				UIManager->CurrentDisplayedPageRef = nullptr;
			}
			else
			{
				//LOG PAge Mismatch!
			}
		}
		InPageToRemove->RemoveFromParent();
	}

}



UNebulaFlowUIConstants* UNebulaFlowUIFunctionLibrary::GetUIConstants(UObject* WorldContextObject)
{

	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (GInstance && GInstance->UIConstantsDataAsset)
	{
		return GInstance->UIConstantsDataAsset;
	}

	UE_LOG(LogNebulaFlowUI, Warning, TEXT("No UI Constants Defined"));

	return nullptr;
}

UNebulaFlowBasePage* UNebulaFlowUIFunctionLibrary::GetCurrentPage(UObject* WorldContextObject)
{
	UNebulaFlowUIManager* UIMAnager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(WorldContextObject);
	if (UIMAnager)
	{
		return UIMAnager->CurrentDisplayedPageRef;
	}
	return nullptr;
}

FString UNebulaFlowUIFunctionLibrary::GetInputIconPath(UObject* WorldContextObject, FString IconName, bool bIsUsingKeyboard /*= false*/ , APlayerController* Owner /* = nullptr */)
{
	UNebulaFlowUIConstants* UIConstants = GetUIConstants(WorldContextObject);
	FString Path =FString("");
	if (UIConstants)
	{
		const UNebulaFlowNavigationIconsAsset* NavigationIconsAsset = UIConstants->GetNavigationIconDataAssets();
		if(NavigationIconsAsset)
		{ 
		//TODO SWITCH ON CURRENT PLATFORM

#if PLATFORM_WINDOWS

			Path = !bIsUsingKeyboard ? NavigationIconsAsset->XboxOnePath : NavigationIconsAsset->KeyboardPath;

#elif PLATFORM_XBOXONE
			
			Path = NavigationIconsAsset->XboxOnePath;

#elif PLATFORM_PS4

			Path = NavigationIconsAsset->PS4Path;

#elif PLATFORM_SWITCH
			
			UWorld* InWorld = WorldContextObject->GetWorld();

			int32 IndexCounter = 0;
			for (auto Iterator = InWorld->GetPlayerControllerIterator(); Iterator; ++Iterator , ++IndexCounter)
			{
				APlayerController* PlayerController = Iterator->Get();
				if (PlayerController == Owner)
				{
					bool Horizontal = UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(IndexCounter);
					if (Horizontal)
					{
						Path = UNebulaFlowConsoleFunctionLibrary::IsJoyconLeft(IndexCounter) ? NavigationIconsAsset->SwitchHorizontalLeftPath : NavigationIconsAsset->SwitchHorizontalRightPath;
					}
					else
					{
						Path = NavigationIconsAsset->SwitchPath;
					}
				}
			}
					
			
#elif PLATFORM_PS5

			Path = NavigationIconsAsset->PS5Path;

#elif PLATFORM_XSX

			Path = NavigationIconsAsset->XSXPath;

#endif

	Path+=IconName;

		}
	}
	else
	{
		UE_LOG(LogNebulaFlowFSM, Warning, TEXT("No UI Constants Found"));
	}
	return Path;
}

void UNebulaFlowUIFunctionLibrary::SetFocusToGameViewport(UObject* WorldContextObject)
{
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(WorldContextObject);
	if (UIManager)
	{
		UIManager->AllowViewportFocus(true);
	}
	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}

bool UNebulaFlowUIFunctionLibrary::IsGameArabic(UObject* WorldContextObject)
{
	FString Language = UKismetInternationalizationLibrary::GetCurrentLocale();
	return Language.Contains("ar") ;
}

void UNebulaFlowUIFunctionLibrary::SetUserFocus(UObject* WorldContextObject, UWidget* InFocusWidget , APlayerController* PlayerController, bool bPlaySound)
{
	if(UNebulaFlowBaseWidget* NebulaWidget = Cast<UNebulaFlowBaseWidget>(InFocusWidget))
	{
		NebulaWidget->SetPlayNextFocusAudioEvents(bPlaySound);
	}
	if (!PlayerController)
	{
		InFocusWidget->SetFocus();
	}
	else
	{
		InFocusWidget->SetUserFocus(PlayerController);
	}
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(WorldContextObject);
	if (UIManager)
	{
		UIManager->OnFirstWidgetFocus(InFocusWidget , PlayerController);
	}
}

UNebulaFlowDialog* UNebulaFlowUIFunctionLibrary::ShowDialog(UObject* WorldContextObject, FName DialogID, TFunction<void(FString)> InCallBack, APlayerController* OwnerController, TArray<ANebulaFlowPlayerController*> SyncControllers, TArray<FText> Params, UTexture2D* Image)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(WorldContextObject); 
	
	if (GInstance && UIManager)
	{
		APlayerController* PController = OwnerController != nullptr ? OwnerController : UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject)->GetPlayerController(GInstance->GetWorld());
		if (PController)
		{
			return UIManager->ShowDialog(GInstance,PController,DialogID,InCallBack,SyncControllers, Params, Image);
		}		 
	}	
	return nullptr;
}

bool UNebulaFlowUIFunctionLibrary::HideDialog(UObject* WorldContextObject, UNebulaFlowDialog* DialogToClose)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(WorldContextObject);
	if (UIManager)
	{
		return UIManager->HideDialog(GInstance,DialogToClose);
	}
	return false;
}

bool UNebulaFlowUIFunctionLibrary::HideGameDialog(UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(WorldContextObject);
	if (UIManager)
	{
		return UIManager->CloseGameDialog(GInstance);
	}
	return false;
}

bool UNebulaFlowUIFunctionLibrary::HideSystemDialog(UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	UNebulaFlowUIManager* UIManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(WorldContextObject);
	if (UIManager)
	{
		return UIManager->CloseSystemDialog(GInstance);
	}
	return false;
}

