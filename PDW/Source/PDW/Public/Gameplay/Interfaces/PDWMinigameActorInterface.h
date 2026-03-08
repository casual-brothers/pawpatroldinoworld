#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Interface.h"
#include "PDWMinigameActorInterface.generated.h"

class UPDWMinigameTargetComponent;

UINTERFACE(BlueprintType, MinimalAPI)
class UPDWMinigameActorInterface : public UInterface
{
	GENERATED_BODY()
};

class PDW_API IPDWMinigameActorInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	UPDWMinigameTargetComponent* BP_GetMinigameTargetComponent();
	virtual UPDWMinigameTargetComponent* GetMinigameTargetComponent() { return Execute_BP_GetMinigameTargetComponent(Cast<UObject>(this)); }

protected:

};