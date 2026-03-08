// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Gameplay/Components/PDWBreadCrumbsComponent.h"
#include "Data/PDWPlayerState.h"
#include "Data/PDWAttributeSet.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "NebulaVehicle/NebulaVehicle.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Core/NebulaFlowCoreDelegates.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "Data/PDWGameSettings.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Managers/PDWEventSubsytem.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Areas/PDWExclusionCollisionVolume.h"
#include "Gameplay/Areas/PDWAutoSwapArea.h"
#include "Components/ForceFeedbackComponent.h"

void APDWPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetHidden(false);

	BeforeSaveHandle = FNebulaFlowCoreDelegates::OnBeforSave.AddUObject(this, &APDWPlayerController::OnBeforeSave);
	SwapCharacterAbility = UPDWGameSettings::GetBasicSkill(EBaseSkillType::SwapPup);
	SwapVehicleAbility = UPDWGameSettings::GetBasicSkill(EBaseSkillType::ToggleVehicle);
	JumpAbility = UPDWGameSettings::GetBasicSkill(EBaseSkillType::Jump);
	InteractAbility = UPDWGameSettings::GetBasicSkill(EBaseSkillType::InteractionBase);
	AddMovementAbilities();

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnInteractionSuccess.AddUniqueDynamic(this, &APDWPlayerController::OnInteractionSuccess);
		EventSubsystem->OnInteractionStateAdd.AddUniqueDynamic(this, &APDWPlayerController::OnInteractionStateAdded);
	}

	// TODO: We don't have a system to keep track of which are we are in so we are doing it here
	// Remove this when a proper system is implemented!!
	UPDWDataFunctionLibrary::SetPlayerAreaId(this, FGameplayTag::EmptyTag);

	//InitializeGASSystem();
	//InitPlayerAbilities();
}
//#TODO DM: IMPLEMENT THE CORRECT FLOW FOR USAGE OF ABILITIES
/*
	on beginplay give ability to the current pup, then everytime  we swap character we give the proper pup specific skills.
	when we want to use the skill we use try activate and we store the handle from give ability to a tmap.
	read https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-abilities-in-unreal-engine
*/
void APDWPlayerController::InitPlayerAbilities()
{
	if (GASComponent)
	{
		/*TestAbilitySpec = FGameplayAbilitySpec(UPDWGameSettings::GetEntitySkill(PDWPlayerState->GetCurrentPup()), 1, -1);
		TestHandle = GASComponent->GiveAbility(TestAbilitySpec);*/
		/*GASComponent->ClearAbility(TestHandle);*/
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APDWPlayerController::InitPlayerAbilities - GASComponent is not initialized"));
	}
}

void APDWPlayerController::AddMovementAbilities()
{
	if (GASComponent && MovementAbilityClass)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(MovementAbilityClass, 1, -1);
		GASComponent->GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

UForceFeedbackEffect* APDWPlayerController::GetVibrationsByTag(FGameplayTag VibrationTag)
{
	return VibrationsConfig.Contains(VibrationTag) ? VibrationsConfig[VibrationTag] : nullptr;
}

void APDWPlayerController::BeginDestroy()
{
	Super::BeginDestroy();
}

void APDWPlayerController::SetNextPupForSwap(const FGameplayTag& inNewPup)
{
	NextPupForSwap = inNewPup;
}

const FGameplayTag& APDWPlayerController::GetNextPupForSwap() const
{
	return NextPupForSwap;
}

const FGameplayTag APDWPlayerController::GetCurrentPup() const
{
	return PDWPlayerState->GetCurrentPup();
}

TSubclassOf<APawn> APDWPlayerController::GetPupClassForSwap(const EPupRequestType& inPupRequest,bool bRequestVehicle /*= false*/) const
{
	if (inPupRequest == EPupRequestType::Current)
	{
		if (bRequestVehicle)
		{
			return UPDWGameSettings::GetPupVehicleClass(PDWPlayerState->GetCurrentPup());
		}
		else
		{
			return UPDWGameSettings::GetPupClass(PDWPlayerState->GetCurrentPup());
		}
	}
	else
	{
		if (bRequestVehicle)
		{
			return UPDWGameSettings::GetPupVehicleClass(NextPupForSwap);
		}
		else
		{
			return UPDWGameSettings::GetPupClass(NextPupForSwap);
		}
	}
}

APDWVehiclePawn* APDWPlayerController::GetVehicleInstance()
{
	return Vehicle;
}

bool APDWPlayerController::GetIsOnVehicle() const
{
	return PDWPlayerState->GetIsOnVehicle();
}

void APDWPlayerController::SetVehicleInstance(APDWVehiclePawn* inVehicle)
{
	Vehicle = inVehicle;
}

void APDWPlayerController::OnInteractFailed()
{
	//ActivateSkill();
}

void APDWPlayerController::HandleSwapEnd()
{
	SetNextPupForSwap(FGameplayTag::EmptyTag);
	RefreshBreadcrumbsOwner();
	if (bDoingSwappingAction)
	{
		bDoingSwappingAction = false;
		if (PendingExclusionVolume)
		{
			PendingExclusionVolume->NotifyActorBeginOverlap(GetIsOnVehicle() ? GetVehicleInstance() : GetPawn());
			PendingExclusionVolume = nullptr;
		}
		else if (PendingSwapArea)
		{
			PendingSwapArea->NotifyActorBeginOverlap(GetIsOnVehicle() ? GetVehicleInstance() : GetPawn());
			PendingSwapArea = nullptr;
		}
	}
	APDWGameplayGameMode* GameMode = Cast<APDWGameplayGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		GameMode->UpdateTargetActorFreeCamera();
	}

	OnSwapEnd.Broadcast(this);
}

void APDWPlayerController::OnPossess(APawn* InPawn)
{
	BreadCrumbsComponent = GetComponentByClass<UPDWBreadCrumbsComponent>();
	GASComponent = GetComponentByClass<UPDWGASComponent>();
	PDWPlayerState = Cast<APDWPlayerState>(PlayerState);
	ForceFeedbackComponent = GetComponentByClass<UForceFeedbackComponent>();
	BreadCrumbsComponent->InitComponent(this);
	CharacterPup = Cast<APDWCharacter>(InPawn);
	if (CharacterPup)
	{
		PDWPlayerState->SetCharacterPup(CharacterPup);
	}
	//GIVE ABILITY LIST AND CALL ONLY TRYACTIVATE ON SINGLE SKILL FUNction.
	Super::OnPossess(InPawn);
}

void APDWPlayerController::Interact()
{
	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(InteractAbility, 1, -1);
	GASComponent->GiveAbilityAndActivateOnce(AbilitySpec);
}

FGameplayAbilitySpecHandle APDWPlayerController::ActivateSkill(bool bPressed,TSubclassOf<UPDWGASAbility> inSkill /*= nullptr*/)
{	
	FGameplayEventData EventData;
	EventData.Instigator = this;
	if (bPressed)
	{
		if (!inSkill)
		{
			if (GetIsOnVehicle())
			{
				inSkill = UPDWGameSettings::GetBasicSkill(EBaseSkillType::VehicleSkill);
			}
			else
			{
				inSkill = UPDWGameSettings::GetEntitySkill(PDWPlayerState->GetCurrentPup());
			}
		}
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(inSkill, 1, -1);
		GASComponent->HandleGameplayEvent(UPDWGameplayTagSettings::GetInputPressedTag(),&EventData);
		CurrentSkillHandler = GASComponent->GiveAbilityAndActivateOnce(AbilitySpec);
		
		//#TODO DM: for rework try to use this can activate false to refresh the skill, if it false it means is already on going so we should 
		// use the false statement to trigger event that will be used to increment skill duration.
		//bool bCanActivate = GASComponent->TryActivateAbility(TestHandle);
		//if (!bCanActivate)
		//{
		//	GASComponent->HandleGameplayEvent(UPDWGameplayTagSettings::Get()->GetInteractionAreaTag(), nullptr);
		//}
	}
	else
	{

		GASComponent->HandleGameplayEvent(UPDWGameplayTagSettings::GetInputReleasedTag(),&EventData);
	}

	return CurrentSkillHandler;
}

void APDWPlayerController::ChangeCharacter(const FGameplayTag& inNewPup)
{
	if (inNewPup != PDWPlayerState->GetCurrentPup())
	{
		OnChangeCharacterRequested.Broadcast(this);
		CancelSkillWithHandler(CurrentSkillHandler);
		//THINK IF I SHOULD SKIP THE NEXTPUPFORSWAP AND JUST USE THE CURRENT
		PDWPlayerState->SetCurrentPup(inNewPup);
		SetNextPupForSwap(inNewPup);
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SwapCharacterAbility, 1, -1);
		GASComponent->GiveAbilityAndActivateOnce(AbilitySpec);
		OnChangeCharacter.Broadcast(inNewPup,this);
		UPDWGameplayFunctionLibrary::PlayForceFeedback({this}, UPDWGameplayTagSettings::GetSwapPupVibrationTag());	
	}
}

void APDWPlayerController::ChangeVehicle(bool bMaintainVehicle /*= false*/,bool bUseCameraBlend /*= true*/)
{
	CancelSkillWithHandler(CurrentSkillHandler);;
	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SwapVehicleAbility, 1, -1);
	//FUCK YOU GAS SYSTEM, YOU SUCK
	FGameplayEventData ExternalData;
	ExternalData.EventMagnitude = 1;
	ExternalData.EventTag = FGameplayTag::EmptyTag;
	if (!bUseCameraBlend)
	{
		ExternalData.EventMagnitude = 0;
	}
	if (bMaintainVehicle)
	{
		ExternalData.EventTag = PDWPlayerState->GetCurrentPup();
	}
	GASComponent->GiveAbilityAndActivateOnce(AbilitySpec,&ExternalData);
	OnSwapVehicle.Broadcast(GetIsOnVehicle(),this);

	FGameplayTag TagToUse = GetIsOnVehicle() ? UPDWGameplayTagSettings::GetChangeToVehicleVibrationTag() : UPDWGameplayTagSettings::GetChangeToPupVibrationTag();
	UPDWGameplayFunctionLibrary::PlayForceFeedback({this}, TagToUse);	
}

UPDWInteractionComponent* APDWPlayerController::GetInteractionComponent()
{
	if (Vehicle)
	{
		return Vehicle->GetInteractionComponent();
	}
	if (CharacterPup)
	{
		return CharacterPup->GetInteractionComponent();
	}
	return nullptr;
}

void APDWPlayerController::CharacterJump()
{
	if (GetIsOnVehicle())
	{
		return;
	}

	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(JumpAbility, 1, -1);
	GASComponent->GiveAbilityAndActivateOnce(AbilitySpec);
}

void APDWPlayerController::Move(const FVector2D& Value)
{
	if (IsValid(CharacterPup) && !PDWPlayerState->GetIsOnVehicle())
	{
		CharacterPup->Move(Value);
	}
	/*else if (IsValid(Vehicle) && PDWPlayerState->GetIsOnVehicle())
	{
		Vehicle->SetSteeringInput(Value.X);
		Vehicle->SetThrottleInput(Value.Y);
	}*/
}

void APDWPlayerController::MiniGameMove(const FVector2D& MovementVector,const FVector& inForwardVector,const FVector& inRightVector)
{
	if (!PDWPlayerState->GetIsOnVehicle() && IsValid(CharacterPup))
	{
		CharacterPup->MiniGameMove(MovementVector,inForwardVector,inRightVector);
	}
	else if(IsValid(Vehicle))
	{
		Vehicle->MiniGameMove(MovementVector,inForwardVector,inRightVector);
	}
}


void APDWPlayerController::MiniGameEnd()
{
	if (PDWPlayerState->GetIsOnVehicle() && IsValid(Vehicle))
	{
		Vehicle->StopCarBP(FVector::ZeroVector,FRotator::ZeroRotator,false);
	}
}

void APDWPlayerController::CancelSkillWithHandler(FGameplayAbilitySpecHandle& inHandler)
{
	GASComponent->CancelAbilityHandle(inHandler);
	//GASComponent->ClearAllAbilities();
}

void APDWPlayerController::RefreshBreadcrumbsOwner()
{
	if (BreadCrumbsComponent)
	{
		if (GetIsOnVehicle())
		{
			BreadCrumbsComponent->SetOwnerPawn(GetVehicleInstance());
		}
		else
		{
			BreadCrumbsComponent->SetOwnerPawn(GetPupInstance());
		}
	}
}

void APDWPlayerController::Look(const FVector2D& Value)
{
	if (PDWPlayerState->GetIsOnVehicle() && Vehicle)
	{
		Vehicle->Look(Value);
	}
	else if (!PDWPlayerState->GetIsOnVehicle() && IsValid(CharacterPup))
	{
		CharacterPup->Look(Value);
	}
}

void APDWPlayerController::OnBeforeSave()
{
	if (GetPawn())
	{
		UPDWDataFunctionLibrary::SetPlayerPosition(this, GetPawn()->GetActorTransform());
		//save here current pup/vehicle?
	}
}

void APDWPlayerController::OnInteractionSuccess(const FPDWInteractionPayload& Payload)
{
	if(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this) != this || !GetPDWPlayerState() || !Payload.InteractionReceiverRef) return;

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnQuestInteractionSuccessEvent(Payload);
	}

	if (Payload.InteractionReceiverRef->GetPupTreatsEvent() == EPupTreatsEvent::OnInteractionSuccess && !UPDWDataFunctionLibrary::IsInteractableUsed(this, Payload.InteractionReceiverRef->GetInteractableID()))
	{
		UPDWDataFunctionLibrary::AddUsedInteractable(this, Payload.InteractionReceiverRef->GetInteractableID());
		if (UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld()))
		{
			EventSubsystem->OnInteractionCompleteEvent(Payload.InteractionReceiverRef);
		}

		if (Payload.InteractionReceiverRef->GetPupTreatsToAdd() > 0)
		{
			GetPDWPlayerState()->ChangePupTreats(Payload.InteractionReceiverRef->GetPupTreatsToAdd());
			Payload.InteractionReceiverRef->OnPupTreatsGiven.Broadcast();
			if (Payload.InteractionReceiverRef->GetShouldSaveAfterInteraction())
			{
				UPDWDataFunctionLibrary::SaveGame(GetWorld());
			}
		}
	}
}

void APDWPlayerController::OnInteractionStateAdded(UPDWInteractionReceiverComponent* Comp, const FGameplayTag& PrevTag, const FGameplayTag& NewTag)
{
	if(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this) != this || !GetPDWPlayerState()) return;

	if (UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>())
	{
		EventSubsystem->OnQuestInteractionStateAddEvent(Comp, PrevTag, NewTag);
	}

	if (Comp->GetPupTreatsEvent() == EPupTreatsEvent::OnStateChange && !UPDWDataFunctionLibrary::IsInteractableUsed(this, Comp->GetInteractableID()))
	{
		UPDWDataFunctionLibrary::AddUsedInteractable(this, Comp->GetInteractableID());
		if (UPDWEventSubsytem* EventSubsystem = UPDWEventSubsytem::Get(GetWorld()))
		{
			EventSubsystem->OnInteractionCompleteEvent(Comp);
		}

		if (Comp->GetPupTreatsToAdd() > 0)
		{
			GetPDWPlayerState()->ChangePupTreats(Comp->GetPupTreatsToAdd());
			Comp->OnPupTreatsGiven.Broadcast();
			if (Comp->GetShouldSaveAfterInteraction())
			{
				UPDWDataFunctionLibrary::SaveGame(GetWorld());
			}
		}
	}
}

void APDWPlayerController::InitializeGASSystem()
{
	if (GASComponent)
	{
		//for some reasson if i do this the skill won't be triggered.
		//APawn* Pippo = GetPawn();
		//GASComponent->InitAbilityActorInfo(this,GetPawn());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APDWPlayerController::InitializeGASSystem - GASComponent is not initialized"));
	}
}

void APDWPlayerController::SetUpAbilitiesOnPupSwap()
{
	//clear ability of previous pup and assign new ability from the new pup.
}
