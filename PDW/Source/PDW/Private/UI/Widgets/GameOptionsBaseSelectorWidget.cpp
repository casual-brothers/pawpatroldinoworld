// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "UI/Widgets/GameOptionsBaseSelectorWidget.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"



FReply UGameOptionsBaseSelectorWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
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
	OnFocusedGameOptions.Broadcast(this);
	return FReply::Unhandled();
}

void UGameOptionsBaseSelectorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetIsFocusable(true);
}
