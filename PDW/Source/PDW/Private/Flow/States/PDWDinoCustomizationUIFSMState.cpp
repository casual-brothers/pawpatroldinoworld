// Fill out your copyright notice in the Description page of Project Settings.


#include "Flow/States/PDWDinoCustomizationUIFSMState.h"
#include "UI/Pages/PDWCustomizationPage.h"
#include "Modes/PDWGameplayGameMode.h"
#include "GameplayTagContainer.h"
#include "Gameplay/FSMFlow/Helpers/PDWFsmHelper.h"
#include "Managers/PDWCustomizationSubsystem.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Managers/PDWPaleoCenterSubsystem.h"
#include "Data/PDWGameSettings.h"
#include "Data/FlowDeveloperSettings.h"
#include "Gameplay/Customization/PDWCustomizationLocation.h"
#include "WPHelperSubsystem.h"
#include "Camera/CameraComponent.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/NebulaGraphicsCustomizationComponent.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Data/PDWPaleoCenterSettings.h"
#include "Managers/PDWEventSubsytem.h"

void UPDWDinoCustomizationUIFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);

	UPDWGameplayFunctionLibrary::ApplyQualitySettings(this, "Customization");

	if (PageRef)
	{
		CustomizationPage = Cast<UPDWCustomizationPage>(PageRef);
		CustomizationPage->SetOwningPlayer(InOption == "0" ? UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this) : UPDWGameplayFunctionLibrary::GetNebulaPlayerControllerTwo(this));
		CustomizationPage->SetVisibility(ESlateVisibility::Hidden);
	}

	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	bCurrentlyInTransition = true;
	UPDWEventSubsytem::Get(this)->OnTransitionInEnd.AddUniqueDynamic(this, &ThisClass::OnTransitionInEnd);
	UIManager->StartTransitionIn();

}

void UPDWDinoCustomizationUIFSMState::OnTransitionInEnd()
{
	bCurrentlyInTransition = false;
	UPDWEventSubsytem::Get(this)->OnTransitionInEnd.RemoveDynamic(this, &ThisClass::OnTransitionInEnd);
	CustomizationPage->SetVisibility(ESlateVisibility::HitTestInvisible);
	APDWGameplayGameMode* GM = APDWGameplayGameMode::Get(this);
	DinoTagToCustomize = GM->FSMHelper->PendingDinoTagCustomization;
	GM->FSMHelper->PendingDinoTagCustomization = FGameplayTag();
	CustomizationPage->InizializeDataDino(DinoTagToCustomize);	
	StartingCustomization = UPDWDataFunctionLibrary::GetDinoCustomization(this, DinoTagToCustomize);

	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		AActor* DinoActor = PaleoCenterSubsystem->GetDinoActorRef(DinoTagToCustomize);
		if (DinoActor)
		{
			if (UPDWCustomizationSubsystem::Get(this)->DinoLocations.Contains(DinoTagToCustomize))
			{
				FCustomizationLocationDinoData LocationData = UPDWCustomizationSubsystem::Get(this)->DinoLocations[DinoTagToCustomize];
				LocationData.DinoLocation->GetCustomizationCameraActor()->SetActive(true, true);
				UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this)->SetViewTarget(LocationData.DinoLocation);
				TArray<AActor*> ActorsToTeleport = { DinoActor };
				UWPHelperSubsystem::Get(this)->Teleport(ActorsToTeleport, {LocationData.DinoLocation->GetTransform()});
				CurrentPenActivity = PaleoCenterSubsystem->GetDinoPenActivity(DinoTagToCustomize);
				PaleoCenterSubsystem->ChangeDinoPenActivity(DinoTagToCustomize, false);

				UWPHelperSubsystem::Get(this)->OnTeleportCompleted.AddUniqueDynamic(this, &ThisClass::OnFirstTeleportComplete);
			}
		}
	}
}

void UPDWDinoCustomizationUIFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);
	if(bCurrentlyInTransition)
		return;

	if (Action == UPDWGameSettings::GetCustomizationAction())
	{
		if (!Parameter.IsEmpty())
		{
			CustomizeDino(FGameplayTag::RequestGameplayTag(FName(Parameter)));
		}
	}
	if (Action == UPDWGameSettings::GetUIActionConfirm())
	{
		FGameplayTag CustomizationTag = StartingCustomization != FGameplayTag::EmptyTag ? StartingCustomization : UPDWPaleoCenterSettings::Get()->DefaultDinoCustomizationTag;
		if (!CustomizationTag.MatchesTagExact(CurrentCustomization))
		{
			//Achievement
			if (UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(this))
			{
				EventSubsystem->OnDinoCustomizeEvent();
			}

		}
		UPDWDataFunctionLibrary::SaveGame(this);
		TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName());
	}
	if (Action == UPDWGameSettings::GetUIActionBack())
	{
		ResetToStartingCustomization();
		UPDWDataFunctionLibrary::SetDinoCustomization(this, DinoTagToCustomize, StartingCustomization);
		TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName());
	}
}

void UPDWDinoCustomizationUIFSMState::OnFSMStateExit_Implementation()
{
	if (UPDWCustomizationSubsystem::Get(this)->DinoLocations.Contains(DinoTagToCustomize))
	{
		UPDWCustomizationSubsystem::Get(this)->DinoLocations[DinoTagToCustomize].DinoLocation->GetCustomizationCameraActor()->SetActive(true, true);
	}
	
	APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	P1->SetViewTarget(P1->GetIsOnVehicle()? P1->GetVehicleInstance() : P1->GetPawn());
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		APDWPlayerController* P2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
		P2->SetViewTarget(P2->GetIsOnVehicle()? P2->GetVehicleInstance() : P2->GetPawn());
	}

	if (UPDWCustomizationSubsystem::Get(this)->DinoLocations.Contains(DinoTagToCustomize))
	{
		UPDWCustomizationSubsystem::Get(this)->DinoLocations[DinoTagToCustomize].DinoLocation->GetCustomizationCameraActor()->SetActive(false, false);
	}
	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		PaleoCenterSubsystem->ChangeDinoPenActivity(DinoTagToCustomize, CurrentPenActivity);
	}

	UGameplayStatics::SetForceDisableSplitscreen(this, false);

	UPDWGameplayFunctionLibrary::RestoreQualitySettings(this, "Customization");

	Super::OnFSMStateExit_Implementation();
}

void UPDWDinoCustomizationUIFSMState::OnFirstTeleportComplete()
{
	UWPHelperSubsystem::Get(this)->OnTeleportCompleted.RemoveDynamic(this, &ThisClass::OnFirstTeleportComplete);
	FCustomizationLocationDinoData LocationData = UPDWCustomizationSubsystem::Get(this)->DinoLocations[DinoTagToCustomize];

	//Safe spot Players
	if (!LocationData.SafeSpotP1.GetTranslation().Equals(FVector::Zero()))
	{
		APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
		P1->GetIsOnVehicle() ? P1->GetVehicleInstance()->SetActorTransform(LocationData.SafeSpotP1) : P1->GetPawn()->SetActorTransform(LocationData.SafeSpotP1);
	}
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		if (!LocationData.SafeSpotP2.GetTranslation().Equals(FVector::Zero()))
		{
			APDWPlayerController* P2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
			P2->GetIsOnVehicle() ? P2->GetVehicleInstance()->SetActorTransform(LocationData.SafeSpotP2) : P2->GetPawn()->SetActorTransform(LocationData.SafeSpotP2);
		}
		UGameplayStatics::SetForceDisableSplitscreen(this,true);
	}

	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	UIManager->StartTransitionOut();
}

void UPDWDinoCustomizationUIFSMState::CustomizeDino(FGameplayTag NewCustomization)
{
	if(!NewCustomization.IsValid()) return;

	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		AActor* DinoActor = PaleoCenterSubsystem->GetDinoActorRef(DinoTagToCustomize);
		if (DinoActor)
		{
			UNebulaGraphicsCustomizationComponent* GrapichsComp = DinoActor->GetComponentByClass<UNebulaGraphicsCustomizationComponent>();
			if (GrapichsComp)
			{
				FGameplayTagContainer ContainersTag;
				ContainersTag.AddTag(NewCustomization);
				GrapichsComp->CustomizeStaticMesh(ContainersTag);
				CurrentCustomization = NewCustomization;
				UPDWDataFunctionLibrary::SetDinoCustomization(this, DinoTagToCustomize, NewCustomization);
			}
		}
	}
}

void UPDWDinoCustomizationUIFSMState::ResetToStartingCustomization()
{
	FGameplayTag CustomizationTag = StartingCustomization != FGameplayTag::EmptyTag ? StartingCustomization : UPDWPaleoCenterSettings::Get()->DefaultDinoCustomizationTag;
	CustomizeDino(CustomizationTag);
}
