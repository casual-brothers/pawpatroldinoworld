#include "FunctionLibraries/NebulaFlowInputFunctionLibrary.h"
#include "Core/NebulaFlowPersistentUser.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/InputSettings.h"
#include "Framework/Commands/InputChord.h"
#include "InputCoreTypes.h"
#include "GameFramework/PlayerInput.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "HAL/ConsoleManager.h"
#include "EnhancedInputSubsystems.h"
#include "UserManagement/NebulaFlowUserSubsystem.h"

bool UNebulaFlowInputFunctionLibrary::RemapActionToKey(const UObject* WorldContextObject, FName ActionContext, FName ActionName, FKey NewKey, bool IsPadAssignment /*= false*/)
{
	if (NewKey.IsGamepadKey() != IsPadAssignment)
	{
		return false;
	}
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	ensure(GInstance && GInstance->RemappableActionsDataAsset);
	if (GInstance)
	{
		if (GInstance->RemappableActionsDataAsset && GInstance->RemappableActionsDataAsset->NotRemappableKeys.Contains(NewKey))
		{
			return false;
		}
		
		UNebulaFlowPersistentUser* User = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject)->GetPersistentUser();
		if (User)
		{

			FName CurrentAssignedAction = FindCurrentActionFromKey(WorldContextObject,NewKey, ActionContext);
			bool bFound = false;
			TArray<FKey> CurrentAssignedKeys = FindCurrentKeyFromAction(WorldContextObject,ActionName,ActionContext, bFound, IsPadAssignment);
			if (CurrentAssignedAction != ActionName)
			{
				RemoveActionAssignment(WorldContextObject,CurrentAssignedAction, NewKey);
				AddActionAssignment(WorldContextObject, ActionName, NewKey);
				for (FKey assignedKey : CurrentAssignedKeys)
				{
					RemoveActionAssignment(WorldContextObject, ActionName, assignedKey);
				}

				if (bFound && CurrentAssignedAction != NAME_None)
				{
					for (FKey assignedKey : CurrentAssignedKeys)
					{
						AddActionAssignment(WorldContextObject, CurrentAssignedAction, assignedKey);
					}
					
				}
				return true;
			}
		}
	}
	return false;
}

void UNebulaFlowInputFunctionLibrary::ApplyAssignmentToInputs(const UObject* WorldContextObject, bool bIsPadAssignment /*= false*/)
{
	UInputSettings* pInputSettings = UInputSettings::GetInputSettings();
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	
	if (pInputSettings && GInstance && GInstance->RemappableActionsDataAsset)
	{
		UNebulaFlowPersistentUser* User = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject)->GetPersistentUser();
		if (User)
		{
			//First Clean Input Settings

			CleanInputs(WorldContextObject, bIsPadAssignment);

			//Now add Updates inputs values
			for (FRemappableAction& currentAction : User->SavedRemappableActions)
			{
				for (FKey currentKey : currentAction.Inputs)
				{
					if (currentKey.IsGamepadKey() == bIsPadAssignment && !GInstance->RemappableActionsDataAsset->NotRemappableKeys.Contains(currentKey))
					{
						if (currentAction.bIsAxis)
						{
							pInputSettings->AddAxisMapping(FInputAxisKeyMapping((currentAction.RelatedAction), currentKey, currentAction.AxisValue), true);
						}
						else
						{
							pInputSettings->AddActionMapping(FInputActionKeyMapping((currentAction.RelatedAction), currentKey), true);
						}						
					}
				}
			}
		}
	}
}

void UNebulaFlowInputFunctionLibrary::ApplyDefaultInputs(const UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if(GInstance)
	{ 
		UNebulaFlowPersistentUser* User = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject)->GetPersistentUser();
		if (User && GInstance->RemappableActionsDataAsset)
		{
			User->SavedRemappableActions.Empty();
			for (FRemappableAction currentaction : GInstance->RemappableActionsDataAsset->Actions)
			{
				User->SavedRemappableActions.Add(currentaction);
			}
		}
		ApplyAssignmentToInputs(WorldContextObject);
		ApplyAssignmentToInputs(WorldContextObject,true);
	}
}

bool UNebulaFlowInputFunctionLibrary::IsKeyRemappable(const UObject* WorldContextObject, FKey Key)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));

	if (GInstance && GInstance->RemappableActionsDataAsset)
	{
		return !GInstance->RemappableActionsDataAsset->NotRemappableKeys.Contains(Key);
	}
	return false;
}

void UNebulaFlowInputFunctionLibrary::SetupPS4JapaneseInputs(const UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));

	if (!GInstance->GetIsPS_JP())
	{
		return;
	}

	/**
	* Set the map context from default UI to the inverted confirm/back context (same as switch)
	*/
	ensure(GInstance->JapanesePlatformData.Num());
	if (!GInstance->JapanesePlatformData.Num())
	{
		return;
	}

	for (ULocalPlayer* LocalPlayer : GInstance->GetLocalPlayers())
	{
		UNebulaFlowLocalPlayer* NebulaLocalPlayer = Cast<UNebulaFlowLocalPlayer>(LocalPlayer);
		if (!NebulaLocalPlayer)
		{
			continue;
		}

		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NebulaLocalPlayer);

		if (!Subsystem)
		{
			continue;
		}

		TMap<const UInputMappingContext*, const UInputMappingContext*> MappingContextRedirectToRemove;

		for (const auto& Redirect : GInstance->JapanesePlatformData)
		{
			if (Subsystem->HasMappingContext(Redirect.Key))
			{
				MappingContextRedirectToRemove.Add(Redirect);
			}
		}

		for (const auto& RedirectToRemove : MappingContextRedirectToRemove)
		{
			int32 Priority = 0;
			Subsystem->HasMappingContext(RedirectToRemove.Key, Priority);
			Subsystem->RemoveMappingContext(RedirectToRemove.Key);
			Subsystem->AddMappingContext(RedirectToRemove.Value, Priority);
		}
	}

}

void UNebulaFlowInputFunctionLibrary::RemoveActionAssignment(const UObject* WorldContextObject, FName ActionName, FKey Key)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));

	if (GInstance && GInstance->RemappableActionsDataAsset)
	{
		UNebulaFlowPersistentUser* User = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject)->GetPersistentUser();

		if (User)
		{
			for (FRemappableAction& currentAction : User->SavedRemappableActions)
			{

				if (currentAction.ActionName == ActionName && !GInstance->RemappableActionsDataAsset->NotRemappableKeys.Contains(Key))
				{
					currentAction.Inputs.Remove(Key);
					return;
				}
			}
		}
	}
}

void UNebulaFlowInputFunctionLibrary::AddActionAssignment(const UObject* WorldContextObject, FName ActionName, FKey Key)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));

	if (GInstance && GInstance->RemappableActionsDataAsset)
	{
		UNebulaFlowPersistentUser* User = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject)->GetPersistentUser();
		if (User)
		{
			for (FRemappableAction& currentAction : User->SavedRemappableActions)
			{
				if (currentAction.ActionName == ActionName && !GInstance->RemappableActionsDataAsset->NotRemappableKeys.Contains(Key))
				{
					currentAction.Inputs.Add(Key);
					return;
				}
			}
		}
	}
}

FName UNebulaFlowInputFunctionLibrary::FindCurrentActionFromKey(const UObject* WorldContextObject, FKey Key, FName ActionContext)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));

	if(GInstance && GInstance->RemappableActionsDataAsset)
	{ 
		UNebulaFlowPersistentUser* User = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject)->GetPersistentUser();
		if (User)
		{
			for (FRemappableAction& currentAction : User->SavedRemappableActions)
			{
				if (currentAction.Context != ActionContext) continue;	

				for (FKey currentKey : currentAction.Inputs)
				{
					if (Key == currentKey && !GInstance->RemappableActionsDataAsset->NotRemappableKeys.Contains(currentKey))
					{
						return currentAction.ActionName;

					}
				}
			}
		}
	}
	return NAME_None;
}

TArray<FKey> UNebulaFlowInputFunctionLibrary::FindCurrentKeyFromAction(const UObject* WorldContextObject, FName ActionName, FName ActionContext, bool& bKeyFound, bool IsPad /*= false*/)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	TArray<FKey> Result;

	if (GInstance)
	{
		UNebulaFlowPersistentUser* User = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject)->GetPersistentUser();
		bKeyFound = false;
		if (User)
		{			
			for (FRemappableAction& currentAction : User->SavedRemappableActions)
			{
				if (currentAction.ActionName == ActionName && currentAction.Context == ActionContext)
				{
					for (FKey currentKey : currentAction.Inputs)
					{
						if (currentKey.IsGamepadKey() == IsPad)
						{
							bKeyFound = true;
							Result.Add(currentKey);
						}
					}
					return Result;
				}
			}
		}
	}
	
	return Result;
}

void UNebulaFlowInputFunctionLibrary::CleanInputs(const UObject* WorldContextObject, bool bIsPadAssignment)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if(GInstance && GInstance->RemappableActionsDataAsset)
	{ 
		UNebulaFlowPersistentUser* User = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(WorldContextObject)->GetPersistentUser();
		UInputSettings* pInputSettings = UInputSettings::GetInputSettings();
		if (User && pInputSettings)
		{
			TArray<FName> CleanedInputs;
			for (FRemappableAction& currentAction : User->SavedRemappableActions)
			{
				//First Clean all old mappings
				CleanedInputs.Add(currentAction.RelatedAction);
				if (currentAction.bIsAxis)
				{
					TArray<FInputAxisKeyMapping> AxisMappings;
					TArray<FInputAxisKeyMapping> AxisToRemove;
					pInputSettings->GetAxisMappingByName(currentAction.RelatedAction, AxisMappings);
					for (FInputAxisKeyMapping currentAxis : AxisMappings)
					{
						if (currentAxis.AxisName == currentAction.RelatedAction && currentAxis.Key.IsGamepadKey() == bIsPadAssignment && !GInstance->RemappableActionsDataAsset->NotRemappableKeys.Contains(currentAxis.Key))
						{
							AxisToRemove.Add(currentAxis);
						}
					}
					for (FInputAxisKeyMapping currentToRemove : AxisToRemove)
					{
						pInputSettings->RemoveAxisMapping(currentToRemove);
					}

				}
				else
				{
					TArray<FInputActionKeyMapping> ActionMappings;
					TArray<FInputActionKeyMapping> ActionsToRemove;
					pInputSettings->GetActionMappingByName(currentAction.RelatedAction, ActionMappings);
					for (FInputActionKeyMapping current : ActionMappings)
					{
						if (current.ActionName == currentAction.RelatedAction && current.Key.IsGamepadKey() == bIsPadAssignment && !GInstance->RemappableActionsDataAsset->NotRemappableKeys.Contains(current.Key))
						{
							ActionsToRemove.Add(current);
						}
					}
					for (FInputActionKeyMapping currentToRemove : ActionsToRemove)
					{
						pInputSettings->RemoveActionMapping(currentToRemove);
					}
				}
			}

		}
	}
}

void UNebulaFlowInputFunctionLibrary::SetMouseInputEnabled(const UObject* WorldContextObject, bool NewValue)
{
	IConsoleManager& ConsoleManager = IConsoleManager::Get();
	IConsoleVariable* CVar = ConsoleManager.FindConsoleVariable(TEXT("Nebula.LockMouseFunctions"));
	if (CVar)
	{
		CVar->Set(!NewValue);
	}
}

bool UNebulaFlowInputFunctionLibrary::IsUsingEnhancedInput(const UObject* WorldContextObject)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (GInstance)
	{
		return GInstance->IsEnhancedInputActive();
	}
	return false;
}

void UNebulaFlowInputFunctionLibrary::AddMappingInput(const UObject* WorldContextObject, UInputMappingContext* InContext, int InPriority)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (!IsValid(GInstance) || !GInstance->IsEnhancedInputActive())
		return;

	for (auto iter = GInstance->GetLocalPlayerIterator(); iter; ++iter)
	{
		ULocalPlayer* player = Cast<ULocalPlayer>(*iter);
		if (player)
		{
			UNebulaFlowLocalPlayer* NebulaLocalPlayer = Cast<UNebulaFlowLocalPlayer>(player);
			if (!IsValid(NebulaLocalPlayer))
				return;

			UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NebulaLocalPlayer);
			if (!Subsystem->HasMappingContext(InContext))
			{
				Subsystem->AddMappingContext(InContext, InPriority);
			}
		}					
	}
}

void UNebulaFlowInputFunctionLibrary::RemoveMappingInput(const UObject* WorldContextObject, UInputMappingContext* InContext)
{
	UNebulaFlowGameInstance* GInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (!IsValid(GInstance) || !GInstance->IsEnhancedInputActive())
		return;

	for (auto iter = GInstance->GetLocalPlayerIterator(); iter; ++iter)
	{
		ULocalPlayer* player = Cast<ULocalPlayer>(*iter);
		if (player)
		{
			UNebulaFlowLocalPlayer* NebulaLocalPlayer = Cast<UNebulaFlowLocalPlayer>(player);
			if (!IsValid(NebulaLocalPlayer))
				return;

			UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NebulaLocalPlayer);
			if (Subsystem->HasMappingContext(InContext))
			{
				Subsystem->RemoveMappingContext(InContext);
			}
		}					
	}
}