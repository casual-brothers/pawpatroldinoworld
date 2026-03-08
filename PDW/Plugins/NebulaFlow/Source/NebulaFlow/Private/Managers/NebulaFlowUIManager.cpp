#include "Managers/NebulaFlowUIManager.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "UI/NebulaFlowUIConstants.h"
#include "Engine/DataTable.h"
#include "UI/NebulaFlowDialog.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NebulaFlowDialogDataStructures.h"
#include "Core/NebulaFlowGameInstance.h"
#include "NebulaFlow.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "Engine/Engine.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Viewport.h"
#include "Core/NebulaFlowPlayerController.h"
#include "Core/NebulaFlowLocalPlayer.h"


UNebulaFlowUIManager::UNebulaFlowUIManager(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	bShouldTick = true;
}


UNebulaFlowDialog* UNebulaFlowUIManager::ShowDialog(UGameInstance* GInstance,APlayerController* Owner, FName DialogID, TFunction<void(FString)> InCallBack, TArray<ANebulaFlowPlayerController*> SyncControllers, TArray<FText> Params, UTexture2D* Image)
{
	
	UNebulaFlowUIConstants* UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(GInstance);
	if(UIConstants)
	{ 
		const UDataTable* DialogConfTable = UIConstants->GetDialogConfigurationsTable();
		ensure(DialogConfTable);
		if(DialogConfTable)
		{ 
			FDialogConfigTableRow* Row = DialogConfTable->FindRow<FDialogConfigTableRow>(DialogID,FString("GENERAL"));
			ensure(Row);
			if(Row)
			{ 		
				ensure(Row->DialogConfiguration.DialogClass);
				if (Row->DialogConfiguration.DialogClass)
				{
					if (Row->DialogConfiguration.DialogType == EDialogType::EGameDialog)
					{
						ensure(CurrentGameDialog == nullptr);
						if (CurrentGameDialog != nullptr)
						{
							UE_LOG(LogNebulaFlowFSM, Warning, TEXT("Cannot have two game dialogs opened! returning already opened dialog"));
							return CurrentGameDialog;
						}
						if (CurrentGameDialog == nullptr)
						{
							bGameDialogInputLocked = true;

							TArray<AActor*> Controllers;
							UGameplayStatics::GetAllActorsOfClass(this, APlayerController::StaticClass(), Controllers);

							TSubclassOf<UNebulaFlowDialog> CurrentDialogClass = (Controllers.Num() > 1 && SyncControllers.Num() > 0) ? Row->DialogConfiguration.DialogSyncClass : Row->DialogConfiguration.DialogClass;
							ensure(CurrentDialogClass);
							if (CurrentDialogClass)
							{
								CurrentGameDialog = CreateWidget<UNebulaFlowDialog>(GInstance, CurrentDialogClass);
								if (CurrentGameDialog)
								{
									if (CurrentDisplayedPageRef)
									{
										CurrentDisplayedPageRef->CacheFocusedWidgets();
									}
									CurrentGameDialog->InitDialog(Owner, DialogID, InCallBack, SyncControllers, Params, Image, false, Row->DialogConfiguration.InputMappingContext);
									CurrentGameDialog->AddToViewport(2);
									SetAllUsersFocusOnWidget(CurrentGameDialog);

									if (Row->DialogConfiguration.VoiceOverEvent)
									{
										UNebulaFlowAudioFunctionLibrary::PlayVoiceSpeaker(GetWorld(), Row->DialogConfiguration.VoiceOverEvent, Row->DialogConfiguration.ProgrammerName);
									}

									return CurrentGameDialog;
								}
							}
						}
					}
					else if (Row->DialogConfiguration.DialogType == EDialogType::ESystemDialog)
					{
						ensure(CurrentSystemDialog == nullptr);
						if (CurrentSystemDialog != nullptr)
						{
							UE_LOG(LogNebulaFlowFSM, Warning, TEXT("Cannot have two System dialogs opened! returning already opened dialog"));
							return CurrentSystemDialog;
						}
						if (CurrentSystemDialog == nullptr)
						{
							if (CurrentDisplayedPageRef)
							{
								CurrentDisplayedPageRef->CacheFocusedWidgets();
							}
							bSystemDialogInputLocked = true;
							CurrentSystemDialog = CreateWidget<UNebulaFlowDialog>(GInstance, Row->DialogConfiguration.DialogClass);
							if (CurrentSystemDialog)
							{
								CurrentSystemDialog->InitDialog(Owner, DialogID, InCallBack, SyncControllers, Params, Image, true, Row->DialogConfiguration.InputMappingContext);
								CurrentSystemDialog->AddToViewport(3);
								SetAllUsersFocusOnWidget(CurrentSystemDialog);
								return CurrentSystemDialog;
							}
						}
					}
					else if (Row->DialogConfiguration.DialogType == EDialogType::EPopupDialog)
					{
						ensure(CurrentPopup == nullptr);
						if (CurrentPopup != nullptr)
						{
							UE_LOG(LogNebulaFlowFSM, Warning, TEXT("Cannot have two popups opened! returning already opened dialog"));
							return CurrentPopup;
						}
						if (CurrentPopup == nullptr && Row->DialogConfiguration.DialogClass)
						{
							CurrentPopup = CreateWidget<UNebulaFlowDialog>(GInstance, Row->DialogConfiguration.DialogClass);
							if (CurrentPopup)
							{
								CurrentPopup->InitDialog(Owner, DialogID, InCallBack, SyncControllers, Params, Image, false, Row->DialogConfiguration.InputMappingContext);
								CurrentPopup->AddToViewport(1);
								return CurrentPopup;
							}
						}
					}
				}
			}
		}
	}
	return nullptr;
}

void UNebulaFlowUIManager::TickManager(float DeltaTime)
{
	Super::TickManager(DeltaTime);
	if (UITimerManager)
	{
		UITimerManager->Tick(DeltaTime);
	}
}

void UNebulaFlowUIManager::RestoreFocus()
{
	TArray<APlayerController*> Controllers;
	CachedWidgetsFocus.GetKeys(Controllers);
	if (bViewportHasFocus)
	{
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
		return;
	}
	for (auto Controller : Controllers)
	{
		if (Controller != nullptr)
		{
			UWidget* FocusedWidget = *CachedWidgetsFocus.Find(Controller);

			if (FocusedWidget)
			{
				FocusedWidget->SetUserFocus(Controller);
			}
			else
			{
				UWidgetBlueprintLibrary::SetFocusToGameViewport();
			}
		}
	}

}

void UNebulaFlowUIManager::ClearCachedFocusedWidget()
{
	CachedWidgetsFocus.Empty();
}

// disabled to fix the focus loss on pad disconnection while changing page

void UNebulaFlowUIManager::OnWidgetFocus(UWidget* FocusedWidget, int32 PlayerIndex)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerControllerFromID(FocusedWidget , PlayerIndex);

	//if we dont want to allow viewport focus we dont save it as last focused widget

	if (PlayerController)
	{
		if (!Cast<UNebulaFlowDialog>(FocusedWidget))
		{
			bViewportHasFocus = false;
			CachedWidgetsFocus.Emplace(PlayerController, FocusedWidget);
		}
	}
}

void UNebulaFlowUIManager::OnFirstWidgetFocus(UWidget* FocusedWidget, APlayerController* PlayerController)
{
	if (PlayerController)
	{
		if (!Cast<UNebulaFlowDialog>(FocusedWidget))
		{
			bViewportHasFocus = false;
			CachedWidgetsFocus.Emplace(PlayerController, FocusedWidget);
		}
	}
}

void UNebulaFlowUIManager::OnViewportFocus()
{
	if (!AllowViewportFocus())
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to give focus to viewport without enabling it, maybe you wanted to call  UNebulaFlowUIFunctionLibrary::SetFocusToGameViewport") );
		RestoreFocus();
		return;
	}
	
	bViewportHasFocus = true;
}

bool UNebulaFlowUIManager::HideDialog(UGameInstance* GInstance, UNebulaFlowDialog* DialogToClose)
{
	bool Succeded = false ;
	if (!GInstance || !DialogToClose)
	{
		return false;
	}
	if (CurrentGameDialog == DialogToClose)
	{
		Succeded = true;		
		if (UITimerManager)
		{
			UITimerManager->SetTimerForNextTick(this, &UNebulaFlowUIManager::ReleaseGameDialogInputLock);
		}
		CurrentGameDialog = nullptr;
		if (DialogToClose)
		{
			DialogToClose->UninitDialog();
			DialogToClose->RemoveFromParent();
		}
		RestoreFocus();

		UNebulaFlowAudioFunctionLibrary::ForceStopSpeaker(this);
	}
	else if (CurrentSystemDialog == DialogToClose)
	{
		Succeded = true;
		if (UITimerManager)
		{
			UITimerManager->SetTimerForNextTick(this, &UNebulaFlowUIManager::ReleaseSystemDialogInputLock);
		}
		CurrentSystemDialog= nullptr;
		if (DialogToClose)
		{
			DialogToClose->UninitDialog();
			DialogToClose->RemoveFromParent();
		}
		if (CurrentGameDialog)
		{
			SetAllUsersFocusOnWidget(CurrentGameDialog);
		}
 		else
 		{			
			RestoreFocus();
		}


	}
	else if (CurrentPopup == DialogToClose)
	{
		Succeded = true;
		CurrentPopup = nullptr;
		if (DialogToClose)
		{
			DialogToClose->UninitDialog();
			DialogToClose->RemoveFromParent();
		}

		RestoreFocus();

		//UNebulaFlowAudioFunctionLibrary::ForceStopSpeaker(this);
	}
	else
	{
		// Log system or game dialog recognized
	}

	UGameplayStatics::GetGameInstance(GetWorld())->GetEngine()->ForceGarbageCollection(true);
	return Succeded;
}

void UNebulaFlowUIManager::ReleaseGameDialogInputLock()
{
	if (CurrentGameDialog == nullptr)
	{
		bGameDialogInputLocked = false;
	}
}

void UNebulaFlowUIManager::InitManager(UNebulaFlowGameInstance* InstanceOwner)
{
	Super::InitManager(InstanceOwner);

	if (UITimerManager == nullptr)
	{
		UITimerManager = new FTimerManager(InstanceOwner);
	}
	FCoreUObjectDelegates::OnWidgetFocusReceived.AddUObject(this, &UNebulaFlowUIManager::OnWidgetFocus);
	FCoreUObjectDelegates::OnViewportFocusReceived.AddUObject(this, &UNebulaFlowUIManager::OnViewportFocus);
}

bool UNebulaFlowUIManager::IsUIInputLocked()
{
	return IsAnyDialogInputLocked();
}


void UNebulaFlowUIManager::ReleaseSystemDialogInputLock()
{
	if (CurrentSystemDialog == nullptr)
	{
		bSystemDialogInputLocked = false;
	}
}

bool UNebulaFlowUIManager::CloseSystemDialog(UGameInstance* GInstance)
{
	if (CurrentSystemDialog)
	{
		return HideDialog(GInstance, CurrentSystemDialog);
	}
	return false;
}


bool UNebulaFlowUIManager::CloseGameDialog(UGameInstance* GInstance)
{
	if (CurrentGameDialog)
	{
		return HideDialog(GInstance, CurrentGameDialog);
	}
	return false;
}

void UNebulaFlowUIManager::SetAllUsersFocusOnWidget(UUserWidget* WidgetToFocus)
{
	TArray<AActor*> Controllers;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerController::StaticClass(), Controllers);
	ensure(WidgetToFocus);
	if(WidgetToFocus)
	{ 
		for (AActor* current : Controllers)
		{
			APlayerController* currentController = Cast<APlayerController>(current);
			if (currentController)
			{
				WidgetToFocus->SetUserFocus(currentController);
			}
		}
	}
}

void UNebulaFlowUIManager::OnLoadLevelStarted(FName LevelName)
{
	Super::OnLoadLevelStarted(LevelName);

	if (CurrentDisplayedPageRef)
	{
		UNebulaFlowUIFunctionLibrary::RemovePage(this,CurrentDisplayedPageRef);
		CurrentDisplayedPageRef = nullptr;
	}
	if (CurrentGameDialog)
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this,CurrentGameDialog);
		CurrentGameDialog = nullptr;
	}
	if (CurrentSystemDialog)
	{
		UNebulaFlowUIFunctionLibrary::HideDialog(this,CurrentSystemDialog);
		CurrentSystemDialog = nullptr;
	}

	ClearCachedFocusedWidget();
}
