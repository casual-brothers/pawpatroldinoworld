// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/PDWNavbarButtonWithData.h"

void UPDWNavbarButtonWithData::NativeConstruct()
{
	Super::NativeConstruct();
	
	InitializeButton(Data);
}
