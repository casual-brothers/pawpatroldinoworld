// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Customization/PDWCollectionWidget.h"
#include "UI/Widgets/Customization/PDWCollectionElement.h"
#include "BlueprintGameplayTagLibrary.h"
#include "Components/PanelWidget.h"
#include "Managers/PDWCustomizationSubsystem.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"
#include "Components/TextBlock.h"

void UPDWCollectionWidget::InitCollection(const FCollectionDataArray& CollectionList, FGameplayTag StartingElement, FName InSelectSoundId)
{
	CollectionContainer->ClearChildren();
	CurrentCollection.Empty();
	SelectSoundID = InSelectSoundId;
	if (CollectionList.CollectionData.Num() <= 0)
	{
		return;
	}
	if (!UBlueprintGameplayTagLibrary::IsGameplayTagValid(StartingElement))
	{
		StartingElement = CollectionList.CollectionData[0].ID;
	}
	for (int32 i = 0; i < CollectionList.CollectionData.Num(); i++)
	{
		UPDWCollectionElement* CollectionElementWidget = CreateWidget<UPDWCollectionElement>(this, CollectionElementClass);
		CollectionContainer->AddChild(CollectionElementWidget);
		CollectionElementWidget->InitCollectionElement(CollectionList.CollectionData[i]);
		if (CollectionList.CollectionData[i].ID == StartingElement)
		{
			CollectionElementWidget->ActiveCollectionElement(true); 
			BP_OnActiveElementChange(CollectionElementWidget);
			CurrentIndex = i;
		}
		CurrentCollection.Add(CollectionElementWidget);
	}
}

void UPDWCollectionWidget::MoveCollectionElements(int32 NumMove)
{
	if (CurrentCollection.Num() <= 1)
	{
		return;
	}
	CurrentCollection[CurrentIndex]->ActiveCollectionElement(false);
	CurrentIndex += NumMove;
	CurrentIndex = (CurrentIndex % CurrentCollection.Num() + CurrentCollection.Num()) % CurrentCollection.Num();
	CurrentCollection[CurrentIndex]->ActiveCollectionElement(true);
	BP_OnActiveElementChange(CurrentCollection[CurrentIndex]);
	if (SelectSoundID != FName())
	{
		UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), SelectSoundID);
	}
}

void UPDWCollectionWidget::SelectCurrentElement()
{
	if (CurrentCollection.Num() == 0)
	{
		return;
	}
	CurrentCollection[CurrentIndex]->OnElementSelected();
	UNebulaFlowCoreFunctionLibrary::TriggerAction(this, CustomActionOnElementSelected.ToString(), CurrentCollection[CurrentIndex]->GetIsUnlocked() ? CurrentCollection[CurrentIndex]->GetElementID().ToString() : "", GetOwningPlayer());
}

void UPDWCollectionWidget::InitCollectionName(const FText& Name)
{
	if (CollectionName)
	{
		CollectionName->SetText(Name);
	}
}