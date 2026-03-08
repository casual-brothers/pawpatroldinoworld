// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PDWCustomizationSubsystem.h"
#include "Data/PDWGameSettings.h"
#include "Data/PDWGraphicCustomizationComponent.h"
#include "Data/PDWCustomizationSettings.h"
#include "Gameplay/Customization/PDWCustomizationLocation.h"
#include "Managers/PDWPaleoCenterSubsystem.h"
#include "Gameplay/Actors/PDWDinoPen.h"
#include "Data/PDWPaleoCenterSettings.h"

UPDWCustomizationSubsystem* UPDWCustomizationSubsystem::Get(UObject* WorldContextObject)
{
    return WorldContextObject->GetWorld()->GetSubsystem<UPDWCustomizationSubsystem>();
}

void UPDWCustomizationSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	FillCustomizationData();
	FillCustomizationLocation(UPDWCustomizationSettings::GetPupP1CustomizationLocationTag(), PupP1CustomizationLocation);
	FillCustomizationLocation(UPDWCustomizationSettings::GetPupP1MultiplayerCustomizationLocationTag(), PupP1MultiplayerCustomizationLocation);
	FillCustomizationLocation(UPDWCustomizationSettings::GetPupP2CustomizationLocationTag(), PupP2CustomizationLocation);
	FillCustomizationLocation(UPDWCustomizationSettings::GetVehicleP1CustomizationLocationTag(), VehicleP1CustomizationLocation);
	FillCustomizationLocation(UPDWCustomizationSettings::GetVehicleP1MultiplayerCustomizationLocationTag(), VehicleP1MultiplayerCustomizationLocation);
	FillCustomizationLocation(UPDWCustomizationSettings::GetVehicleP2CustomizationLocationTag(), VehicleP2CustomizationLocation);
	FillDinoCustomizationLocations();
}

FCustomizationDinoData UPDWCustomizationSubsystem::GetCustomizationDinoDataByTag(FGameplayTag TagToSearch)
{
	if (CustomizationDinoDataMap.Contains(TagToSearch))
	{
		return CustomizationDinoDataMap[TagToSearch];
	}
	return FCustomizationDinoData();
}

void UPDWCustomizationSubsystem::FillCustomizationData()
{
	//Fill Pup Info
	TMap<FGameplayTag, FEntity> EntityConfig;
	UPDWGameSettings::GetEntityConfig(EntityConfig);
	
	for (auto &[PupTag, PupInfo] : EntityConfig)
	{
		FCustomizationPupData NewCustmizzationData;
		NewCustmizzationData.PupUIData.InitializeAs<FPupUIInfo>(PupInfo.PupUIInfo);
		//EVENTUALLY make it async load
		TArray<FPDWUICustomization> StructToFill;
		UPDWCustomizationDataAsset* CustomizationDataAsset = Cast<UPDWCustomizationDataAsset>(PupInfo.CustomizationInfo.CustomizationAsset.TryLoad());
		if (CustomizationDataAsset)
		{
			StructToFill = CustomizationDataAsset->UICustomizationList;
			for (FPDWUICustomization& BaseCustomization : StructToFill)
			{
				FCollectionData NewCollectionData;
				NewCollectionData.ID = BaseCustomization.ID;
				NewCollectionData.CustomData = BaseCustomization.UICustomizationData.UIData;
				if (BaseCustomization.ID.MatchesTag(UPDWCustomizationSettings::GetPupCustomizationTag()))
				{
					NewCustmizzationData.PupCustomizationData.CollectionData.Add(NewCollectionData);
				}
				else if(BaseCustomization.ID.MatchesTag(UPDWCustomizationSettings::GetVehicleCustomizationTag()))
				{
					NewCustmizzationData.VehicleCustomizationData.CollectionData.Add(NewCollectionData);
				}
			}
		}

		CustomizationPupDataMap.Add(PupTag, NewCustmizzationData);
	}
	
	//Fill Dino Info
	if(UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		TArray<UPDWDinoPen*> DinoPens = PaleoCenterSubsystem->GetDinoPens();
		for (UPDWDinoPen* Pen : DinoPens)
		{
			FCustomizationDinoData DinoData;
			TArray<FPDWUICustomization> StructToFill;
			if (UPDWPaleoCenterSettings::Get()->DinoCustomizationAsset.Contains(Pen->DinoPenTag))
			{
				UPDWCustomizationDataAsset* CustomizationDataAsset = Cast<UPDWCustomizationDataAsset>(UPDWPaleoCenterSettings::Get()->DinoCustomizationAsset[Pen->DinoPenTag].TryLoad());
				if (CustomizationDataAsset)
				{
					StructToFill = CustomizationDataAsset->UICustomizationList;
					for (FPDWUICustomization& BaseCustomization : StructToFill)
					{
						FCollectionData NewCollectionData;
						NewCollectionData.ID = BaseCustomization.ID;
						NewCollectionData.CustomData = BaseCustomization.UICustomizationData.UIData;
						DinoData.DinoCustomizationData.CollectionData.Add(NewCollectionData);
					}
				}
			}
			else
			{
				continue;
			}
			CustomizationDinoDataMap.Add(Pen->DinoPenTag, DinoData);
		}
	}
}

void UPDWCustomizationSubsystem::FillCustomizationLocation(FName Tag, APDWCustomizationLocation*& CustomizationToFill)
{
	TArray<AActor*> ActorsToFind {};
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, ActorsToFind);
	if (ActorsToFind.Num())
	{
		CustomizationToFill = Cast<APDWCustomizationLocation>(ActorsToFind[0]);
	}
}

void UPDWCustomizationSubsystem::FillDinoCustomizationLocations()
{
	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		TArray<UPDWDinoPen*> DinoPens = PaleoCenterSubsystem->GetDinoPens();
		for (UPDWDinoPen* Pen : DinoPens)
		{
			FCustomizationLocationDinoData NewLocationDinoData;
			TArray<AActor*> ActorsToFind {};
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), Pen->DinoPenTag.GetTagName(), ActorsToFind);
			if (ActorsToFind.Num())
			{
				NewLocationDinoData.DinoLocation = Cast<APDWCustomizationLocation>(ActorsToFind[0]);
			}

			FString SafeSpotTag = Pen->DinoPenTag.ToString();
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(SafeSpotTag + "SafeSpotP1"), ActorsToFind);
			if (ActorsToFind.Num())
			{
				NewLocationDinoData.SafeSpotP1 = ActorsToFind[0]->GetTransform();
			}

			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(SafeSpotTag + "SafeSpotP2"), ActorsToFind);
			if (ActorsToFind.Num())
			{
				NewLocationDinoData.SafeSpotP2 = ActorsToFind[0]->GetTransform();
			}
			
			DinoLocations.Add(Pen->DinoPenTag, NewLocationDinoData);
		}
	}
}