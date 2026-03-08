// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/PDWQuestTargetContainer.h"
#include "Components/PanelWidget.h"
#include "UI/HUD/PDWQuestTargetElement.h"
#include "Components/Widget.h"
#include "Managers/QuestSubsystem.h"

void UPDWQuestTargetContainer::InitTargetContainer(FPDWQuestTargetData StepData)
{
	UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
	if (QuestSubsystem)
	{
		QuestSubsystem->OnQuestTargetUpdated.AddUniqueDynamic(this, &ThisClass::OnQuestTargetUpdated);
	}

	OnQuestTargetUpdated(StepData);
}

void UPDWQuestTargetContainer::OnQuestTargetUpdated(FPDWQuestTargetData NewQuestTarget)
{
	TargetsContainer->ClearChildren();
	if (NewQuestTarget.TargetQuantity < 2)
	{
		return;
	}
	for (int32 i = 0; i < NewQuestTarget.TargetQuantity; ++i)
	{
		UPDWQuestTargetElement* NewElement = CreateWidget<UPDWQuestTargetElement>(this, TargetElementClass);
		TargetsContainer->AddChild(NewElement);
		NewElement->BP_InitTargetElement(NewQuestTarget.IdentityTag);
		FMargin PaddingToApply = ElementPadding;
		if (i == 0)
		{
			PaddingToApply.Left = 0;
		}
		if (i == NewQuestTarget.TargetQuantity)
		{
			PaddingToApply.Right = 0;
		}

		NewElement->SetPadding(PaddingToApply);

		if (i < NewQuestTarget.CurrentQuantity)
		{
			NewElement->OnFoundTarget();
		}
	}
	
	//for (int32 i = 0; i < NewQuestTarget.CurrentQuantity; ++i)
	//{
	//	UWidget* Element = TargetsContainer->GetChildAt(i);
	//	if (Element)
	//	{
	//		UPDWQuestTargetElement* TargetElement = Cast<UPDWQuestTargetElement>(Element);
	//		if (TargetElement)
	//		{
	//			TargetElement->OnFoundTarget();
	//		}
	//	}
	//}
}

void UPDWQuestTargetContainer::NativeDestruct()
{
	UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
	if (QuestSubsystem)
	{
		QuestSubsystem->OnQuestTargetUpdated.RemoveDynamic(this, &ThisClass::OnQuestTargetUpdated);
	}
	Super::NativeDestruct();
}