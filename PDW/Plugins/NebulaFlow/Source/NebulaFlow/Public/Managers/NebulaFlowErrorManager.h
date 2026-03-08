
#pragma once


#include "NebulaFlowManagerBase.h"
#include "NebulaFlowErrorManager.generated.h"

class UNebulaFlowBaseErrorInstance;
class UNebulaFlowLocalPlayer;

UENUM(Blueprintable, BlueprintType)
enum class EErrorInstanceType : uint8
{
	EDefault,
	EPlayerPadConnectionLost,
	EPlayerProfileChanged,
	ENetworkConnectionLost,
	EMaxAlert
};

UCLASS()
class NEBULAFLOW_API UNebulaFlowErrorManager : public UNebulaFlowManagerBase
{

	GENERATED_BODY()

public:

	virtual void TickManager(float DeltaTime) override;

	UNebulaFlowBaseErrorInstance* CreateErrorInstance (TSubclassOf<UNebulaFlowBaseErrorInstance> ErrorInstanceClass, FName DialogID = NAME_None, UNebulaFlowLocalPlayer* inLocalPlayer = nullptr);

	void ResolveError(UNebulaFlowBaseErrorInstance* inInstance);
	
	UNebulaFlowErrorManager(const FObjectInitializer& ObjectInitializer);

	//DebugOnly to create a Max Alert debug instance
	UNebulaFlowBaseErrorInstance* CreateErrorInstanceDebug(TSubclassOf<UNebulaFlowBaseErrorInstance> ErrorInstanceClass, FName DialogID = NAME_None, UNebulaFlowLocalPlayer* inLocalPlayer = nullptr);

private:
	
	UPROPERTY()
	TArray<UNebulaFlowBaseErrorInstance*> CurrentErrorInstances;

	void AddErrorInstance(UNebulaFlowBaseErrorInstance* AddedInstance);

	void ManageCurrentError();

	//
	bool bCreateDebugInstance = false;
};