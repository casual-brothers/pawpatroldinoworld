#include "UI/Widgets/NebulaFlowSaveIcon.h"



UNebulaFlowSaveIcon::UNebulaFlowSaveIcon(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	
}

void UNebulaFlowSaveIcon::IniatializeSaveIcon(UNebulaFlowGameInstance* GInstance)
{
	GInstanceRef = GInstance;
}

void UNebulaFlowSaveIcon::UpdateSaveIcon(float deltaTime)
{
	if (SaveTimer >= TimeOnScreen && GInstanceRef)
	{
		GInstanceRef->HideSaveIcon();
	}
	else
	{
		SaveTimer+=deltaTime;
	}
}


