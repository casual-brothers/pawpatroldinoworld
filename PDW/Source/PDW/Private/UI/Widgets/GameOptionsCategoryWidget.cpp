// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "UI/Widgets/GameOptionsCategoryWidget.h"
#include "Data/GameOptionsData.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "FunctionLibraries/GameOptionsFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"


void UGameOptionsCategoryWidget::Setup(EGameOptionsCategory InCategory)
{
	Category = InCategory;
	if (GameOptionsText)
	{
		GameOptionsText->SetText(UGameOptionsFunctionLibrary::GetGameOptionsCategoryName(Category));
	}
	if (GameOptionsIcon)
	{
		GameOptionsIcon->SetBrushFromTexture( UGameOptionsFunctionLibrary::GetGameOptionsCategoryImage(Category).Get());
	}

	FUIAudioEvents NewAudio = UGameOptionsFunctionLibrary::GetGameOptionsCategoryAudio(InCategory);
	if (!NewAudio.OnFocus.IsNone())
	{
		UIAudioEvents.OnFocus = NewAudio.OnFocus;
	}
	if (!NewAudio.OnFocusVoiceOver.IsNone())
	{
		UIAudioEvents.OnFocusVoiceOver = NewAudio.OnFocusVoiceOver;
	}
	if (!NewAudio.OnPressed.IsNone())
	{
		UIAudioEvents.OnPressed = NewAudio.OnPressed;
	}
}

FReply UGameOptionsCategoryWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusReceived(InGeometry, InFocusEvent);

	if (bPlayNextFocusAudioEvents)
	{
		UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), UIAudioEvents.OnFocus);
	}
	else
	{
		bPlayNextFocusAudioEvents = true;
	}
	UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), UIAudioEvents.OnFocusVoiceOver);
	OnFocusedCategory.Broadcast(Category);

	return FReply::Handled();
}
