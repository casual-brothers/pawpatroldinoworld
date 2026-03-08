// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/PDWPaleoCenterSubsystem.h"
#include "FlowSubsystem.h"
#include "Data/PDWPaleoCenterSettings.h"
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "Gameplay/Actors/PDWDinoPen.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Gameplay/Actors/PDWDinoPenArea.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWPaleoCenterSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	//Wait for event OnComponentRegistered (Dino Pen) (use this function or init game of gamemode?)
	//Load timer from saves?
}

void UPDWPaleoCenterSubsystem::Init()
{
	UPDWPaleoCenterSettings* PaleoCenterSettings = UPDWPaleoCenterSettings::Get();
	PaleoCenterConfig = Cast<UPDWPaleoCenterConfig>(PaleoCenterSettings->PaleoCenterConfig.TryLoad());

	AllDinoNeeds = UPDWGameplayTagSettings::GetDinoNeeds();

	for(TSubclassOf<UPDWDinoPen> DinoPen : PaleoCenterConfig->DinoPens)
	{
		UPDWDinoPen* NewDinoPen = NewObject<UPDWDinoPen>(this, DinoPen);
		//NewDinoPen->Init();
		DinoPens.Add(NewDinoPen);
	}
}

void UPDWPaleoCenterSubsystem::OnStartPlay()
{
	for (int32 i = 0; i < DinoPens.Num(); i++)
	{
		DinoPens[i]->OnStartPlay();
	}
}

void UPDWPaleoCenterSubsystem::StartPaleoQuests(bool FromLoad)
{
	for (int32 i = 0; i < DinoPens.Num(); i++)
	{
		DinoPens[i]->StartPaleoQuests(FromLoad);
	}
}

void UPDWPaleoCenterSubsystem::Uninit()
{
	for(UPDWDinoPen* DinoPen : DinoPens)
	{
		if (DinoPen)
		{
			DinoPen->Uninit();
		}
	}
}

void UPDWPaleoCenterSubsystem::UnlockDinoPen(FGameplayTag DinoPenTag, bool StartInactive)
{
	for (int32 i = 0; i < DinoPens.Num(); i++)
	{
		if (DinoPens[i]->DinoPenTag == DinoPenTag)
		{
			DinoPens[i]->UnlockPen(StartInactive);

			if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
			{
				EventSubsystem->OnDinoPenUnlockedEvent(DinoPenTag);
			}
		}
	}	
}

void UPDWPaleoCenterSubsystem::ChangeDinoPenActivity(FGameplayTag DinoPenTag, bool IsActive, bool ResetNeedTimer)
{
	for (int32 i = 0; i < DinoPens.Num(); i++)
	{
		if (DinoPens[i]->DinoPenTag == DinoPenTag)
		{
			DinoPens[i]->SetIsPenActive(IsActive, ResetNeedTimer);
		}
	}	
}

bool UPDWPaleoCenterSubsystem::GetDinoPenActivity(FGameplayTag DinoPenTag)
{
	for (int32 i = 0; i < DinoPens.Num(); i++)
	{
		if (DinoPens[i]->DinoPenTag == DinoPenTag)
		{
			return DinoPens[i]->GetIsPenActive();
		}
	}	

	return false;
}

bool UPDWPaleoCenterSubsystem::IsDinoPenUnlocked(FGameplayTag DinoPenTag)
{
	for (int32 i = 0; i < DinoPens.Num(); i++)
	{
		if (DinoPens[i]->DinoPenTag == DinoPenTag)
		{
			return DinoPens[i]->GetIsPenUnlocked();
		}
	}	

	return false;
}

AActor* UPDWPaleoCenterSubsystem::GetDinoActorRef(FGameplayTag DinoPenTag)
{
	for (int32 i = 0; i < DinoPens.Num(); i++)
	{
		if (DinoPens[i]->DinoPenTag == DinoPenTag)
		{
			return DinoPens[i]->GetDinoActorRef();
		}
	}	

	return nullptr;
}

void UPDWPaleoCenterSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TickControl && PaleoCenterConfig)
	{
		UpdateFoodAndWaterQuantity(DeltaTime);
		CheckDinoNeeds(DeltaTime);		
	}
}

void UPDWPaleoCenterSubsystem::UpdateFoodAndWaterQuantity(float DeltaTime)
{
	CurrentUpdateFoodAndWaterTimer += DeltaTime;
	if(CurrentUpdateFoodAndWaterTimer < PaleoCenterConfig->UpdateFoodAndWaterInterval)
		return;

	CurrentUpdateFoodAndWaterTimer = 0.f;

	for (UPDWDinoPen* DinoPen : DinoPens)
	{
		if (DinoPen && DinoPen->GetIsPenActive() && DinoPen->GetShouldSimulate())
		{
			DinoPen->DecrementWaterStation(DinoPen->SimulatedWaterDecrement);
		}
	}
}

void UPDWPaleoCenterSubsystem::CheckDinoNeeds(float DeltaTime)
{
	CurrentUpdateDinoNeedsTimer += DeltaTime;
	if(CurrentUpdateDinoNeedsTimer < PaleoCenterConfig->UpdateDinoNeedsInterval)
		return;

	CurrentUpdateDinoNeedsTimer = 0.f;

	FGameplayTag& RandomNeed = AllDinoNeeds[FMath::RandRange(0, AllDinoNeeds.Num() - 1)];
	
	int32 MaxNeeds = PaleoCenterConfig->MaxNeeds.Contains(RandomNeed) ? PaleoCenterConfig->MaxNeeds[RandomNeed] : DinoPens.Num();
	int32 NeedContuer = 0;
	TArray<int32> ActivePens {};

	for (int32 i = 0; i < DinoPens.Num(); i++)
	{
		if (DinoPens[i]->GetIsPenActive())
		{
			ActivePens.Add(i);
			if (DinoPens[i]->GetCurrentDinoNeed() == RandomNeed)
			{
				NeedContuer++;
			}
		}
	}

	if(ActivePens.Num() > 0)
	{ 
		int32 RandomDinoIndex = ActivePens[FMath::RandRange(0, ActivePens.Num() - 1)];
		if (DinoPens.IsValidIndex(RandomDinoIndex) && DinoPens[RandomDinoIndex]->GetCurrentDinoNeed() == UPDWGameplayTagSettings::GetHappyDino() && NeedContuer < MaxNeeds)
		{
			DinoPens[RandomDinoIndex]->UpdateDinoNeeds(RandomNeed);
		}
	}
}

void UPDWPaleoCenterSubsystem::AddDinoPenRef(UPDWDinoPen* DinoPen)
{
	if (UnlockingDinoPen != FGameplayTag::EmptyTag)
	{
		DinoPen->DinoPenTag = UnlockingDinoPen;
		UnlockingDinoPen = FGameplayTag::EmptyTag;
	}

	DinoPens.Add(DinoPen);
}

void UPDWPaleoCenterSubsystem::RemoveDinoPenRef(UPDWDinoPen* DinoPen)
{
	DinoPens.Remove(DinoPen); 
}

FPDWDinoPenInfo UPDWPaleoCenterSubsystem::GetDinoPenInfo(FGameplayTag DinoPenTag)
{
	for (int32 i = 0; i < DinoPens.Num(); i++)
	{
		if (DinoPens[i]->GetIsPenUnlocked() && DinoPens[i]->DinoPenTag == DinoPenTag)
		{
			return DinoPens[i]->GetPenInfo();
		}
	}

	return FPDWDinoPenInfo();
}

FPDWDinoPenInfo UPDWPaleoCenterSubsystem::GetDinoPenInfoAndActivity(FGameplayTag DinoPenTag)
{
	for (int32 i = 0; i < DinoPens.Num(); i++)
	{
		if (DinoPens[i]->DinoPenTag == DinoPenTag) 
		{
			return DinoPens[i]->GetPenInfo();
		}
	}

	return FPDWDinoPenInfo();
}

APDWDinoPenArea* UPDWPaleoCenterSubsystem::GetPenAreaByPenTag(FGameplayTag PenTag)
{
	for (int32 i = 0; i < DinoPens.Num(); i++)
	{
		if (DinoPens[i]->DinoPenTag == PenTag)
		{
			return DinoPens[i]->GetPenAreaRef();
		}
	}

	return nullptr;
}

void UPDWPaleoCenterSubsystem::OnDinoLoadedEvent(AActor* DinoRef)
{
	OnDinoLoad.Broadcast(DinoRef);
}
