// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WPHelperPoolingComponent.h"


UWPHelperPoolingComponent::UWPHelperPoolingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

//#TODO : WORKING, SHOULD BE CHANGED TOWARDS GENERIC ACTOR POOLING SYSTEM (controller is a bit treaky and not worth)

AController* UWPHelperPoolingComponent::RequestController(APawn* inInstigator, const TSubclassOf<AController>& inControllerClass)
{
	ensureMsgf(AIControllerQueue.Contains(inControllerClass), TEXT("Missing AI controller class definition in UWHelperPoolingComponent"));
	if (!AIControllerQueue[inControllerClass].ControllerQueue.IsEmpty())
	{
		AController* Controller = AIControllerQueue[inControllerClass].ControllerQueue.Pop();
		Controller->SetActorTickEnabled(true);
		Controller->SetInstigator(inInstigator);
		return Controller;
	}
	else
	{
		RequestBackUpElements(inControllerClass);
		return RequestController(inInstigator,inControllerClass);
	}
}

void UWPHelperPoolingComponent::ReleaseController(AController* inController,const TSubclassOf<AController>& inControllerClass)
{
	inController->SetActorTickEnabled(false);
	inController->SetInstigator(nullptr);
	AIControllerQueue[inControllerClass].ControllerQueue.Add(inController);
}

void UWPHelperPoolingComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializePool();
}

void UWPHelperPoolingComponent::InitializePool()
{
	ensureMsgf(!AIControllerPoolSettings.IsEmpty(),TEXT("Missing AI controller class definition in UWHelperPoolingComponent"));
	//#TODO : MAYBE SHOULD BE PARALLEL FOR?
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.ObjectFlags |= RF_Transient;
	for (const auto& [ControllerClass, ControllerSetting] : AIControllerPoolSettings)
	{
		for (int32 i = 0; i < ControllerSetting.PoolSize; ++i)
		{
			AController* Controller = GetWorld()->SpawnActor<AController>(ControllerClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), SpawnInfo);
			Controller->SetActorTickEnabled(false);
			if (!AIControllerQueue.Contains(ControllerClass))
			{
				FAiControllerQueue TempStruct;
				TempStruct.ControllerQueue.Add(Controller);
				AIControllerQueue.Add(ControllerClass, TempStruct);
			}
			else
			{
				AIControllerQueue[ControllerClass].ControllerQueue.Add(Controller);
			}
		}
	}
}

void UWPHelperPoolingComponent::RequestBackUpElements(const TSubclassOf<AController>& inControllerClass)
{
	const int32 BackUpSize = AIControllerPoolSettings[inControllerClass].BackUpPoolSize;
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.ObjectFlags |= RF_Transient;

	for (int32 i = 0; i < BackUpSize; ++i)
	{
		AController* Controller = GetWorld()->SpawnActor<AController>(inControllerClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), SpawnInfo);
		Controller->SetActorTickEnabled(false);
		AIControllerQueue[inControllerClass].ControllerQueue.Add(Controller);
	}
}
