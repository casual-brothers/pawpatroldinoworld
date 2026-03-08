
#pragma once


#include "GameFramework/CheatManager.h"
#include "NebulaFlowCheatManager.generated.h"



UCLASS()
class NEBULAFLOW_API UNebulaFlowCheatManager : public UCheatManager
{

	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(exec, BlueprintCallable, Category = "Nebula Cheat Manager")
	virtual void Nebula_SimulateDefaultError(FString ErrorText);

	UFUNCTION(exec, BlueprintCallable, Category = "Nebula Cheat Manager")
		virtual void NebulaFlow_SimulateProfileChangedError();

	UFUNCTION(exec, BlueprintCallable, Category = "Nebula Cheat Manager")
	virtual void Nebula_SimulateMaxAlertError(FString ErrorText);

	UFUNCTION(exec, BlueprintCallable, Category = "Nebula Cheat Manager")
	void Nebula_SimulateKeyAssignment();

	UFUNCTION(exec, BlueprintCallable, Category = "Nebula Cheat Manager")
	void Nebula_ToggleUI();
};