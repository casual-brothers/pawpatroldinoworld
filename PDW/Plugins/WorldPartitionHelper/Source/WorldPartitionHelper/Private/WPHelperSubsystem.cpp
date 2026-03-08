// Fill out your copyright notice in the Description page of Project Settings.


#include "WPHelperSubsystem.h"
#include "WorldPartitionHelper.h"
#include "WPHelperSettings.h"
#include "Actors/WPTeleportActor.h"
#include "Kismet/GameplayStatics.h"
#include "ActorPartition/ActorPartitionSubsystem.h"
#include "WorldPartition/WorldPartitionRuntimeSpatialHash.h"
#include "Components/WPLoadingHandlerComponent.h"
#include "WorldPartition/WorldPartitionSubsystem.h"
#include "WorldPartition/RuntimeHashSet/WorldPartitionRuntimeHashSet.h"
#include "Actors/WPBaseStreamingSourceActor.h"
#include "Actors/WPTeleportLocation.h"



DECLARE_CYCLE_STAT(TEXT("UWPHelperSubsystem ~ RegisterEntity"), STAT_RegisterEntity, STATGROUP_WorldPartitionHelper);
DECLARE_CYCLE_STAT(TEXT("UWPHelperSubsystem ~ QueryCell"), STAT_QueryCell, STATGROUP_WorldPartitionHelper);
DECLARE_CYCLE_STAT(TEXT("UWPHelperSubsystem ~ GetCellHash"), STAT_GetCellHash, STATGROUP_WorldPartitionHelper);


UWPHelperSubsystem* UWPHelperSubsystem::Get(UObject* WorldContext)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContext);
	return GameInstance->GetSubsystem<UWPHelperSubsystem>();
}

FOnStreamingSourceRequested* UWPHelperSubsystem::RequestStreamingSourceAtLocation(const FVector& inLocation, const FName& StreamingSourceID)
{
	TSubclassOf<AWPBaseStreamingSourceActor> StreamingSourceClass = UWPHelperSettings::Get()->StreamingSourceActorClass;
	ensureMsgf(StreamingSourceClass, TEXT("UWPHelperSubsystem: TeleportClass Not Assigned!"));
	AWPBaseStreamingSourceActor* StreamingSourceSpawned = GetWorld()->SpawnActor<AWPBaseStreamingSourceActor>(StreamingSourceClass,inLocation,FRotator::ZeroRotator);
	if(StreamingSourceSpawned)
	{
		StreamingSourceSpawned->StreamingSourceID = StreamingSourceID;
		StreamingSourcesMap.Add(StreamingSourceID, StreamingSourceSpawned);
		return StreamingSourceSpawned->EnableStreamingSource();
	}
	else
	{
		UE_LOG(WorldPartitionHelperLog, Warning, TEXT("UWPHelperSubsystem: Failed to spawn streaming source at location %s!"), *inLocation.ToString());
	}
	return nullptr;
}

void UWPHelperSubsystem::RequestDestructionOfStreamingSource(const FName& StreamingSourceID)
{
	if(StreamingSourcesMap.Contains(StreamingSourceID))
	{
		StreamingSourcesMap[StreamingSourceID]->Destroy();
		StreamingSourcesMap.Remove(StreamingSourceID);
	}
	else
	{
		UE_LOG(WorldPartitionHelperLog, Warning, TEXT("UWPHelperSubsystem: Failed to find streaming source with ID %s for destruction!"), *StreamingSourceID.ToString());
	}
}

void UWPHelperSubsystem::EnableStreamingSource(const FName& StreamingSourceID)
{
	if (StreamingSourcesMap.Contains(StreamingSourceID))
	{
		StreamingSourcesMap[StreamingSourceID]->EnableStreamingSource();
	}
}

void UWPHelperSubsystem::RegisterStreamingSourceActor(const FName& ID, AWPBaseStreamingSourceActor* StreamingSource)
{
	if (!StreamingSourcesMap.Contains(ID))
	{
		StreamingSourcesMap.Add(ID,StreamingSource);
	}
}

void UWPHelperSubsystem::RemoveStreamingSourceActor(const FName& ID)
{
	if (StreamingSourcesMap.Contains(ID))
	{
		StreamingSourcesMap.Remove(ID);
	}
}

//************************************
// Method:    Teleport
// FullName:  UWPHelperSubsystem::Teleport
// Access:    public 
// Returns:   void
// Qualifier: Wrapper function to call whenever you want to teleport to a destination in the world.
// Parameter: TArray<AActor *> TeleportingActors - the actors you want to teleport
// Parameter: const FTransform & TargetLocation
//************************************
void UWPHelperSubsystem::Teleport(TArray<AActor*>& TeleportingActors, const TArray<FTransform>& TargetLocations)
{
	if (!TeleportActor)
	{
		SpawnTeleportActor();
	}

	TeleportActor->StartTeleport(TeleportingActors,TargetLocations);
}

//************************************
// Method:    TeleportToLocation
// FullName:  UWPHelperSubsystem::TeleportToLocation
// Access:    public 
// Returns:   void
// Qualifier: Wrapper function to call whenever you want to teleport to a destination in the world using a TeleportID.
// Parameter: const FGameplayTag & TeleportLocationID
// Parameter: TArray<AActor * > & inActorsToTeleport
//************************************
void UWPHelperSubsystem::TeleportToLocation(const FGameplayTag& TeleportLocationID, TArray<AActor*>& inActorsToTeleport)
{
	if(!TeleportLocationsMap.Contains(TeleportLocationID))
	{
		UE_LOG(WorldPartitionHelperLog, Warning, TEXT("UWPHelperSubsystem: Teleport ID %s not found!"), *TeleportLocationID.GetTagName().ToString());
		return;
	}
	TArray<FTransform> TeleportTransforms;
	TeleportLocationsMap[TeleportLocationID]->GetTeleportTransform(TeleportTransforms);
	Teleport(inActorsToTeleport,TeleportTransforms);
}

//************************************
// Method:    SetTeleportDebug
// FullName:  UWPHelperSubsystem::SetTeleportDebug
// Access:    public 
// Returns:   void
// Qualifier: Toggle the Debug mode allowing you to see the amount of time to load the partition where you will teleport.
// Parameter: bool bEnable
//************************************
void UWPHelperSubsystem::SetTeleportDebug(bool bEnable)
{
	if (!TeleportActor)
	{
		SpawnTeleportActor();
	}
	
	TeleportActor->SetDebugMode(bEnable);
}

//************************************
// Method:    RegisterEntity
// FullName:  UWPHelperSubsystem::RegisterEntity
// Access:    public 
// Returns:   void
// Qualifier: Will register the given actor in the EntitiesCellMap -> CellHash - Actor and trigger the check to enable physics.
// Parameter: AActor * inActor
// Parameter: const FVector & inLocation
//************************************
void UWPHelperSubsystem::RegisterEntity(UWPLoadingHandlerComponent* inLoadingComponent, const FVector& inLocation)
{
	SCOPE_CYCLE_COUNTER(STAT_RegisterEntity);
	//#TODO -> GETTING GRID FROM ACTOR IS ONLY ON EDITOR
	FName GridName = UWPHelperSettings::Get()->DefaultGridName;
	AsyncTask(ENamedThreads::NormalThreadPriority,[this,GridName,inLoadingComponent,inLocation]()
	{
			const int32 CellHash = GetCellHash(inLocation, GridName);
			if (!EntitiesCellMap.Contains(CellHash))
			{
				FCellEntities CellEntities;
				CellEntities.Entities.AddUnique(inLoadingComponent);
				CellEntities.CellLocation = inLocation;
				CellEntities.GridName = GridName;
				EntitiesCellMap.Add(CellHash, CellEntities);
			}
			else
			{
				EntitiesCellMap[CellHash].CellLocation = inLocation;
				EntitiesCellMap[CellHash].Entities.AddUnique(inLoadingComponent);
			}

			AsyncTask(ENamedThreads::GameThread, [this]()
				{

					ToggleCellQuery(true);
				});
	});
}

void UWPHelperSubsystem::RegisterTeleportLocation(const FGameplayTag& inTeleportID, AWPTeleportLocation* inTeleportLocationActor)
{
	if (TeleportLocationsMap.Contains(inTeleportID))
	{

	//#if WITH_EDITOR
	//	auto Text = "TeleportID \'{0}\' already registered with actor \'{1}\' and actor \'{2}\' fix one entry";
	//	FText TeleportID = FText::FromString(inTeleportID.GetTagName().ToString());
	//	FText ExistingActor = FText::FromString(TeleportLocationsMap[inTeleportID] ? TeleportLocationsMap[inTeleportID]->GetName() : TEXT("None"));
	//	FText NewActor = FText::FromString(inTeleportLocationActor ? inTeleportLocationActor->GetName() : TEXT("None"));
	//	FText Title = FText::FromString(TEXT("PLEASE FIX ME"));
	//	FText Content = FText::Format(FText::FromString(Text), TeleportID,ExistingActor,NewActor);
	//	FMessageDialog::Open(EAppMsgType::Ok, Content, &Title);
	//#endif //WITH_EDITOR

		UE_LOG(WorldPartitionHelperLog, Warning, TEXT("UWPHelperSubsystem: Teleport ID %s is already registered, overwriting the previous location."), *inTeleportID.GetTagName().ToString());
	}
	TeleportLocationsMap.Add(inTeleportID, inTeleportLocationActor);
}

AWPTeleportLocation* UWPHelperSubsystem::GetTeleportActorByTag(const FGameplayTag& TeleportLocationID)
{
	 return TeleportLocationsMap.Contains(TeleportLocationID) ? TeleportLocationsMap[TeleportLocationID] : nullptr;
}

void UWPHelperSubsystem::StartCustomPerformanceTrace()
{
	if (bAlreadyDone)
	{
		return;
	}
	bAlreadyDone = true;
	AGameModeBase* GameplayGameMode =UGameplayStatics::GetGameMode(this);
	UGameplayStatics::GetPlayerController(this,0)->ConsoleCommand("Trace.File");
}

void UWPHelperSubsystem::StopCustomPerformanceTrace()
{
	AGameModeBase* GameplayGameMode =UGameplayStatics::GetGameMode(this);
	UGameplayStatics::GetPlayerController(this,0)->ConsoleCommand("Trace.Stop");
}

//************************************
// Method:    ToggleCellQuery
// FullName:  UWPHelperSubsystem::ToggleCellQuery
// Access:    private 
// Returns:   void
// Qualifier: Turn the query on or off.
// Parameter: const bool bInActive
//************************************
void UWPHelperSubsystem::ToggleCellQuery(const bool bInActive)
{
	UWorld* World = GetWorld();
	ensureMsgf(World,TEXT("UWPHelperSubsystem: MissingWorld during query cell"));


	if (!bStartQuery && bInActive)
	{
		//StartCustomPerformanceTrace();
		World->GetTimerManager().SetTimer(CellQueryTimerHandle,this,&ThisClass::QueryCells,UWPHelperSettings::Get()->CellQueryInterval,true);
	}
	
	bStartQuery = bInActive;
	if (!bStartQuery)
	{
		//StopCustomPerformanceTrace();
		World->GetTimerManager().ClearTimer(CellQueryTimerHandle);
	}

}


//************************************
// Method:    QueryCells
// FullName:  UWPHelperSubsystem::QueryCells
// Access:    private 
// Returns:   void
// Qualifier: Will query cell one by one, once a cell is loaded will turn on actor logics. Once all cells are loaded will clear the map and stop the query.
//************************************
void UWPHelperSubsystem::QueryCells()
{
	SCOPE_CYCLE_COUNTER(STAT_QueryCell);
	UWorldPartitionSubsystem* WorldPartitionSubSystem = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>();
	for (TMap<int32, FCellEntities>::TIterator ITRemove = EntitiesCellMap.CreateIterator(); ITRemove; ++ITRemove)
	{
		TArray<FWorldPartitionStreamingQuerySource> QuerySources;
		FWorldPartitionStreamingQuerySource& QuerySource = QuerySources.Emplace_GetRef();
		QuerySource.bSpatialQuery = true;
		QuerySource.Location = ITRemove.Value().CellLocation;
		QuerySource.Rotation = FRotator::ZeroRotator;
		QuerySource.TargetGrids = {ITRemove.Value().GridName};
		QuerySource.bUseGridLoadingRange = false;
		QuerySource.Radius = 1.f; // 1cm should be enough to know if grid is loaded at specific area
		QuerySource.bDataLayersOnly = false;
		
		if (WorldPartitionSubSystem->IsStreamingCompleted(EWorldPartitionRuntimeCellState::Activated, QuerySources, false))
		{
			//If the cell is loaded i activate physics for all the actors in the cell and remove the cell from the map
			for (UWPLoadingHandlerComponent* LoadingComponent : ITRemove.Value().Entities)
			{
				if (LoadingComponent)
				{
					LoadingComponent->OnLoadingComplete();
				}
				else
				{
					ensureMsgf(false, TEXT("UWPHelperSubsystem: LoadingComponent is null! This should not happen! - VERIFY WHAT HAPPENED"));
				}
			}
			ITRemove.RemoveCurrent();
			if (EntitiesCellMap.IsEmpty())
			{
				ToggleCellQuery(false);
			}
		}
	}
}

//************************************
// Method:    SpawnTeleportActor
// FullName:  UWPHelperSubsystem::SpawnTeleportActor
// Access:    private 
// Returns:   bool
// Qualifier: Spawns the teleport actor in the world.
//************************************
bool UWPHelperSubsystem::SpawnTeleportActor()
{
	TSubclassOf<AWPTeleportActor> TeleportActorToSpawn = UWPHelperSettings::Get()->TeleportActorClass;
	ensureMsgf(TeleportActorToSpawn, TEXT("UWPHelperSubsystem: TeleportClass Not Assigned!"));
	TeleportActor = GetWorld()->SpawnActor<AWPTeleportActor>(TeleportActorToSpawn,FVector::ZeroVector,FRotator::ZeroRotator);
	return TeleportActor != nullptr;
}


//************************************
// Method:    GetCellHash
// FullName:  UWPHelperSubsystem::GetCellHash
// Access:    private 
// Returns:   int32
// Qualifier: From the Actor location you obtain the cell Hash where it is located.
// Parameter: const FVector & inActorLocation
//************************************
int32 UWPHelperSubsystem::GetCellHash(const FVector& inActorLocation,const FName& inGridID)
{
	SCOPE_CYCLE_COUNTER(STAT_GetCellHash);
	ULevel* Level = GetWorld()->GetCurrentLevel();
	UWorldPartition* WorldPartition = GetWorld()->GetWorldPartition();
	ensureMsgf(WorldPartition, TEXT("UWPHelperSubsystem: Trying to use Worldpartition logic in a map WITHOUT world partition."));
	if(!WorldPartition)
		return 0;
	if (!GridsInfo.Contains(inGridID))
	{
		//I WANT TO CALCULATE WORLD PARTITION GRID INFO just 1 time for each grid.
		TMap<FName,FWorldPartitionHelperInfo> TempGridsInfo;
		if (const UWorldPartitionRuntimeSpatialHash* RuntimeSpatialHash = Cast<UWorldPartitionRuntimeSpatialHash>(WorldPartition->RuntimeHash))
		{
			RuntimeSpatialHash->ForEachStreamingGrid([&TempGridsInfo](const FSpatialHashStreamingGrid& SpatialHashStreamingGrid)
					{	
						FWorldPartitionHelperInfo InfoStruct;
						InfoStruct.GridName = SpatialHashStreamingGrid.GridName;
						InfoStruct.WorldBounds = SpatialHashStreamingGrid.WorldBounds;
						InfoStruct.CellSize = SpatialHashStreamingGrid.CellSize;
						InfoStruct.LoadingRange = SpatialHashStreamingGrid.LoadingRange;
						TempGridsInfo.Add(SpatialHashStreamingGrid.GridName,InfoStruct);
					});
			GridsInfo.Append(TempGridsInfo);
		}

		// #TODO_PDW <It seems with new releases that hashset is best for performance. Need more study on that and implement> [#daniele.m, 25 September 2025, GetCellHash]
		if (const UWorldPartitionRuntimeHashSet* RuntimeHashSet = Cast<UWorldPartitionRuntimeHashSet>(WorldPartition->RuntimeHash))
		{
			/*FBox CellBox;
			FGuid CellGuid;
			RuntimeHashSet->ForEachStreamingCells([&CellBox,&CellGuid](const UWorldPartitionRuntimeCell* Cell)
					{	
						CellBox=Cell->GetCellBounds();
						CellGuid = Cell->GetGuid();
						return true;
					});*/
		}
	}
	const UActorPartitionSubsystem::FCellCoord CellCoordinates =  UActorPartitionSubsystem::FCellCoord::GetCellCoord(inActorLocation,Level,GridsInfo[inGridID].CellSize);
	return GetTypeHash(CellCoordinates);
}
