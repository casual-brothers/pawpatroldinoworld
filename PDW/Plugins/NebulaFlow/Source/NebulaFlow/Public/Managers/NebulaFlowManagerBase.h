
#pragma once


#include "UObject/Object.h"
#include "NebulaFlowManagerBase.generated.h"

class UNebulaFlowGameInstance;

UCLASS(Abstract)
class NEBULAFLOW_API UNebulaFlowManagerBase: public UObject
{
	GENERATED_BODY()


public:

	virtual void InitManager(UNebulaFlowGameInstance* InstanceOwner);

	virtual void UninitManager();

	virtual void TickManager(float DeltaTime);

	virtual bool ShouldManagerTick(){return bShouldTick;}

	virtual UWorld* GetWorld() const override;

	UFUNCTION()
	virtual void OnLoadLevelStarted(FName LevelName);

	UFUNCTION()
	virtual void OnLoadLevelEnded(FName LevelName);

	UFUNCTION()
	virtual void OnGameContextChanged(EGameContext NewContext);


protected:

	UPROPERTY()
	UNebulaFlowGameInstance* GInstanceRef;

	UPROPERTY(EditAnywhere, Category="Managers")
	bool bShouldTick = true;

	FDelegateHandle LevelLoadStartHandle;

	FDelegateHandle LevelLoadEndedHandle;

};
