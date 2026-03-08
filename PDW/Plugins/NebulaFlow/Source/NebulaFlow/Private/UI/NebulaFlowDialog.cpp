#include "UI/NebulaFlowDialog.h"
#include "Components/TextBlock.h"
#include "UI/NebulaFlowUIConstants.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "NebulaFlow.h"
#include "Engine/DataTable.h"
#include "UI/Widgets/NebulaFlowDialogButton.h"
#include "Components/PanelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Framework/Application/SlateApplication.h"
#include "Core/NebulaFlowPlayerController.h"
#include "Engine/Texture2D.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "Core/NebulaFlowCoreStructures.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"


UNebulaFlowDialog::UNebulaFlowDialog(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

}

void UNebulaFlowDialog::NativeConstruct()
{
	Super::NativeConstruct();
}

void UNebulaFlowDialog::SendDialogResponse(FString Response, const ANebulaFlowPlayerController* inController, int32 ResponsePriority)
{
	if (inController != nullptr && bIsSyncDialog && CurrentSyncResponses.Contains(inController))
	{
		if(CurrentSyncResponses[inController] != Response)
		{ 
			CurrentSyncResponses[inController] = Response;
			CurrentSyncResponsePriorities[inController] = ResponsePriority;
			OnResponsesUpdated(inController);
			FString CurrentResponse = CheckSyncResponses();
			if (!CurrentResponse.IsEmpty() && DialogCallBack)
			{
				DialogCallBack(CurrentResponse);
			}
		}
	}
	else if(!bIsSyncDialog && DialogCallBack)
	{ 
		DialogCallBack(Response);
		OnResponsed(Response);
	}
}

void UNebulaFlowDialog::InitDialog(APlayerController* InControllerOwner, FName DialogID, TFunction<void(FString)> InCallBack, const TArray<ANebulaFlowPlayerController*>& SyncControllers, const TArray<FText>& Params, UTexture2D* Image, bool ibForceFocus, TObjectPtr<UInputMappingContext> InputMappingContext)
{
	ControllerOwner = InControllerOwner;
	DialogCallBack = InCallBack;
	bForceFocus = ibForceFocus;
	if (SyncControllers.Num() > 0)
	{
		bIsSyncDialog = true;
		for (ANebulaFlowPlayerController* pCurrent : SyncControllers)
		{
			CurrentSyncResponses.Add(pCurrent,FString(""));
			CurrentSyncResponsePriorities.Add(pCurrent,-1);
		}
	}
	//ensure(DialogText);
	if (DialogText)
	{
		UNebulaFlowUIConstants* UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(this); 
		if (UIConstants)
		{
			const UDataTable* DialogConfTable = UIConstants->GetDialogConfigurationsTable();
			ensureAlways(DialogConfTable);
			if (DialogConfTable)
			{
				FDialogConfigTableRow* Row = DialogConfTable->FindRow<FDialogConfigTableRow>(DialogID,FString("General"));
				ensure(Row);
				if (Row)
				{
					CachedDialogConfiguration = Row->DialogConfiguration;
					FText Template = RetrieveDialogText();

					if (Params.Num() > 0)
					{
						ShowFormattedText(Template, Params[0]);
					}
					else
					{
						DialogText->SetText(Template);
					}

					if (CachedDialogConfiguration.bHasSecondaryText && SecondaryDialogText)
					{
						SecondaryDialogText->SetText(RetrieveDialogText(false));
					}
					if (AdditionalImage)
					{
						if (Image)
						{
							SetAdditionImage(Image);
						}
						else if (CachedDialogConfiguration.AdditionalTexture)
						{
							SetAdditionImage(CachedDialogConfiguration.AdditionalTexture);
						}
					}
					if (ButtonsContainer)
					{
						for (FDialogButtonConfiguration currentConf : CachedDialogConfiguration.ButtonConfigurations)
						{
							UNebulaFlowDialogButton* newButton = CreateWidget<UNebulaFlowDialogButton>(ControllerOwner, currentConf.DialogButtonClass);
							if (newButton)
							{
								newButton->InitializeDialogButton(this, ControllerOwner, currentConf, CachedDialogConfiguration.DialogType, SyncControllers);
								ButtonsContainer->AddChild(newButton);
							}
						}
					}

					UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), OpenSoundID);
				}
				else
				{
					UE_LOG(LogNebulaFlowUI, Warning, TEXT("Dialog ID Not Found in Dialog Configuration Table"));
				}
			}
			else
			{
				UE_LOG(LogNebulaFlowUI, Warning, TEXT("No Dialog Configuration Defined in UI Constants"));
			}
		}

	}
}

void UNebulaFlowDialog::UninitDialog()
{
	DialogCallBack = nullptr;
	//RestoreFocus();
}

void UNebulaFlowDialog::SetDialogText(FText inText)
{
	if (DialogText)
	{
		DialogText->SetText(inText);
	}
}

void UNebulaFlowDialog::SetAdditionImage(TSoftObjectPtr<UTexture2D> Image)
{
	if (AdditionalImage)
	{
		AdditionalImage->SetBrushFromSoftTexture(Image);
		AdditionalImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}
void UNebulaFlowDialog::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bForceFocus)
	{
		FSlateApplication::Get().SetAllUserFocus(this->TakeWidget());
	}
}

FReply UNebulaFlowDialog::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FNebulaFlowCoreDelegates::OnAnyInputPressed.Broadcast(InKeyEvent.GetKey(), InKeyEvent.GetKey().IsGamepadKey(), UGameplayStatics::GetPlayerControllerFromID(this, InKeyEvent.GetUserIndex()));
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UNebulaFlowDialog::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FNebulaFlowCoreDelegates::OnAnyInputReleased.Broadcast(InKeyEvent.GetKey(), InKeyEvent.GetKey().IsGamepadKey(), UGameplayStatics::GetPlayerControllerFromID(this, InKeyEvent.GetUserIndex()));
	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

void UNebulaFlowDialog::RestoreFocus()
{
	/*
	for (TPair<APlayerController*, TSharedPtr<class SWidget>> OldFocusWidget : FocusToRestore)
	{
		if (OldFocusWidget.Value.IsValid())
		{			
			FSlateApplication::Get().SetUserFocus(OldFocusWidget.Key->GetLocalPlayer()->GetControllerId(), OldFocusWidget.Value, EFocusCause::SetDirectly);
		}
	}
	*/
}

FString UNebulaFlowDialog::CheckSyncResponses()
{
	if (CurrentSyncResponses.Num() > 0)
	{
		TArray<ANebulaFlowPlayerController*> MapKeys;
		CurrentSyncResponses.GetKeys(MapKeys);
		if(MapKeys.Num()>0)
		{ 
			FString ActualResponse = CurrentSyncResponses[MapKeys[0]];
			int32 ActualResponsePriority = CurrentSyncResponsePriorities[MapKeys[0]];
			for (ANebulaFlowPlayerController* currentController : MapKeys)
			{
				if (CurrentSyncResponses[currentController].IsEmpty() && CachedDialogConfiguration.bNeedAllPlayersAnswer)
				{				
					ActualResponse = FString("");
					break;
				}
				else if(CurrentSyncResponses[currentController]!=(ActualResponse) && CachedDialogConfiguration.bMustPlayersAgree)
				{
					ActualResponse = FString("");
					break;
				}
				else if (CurrentSyncResponsePriorities[currentController] > ActualResponsePriority)
				{
					ActualResponse = CurrentSyncResponses[currentController];
					ActualResponsePriority = CurrentSyncResponsePriorities[currentController];
				}
			}
			return ActualResponse;
		}
	}
	return FString("");
}

FText UNebulaFlowDialog::RetrieveDialogText(bool bIsMainText /*= true*/)
{
	FText currentText = bIsMainText ? CachedDialogConfiguration.DialogText : CachedDialogConfiguration.SecondaryDialogText;

	TMap<EPlatformTargets,FText>& currentMapTexts = bIsMainText ? CachedDialogConfiguration.PlatformDialogTexts : CachedDialogConfiguration.PlatformSecondaryDialogTexts;

#if PLATFORM_PS4 || PLATFORM_PS5	

	if(currentMapTexts.Contains(EPlatformTargets::ESony))
	{
		currentText = currentMapTexts[EPlatformTargets::ESony];
	}

#elif PLATFORM_WINGDK || PLATFORM_XBOXONE || PLATFORM_XSX

	if (currentMapTexts.Contains(EPlatformTargets::EMicrosoft))
	{
		currentText = currentMapTexts[EPlatformTargets::EMicrosoft];
	}

#elif PLATFORM_SWITCH

	if (currentMapTexts.Contains(EPlatformTargets::ENintendo))
	{
		currentText = currentMapTexts[EPlatformTargets::ENintendo];
	}

#elif PLATFORM_WINDOWS

	if (currentMapTexts.Contains(EPlatformTargets::ESteam))
	{
		currentText = currentMapTexts[EPlatformTargets::ESteam];
	}

#endif

	return currentText;
	
}

