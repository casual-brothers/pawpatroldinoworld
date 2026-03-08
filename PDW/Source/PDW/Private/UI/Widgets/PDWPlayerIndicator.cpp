// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWPlayerIndicator.h"

void UPDWPlayerIndicator::InitIndicator(EMiniGamePlayerType inAssinedPlayer)
{
	AssinedPlayer = inAssinedPlayer;
	BP_InitIndicator(inAssinedPlayer);
}