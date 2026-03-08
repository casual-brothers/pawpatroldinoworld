#include "NebulaInteractionSystemSettings.h"

UNebulaInteractionSystemSettings::UNebulaInteractionSystemSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CategoryName = "Nebula";
	SectionName = "NebulaInteractionSystem";
}

UNebulaInteractionSystemSettings* UNebulaInteractionSystemSettings::Get()
{
	return CastChecked<UNebulaInteractionSystemSettings>(UNebulaInteractionSystemSettings::StaticClass()->GetDefaultObject());
}

FGameplayTag& UNebulaInteractionSystemSettings::GetInteractionActionTag()
{
	return UNebulaInteractionSystemSettings::Get()->DefaultInteractionActionTag;
}

FCollisionProfileName UNebulaInteractionSystemSettings::GetInteractionCollisionProfileName()
{
	return UNebulaInteractionSystemSettings::Get()->InteractionCollisionProfileName;
}

FCollisionProfileName UNebulaInteractionSystemSettings::GetInteractionReceiverCollisionProfileName()
{
	return UNebulaInteractionSystemSettings::Get()->InteractionReceiverCollisionProfileName;
}

