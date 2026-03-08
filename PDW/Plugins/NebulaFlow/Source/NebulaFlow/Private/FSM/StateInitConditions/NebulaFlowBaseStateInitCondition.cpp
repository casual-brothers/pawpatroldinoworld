#include "FSM/StateInitConditions/NebulaFlowBaseStateInitCondition.h"


UNebulaFlowBaseStateInitCondition::UNebulaFlowBaseStateInitCondition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
}

void UNebulaFlowBaseStateInitCondition::Initialize(const UNebulaFlowBaseFSMState* Owner)
{
	//To override in Derived classes always call Super
	if(bIsInitialized)
	{
		return;
	}
	bIsInitialized=true;
}

bool UNebulaFlowBaseStateInitCondition::CheckResolvedCondition(float deltaTime)
{
	//To override in classes do NOT call super
	return true;
}
