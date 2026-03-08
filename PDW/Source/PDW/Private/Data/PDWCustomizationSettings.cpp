// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PDWCustomizationSettings.h"

UPDWCustomizationSettings* UPDWCustomizationSettings::Get()
{
	return CastChecked<UPDWCustomizationSettings>(UPDWCustomizationSettings::StaticClass()->GetDefaultObject());
}
