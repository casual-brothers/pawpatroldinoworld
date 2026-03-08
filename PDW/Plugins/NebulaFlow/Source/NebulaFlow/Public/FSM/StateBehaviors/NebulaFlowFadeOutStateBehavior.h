// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "NebulaFlowBaseStateBehavior.h"

#include "NebulaFlowFadeOutStateBehavior.generated.h"



UCLASS(Abstract,Blueprintable, EditInlineNew, DefaultToInstanced)
class NEBULAFLOW_API UNebulaFlowFadeOutStateBehavior : public UNebulaFlowBaseStateBehavior
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere)
	float FadeOutTime = 1.f;

	UPROPERTY(EditAnywhere)
	FLinearColor FadeColor;

protected:

		virtual void BeforeOnEnter_Implementation();

		void OnExit_Implementation();
};