// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Dialogue/PDWAnswerButton.h"
#include "Components/RichTextBlock.h"

void UPDWAnswerButton::SetButtonText(FText Text)
{
	ButtonText->SetText(Text);
}