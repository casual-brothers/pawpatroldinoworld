#include "Managers/NebulaFlowCheatManager.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowErrorManager.h"
#include "ErrorsManagement/NebulaFlowDefaultErrorInstance.h"
#include "ErrorsManagement/NebulaFlowProfileChangedErrorInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowGameInstance.h"
#include "FunctionLibraries/NebulaFlowInputFunctionLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


UNebulaFlowCheatManager::UNebulaFlowCheatManager(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

}

void UNebulaFlowCheatManager::Nebula_SimulateDefaultError(FString ErrorText)
{
	UNebulaFlowErrorManager* ErrorManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowErrorManager(this);
	if (ErrorManager)
	{
		UNebulaFlowDefaultErrorInstance* ErrorInstance = Cast<UNebulaFlowDefaultErrorInstance>(ErrorManager->CreateErrorInstance(UNebulaFlowDefaultErrorInstance::StaticClass()));
		ErrorInstance->SetErrorText(FText::FromString(ErrorText));
	}

}

void UNebulaFlowCheatManager::NebulaFlow_SimulateProfileChangedError()
{
	UNebulaFlowErrorManager* ErrorManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowErrorManager(this);
	if (ErrorManager)
	{
		ErrorManager->CreateErrorInstance(UNebulaFlowProfileChangedErrorInstance::StaticClass(),UNebulaFlowProfileChangedErrorInstance::PROFILE_CHANGED_DIALOG_ID);
	}
}

void UNebulaFlowCheatManager::Nebula_SimulateMaxAlertError(FString ErrorText)
{
	UNebulaFlowErrorManager* ErrorManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowErrorManager(this);
	if (ErrorManager)
	{
		UNebulaFlowDefaultErrorInstance* ErrorInstance = Cast<UNebulaFlowDefaultErrorInstance>(ErrorManager->CreateErrorInstanceDebug(UNebulaFlowDefaultErrorInstance::StaticClass()));
		ErrorInstance->SetErrorText(FText::FromString(ErrorText));
	}
}

void UNebulaFlowCheatManager::Nebula_SimulateKeyAssignment()
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GInstance && GInstance->RemappableActionsDataAssetDebug)
	{
		for (FRemappableAction current : GInstance->RemappableActionsDataAssetDebug->Actions)
		{
			UNebulaFlowInputFunctionLibrary::RemapActionToKey(GInstance,current.Context, current.ActionName,current.Inputs[0]);
		}
		UNebulaFlowInputFunctionLibrary::ApplyAssignmentToInputs(GInstance);
	}
}

void UNebulaFlowCheatManager::Nebula_ToggleUI()
{
	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, UUserWidget::StaticClass());

	if (Widgets.Num() > 0)
	{
		for (UUserWidget* Widget : Widgets)
		{
			if (Widget->GetRenderTransform().Scale.ComponentwiseAllLessThan(FVector2D(1.f, 1.f)))
			{
				Widget->SetRenderScale(FVector2D(1.f, 1.f));
			}
			else
			{
				Widget->SetRenderScale(FVector2D(0.001f, 0.001f));
			}
		}
	}
}