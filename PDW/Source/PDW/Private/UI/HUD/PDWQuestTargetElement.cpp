// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/PDWQuestTargetElement.h"

void UPDWQuestTargetElement::OnFoundTarget()
{
	if (!bIsFound)
	{
		bIsFound = !bIsFound;
		BP_OnFoundTarget();
	}
}