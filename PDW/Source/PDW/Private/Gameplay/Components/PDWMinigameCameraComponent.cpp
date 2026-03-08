// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWMinigameCameraComponent.h"
#include "WorldPartition/WorldPartitionSubsystem.h"

bool UPDWMinigameCameraComponent::GetStreamingSource(FWorldPartitionStreamingSource& StreamingSource) const
{
	checkNoEntry();
	return false;
}

bool UPDWMinigameCameraComponent::GetStreamingSources(TArray<FWorldPartitionStreamingSource>& OutStreamingSources) const
{
	if (IsStreamingSourceEnabled())
	{
		return GetStreamingSourcesInternal(OutStreamingSources);
	}
	return false;
}

void UPDWMinigameCameraComponent::OnRegister()
{
	//UWorldPartitionSubsystem* WorldPartitionSubsystem = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>();
	//check(WorldPartitionSubsystem);
	//WorldPartitionSubsystem->RegisterStreamingSourceProvider(this);
	Super::OnRegister();
}

void UPDWMinigameCameraComponent::OnUnregister()
{
	//UWorldPartitionSubsystem* WorldPartitionSubsystem = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>();
	//check(WorldPartitionSubsystem);
	//verify(WorldPartitionSubsystem->UnregisterStreamingSourceProvider(this));
	Super::OnUnregister();
}

void UPDWMinigameCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	UWorldPartitionSubsystem* WorldPartitionSubsystem = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>();
	check(WorldPartitionSubsystem);
	WorldPartitionSubsystem->RegisterStreamingSourceProvider(this);
}

void UPDWMinigameCameraComponent::EndPlay(EEndPlayReason::Type Reason)
{
	UWorldPartitionSubsystem* WorldPartitionSubsystem = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>();
	check(WorldPartitionSubsystem);
	verify(WorldPartitionSubsystem->UnregisterStreamingSourceProvider(this));
	Super::EndPlay(Reason);
}

void UPDWMinigameCameraComponent::GetStreamingSourceLocationAndRotation(FVector& OutLocation, FRotator& OutRotation) const
{
	if (const AActor* ViewTarget = GetOwner())
	{
		OutLocation = GetComponentLocation();
		OutRotation = GetComponentRotation();
		return;
	}
}

void UPDWMinigameCameraComponent::GetStreamingSourceShapes(TArray<FStreamingSourceShape>& OutShapes) const
{
	if (StreamingSourceShapes.Num())
	{
		OutShapes.Append(StreamingSourceShapes);
	}
}

bool UPDWMinigameCameraComponent::GetStreamingSourcesInternal(TArray<FWorldPartitionStreamingSource>& OutStreamingSources) const
{
	FWorldPartitionStreamingSource& StreamingSource = OutStreamingSources.AddDefaulted_GetRef();
	GetStreamingSourceLocationAndRotation(StreamingSource.Location, StreamingSource.Rotation);
	StreamingSource.Name = GetFName();
	StreamingSource.TargetState = StreamingSourceShouldActivate() ? EStreamingSourceTargetState::Activated : EStreamingSourceTargetState::Loaded;
	StreamingSource.bBlockOnSlowLoading = StreamingSourceShouldBlockOnSlowStreaming();
	StreamingSource.DebugColor = StreamingSourceDebugColor;
	StreamingSource.Priority = GetStreamingSourcePriority();
	StreamingSource.bRemote = true;
	GetStreamingSourceShapes(StreamingSource.Shapes);
	return true;
}
