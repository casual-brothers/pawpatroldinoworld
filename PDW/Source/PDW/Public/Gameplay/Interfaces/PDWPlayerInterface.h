#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "PDWPlayerInterface.generated.h"

class APDWPlayerController;
class UPDWInteractionComponent;
class APDWPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerPossesed);

UINTERFACE(BlueprintType, MinimalAPI,meta = (CannotImplementInterfaceInBlueprint))
class UPDWPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

class PDW_API IPDWPlayerInterface
{
	GENERATED_BODY()

public:

	FOnPlayerPossesed OnPlayerPossesed;

	UFUNCTION(BlueprintCallable)
	virtual APDWPlayerState* GetPDWPlayerState() = 0;

	/** Returns the PDW Player Controller associated with this interface */
	UFUNCTION(BlueprintCallable)
	virtual APDWPlayerController* GetPDWPlayerController(){return nullptr;};

	UFUNCTION(BlueprintCallable)
	virtual UPDWInteractionComponent* GetInteractionComponent(){return nullptr;};

	UFUNCTION(BlueprintCallable)
	virtual void SetActivation(bool inActive){};

	UFUNCTION(BlueprintCallable)
	virtual void MiniGameMove(const FVector2D& MovementVector,const FVector& inForwardVector,const FVector& inRightVector) = 0;
protected:

};