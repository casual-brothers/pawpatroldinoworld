// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DinoPen/PDWDinoPenCollection.h"
#include "UI/Widgets/DinoPen/PDWDinoPenCollectionElement.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

void UPDWDinoPenCollection::InitDinoPenByEggs(FPDWDinoPenEggs EggsInfo)
{
	IsBabyDinoBorn = EggsInfo.IsBabyDinoBorn;
	InitElement(EggsInfo.MaxEggs, EggsInfo.EggCollected, EggsInfo.EggId);
}

void UPDWDinoPenCollection::InitDinoPenByDecorations(FPDWDinoPenDecorations DecorationInfo)
{
	InitElement(DecorationInfo.MaxDecorations, DecorationInfo.DecorationsUnlocked, DecorationInfo.DinoId);
}

void UPDWDinoPenCollection::InitElement(int32 MaxValue, int32 CurrentValue, FGameplayTag ElementType)
{
	CollectionContainer->ClearChildren();
	for (int32 i = 0; i < MaxValue; i++)
	{
		UPDWDinoPenCollectionElement* Element = CreateWidget<UPDWDinoPenCollectionElement>(this, ColectionElementClass);
		CollectionContainer->AddChild(Element);
		Element->BP_InitElement(ElementType);
	}

	for (int32 i = 0; i < CurrentValue; i++)
	{
		UWidget* WidgetElement = CollectionContainer->GetChildAt(i);
		if (UPDWDinoPenCollectionElement* Element = Cast<UPDWDinoPenCollectionElement>(WidgetElement))
		{
			Element->BP_ActiveElement(true);
		}
	}

	BP_InitElement(MaxValue, CurrentValue, ElementType);
}

void UPDWDinoPenCollection::SetDinoName(FText DinoName)
{
	if (DinoText)
	{
		DinoText->SetText(FText::Format(DinoSentence, DinoName));
	}
}