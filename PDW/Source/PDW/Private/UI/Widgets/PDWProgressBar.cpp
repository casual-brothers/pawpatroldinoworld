// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWProgressBar.h"

void UPDWProgressBar::InitProgressBar(float CurrentPercentage, float MaxPercentage)
{
	BP_InitProgressBar(CurrentPercentage, MaxPercentage);
}

void UPDWProgressBar::ChangeProgressPercentage(float NewPercentage)
{
	BP_ChangeProgressPercentage(NewPercentage);
}