// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Customization/PDWCollectionElement.h"
#include "Managers/PDWCustomizationSubsystem.h"

void UPDWCollectionElement::InitCollectionElement(const FCollectionData& CollectionData)
{
	IDElementTag = CollectionData.ID;
	bIsUnlocked = CollectionData.bIsUnlocked;
	BP_InitCollectionElement(CollectionData);
}

void UPDWCollectionElement::ActiveCollectionElement(bool Active)
{
	if (bIsActive != Active)
	{
		bIsActive = Active;
		BP_ActiveCollectionElement(Active);
	}
}

void UPDWCollectionElement::OnElementSelected()
{
	BP_OnElementSelected();
}