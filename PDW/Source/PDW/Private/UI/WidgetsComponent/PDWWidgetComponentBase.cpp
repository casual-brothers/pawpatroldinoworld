// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetsComponent/PDWWidgetComponentBase.h"

UPDWWidgetComponentBase::UPDWWidgetComponentBase()
{
	//Starting Initialization
	
	//User Interface
	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(64, 64);
	TickMode = ETickMode::Automatic;
	
	//Component Tick
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	
	//Physics
	SetEnableGravity(false);
	bApplyImpulseOnDamage = false;
	bReplicatePhysicsToAutonomousProxy = false;
	if (FBodyInstance* BodyInst = GetBodyInstance())
	{
		BodyInst->SetInertiaConditioningEnabled(false);
		BodyInst->bUpdateMassWhenScaleChanges = false;
	}

	//Collision
	SetGenerateOverlapEvents(false);
	CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	SetCollisionProfileName(FName("NoCollision"));

	//Rendering
	bVisibleInReflectionCaptures = false;
	bVisibleInRealTimeSkyCaptures = false;
	bVisibleInRayTracing = false;
	bReceivesDecals = false;
	bUseAsOccluder = false;

	//HLOD
	bEnableAutoLODGeneration = false;

	//Mobile
	bReceiveMobileCSMShadows = false;
}