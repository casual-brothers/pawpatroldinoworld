// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/MiniGames/PDWJumpOnSplineBehaviour.h"

void UPDWJumpOnSplineBehaviour::ExecuteBehaviour(const FInputActionInstance& inInputInstance)
{
	Super::ExecuteBehaviour(inInputInstance);

	if (bCanJump && !bJumping && JumpConfig.JumpingCurve)
	{
		bJumping = true;
		bCanJump = false;
	}

}

void UPDWJumpOnSplineBehaviour::CustomTick(const float inDeltaTime)
{
	Super::CustomTick(inDeltaTime);
	if(!MiniGameOwner)
		return;
	if(bJumping)
	{
		Timer += inDeltaTime;
		const float CurveValue = JumpConfig.JumpingCurve->GetFloatValue(Timer);
		EventSubsystem->OnJumpOnSpline(CurveValue*JumpConfig.JumpingMultiplier);
		if (Timer>= MaxTimer)
		{
			EventSubsystem->OnStopJumpOnSpline();
			bJumping = false;
			Timer = 0.0f;
			bCanJump = true;
		}
	}
}

void UPDWJumpOnSplineBehaviour::InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp)
{
	Super::InitializeBehaviour(inController, inMiniGameComp);
	if(JumpConfig.JumpingCurve)
	{
		float MinTimer;
		JumpConfig.JumpingCurve->GetTimeRange(MinTimer,MaxTimer);
	}
	Timer=0.0f;
	EventSubsystem = UPDWEventSubsytem::Get(inController);
}
