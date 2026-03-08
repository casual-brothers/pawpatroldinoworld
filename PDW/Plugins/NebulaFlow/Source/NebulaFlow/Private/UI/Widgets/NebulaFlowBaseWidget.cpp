#include "UI/Widgets/NebulaFlowBaseWidget.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"




UNebulaFlowBaseWidget::UNebulaFlowBaseWidget(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

}

void UNebulaFlowBaseWidget::TriggerAction(FString Action, FString Parameter, APlayerController* ControllerSender /*= nullptr*/)
{
	UNebulaFlowCoreFunctionLibrary::TriggerAction(this,Action,Parameter,ControllerSender != nullptr ? ControllerSender : GetOwningPlayer());
}

void UNebulaFlowBaseWidget::SetPlayNextFocusAudioEvents(bool bInPlayAudioEvents)
{
	bPlayNextFocusAudioEvents = bInPlayAudioEvents;
}

void UNebulaFlowBaseWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

#if PLATFORM_WINDOWS
	
	if(PlatformVisibility.HideOnPc)
		SetVisibility(ESlateVisibility::Collapsed);

#elif PLATFORM_XBOXONE || PLATFORM_XSX
	
	if (PlatformVisibility.HideOnXBox)
		SetVisibility(ESlateVisibility::Collapsed);

#elif PLATFORM_PS4 || PLATFORM_PS5

	if (PlatformVisibility.HideOnPS)
		SetVisibility(ESlateVisibility::Collapsed);

#elif PLATFORM_SWITCH

	if (PlatformVisibility.HideOnSwitch)
		SetVisibility(ESlateVisibility::Collapsed);

#endif

}

