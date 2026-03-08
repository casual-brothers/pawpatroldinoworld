// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PDWHUDSubsystem.h"
#include "UI/HUD/PDWGameplayHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Data/PDWGameSettings.h"
#include "UI/HUD/PDWMinigameHUD.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "Managers/PDWPaleoCenterSubsystem.h"
#include "Data/PDWPaleoCenterSettings.h"
#include "Data/PDWGameplayTagSettings.h"
#include "UI/Widgets/PDWSightWidget.h"
#include "GameplayTagContainer.h"


void UPDWHUDSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	if (UPDWEventSubsytem* EventSubsystem = InWorld.GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnPenAreaBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnPenAreaBeginOverlapp);
		EventSubsystem->OnPenAreaEndOverlap.AddUniqueDynamic(this, &ThisClass::OnPenAreaEndOverlap);
	}
}

void UPDWHUDSubsystem::Deinitialize()
{
	if (GetWorld()->IsGameWorld())
	{
		if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
		{
			EventSubsystem->OnPenAreaBeginOverlap.RemoveDynamic(this, &ThisClass::OnPenAreaBeginOverlapp);
			EventSubsystem->OnPenAreaEndOverlap.RemoveDynamic(this, &ThisClass::OnPenAreaEndOverlap);
		}
	}
	Super::Deinitialize();
}

UPDWHUDSubsystem* UPDWHUDSubsystem::Get(UObject* WorldContextObject)
{
    return WorldContextObject->GetWorld()->GetSubsystem<UPDWHUDSubsystem>();
}

void UPDWHUDSubsystem::ManageHUD(const EHUDControls& Control, EHUDType Hudtype)
{
	TArray<APlayerController*> Players {};

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			Players.Add(PC);
		}
	}

	switch (Control)
	{
	case EHUDControls::Create:
		if (CurrentHUD && CurrentHUD->GetHUDType() != Hudtype)
		{
			if (Hudtype == CurrentHUD->GetHUDType())
			{
				CurrentMinigameData = nullptr;
			}
			CurrentHUD->RemoveFromParent();
			CurrentHUD = nullptr;
		}
		
		if (!CurrentHUD)
		{
			CurrentHUD = CreateWidget<UPDWBaseHUD>(Players[0], UPDWGameSettings::GetHUDClass(Hudtype));
			OnHUDReady.Broadcast();
		}
		
		if (CurrentHUD)
		{
			CurrentHUD->InitHUD(Players);
			CurrentHUD->AddToViewport();
			CurrentHUD->SetVisibility(ESlateVisibility::HitTestInvisible);
			if (Hudtype == EHUDType::Minigame && CurrentMinigameData)
			{
				RequestMinigameData(CurrentMinigameData);
				ShowSmellTarget(CurrentSmellTargetMinigame);
				CurrentSmellTargetMinigame.Reset();
			}
			if (ConversationToPlay.ConversationType != EConversationType::None)
			{
				RequestHUDDialogue(ConversationToPlay);
				if (!ConversationToPlay.bLoopConversation)
				{
					ConversationToPlay = FConversation();
				}
			}
			if (CurrentHUD->GetHUDType() == EHUDType::Gameplay)
			{
				UPDWGameplayHUD* GameplayHUD = Cast<UPDWGameplayHUD>(CurrentHUD);
				if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
				{
					for (TPair<APDWPlayerController*, FGameplayTag> CurrentPenPlayer : CurrentPlayersInPenArea)
					{
						GameplayHUD->ManageEnterOverlappPen(PaleoCenterSubsystem->GetDinoPenInfo(CurrentPenPlayer.Value), CurrentPenPlayer.Key);
					}
				}
			}
		}

		break;
	case EHUDControls::Hide:
		if (CurrentHUD)
		{
			CurrentHUD->HideHUD();
			CurrentHUD->SetVisibility(ESlateVisibility::Collapsed);
		}
		break;
	case EHUDControls::Destroy:
		if (CurrentHUD)
		{
			CurrentHUD->RemoveFromParent();
			CurrentHUD = nullptr;
			if (Hudtype == EHUDType::Minigame)
			{
				CurrentMinigameData = nullptr;
			}
		}
		break;
	case EHUDControls::Show:
		if (CurrentHUD)
		{
			CurrentHUD->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		break;
	}
}

void UPDWHUDSubsystem::RequestToggleSwapCharacter(APlayerController* Sender, bool Open, bool WithSelection)
{
	UPDWGameplayHUD* GameplayHud = nullptr;
	if (CurrentHUD && CurrentHUD->IsVisible())
	{
		GameplayHud = Cast<UPDWGameplayHUD>(CurrentHUD);
	}
	if (!GameplayHud) return;

	GameplayHud->ManageRequestCharacterSelector(Sender, Open, WithSelection);
	if (!Open)
	{
		UPDWEventSubsytem::Get(this)->OnSwapCharacterMenuClosedEvent();
	}
}

void UPDWHUDSubsystem::RequestInputActionIcon(APlayerController* Sender, FInstancedStruct& InputActionToShow, FNavButtonTriggerCallback InTriggerCallBack, FNavButtonProgressCallback InProgressCallBack)
{
	if (CurrentHUD)
	{
		CurrentHUD->ManageInputActionIcon(Sender, InputActionToShow, InTriggerCallBack, InProgressCallBack);
	}
}

void UPDWHUDSubsystem::RequestSightInputActionIcon(APlayerController* Sender, FInstancedStruct& InputActionToShow, FNavButtonTriggerCallback InTriggerCallBack /*= FNavButtonTriggerCallback()*/, FNavButtonProgressCallback InProgressCallBack /*= FNavButtonProgressCallback()*/)
{
	for (UPDWSightWidget* SingleSight : Sights)
	{
		if (SingleSight->GetOwningPlayer() == Sender)
		{
			SingleSight->StartMinigameInputActionIcon(InputActionToShow, InTriggerCallBack, InProgressCallBack);
			break;
		}
	}
}

void UPDWHUDSubsystem::BP_RequestInputActionIcon(APlayerController* Sender, FInstancedStruct& InputActionToShow)
{
	RequestInputActionIcon(Sender,InputActionToShow);
}

void UPDWHUDSubsystem::RequestMinigameData(UPDWMinigameConfigData* MinigameData)
{
	if (MinigameData)
	{
		CurrentMinigameData = MinigameData;
		if (CurrentHUD)
		{
			UPDWMinigameHUD* MinigameHUD = Cast<UPDWMinigameHUD>(CurrentHUD);
			if (MinigameHUD)
			{
				MinigameHUD->InitWithMinigameData(CurrentMinigameData);
			}
		}
	}
}

void UPDWHUDSubsystem::RequestHUDDialogue(const FConversation& Conversation)
{
	if(!CurrentHUD || Conversation.bLoopConversation)
	{
		ConversationToPlay = Conversation;
	}
	if (CurrentHUD)
	{
		CurrentHUD->ManageTriggerDialogue(Conversation);
	}
}

void UPDWHUDSubsystem::EndCurrentHUDDialogue()
{
	if (ConversationToPlay.bLoopConversation)
	{
		ConversationToPlay = FConversation();
	}
	if (CurrentHUD)
	{
		CurrentHUD->ManageEndCurrentDialogue();
	}
}

void UPDWHUDSubsystem::RequestTutorialInputAction(const FPDWTutorialButtonData& TutorialButtonData)
{
	if (CurrentHUD)
	{
		CurrentHUD->ManageTutorialInputIcon(TutorialButtonData);
	}
}

void UPDWHUDSubsystem::EndCurrentTutorialInputAction()
{
	if (CurrentHUD)
	{
		CurrentHUD->ManageEndTutorialInputIcon();
	}
}

void UPDWHUDSubsystem::CheckHUDWidgetsConfiguration()
{
	if (CurrentHUD)
	{
		CurrentHUD->BP_CheckHUDConfiguration();
	}
}

UPDWSightWidget* UPDWHUDSubsystem::CreateSight(const TSubclassOf<UPDWSightWidget> SightToCreateClass, APDWPlayerController* OwnerWidget)
{
	if (!SightToCreateClass)
	{
		return nullptr;
	}

	UPDWSightWidget* NewSight = CreateWidget<UPDWSightWidget>(GetWorld(), SightToCreateClass);
	NewSight->InitSight(OwnerWidget);
	Sights.Add(NewSight);
	if (NewSight)
	{
		NewSight->AddToViewport(2);
	}

	return NewSight;
}

void UPDWHUDSubsystem::RemoveSight(APDWPlayerController* OwnerWidget)
{
	TArray<UPDWSightWidget*> SightToRemove {};
	for (UPDWSightWidget* Sight : Sights)
	{
		if (Sight->GetOwningPlayer() == OwnerWidget)
		{
			Sight->RemoveFromParent();
			SightToRemove.Add(Sight);
		}
	}
	for (UPDWSightWidget* SingleSight : SightToRemove)
	{
		Sights.Remove(SingleSight);
	}
}

void UPDWHUDSubsystem::RestoreSight(APDWPlayerController* OwnerWidget)
{
	for (UPDWSightWidget* Sight : Sights)
	{
		if (Sight->GetOwningPlayer() == OwnerWidget)
		{
			Sight->RestoreSight();
			break;
		}
	}
}

#pragma region Paleocenter

void UPDWHUDSubsystem::OnPenAreaBeginOverlapp(FPDWDinoPenInfo PenInfo, AActor* OverlappedActor)
{
	if (APawn* OverlappedPawn = Cast<APawn>(OverlappedActor))
	{
		APDWPlayerController* PlayerController = Cast<APDWPlayerController>(OverlappedPawn->GetController());
		if (!PlayerController)
			return;

		if (CurrentPlayersInPenArea.Contains(PlayerController))
		{
			CurrentPlayersInPenArea[PlayerController] = PenInfo.DinoPenTag;
		}
		else
		{
			CurrentPlayersInPenArea.Add(PlayerController, PenInfo.DinoPenTag);
		}
		if (PenInfo.DinoNeed.DinoNeed != UPDWGameplayTagSettings::Get()->GetHappyDino())
		{
			PenNidoNeedMessage(PenInfo.DinoNeed);
		}

		UPDWEventSubsytem::Get(this)->OnDinoNeedUpdate.AddUniqueDynamic(this, &ThisClass::PenNidoNeedMessage);

		UPDWGameplayHUD* GameplayHUD = Cast<UPDWGameplayHUD>(CurrentHUD);
		if (GameplayHUD && GameplayHUD->GetHUDType() == EHUDType::Gameplay)
		{
			GameplayHUD->ManageEnterOverlappPen(PenInfo, PlayerController);
		}
	}
}

void UPDWHUDSubsystem::OnPenAreaEndOverlap(AActor* OverlappedActor)
{
	if (APawn* OverlappedPawn = Cast<APawn>(OverlappedActor))
	{
		APDWPlayerController* PlayerController = Cast<APDWPlayerController>(OverlappedPawn->GetController());
		if (!PlayerController)
			return;

		CurrentPlayersInPenArea.Remove(PlayerController);
		if (CurrentPlayersInPenArea.Num() == 0)
		{
			UPDWEventSubsytem::Get(this)->OnDinoNeedUpdate.RemoveDynamic(this, &ThisClass::PenNidoNeedMessage);
		}

		UPDWGameplayHUD* GameplayHUD = Cast<UPDWGameplayHUD>(CurrentHUD);
		if (GameplayHUD && GameplayHUD->GetHUDType() == EHUDType::Gameplay)
		{
			GameplayHUD->ManageExitOverlappPen(PlayerController);
		}
	}
}

void UPDWHUDSubsystem::PenNidoNeedMessage(FPDWDinoPenNeed DinoNeed)
{
	if (DinoNeed.DinoNeed != UPDWGameplayTagSettings::Get()->GetHappyDino() && (!DinoNeed.bDinoWaitingPlayerHelp))
	{
		return;
	}
	UPDWDinoNeedConversationData* ConversationData = Cast<UPDWDinoNeedConversationData>(UPDWPaleoCenterSettings::Get()->DinoNeedConversationConfig.TryLoad());
	if (ConversationData)
	{
		FGameplayTagContainer TagToSearch;
		TagToSearch.AddTag(DinoNeed.DinoNeed);
		if (DinoNeed.DinoNeed == UPDWGameplayTagSettings::Get()->GetHungryDino())
		{
			if (DinoNeed.bPlayerHaveTheItem)
			{
				TagToSearch.RemoveTag(DinoNeed.DinoNeed);
				TagToSearch.AddTag(UPDWGameplayTagSettings::Get()->GetHungryDinoWithItem());
			}
			TagToSearch.AddTag(DinoNeed.NeededItem);
		}
		if (FConversation* Conversation = ConversationData->GetConversationByTagContainer(TagToSearch))
		{
			Conversation->ConversationType = EConversationType::HUDComment;
			RequestHUDDialogue(*Conversation);
		}
	}
}

#pragma endregion

void UPDWHUDSubsystem::ShowSmellTarget(FGameplayTagContainer MinigameID)
{
	if (CurrentHUD)
	{
		UPDWMinigameHUD* MinigameHUD = Cast<UPDWMinigameHUD>(CurrentHUD);
		if (MinigameHUD)
		{
			MinigameHUD->ShowSmellTarget(MinigameID);
		}
	}
	else
	{
		CurrentSmellTargetMinigame = MinigameID;
	}
}