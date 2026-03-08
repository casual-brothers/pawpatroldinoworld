#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/NavigationConfig.h" // from Slate

struct FDigitalNavigationState
{
	double LastNavigationTime;
	int32 Repeats;

	FDigitalNavigationState()
		: LastNavigationTime(0)
		, Repeats(0)
	{
	}
};


class PDW_API FCustomNavigationConfig : public FNavigationConfig
{
public:
	FCustomNavigationConfig();

	void SwapAnalogStick(bool bUseRightInsteadOfLeft);
	void SetJoystickDeadZone(float NewDeadZone);
protected:
	virtual float GetRepeatRateForPressure(float InPressure, int32 InRepeats) const override;

private:
	TMap<EUINavigation, FDigitalNavigationState> KeyNavigationTimes{};
};