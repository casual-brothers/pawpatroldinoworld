// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "UI/Pages/PDWMainMenuPage.h"

#include "Data/PDWPersistentUser.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "UI/Widgets/PDWSlotButtonWidget.h"
#include "Components/Image.h"
#include "FunctionLibraries/ConsoleVarLibrary.h"
#include "BinkMediaPlayer.h"
#include "BinkMediaTexture.h"
#include "MediaPlayer.h"
#include "FileMediaSource.h"
#include "Materials/MaterialInstanceDynamic.h"

void UPDWMainMenuPage::Init(int buttonSelectedSlot)
{	
	UPDWPersistentUser* PersistentUser = UPDWGameplayFunctionLibrary::GetPDWPersistentUser(this);
	if (!ensureMsgf(PersistentUser, TEXT("Couldn't retrieve Persistent User while showing SlotPage!")))
	{
		return;
	}

	TArray<FGameProgressionSlot>& ProgressionSlots = PersistentUser->GetGameProgressionSlots();
	
	bCanContinue = false;

	FGameProgressionSlot ProgressionSlot;

	TArray<TObjectPtr<UPDWSlotButtonWidget>> SlotButtons = {BtnSlot0, BtnSlot1, BtnSlot2};

	for (uint8 i = 0; i < SlotButtons.Num(); ++i)
	{
		if (ProgressionSlots.IsValidIndex(i))
		{
			ProgressionSlot = ProgressionSlots[i];
			
			TObjectPtr<UPDWSlotButtonWidget> SlotButton = SlotButtons[i];

			if (ProgressionSlot.SlotMetaData.SlotIndex > -1)
			{
				bCanContinue = true;
				SlotButton->SetupFromSlot(ProgressionSlot);
			}
			else
			{
				SlotButton->SetupEmpty();
			}
		}
	}

	if (!bCanContinue)
	{
		BtnContinue->SetVisibility(ESlateVisibility::Collapsed);
		BtnLoadGame->SetVisibility(ESlateVisibility::Collapsed);
		
		UNebulaFlowUIFunctionLibrary::SetUserFocus(this, BtnNewGame, GetOwningPlayer(), false);
	}
	else
	{
		UNebulaFlowUIFunctionLibrary::SetUserFocus(this, BtnContinue, GetOwningPlayer(), false);
	}	
}

void UPDWMainMenuPage::SwitchToSlotButtons(bool bIsNewGame)
{
	TArray<TObjectPtr<UPDWSlotButtonWidget>> SlotButtons = {BtnSlot0, BtnSlot1, BtnSlot2};
	UPDWSlotButtonWidget* FirstAvailable = BtnSlot0;
	for (int32 i = SlotButtons.Num()-1; i>= 0; --i)
	{
		if(SlotButtons[i]->CheckEnable(bIsNewGame))
		{		
			FirstAvailable = SlotButtons[i];
		}
	}
	UNebulaFlowUIFunctionLibrary::SetUserFocus(this, FirstAvailable, GetOwningPlayer(), false);
	OnSwitchToSlots.Broadcast();
}

void UPDWMainMenuPage::SwitchToMenuButtons(bool bWasLoadGame)
{
	if (!bCanContinue || !bWasLoadGame)
	{
		UNebulaFlowUIFunctionLibrary::SetUserFocus(this, BtnNewGame, GetOwningPlayer(), false);
	}
	else
	{
		UNebulaFlowUIFunctionLibrary::SetUserFocus(this, BtnLoadGame, GetOwningPlayer(), false);
	}	
	OnSwitchToMainMenu.Broadcast();
}

void UPDWMainMenuPage::NativeConstruct()
{
	Super::NativeConstruct();

	UPDWPersistentUser* PersistentUser = UPDWGameplayFunctionLibrary::GetPDWPersistentUser(this);
	if (PersistentUser == nullptr)
		return;

	int currentSlotID = PersistentUser->GetCurrentUseSlotIndex();

	Init(currentSlotID);
	bool bSuccess = false;
	FGameplayTag LastAreaVisited = FGameplayTag::RequestGameplayTag("Map.DinoPlains");
	FGameProgressionSlot& SaveSlot = PersistentUser->GetCurrentUsedProgressionSlot(bSuccess);
	if (bSuccess)
	{
		 LastAreaVisited = SaveSlot.PlayerData.LastVisitedAreaId;
	}

		
#if PLATFORM_SWITCH
	UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(SwitchMaterial, this);
	BackgroundImage->SetBrushFromMaterial(DynMat);
#else
	UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(DefaultMaterial, this);
	BackgroundImage->SetBrushFromMaterial(DynMat);
#endif
	
	float Gamma = UConsoleVarLibrary::GetConsoleFloat(FName("r.TonemapperGamma"));
	DynMat->SetScalarParameterValue(FName("Gamma"), Gamma);

	if (BackgroundVideosMaterial.Contains(LastAreaVisited))
	{
#if PLATFORM_SWITCH
		auto SwitchVideo = BackgroundVideosMaterial[LastAreaVisited].SwitchVideoToPlay.LoadSynchronous();
		if(SwitchVideo)
		{
			MediaPlayer->OpenSource(SwitchVideo);
		}

#else
		UBinkMediaPlayer* Video = BackgroundVideosMaterial[LastAreaVisited].BinkVideo.LoadSynchronous();
		if (Video)
		{
			BinkMediaTexture->SetMediaPlayer(Video);
			Video->InitializePlayer();
			Video->Play();
		}
#endif
	}
}
