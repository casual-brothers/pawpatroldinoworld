// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PDWManageHudElementsNode.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Managers/PDWUIManager.h"
#include "Managers/PDWHUDSubsystem.h"

void UPDWManageHudElementsNode::OnLoad_Implementation()
{
	Super::OnLoad_Implementation();
	ChangeElements();
}

void UPDWManageHudElementsNode::ChangeElements()
{
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	for (FWidgetHudCommand Command : HudElementsToChange)
	{
		UIManager->SetWidgetsHUDtoHide(Command.WidgetToHide, Command.bHide);
	}
	UPDWHUDSubsystem::Get(this)->CheckHUDWidgetsConfiguration();
	TriggerFirstOutput(true);
}

void UPDWManageHudElementsNode::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);
	ChangeElements();
}

#if WITH_EDITOR
FString UPDWManageHudElementsNode::GetNodeDescription() const
{
	FString Result;
	UEnum* EnumPtr = StaticEnum<EHudWidgetNames>();
	if (EnumPtr)
	{
		for (FWidgetHudCommand Command : HudElementsToChange)
		{
			FString Name = EnumPtr->GetNameStringByValue(static_cast<int64>(Command.WidgetToHide));
			Result.Append((Command.bHide ? TEXT("Hide ") : TEXT("Show ")) + Name + LINE_TERMINATOR);
		}
	}

	return Result;
}
#endif


void UPDWManageHudElementsNode::OnPassThrough_Implementation()
{
	if (CheatMode)
	{
		ChangeElements();
	}

	Super::OnPassThrough_Implementation();
}