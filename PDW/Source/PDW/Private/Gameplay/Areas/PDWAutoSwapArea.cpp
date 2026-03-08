// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Areas/PDWAutoSwapArea.h"
#include "Data/PDWGameSettings.h"
#include "Components/BrushComponent.h"
#include "Data/PDWPlayerState.h"
#include "Managers/PDWEventSubsytem.h"
#include "Gameplay/Components/PDWGASComponent.h"
#include "Data/PDWGameplayTagSettings.h"

// #DEV <starting to be a bit messy, if bug appears, refactor> [#daniele.m, 15 October 2025, ]

APDWAutoSwapArea::APDWAutoSwapArea()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	GetBrushComponent()->SetCollisionProfileName(UPDWGameSettings::GetAutoSwapAreaProfileName().Name);
	bGenerateOverlapEventsDuringLevelStreaming = true;
}

const FGameplayTagContainer& APDWAutoSwapArea::GetAllowedPup() const
{
	return AllowedPups;
}

const FGameplayTagContainer& APDWAutoSwapArea::GetAllowedVehicle() const
{
	return AllowedVehicles;
}

const FGameplayTag& APDWAutoSwapArea::GetDefaultAllowedPup() const
{
	return DefaultAllowedPup;
}

void APDWAutoSwapArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if(bLogicDisabled) 
		return;

	IPDWPlayerInterface* Player = Cast<IPDWPlayerInterface>(OtherActor);
	if (Player)
	{
		APDWPlayerController* Controller = Player->GetPDWPlayerController();
		if (Controller)
		{
			Controller->OnPupCustomizationCompleted.AddUniqueDynamic(this, &APDWAutoSwapArea::OnCustomizationCompleted);
			Controller->CurrentSwapArea = this;
			if (Controller->bDoingSwappingAction)
			{
				Controller->PendingSwapArea = this;
				return;
			}

			Super::NotifyActorBeginOverlap(OtherActor);

			Controller->OnSwapVehicle.AddUniqueDynamic(this,&APDWAutoSwapArea::OnVehicleSwap);
			const FGameplayTag CurrentPup = Controller->GetPDWPlayerState()->GetCurrentPup();
			const bool bIsOnVehicle = Controller->GetPDWPlayerState()->GetIsOnVehicle();

			UPDWEventSubsytem::Get(this)->OnPlayerAreaChangeEvent(Controller, bIsOnVehicle ? AllowedVehicles : AllowedPups, true, this);

			if (AllowedPups.HasTag(CurrentPup) && AutoSwapConfiguration.Contains(CurrentPup))
			{
				if (AutoSwapConfiguration[CurrentPup].AutoSwapRule & (uint8)EAutoSwapRule::SkillOnly)
				{
					if (AutoSwapConfiguration[CurrentPup].AllowedAbility)
					{
						UPDWGASComponent* GASComp = Controller->GetGASComponent();
						if (GASComp)
						{
							if (FGameplayAbilitySpec* AbilitySpec = GASComp->FindAbilitySpecFromClass(AutoSwapConfiguration[CurrentPup].AllowedAbility))
							{
								if (AbilitySpec->IsActive())
								{
									FGameplayEventData EventData;
									EventData.Instigator = this;
									EventData.Target=Controller;
									GASComp->HandleGameplayEvent(UPDWGameplayTagSettings::GetAutoSwapRefreshTag(),&EventData);
									AbilitySpecHandle = AbilitySpec->Handle;
									return;
								}
							}
						}
					}
				}

				if (AutoSwapConfiguration[CurrentPup].AutoSwapRule & (uint8)EAutoSwapRule::VehicleOnly)
				{
					if (!bIsOnVehicle)
					{
						Controller->ChangeCharacter(DefaultAllowedPup);
						Controller->OnPupCustomizationCompleted.AddUniqueDynamic(this, &APDWAutoSwapArea::OnCustomizationCompleted);

					}
				}

				if (!bIsOnVehicle)
				{
					AbilitySpecHandle = Controller->ActivateSkill(true, AutoSwapConfiguration[CurrentPup].AbilityToTrigger);
				}
			}
			else
			{
				//IM NOT AN ALLOWED PUP SO I NEED TO SWAP INTO ZUMA CAR OR PUP BASED ON WIC
				if (bIsOnVehicle)
				{
					Controller->ChangeCharacter(DefaultAllowedPup);
				}
				else
				{
					Controller->ChangeCharacter(DefaultAllowedPup);
					Controller->OnPupCustomizationCompleted.AddUniqueDynamic(this, &APDWAutoSwapArea::OnCustomizationCompleted);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("APDWAutoSwapArea::NotifyActorBeginOverlap - Overlapped actor does not have a PDWPlayerController: %s"), *OtherActor->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APDWAutoSwapArea::NotifyActorBeginOverlap - Overlapped actor is not a Pawn: %s"), *OtherActor->GetName());
	}
}

void APDWAutoSwapArea::NotifyActorEndOverlap(AActor* OtherActor)
{
	if(bLogicDisabled) 
		return;
	Super::NotifyActorEndOverlap(OtherActor);
	IPDWPlayerInterface* Player = Cast<IPDWPlayerInterface>(OtherActor);
	if (Player)
	{
		APDWPlayerController* Controller = Player->GetPDWPlayerController();
		if (Controller)
		{
			Controller->CurrentSwapArea = nullptr;
			Controller->PendingSwapArea = nullptr;
			UPDWGASComponent* GASComp = Controller->GetGASComponent();
			if (GASComp)
			{
				FGameplayEventData EventData;
				EventData.Instigator = this;
				EventData.Target = Controller;
				GASComp->HandleGameplayEvent(UPDWGameplayTagSettings::GetAutoSwapStopRefreshTag(), &EventData);
				GASComp->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("Event.LeavingSwimmingArea"), &EventData);
			}

			Controller->CancelSkillWithHandler(AbilitySpecHandle);
			Controller->OnSwapVehicle.RemoveDynamic(this,&APDWAutoSwapArea::OnVehicleSwap);
			Controller->OnPupCustomizationCompleted.RemoveDynamic(this, &APDWAutoSwapArea::OnCustomizationCompleted);
			UPDWEventSubsytem::Get(this)->OnPlayerAreaChangeEvent(Controller, AllowedPups, false, this);
		}
	}
}

void APDWAutoSwapArea::DisableLogic()
{
	bLogicDisabled = true;
}

void APDWAutoSwapArea::EnableLogic(AActor* Actor)
{
	bLogicDisabled = false;
}

void APDWAutoSwapArea::OnCustomizationCompleted(APDWPlayerController* inController)
{
	AbilitySpecHandle = inController->ActivateSkill(true, AutoSwapConfiguration[DefaultAllowedPup].AbilityToTrigger);
}

void APDWAutoSwapArea::OnVehicleSwap(bool bInVehicleMode,APDWPlayerController* inController)
{
	if(!inController || !inController->GetPDWPlayerState()) return;

	if (bInVehicleMode)
	{
		inController->CancelSkillWithHandler(AbilitySpecHandle);
	}
	else if(!AllowedPups.HasTag(inController->GetPDWPlayerState()->GetCurrentPup()) || !AutoSwapConfiguration.Contains(inController->GetPDWPlayerState()->GetCurrentPup()))
	{
		AbilitySpecHandle = inController->ActivateSkill(true, AutoSwapConfiguration[DefaultAllowedPup].AbilityToTrigger);
	}
}

void APDWAutoSwapArea::BeginPlay()
{
	Super::BeginPlay();

	UPDWEventSubsytem::Get(this)->OnMinigameInitialization.AddUniqueDynamic(this,&APDWAutoSwapArea::DisableLogic);
	UPDWEventSubsytem::Get(this)->OnMinigameCompleteOrLeft.AddUniqueDynamic(this,&APDWAutoSwapArea::EnableLogic);
}

void APDWAutoSwapArea::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UPDWEventSubsytem::Get(this)->OnMinigameInitialization.RemoveDynamic(this, &APDWAutoSwapArea::DisableLogic);
	UPDWEventSubsytem::Get(this)->OnMinigameCompleteOrLeft.RemoveDynamic(this, &APDWAutoSwapArea::EnableLogic);
}
