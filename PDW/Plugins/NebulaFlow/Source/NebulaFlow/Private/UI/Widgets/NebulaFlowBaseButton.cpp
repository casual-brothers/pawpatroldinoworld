#include "UI/Widgets/NebulaFlowBaseButton.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SlateWrapperTypes.h"
#include "Input/NavigationReply.h"




UNebulaFlowBaseButton::UNebulaFlowBaseButton(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

}

void UNebulaFlowBaseButton::UpdatePlatformButtonVisibility()
{
#if PLATFORM_WINDOWS
	{
		if (HideOnPC)
		{
			SetVisibility(ESlateVisibility::Hidden);
		}
		else if (CollapseOnPC)
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
#elif ((PLATFORM_PS4 || PLATFORM_PS5))
	{
		if (HideOnPS5)
		{
			SetVisibility(ESlateVisibility::Hidden);
		}
		else if(CollapseOnPS5)
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
#elif (PLATFORM_SWITCH)
	{
		if (HideOnSwitch)
		{
			SetVisibility(ESlateVisibility::Hidden);
		}
		else if (CollapseOnSwitch)
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
#elif ((PLATFORM_XBOXONE || PLATFORM_XSX))
	{
		if (HideOnXBox)
		{
			SetVisibility(ESlateVisibility::Hidden);
		}
		else if (CollapseOnXBox)
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
#endif
}

void UNebulaFlowBaseButton::OnUserButtonClicked(const int32 UserIndex)
{
	UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), UIAudioEvents.OnPressed);

	TriggerAction(ButtonActionClicked, ButtonParameter, nullptr);

	UserButtonClicked.Broadcast(this, UserIndex);
}

void UNebulaFlowBaseButton::OnButtonClicked()
{
	UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), UIAudioEvents.OnPressed);

	TriggerAction(ButtonActionClicked, ButtonParameter, nullptr);

	ButtonClicked.Broadcast(this);
}

void UNebulaFlowBaseButton::OnButtonFocused()
{
	if (bPlayNextFocusAudioEvents)
	{
		UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), UIAudioEvents.OnFocus);
		UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), UIAudioEvents.OnFocusVoiceOver);
	}
	else
	{
		bPlayNextFocusAudioEvents = true;
	}

	TriggerAction(ButtonActionFocus, ButtonParameter, nullptr);

	ButtonFocused.Broadcast(this);
}

void UNebulaFlowBaseButton::OnButtonUnFocused()
{
	ButtonUnFocused.Broadcast(this);
}

void UNebulaFlowBaseButton::OnButtonHovered()
{
	if (Button->IsHoverable)
	{
		ButtonHovered.Broadcast(this);
	}
}

void UNebulaFlowBaseButton::OnButtonUnHovered()
{
	if (Button->IsHoverable)
	{
		ButtonUnHovered.Broadcast(this);
	}
}

void UNebulaFlowBaseButton::NativeConstruct()
{
	Super::NativeConstruct();

	UpdatePlatformButtonVisibility();

	if (Button)
	{
		Button->OnClicked.AddUniqueDynamic(this, &UNebulaFlowBaseButton::OnButtonClicked);
		Button->OnUserClicked.AddUniqueDynamic(this, &UNebulaFlowBaseButton::OnUserButtonClicked);
		Button->OnFocused.AddUniqueDynamic(this, &UNebulaFlowBaseButton::OnButtonFocused);
		Button->OnUnfocused.AddUniqueDynamic(this, &UNebulaFlowBaseButton::OnButtonUnFocused);
		Button->OnHovered.AddUniqueDynamic(this, &UNebulaFlowBaseButton::OnButtonHovered);
		Button->OnUnhovered.AddUniqueDynamic(this, &UNebulaFlowBaseButton::OnButtonUnHovered);
	}
}

void UNebulaFlowBaseButton::NativeDestruct()
{
	if (Button)
	{
		Button->OnClicked.RemoveDynamic(this, &UNebulaFlowBaseButton::OnButtonClicked);
		Button->OnUserClicked.RemoveDynamic(this, &UNebulaFlowBaseButton::OnUserButtonClicked);
		Button->OnFocused.RemoveDynamic(this, &UNebulaFlowBaseButton::OnButtonFocused);
		Button->OnUnfocused.RemoveDynamic(this, &UNebulaFlowBaseButton::OnButtonUnFocused);
		Button->OnHovered.RemoveDynamic(this, &UNebulaFlowBaseButton::OnButtonHovered);
		Button->OnUnhovered.RemoveDynamic(this, &UNebulaFlowBaseButton::OnButtonUnHovered);
	}

	Super::NativeDestruct();
}

FReply UNebulaFlowBaseButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (Button != nullptr && Button->GetIsFocusable())
	{
		FEventReply EventReply = FEventReply(true);
		Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
		return UWidgetBlueprintLibrary::SetUserFocus(EventReply, Button, false).NativeReply;
	}
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}