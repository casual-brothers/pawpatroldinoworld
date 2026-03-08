// Fill out your copyright notice in the Description page of Project Settings.


#include "Flow/States/PDWCustomizationUIFSMState.h"
#include "GameplayTagContainer.h"
#include "Components/NebulaGraphicsCustomizationComponent.h"
#include "UI/Widgets/Customization/PDWCollectionWidget.h"
#include "Data/PDWPupConfigurationAsset.h"
#include "Data/PDWGameSettings.h"
#include "UI/Pages/PDWCustomizationPage.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Data/PDWGraphicCustomizationComponent.h"
#include "Managers/PDWCustomizationSubsystem.h"
#include "Data/FlowDeveloperSettings.h"
#include "Gameplay/Customization/PDWCustomizationLocation.h"
#include "WPHelperSubsystem.h"
#include "Camera/CameraComponent.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Managers/PDWUIManager.h"
#include "Managers/PDWEventSubsytem.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"

void UPDWCustomizationUIFSMState::OnFSMStateEnter_Implementation(const FString& InOption /*= FString("")*/)
{
	Super::OnFSMStateEnter_Implementation(InOption);
	
	UPDWGameplayFunctionLibrary::ApplyQualitySettings(this, "Customization");

	StartingCustomization = UPDWDataFunctionLibrary::GetCustomizationData(this);
	CurrentCustomization = StartingCustomization;

	UPDWCustomizationSubsystem* CustomizationSub = UPDWCustomizationSubsystem::Get(this);

	PupP1CustomizationLocation = UPDWGameplayFunctionLibrary::IsMultiplayerOn(this) ? CustomizationSub->PupP1MultiplayerCustomizationLocation : CustomizationSub->PupP1CustomizationLocation;
	VehicleP1CustomizationLocation = UPDWGameplayFunctionLibrary::IsMultiplayerOn(this) ? CustomizationSub->VehicleP1MultiplayerCustomizationLocation : CustomizationSub->VehicleP1CustomizationLocation;

	if (!PupP1CustomizationLocation)
	{
		TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName());
		return;
	}
	if (PageRef)
	{
		CustomizationPage = Cast<UPDWCustomizationPage>(PageRef);
		CustomizationPage->SetVisibility(ESlateVisibility::Hidden);
	}

	APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	if (P1->GetVehicleInstance())
	{
		P1->GetVehicleInstance()->SetCustomizationSuspensionsLenght();
	}
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		APDWPlayerController* P2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
		if (P2->GetVehicleInstance())
		{
			P2->GetVehicleInstance()->SetCustomizationSuspensionsLenght();
		}
	}

	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (UIManager)
	{
		if (UIManager->IsTransitionOnWithScreenBlack())
		{
			OnTrasitionInOnEnterEnd();
		}
		else
		{
			UIManager->StartTransitionIn();
			UPDWEventSubsytem::Get(this)->OnTransitionInEnd.AddUniqueDynamic(this, &ThisClass::OnTrasitionInOnEnterEnd);
		}
	}
}

void UPDWCustomizationUIFSMState::OnTrasitionInOnEnterEnd()
{
	UPDWEventSubsytem::Get(this)->OnTransitionInEnd.RemoveDynamic(this, &ThisClass::OnTrasitionInOnEnterEnd);
	APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	OldPositionP1 = P1->GetPawn()->GetTransform();
	PupP1CustomizationLocation->GetCustomizationCameraActor()->SetActive(true, true);
	VehicleP1CustomizationLocation->GetCustomizationCameraActor()->SetActive(true, true);
	TArray<AActor*> ActorsToTeleport = { P1->GetIsOnVehicle() ? P1->GetVehicleInstance() : P1->GetPawn() };
	// #TODO_PDW <REWORK THIS CALL, NOW WE CAN TELEPORT ARRAY OF ACTORS AND WE HAVE NEW TELEPORT FEATURE> [#daniele.m, 8 October 2025, OnFirstTeleportCompleted]
	UWPHelperSubsystem::Get(this)->Teleport(ActorsToTeleport, {P1->GetIsOnVehicle() ? VehicleP1CustomizationLocation->GetTransform() : PupP1CustomizationLocation->GetTransform()});
	CustomizationPage->InizializeDataPup(P1);
	UWPHelperSubsystem::Get(this)->OnTeleportCompleted.AddUniqueDynamic(this, &ThisClass::OnFirstTeleportCompleted);
}

void UPDWCustomizationUIFSMState::OnFirstTeleportCompleted()
{
	UWPHelperSubsystem::Get(this)->OnTeleportCompleted.RemoveDynamic(this, &ThisClass::OnFirstTeleportCompleted);
	
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		UPDWCustomizationSubsystem* CustomizationSub = UPDWCustomizationSubsystem::Get(this);
		CustomizationSub->PupP2CustomizationLocation->GetCustomizationCameraActor()->SetActive(true, true);
		CustomizationSub->VehicleP2CustomizationLocation->GetCustomizationCameraActor()->SetActive(true, true);
		APDWPlayerController* P2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
		OldPositionP2 = P2->GetPawn()->GetTransform();
		// #TODO_PDW <REWORK THIS CALL, NOW WE CAN TELEPORT ARRAY OF ACTORS AND WE HAVE NEW TELEPORT FEATURE> [#daniele.m, 8 October 2025, OnFirstTeleportCompleted]
		TArray<AActor*> ActorsToTeleport = { P2->GetIsOnVehicle() ? P2->GetVehicleInstance() : P2->GetPawn() };
		UWPHelperSubsystem::Get(this)->Teleport(ActorsToTeleport, {P2->GetIsOnVehicle() ? CustomizationSub->VehicleP2CustomizationLocation->GetTransform() : CustomizationSub->PupP2CustomizationLocation->GetTransform()});
		CustomizationPage->InizializeDataPup(P2);
		UWPHelperSubsystem::Get(this)->OnTeleportCompleted.AddUniqueDynamic(this, &ThisClass::OnAllTeleportCompleted);
	}
	else
	{
		OnAllTeleportCompleted();
	}
}

void UPDWCustomizationUIFSMState::OnAllTeleportCompleted()
{
	UPDWCustomizationSubsystem* CustomizationSub = UPDWCustomizationSubsystem::Get(this);
	APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	P1->SetViewTarget(P1->GetIsOnVehicle() ? VehicleP1CustomizationLocation : PupP1CustomizationLocation);
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		APDWPlayerController* P2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
		P2->SetViewTarget(P2->GetIsOnVehicle() ? CustomizationSub->VehicleP2CustomizationLocation : CustomizationSub->PupP2CustomizationLocation);
	}
	UWPHelperSubsystem::Get(this)->OnTeleportCompleted.RemoveDynamic(this, &ThisClass::OnAllTeleportCompleted);
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	UIManager->StartTransitionOut();
	CustomizationPage->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UPDWCustomizationUIFSMState::OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender)
{
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (UIManager->IsTransitionOn())
	{
		return;
	}

	Super::OnFSMStateAction_Implementation(Action, Parameter, ControllerSender);

	APDWPlayerController* PDWPlayerController = Cast<APDWPlayerController>(ControllerSender);
	if (Action == UPDWGameSettings::GetActionChangePup())
	{
		PDWPlayerController->OnSwapEnd.AddUniqueDynamic(this, &ThisClass::RestorePupPosition);
		PDWPlayerController->ChangeCharacter(FGameplayTag::RequestGameplayTag(FName(Parameter)));
		CustomizationPage->InizializeDataPup(PDWPlayerController);
	}
	if (Action == UPDWGameSettings::GetCustomizationAction())
	{
		CustomizePup(PDWPlayerController, Parameter.IsEmpty() ? FGameplayTag() : FGameplayTag::RequestGameplayTag(FName(Parameter)));
		PlayPupOnChangeCustomizationAudio(PDWPlayerController, Parameter);
	}
	if (Action == UPDWGameSettings::GetToggleVehiclePupAction())
	{
		PDWPlayerController->OnSwapEnd.AddUniqueDynamic(this, &ThisClass::RestorePupPosition);
		PDWPlayerController->ChangeVehicle();
	}
	if (Action == UPDWGameSettings::GetUIActionConfirm())
	{
		UPDWDataFunctionLibrary::SaveGame(this);
		StartExitFlow();
	}
	if (Action == UPDWGameSettings::GetUIActionBack())
	{
		ResetToStartingCustomization();
		UPDWDataFunctionLibrary::SetCustomizationData(this, StartingCustomization);
		StartExitFlow();
	}
}

void UPDWCustomizationUIFSMState::RestorePupPosition(APDWPlayerController* PDWPlayerController)
{
	PDWPlayerController->OnSwapEnd.RemoveDynamic(this, &ThisClass::RestorePupPosition);
	CustomizationPage->SwitchPupToVehicle(PDWPlayerController);
	UPDWCustomizationSubsystem* CustomizationSub = UPDWCustomizationSubsystem::Get(this);
	if (UPDWGameplayFunctionLibrary::IsSecondPlayer(PDWPlayerController))
	{
		PDWPlayerController->SetViewTarget(PDWPlayerController->GetIsOnVehicle() ? CustomizationSub->VehicleP2CustomizationLocation : CustomizationSub->PupP2CustomizationLocation);
		PDWPlayerController->GetIsOnVehicle() ? PDWPlayerController->GetVehicleInstance()->SetActorTransform(CustomizationSub->VehicleP2CustomizationLocation->GetTransform()) : PDWPlayerController->GetPawn()->SetActorTransform(CustomizationSub->PupP2CustomizationLocation->GetTransform());
	}
	else
	{
		PDWPlayerController->SetViewTarget(PDWPlayerController->GetIsOnVehicle() ? VehicleP1CustomizationLocation : PupP1CustomizationLocation);
		PDWPlayerController->GetIsOnVehicle() ? PDWPlayerController->GetVehicleInstance()->SetActorTransform(VehicleP1CustomizationLocation->GetTransform()) : PDWPlayerController->GetPawn()->SetActorTransform(PupP1CustomizationLocation->GetTransform());
	}
	
	if (PDWPlayerController->GetVehicleInstance())
	{
		PDWPlayerController->GetVehicleInstance()->SetCustomizationSuspensionsLenght();
	}
}

void UPDWCustomizationUIFSMState::CustomizePup(APDWPlayerController* PDWPlayerController, FGameplayTag NewCustomization)
{
	if (UPDWGameplayFunctionLibrary::IsSecondPlayer(PDWPlayerController))
	{
		UNebulaGraphicsCustomizationComponent* GraphicsCustomizationComp = PDWPlayerController->GetIsOnVehicle() ? PDWPlayerController->GetVehicleInstance()->GetComponentByClass<UNebulaGraphicsCustomizationComponent>() :
			PDWPlayerController->GetPupInstance()->GetComponentByClass<UNebulaGraphicsCustomizationComponent>();
		if (GraphicsCustomizationComp)
		{
			FGameplayTagContainer ContainerTags;
			ContainerTags.AddTag(NewCustomization);
			if (PDWPlayerController->GetIsOnVehicle())
			{
				GraphicsCustomizationComp->CustomizeMaterial(ContainerTags);
				CurrentCustomization.GuestVehicleCustomizations.Emplace(PDWPlayerController->GetCurrentPup(), NewCustomization);
			}
			else
			{
				GraphicsCustomizationComp->CustomizeStaticMesh(ContainerTags);
				GraphicsCustomizationComp->CustomizeTexture(ContainerTags);
				CurrentCustomization.GuestPupCustomizations.Emplace(PDWPlayerController->GetCurrentPup(), NewCustomization);
			}
		}
	}
	else
	{
		UNebulaGraphicsCustomizationComponent* GraphicsCustomizationComp = PDWPlayerController->GetIsOnVehicle() ? PDWPlayerController->GetVehicleInstance()->GetComponentByClass<UNebulaGraphicsCustomizationComponent>() :
			PDWPlayerController->GetPupInstance()->GetComponentByClass<UNebulaGraphicsCustomizationComponent>();
		if (GraphicsCustomizationComp)
		{
			FGameplayTagContainer ContainerTags;
			ContainerTags.AddTag(NewCustomization);
			if (PDWPlayerController->GetIsOnVehicle())
			{
				GraphicsCustomizationComp->CustomizeMaterial(ContainerTags);
				CurrentCustomization.VehicleCustomizations.Emplace(PDWPlayerController->GetCurrentPup(), NewCustomization);
			}
			else
			{
				GraphicsCustomizationComp->CustomizeStaticMesh(ContainerTags);
				GraphicsCustomizationComp->CustomizeTexture(ContainerTags);
				CurrentCustomization.PupCustomizations.Emplace(PDWPlayerController->GetCurrentPup(), NewCustomization);
			}
		}
	}

	UPDWDataFunctionLibrary::SetCustomizationData(this, CurrentCustomization);
}

void UPDWCustomizationUIFSMState::OnFSMStateExit_Implementation()
{
	UPDWGameplayFunctionLibrary::RestoreQualitySettings(this, "Customization");

	APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	if (P1->GetVehicleInstance())
	{
		P1->GetVehicleInstance()->ResetSuspensionsLenght();
	}
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		APDWPlayerController* P2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
		if (P2->GetVehicleInstance())
		{
			P2->GetVehicleInstance()->ResetSuspensionsLenght();
		}
	}

	Super::OnFSMStateExit_Implementation();
}

void UPDWCustomizationUIFSMState::StartExitFlow()
{
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	UIManager->StartTransitionIn();
	UPDWEventSubsytem::Get(this)->OnTransitionInEnd.AddUniqueDynamic(this, &ThisClass::OnTrasitionInOnExitEnd);
}

void UPDWCustomizationUIFSMState::OnTrasitionInOnExitEnd()
{
	UPDWEventSubsytem::Get(this)->OnTransitionInEnd.RemoveDynamic(this, &ThisClass::OnTrasitionInOnExitEnd);
	
	//Teleport P1
	APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	PupP1CustomizationLocation->GetCustomizationCameraActor()->SetActive(false, false);
	VehicleP1CustomizationLocation->GetCustomizationCameraActor()->SetActive(false, false);
	P1->SetViewTarget(P1->GetIsOnVehicle()? P1->GetVehicleInstance() : P1->GetPawn());
	TArray<AActor*> ActorsToTeleport = { P1->GetIsOnVehicle() ? P1->GetVehicleInstance() : P1->GetPawn() };
	UWPHelperSubsystem::Get(this)->OnTeleportCompleted.AddUniqueDynamic(this, &ThisClass::OnExitTeleportCompleted);
	UWPHelperSubsystem::Get(this)->Teleport(ActorsToTeleport, {OldPositionP1});
}

void UPDWCustomizationUIFSMState::OnExitTeleportCompleted()
{
	UWPHelperSubsystem::Get(this)->OnTeleportCompleted.RemoveDynamic(this, &ThisClass::OnExitTeleportCompleted);
	//Teleport P2
	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this))
	{
		APDWPlayerController* P2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
		UPDWCustomizationSubsystem::Get(this)->PupP2CustomizationLocation->GetCustomizationCameraActor()->SetActive(false, false);
		UPDWCustomizationSubsystem::Get(this)->VehicleP2CustomizationLocation->GetCustomizationCameraActor()->SetActive(false, false);
		P2->SetViewTarget(P2->GetIsOnVehicle()? P2->GetVehicleInstance() : P2->GetPawn());
		TArray<AActor*> ActorsToTeleport = { P2->GetIsOnVehicle() ? P2->GetVehicleInstance() : P2->GetPawn() };
		UWPHelperSubsystem::Get(this)->Teleport(ActorsToTeleport, {OldPositionP2});
	}
	
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	UIManager->StartTransitionOut();
	TriggerTransition(UFlowDeveloperSettings::GetSwitcherTag().GetTagName());
}

void UPDWCustomizationUIFSMState::ResetToStartingCustomization()
{
	APDWPlayerController* P1 = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
	if (P1)
	{
		const FPupCustomizationInfo& PupCustomizationInfo = UPDWGameSettings::GetPupCustomizationInfo(P1->GetCurrentPup());
		FGameplayTag DefaultPupCustomization = StartingCustomization.PupCustomizations.Contains(P1->GetCurrentPup()) ? StartingCustomization.PupCustomizations[P1->GetCurrentPup()] : PupCustomizationInfo.HatCustomizationTag;
		FGameplayTag DefaultVehicleCustomization = StartingCustomization.VehicleCustomizations.Contains(P1->GetCurrentPup()) ? StartingCustomization.VehicleCustomizations[P1->GetCurrentPup()] : PupCustomizationInfo.DefaultVehicleCustomizationTag;
		CustomizePup(P1, P1->GetIsOnVehicle() ? DefaultVehicleCustomization : DefaultPupCustomization);
	}

	APDWPlayerController* P2 = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
	if (P2 && StartingCustomization.PupCustomizations.Contains(P2->GetCurrentPup()))
	{
		const FPupCustomizationInfo& PupCustomizationInfo = UPDWGameSettings::GetPupCustomizationInfo(P2->GetCurrentPup());
		FGameplayTag DefaultPupCustomization = StartingCustomization.PupCustomizations.Contains(P2->GetCurrentPup()) ? StartingCustomization.PupCustomizations[P2->GetCurrentPup()] : PupCustomizationInfo.HatCustomizationTag;
		FGameplayTag DefaultVehicleCustomization = StartingCustomization.GuestVehicleCustomizations.Contains(P1->GetCurrentPup()) ? StartingCustomization.GuestVehicleCustomizations[P1->GetCurrentPup()] : PupCustomizationInfo.DefaultVehicleCustomizationTag;
		CustomizePup(P2, P2->GetIsOnVehicle() ? DefaultVehicleCustomization : DefaultPupCustomization);
	}
}

void UPDWCustomizationUIFSMState::PlayPupOnChangeCustomizationAudio(APDWPlayerController* PDWPlayerController, FString Parameter)
{
	if (!PDWPlayerController)
	{
		return;
	}
	
	if (Parameter.IsEmpty())
	{
		AudioToPlay = LockedItemAudioId;
	}
	else
	{
		if (AudioByTag.Contains(PDWPlayerController->GetCurrentPup()) && AudioByTag[PDWPlayerController->GetCurrentPup()].AudioToPlay.Num())
		{
			AudioToPlay = AudioByTag[PDWPlayerController->GetCurrentPup()].AudioToPlay[FMath::RandHelper(AudioByTag[PDWPlayerController->GetCurrentPup()].AudioToPlay.Num())];
		}
	}
	CurrentTime = 0.f;
	bPlayAudio = true;
}

void UPDWCustomizationUIFSMState::OnFSMStateUpdate_Implementation(float deltaTime)
{
	Super::OnFSMStateUpdate_Implementation(deltaTime);

	if (bPlayAudio)
	{
		CurrentTime += deltaTime;
		if (CurrentTime >= .2f)
		{
			bPlayAudio = false;
			CurrentTime = 0.f;
			UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), AudioToPlay);
		}
	}
}