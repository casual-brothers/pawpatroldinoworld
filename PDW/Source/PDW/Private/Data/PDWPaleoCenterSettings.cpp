// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PDWPaleoCenterSettings.h"

UPDWPaleoCenterSettings::UPDWPaleoCenterSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UPDWPaleoCenterSettings* UPDWPaleoCenterSettings::Get()
{
	return CastChecked<UPDWPaleoCenterSettings>(UPDWPaleoCenterSettings::StaticClass()->GetDefaultObject());
}
