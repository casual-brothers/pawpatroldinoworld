// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/PDWIDAssignTool.h"
#include "Kismet/GameplayStatics.h"
#include "FlowComponent.h"
#include "Gameplay/Actors/PDWPupTreat.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Gameplay/Components/PDWTagComponent.h"

void UPDWIDAssignTool::AssignTreatsID(UObject* WorldContextObject)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APDWPupTreat::StaticClass(), FoundActors);
	TArray<int32> AssignedIDs;

	for (AActor* Actor : FoundActors)
	{
		APDWPupTreat* Collectible = Cast<APDWPupTreat>(Actor);

		if (Collectible && !AssignedIDs.Contains(Collectible->CollectibleIndex))
		{
			AssignedIDs.Add(Collectible->CollectibleIndex);
		}		
	}

	for (AActor* Actor : FoundActors)
	{
		APDWPupTreat* Collectible = Cast<APDWPupTreat>(Actor);

		if (Collectible && Collectible->CollectibleIndex == -1)
		{
			int32 IDToAssign = FindFirstAvailableId(AssignedIDs);
			AssignedIDs.Add(IDToAssign);
			Collectible->CollectibleIndex = IDToAssign;
			Actor->MarkPackageDirty();
		}
	}
}

void UPDWIDAssignTool::AssignInteractableID(UObject* WorldContextObject)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AActor::StaticClass(), FoundActors);
	TArray<int32> AssignedIDs;

	for(AActor* Actor: FoundActors)
	{
		if (Actor)
		{
			if(UPDWInteractionReceiverComponent* InteractionReceiver = Actor->FindComponentByClass<UPDWInteractionReceiverComponent>())
			{
				if (!AssignedIDs.Contains(InteractionReceiver->GetInteractableID()))
				{
					if (InteractionReceiver->GetInteractableID() != -1)
					{
						AssignedIDs.Add(InteractionReceiver->GetInteractableID());
					}
					else
					{
						int32 IDToAssign = FindFirstAvailableId(AssignedIDs);
						InteractionReceiver->SetInteractableID(IDToAssign);
						AssignedIDs.Add(IDToAssign);
						Actor->MarkPackageDirty();
					}
				}
			}
		}
	}
}

void UPDWIDAssignTool::AssignQuestTargetID(UObject* WorldContextObject)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AActor::StaticClass(), FoundActors);
	TArray<int32> AssignedIDs;

	for(AActor* Actor: FoundActors)
	{
		if (Actor)
		{
			if(UPDWTagComponent* TagComponent = Actor->FindComponentByClass<UPDWTagComponent>())
			{
				if (!AssignedIDs.Contains(TagComponent->QuestTargetID))
				{
					if (TagComponent->QuestTargetID != -1)
					{
						AssignedIDs.Add(TagComponent->QuestTargetID);
					}
					else
					{
						int32 IDToAssign = FindFirstAvailableId(AssignedIDs);
						TagComponent->QuestTargetID = IDToAssign;
						AssignedIDs.Add(IDToAssign);
						Actor->MarkPackageDirty();
					}
				}
			}
		}
	}
}

int32 UPDWIDAssignTool::FindFirstAvailableId(TArray<int32>& AlreadyAssignedIDs)
{
	int32 currentIndex = 0;
	while (true)
	{
		if (!AlreadyAssignedIDs.Contains(currentIndex))
		{
			return currentIndex;
		}
		currentIndex++;
	}
	return -1;
}

void UPDWIDAssignTool::ResetTreatsIDs(UObject* WorldContextObject)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APDWPupTreat::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		APDWPupTreat* Collectible = Cast<APDWPupTreat>(Actor);

		if (Collectible)
		{
			Collectible->CollectibleIndex = -1;
			Actor->MarkPackageDirty();
		}
	}
}

void UPDWIDAssignTool::ResetInteractableIDs(UObject* WorldContextObject)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AActor::StaticClass(), FoundActors);

	for(AActor* Actor: FoundActors)
	{
		if(UPDWInteractionReceiverComponent* InteractionReceiver = Actor->FindComponentByClass<UPDWInteractionReceiverComponent>())
		{
			InteractionReceiver->SetInteractableID(-1);
		}	
	}
}

void UPDWIDAssignTool::ResetQuestTargetIDs(UObject* WorldContextObject)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AActor::StaticClass(), FoundActors);

	for(AActor* Actor: FoundActors)
	{
		if(UPDWTagComponent* TagComponent = Actor->FindComponentByClass<UPDWTagComponent>())
		{
			TagComponent->QuestTargetID = -1;
		}	
	}
}

void UPDWIDAssignTool::CheckTreatsDuplicates(UObject* WorldContextObject)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APDWPupTreat::StaticClass(), FoundActors);
	TArray<int32> AssignedIDs;

	for (AActor* Actor : FoundActors)
	{
		if(Actor)
		{
			APDWPupTreat* Collectible = Cast<APDWPupTreat>(Actor);

			if(Collectible)
			{
				if (Collectible->CollectibleIndex != -1)
				{
					if (AssignedIDs.Contains(Collectible->CollectibleIndex))
					{
						UE_LOG(LogTemp, Error, TEXT("%s with ID '%d' Is a Duplicate!"), *(Collectible->GetActorNameOrLabel()), (Collectible->CollectibleIndex));
					}
					
					AssignedIDs.Add(Collectible->CollectibleIndex);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ID '%s' has no ID!"), *(Collectible->GetActorNameOrLabel()));
				}
			}
		}
	}
}

void UPDWIDAssignTool::CheckInteractableDuplicates(UObject* WorldContextObject)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AActor::StaticClass(), FoundActors);
	TArray<int32> AssignedIDs;

	for(AActor* Actor: FoundActors)
	{
		if (Actor)
		{
			if(UPDWInteractionReceiverComponent* InteractionReceiver = Actor->FindComponentByClass<UPDWInteractionReceiverComponent>())
			{
				if (InteractionReceiver->GetInteractableID() != -1)
				{
					if (AssignedIDs.Contains(InteractionReceiver->GetInteractableID()))
					{
						UE_LOG(LogTemp, Error, TEXT("%s with ID '%d' Is a Duplicate!"), *(InteractionReceiver->GetOwner()->GetActorNameOrLabel()), (InteractionReceiver->GetInteractableID()));
					}

					AssignedIDs.Add(InteractionReceiver->GetInteractableID());
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ID '%s' has no ID!"), *(InteractionReceiver->GetOwner()->GetActorNameOrLabel()));
				}				
			}
		}
	}
}

void UPDWIDAssignTool::CheckQuestTargetDuplicates(UObject* WorldContextObject)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AActor::StaticClass(), FoundActors);
	TArray<int32> AssignedIDs;

	for(AActor* Actor: FoundActors)
	{
		if (Actor)
		{
			if(UPDWTagComponent* TagComponent = Actor->FindComponentByClass<UPDWTagComponent>())
			{
				if (TagComponent->QuestTargetID != -1)
				{
					if (AssignedIDs.Contains(TagComponent->QuestTargetID))
					{
						UE_LOG(LogTemp, Error, TEXT("%s with ID '%d' Is a Duplicate!"), *(TagComponent->GetOwner()->GetActorNameOrLabel()), (TagComponent->QuestTargetID));
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ID '%s' has no ID!"), *(TagComponent->GetOwner()->GetActorNameOrLabel()));
				}			
			}
		}
	}
}
