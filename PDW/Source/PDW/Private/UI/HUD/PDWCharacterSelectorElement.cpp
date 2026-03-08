// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/PDWCharacterSelectorElement.h"
#include "Components/Button.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"

void UPDWCharacterSelectorElement::NativeConstruct()
{
	Super::NativeConstruct();
	CharButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButtonClicked);
}

void UPDWCharacterSelectorElement::NativeDestruct()
{
	CharButton->OnClicked.RemoveDynamic(this, &ThisClass::OnButtonClicked);
	Super::NativeDestruct();
}

void UPDWCharacterSelectorElement::SetIsBlocked(bool Block)
{
	if (bIsBlocked != Block)
	{
		bIsBlocked = Block;
		BlockWidget(Block);
	}
}

void UPDWCharacterSelectorElement::SetIsHighlighted(bool Highlight)
{
	if (bIsHighlighted != Highlight)
	{
		bIsHighlighted = Highlight;
		HighlightWidget(Highlight);
	}
}

void UPDWCharacterSelectorElement::OnButtonClicked()
{
	if (!bIsBlocked)
	{
		UNebulaFlowCoreFunctionLibrary::TriggerAction(this, OnClickButtonAction.ToString(), "", GetOwningPlayer());
	}
}