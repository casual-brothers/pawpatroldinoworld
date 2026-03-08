// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Pages/PDWCustomizationPage.h"
#include "UI/Widgets/Customization/PDWCollectionWidget.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Managers/PDWCustomizationSubsystem.h"
#include "Managers/PDWPaleoCenterSubsystem.h"
#include "Components/TextBlock.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Data/PDWPaleoCenterSettings.h"
#include "FunctionLibraries/PDWUIFunctionLibrary.h"

void UPDWCustomizationPage::InizializeDataPup(APDWPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	CustomizationNavButtonP1->BP_InitButton(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this)->GetIsOnVehicle());
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		CustomizationNavButtonP2->BP_InitButton(UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this)->GetIsOnVehicle());
	}

	UPDWCollectionWidget* CustomizationCollectionToChange = CustomizationCollection;
	UPDWCollectionWidget* PupCollectionToChange = PupCollection;

	if (UPDWGameplayFunctionLibrary::IsSecondPlayer(PlayerController))
	{
		CustomizationCollectionToChange = CustomizationCollectionP2;
		PupCollectionToChange = PupCollectionP2;
	}

	const TMap<FGameplayTag, FCustomizationPupData>& CustomizationFullList = UPDWCustomizationSubsystem::Get(this)->GetCustomizationPupDataMap();
	TArray<FGameplayTag> Pups;
	CustomizationFullList.GetKeys(Pups);

	FGameplayTag CurrentActorTag = PlayerController->GetCurrentPup();
	FCustomizationData CustmozationData = UPDWDataFunctionLibrary::GetCustomizationData(this);
	

	if (CustomizationCollectionToChange)
	{
		CustomizationCollectionToChange->SetOwningPlayer(PlayerController);
		TMap<FGameplayTag, int32> Inventory = UPDWDataFunctionLibrary::GetInventory(this);
		TSet<FGameplayTag> InventoryTags;
		Inventory.GetKeys(InventoryTags);

		if (PlayerController->GetIsOnVehicle())
		{
			TMap<FGameplayTag, FGameplayTag> VehiclesCurrentCustmization = UPDWGameplayFunctionLibrary::IsSecondPlayer(PlayerController) ? CustmozationData.GuestVehicleCustomizations : CustmozationData.VehicleCustomizations;
			FCollectionDataArray VehicleCustmizationData = CustomizationFullList[CurrentActorTag].VehicleCustomizationData;

			if (VehicleCustmizationData.CollectionData.IsEmpty())
			{
				return;
			}

			for(int32 i = VehicleCustmizationData.CollectionData.Num() - 1; i > 0; i--)
			{
				VehicleCustmizationData.CollectionData[i].bIsUnlocked = InventoryTags.Contains(VehicleCustmizationData.CollectionData[i].ID);
			}

			FGameplayTag StartingVehicleCustomization = VehiclesCurrentCustmization.Contains(CurrentActorTag) ? VehiclesCurrentCustmization[CurrentActorTag] : CustomizationFullList[CurrentActorTag].VehicleCustomizationData.CollectionData[0].ID;
			CustomizationCollectionToChange->InitCollection(VehicleCustmizationData, StartingVehicleCustomization, SelectSoundVehicle);
		}
		else
		{
			TMap<FGameplayTag, FGameplayTag> PupCurrentCustmization = UPDWGameplayFunctionLibrary::IsSecondPlayer(PlayerController) ? CustmozationData.GuestPupCustomizations : CustmozationData.PupCustomizations;
			FCollectionDataArray PupCustmizationData = CustomizationFullList[CurrentActorTag].PupCustomizationData;

			if (PupCustmizationData.CollectionData.IsEmpty())
			{
				return;
			}

			for(int32 i = PupCustmizationData.CollectionData.Num() - 1; i > 0; i--)
			{
				PupCustmizationData.CollectionData[i].bIsUnlocked = InventoryTags.Contains(PupCustmizationData.CollectionData[i].ID);
			}

			FGameplayTag StartingPupCustomization = PupCurrentCustmization.Contains(CurrentActorTag) ? PupCurrentCustmization[CurrentActorTag] : CustomizationFullList[CurrentActorTag].PupCustomizationData.CollectionData[0].ID;
			CustomizationCollectionToChange->InitCollection(PupCustmizationData, StartingPupCustomization, SelectSoundPup);
		}
		CustomizationCollectionToChange->InitCollectionName(UPDWUIFunctionLibrary::GetCharacterInfoByTag(this, CurrentActorTag).DisplayCharacterName);
	}

	if (PupCollectionToChange)
	{
		FCollectionDataArray NewPupDataArray;
		for (FGameplayTag PupTag : Pups)
		{
			FCollectionData NewPupData;
			NewPupData.ID = PupTag;
			NewPupData.CustomData = CustomizationFullList[PupTag].PupUIData;
			NewPupDataArray.CollectionData.Add(NewPupData);
		}

		PupCollectionToChange->SetOwningPlayer(PlayerController);
		PupCollectionToChange->InitCollection(NewPupDataArray, CurrentActorTag);
	}
}

void UPDWCustomizationPage::SwitchPupToVehicle(APDWPlayerController* PlayerController)
{
	InizializeDataPup(PlayerController);

	if (UPDWGameplayFunctionLibrary::IsSecondPlayer(PlayerController))
	{
		CustomizationNavButtonP2->BP_SwitchVechicleToPup(PlayerController->GetIsOnVehicle());
	}
	else
	{
		CustomizationNavButtonP1->BP_SwitchVechicleToPup(PlayerController->GetIsOnVehicle());
	}
}

void UPDWCustomizationPage::InizializeDataDino(FGameplayTag DinoTag)
{
	FCustomizationDinoData DinoData = UPDWCustomizationSubsystem::Get(this)->GetCustomizationDinoDataByTag(DinoTag);
	TMap<FGameplayTag, int32> Inventory = UPDWDataFunctionLibrary::GetInventory(this);
	TSet<FGameplayTag> InventoryTags;
	Inventory.GetKeys(InventoryTags);

	for(int32 i = DinoData.DinoCustomizationData.CollectionData.Num() - 1; i > 0; i--)
	{
		if (!InventoryTags.Contains(DinoData.DinoCustomizationData.CollectionData[i].ID))
		{
			DinoData.DinoCustomizationData.CollectionData[i].bIsUnlocked = false;
		}
	}

	DinoCollection->SetOwningPlayer(GetOwningPlayer());
	FGameplayTag SavedCustomization = UPDWDataFunctionLibrary::GetDinoCustomization(this, DinoTag);
	FGameplayTag DefaultCustomization = UPDWPaleoCenterSettings::Get()->DefaultDinoCustomizationTag;
	DinoCollection->InitCollection(DinoData.DinoCustomizationData, SavedCustomization != FGameplayTag::EmptyTag ? SavedCustomization : DefaultCustomization, SelectSoundDino);


	UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>();
	DinoCollection->InitCollectionName(PaleoCenterSubsystem->GetDinoPenInfo(DinoTag).DinoDisplayName);
}