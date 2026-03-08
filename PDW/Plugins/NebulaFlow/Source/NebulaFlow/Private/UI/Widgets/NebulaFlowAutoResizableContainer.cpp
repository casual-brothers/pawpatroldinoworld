// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/NebulaFlowAutoResizableContainer.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"


void UNebulaFlowAutoResizableContainer::SetText(FText NewText)
{
	if(!NewText.IsEmpty())
	TextBlock->SetText(NewText);
}

void UNebulaFlowAutoResizableContainer::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (BorderContainer && BorderBrush.HasUObject())
	{
		BorderBrush.DrawAs = ESlateBrushDrawType::Box;
		BorderContainer->SetBrush(BorderBrush);
	}
	if (TextBlock && !Text.IsEmpty())
	{
		TextBlock->SetText(Text);
		TextBlock->SetColorAndOpacity(InColorAndOpacity);
		TextBlock->SetFont(Font);
		TextBlock->SetStrikeBrush(StrikeBrush);
		TextBlock->SetShadowOffset(ShadowOffset);
		TextBlock->SetShadowColorAndOpacity(ShadowColorAndOpacity);
		TextBlock->SetMinDesiredWidth(MinDesiredWidth);
		TextBlock->SetTextTransformPolicy(TextTransformPolicy);
	}
}
