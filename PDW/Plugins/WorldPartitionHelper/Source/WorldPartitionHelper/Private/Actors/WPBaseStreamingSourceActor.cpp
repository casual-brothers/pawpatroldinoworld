// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WPBaseStreamingSourceActor.h"
#include "Components/WorldPartitionStreamingSourceComponent.h"
#include "WPHelperSubsystem.h"

// Sets default values
AWPBaseStreamingSourceActor::AWPBaseStreamingSourceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	WorldPartitionStreamingSourceComponent = CreateDefaultSubobject<UWorldPartitionStreamingSourceComponent>(TEXT("StreamingSource"));
	WorldPartitionStreamingSourceComponent->DisableStreamingSource();
#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
#endif

}

// Called when the game starts or when spawned
void AWPBaseStreamingSourceActor::BeginPlay()
{
	Super::BeginPlay();
	WPHelperSubsystem = UWPHelperSubsystem::Get(this);
	WPHelperSubsystem->RegisterStreamingSourceActor(StreamingSourceID,this);
	if(StartStreamingSourceOnBeginPlay)
	{
		EnableStreamingSource();
	}
}

void AWPBaseStreamingSourceActor::OnStreamingCompleted()
{
	OnStreamingSourceLoadCompleted.Broadcast();
	if (!bManuallyDisableStreamingSource)
	{
		WorldPartitionStreamingSourceComponent->DisableStreamingSource();
	}
	SetActorTickEnabled(false);
	//Implement your own code on child class
}

void AWPBaseStreamingSourceActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (WPHelperSubsystem)
	{
		WPHelperSubsystem->RemoveStreamingSourceActor(StreamingSourceID);
	}
	Super::EndPlay(EndPlayReason);
}

FOnStreamingSourceRequested* AWPBaseStreamingSourceActor::EnableStreamingSource()
{
	WorldPartitionStreamingSourceComponent->EnableStreamingSource();
	SetActorTickEnabled(true);
	return &OnStreamingSourceLoadCompleted;
}

void AWPBaseStreamingSourceActor::DisableStreamingSource(bool bDestroy /*=true*/)
{
	WorldPartitionStreamingSourceComponent->DisableStreamingSource();
	if (bDestroy)
	{
		Destroy();
	}
}

// Called every frame
void AWPBaseStreamingSourceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bDebugMode)
	{
		CalculateLoadingTime(DeltaTime);
	}

	const bool bStreamCompleted = WorldPartitionStreamingSourceComponent->IsStreamingCompleted();
	if (bStreamCompleted)
	{
		OnStreamingCompleted();
	}
}

void AWPBaseStreamingSourceActor::CalculateLoadingTime(const float inTime)
{
	LoadingTime+=inTime;
	GEngine->AddOnScreenDebugMessage(5,2.0f,FColor::Red,FString::Printf(TEXT("LoadingTime: %f"),LoadingTime));
}

