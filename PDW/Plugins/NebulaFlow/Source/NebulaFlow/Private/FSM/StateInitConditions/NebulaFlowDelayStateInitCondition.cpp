#include "FSM/StateInitConditions/NebulaFlowDelayStateInitCondition.h"
#include "FSM/NebulaFlowBaseFSMState.h"


UNebulaFlowDelayStateInitCondition::UNebulaFlowDelayStateInitCondition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)

{
}

void UNebulaFlowDelayStateInitCondition::Initialize(const UNebulaFlowBaseFSMState* Owner)
{
	//To override in Derived classes
	Super::Initialize(Owner);
}

bool UNebulaFlowDelayStateInitCondition::CheckResolvedCondition(float deltaTime)
{
	Super::CheckResolvedCondition(deltaTime);
	Counter+=deltaTime;
	return Counter >=DelayTime;
}
