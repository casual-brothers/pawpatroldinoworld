// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "UI/Widgets/PDWRemappingActionButton.h"
#include "Managers/PDWEventSubsytem.h"
#include "Components/TextBlock.h"
#include "UI/Widgets/NebulaFlowNavigationIcon.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"



void UPDWRemappingActionButton::InitializeWidget(const TSet<FPlayerKeyMapping>& KeyMapping, bool IsKeyboardButton, EPlayerMappableKeySlot In_Slot, UInputMappingContext* InIMC)
{
	IMC = InIMC;
	StoredKeyMapping = &KeyMapping;
	bIsKeyboardButton = IsKeyboardButton;
	Slot = In_Slot;
	InputName->SetText(KeyMapping.begin()->GetDisplayName());
	Button->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButtonClicked);
	Button->OnHovered.AddUniqueDynamic(this, &ThisClass::OnButtonHovered);
	ActionName = KeyMapping.begin()->GetAssociatedInputAction();
	OverriddenActionName = FName(KeyMapping.begin()->GetMappingName().ToString());
	NavIcon->InitNavigationIcon(ActionName, 0, 0, OverriddenActionName);

	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	if (EventSubsystem)
	{
		EventSubsystem->OnRemappingButtonFocus.AddUniqueDynamic(this, &UPDWRemappingActionButton::CheckIfUnfocus);
		EventSubsystem->OnRemappingButtonNewKeyChosen.AddUniqueDynamic(this, &UPDWRemappingActionButton::OnRemappingButtonNewKeyChosen);
	}

}


void UPDWRemappingActionButton::NativeDestruct()
{
	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	if (EventSubsystem)
	{
		EventSubsystem->OnRemappingButtonFocus.RemoveAll(this);
		EventSubsystem->OnRemappingButtonNewKeyChosen.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UPDWRemappingActionButton::OnButtonClicked()
{
	if (bIsRemapping)
	{
		return;
	}
	else
	{
		Super::OnButtonClicked();
		UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(this);
		EventSubsystem->OnRemappingButtonClick.Broadcast(this);

		NavIcon->SetVisibility(ESlateVisibility::Collapsed);
		bIsRemapping = true;
	}
}

void UPDWRemappingActionButton::OnButtonFocused()
{
	Super::OnButtonFocused();

	NavIcon->SetVisibility(ESlateVisibility::HitTestInvisible);

	UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld());
	if (EventSubsystem)
	{
		EventSubsystem->OnRemappingButtonFocus.Broadcast(this);
	}
	//if (UScrollBox* Parent = Cast<UScrollBox>(GetParent()))
	//{
	//	TArray<UWidget*> Children = Parent->GetAllChildren();
	//	if (Parent->GetAllChildren().Num() > 1)
	//	{
	//		if (Children[1] == this)
	//		{
	//			Parent->ScrollToStart();
	//		}
	//	}
	//}
}


void UPDWRemappingActionButton::CheckIfUnfocus(UPDWRemappingActionButton* ButtonCurrentlyInFocus)
{
	if (ButtonCurrentlyInFocus != this)
	{
		OnUnfocused();
	}
}

void UPDWRemappingActionButton::OnRemappingButtonNewKeyChosen(UPDWRemappingActionButton* RemappingButton, FKey InputKey)
{
	if (RemappingButton == this)
	{
		UNebulaFlowUIFunctionLibrary::SetUserFocus(GetWorld(), this, GetOwningPlayer());
	}
}