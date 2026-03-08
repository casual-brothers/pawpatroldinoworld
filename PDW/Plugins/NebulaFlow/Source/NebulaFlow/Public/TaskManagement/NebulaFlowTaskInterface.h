#pragma once

#include "UObject/Interface.h"
#include "NebulaFlowTaskInterface.generated.h"

UENUM()
enum class ETaskStatus : uint8
{
	ECreated,
	EExecute,
	ESuspended,
	ETerminated
};

UCLASS(Abstract, BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowTaskInterface : public UObject
{
	GENERATED_BODY()

public:

	void SetPriority(int32 iPriority);	
	int32 GetPriority() const;
	ETaskStatus GetStatus() const;
	bool IsValid();
	void Execute();
	void Suspend(bool SuspendPlatform = false);

protected:

	virtual void ExecutionImplementation(){};
	virtual void SuspensionImpementation(){};
	virtual void ContinueExecutionImplementation(){};
	virtual bool IsValidImplementation(){ return true; };
	virtual void OnTermination(){};
	void Terminate();

private:

	void HandleExecution();

protected:

	int32 Priority;

	bool bWasSuspended = false;

private:

	ETaskStatus Status = ETaskStatus::ECreated;

};