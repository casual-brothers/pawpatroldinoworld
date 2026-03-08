// Copyright Epic Games, Inc. All Rights Reserved.


#include "Gameplay\Pawns\PDWCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/Components/PDWSpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Gameplay/Components/PDWInteractionComponent.h"
#include "Gameplay/Components/PDWGASComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Public/GameplayAbilitySpec.h"
#include "Gameplay/GAS/PDWGASAbility.h"
#include "Abilities/GameplayAbility.h"
#include "Gameplay/Components/PDWBreadCrumbsComponent.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveFloat.h"
#include "Data/PDWAttributeSet.h"
#include "Data/PDWPlayerState.h"
#include "Gameplay/Animation/PDWAnimInstance.h"
#include "Data/PDWGameSettings.h"
#include "Data/PDWPupConfigurationAsset.h"
#include "Data/PDWSaveStructures.h"
#include "Components/NebulaGraphicsCustomizationComponent.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Data/PDWGameplayTagSettings.h"
//////////////////////////////////////////////////////////////////////////
// APDWCharacter

APDWCharacter::APDWCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	InteractionComponent = CreateDefaultSubobject<UPDWInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->SetupAttachment(RootComponent);
}

void APDWCharacter::SetActivation(bool inActive)
{
	if (!inActive)
	{
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		InteractionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CachedGravity = GetCharacterMovement()->GravityScale;
		GetCharacterMovement()->GravityScale = 0.0f;
	}
	else
	{
		InteractionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCharacterMovement()->GravityScale = CachedGravity;
	}
}

void APDWCharacter::PreInitializeComponents()
{
	AutomaticSpringArm = GetComponentByClass<UPDWSpringArmComponent>();
	InteractionComponent = GetComponentByClass<UPDWInteractionComponent>();
	Super::PreInitializeComponents();
}

void APDWCharacter::InitCharacterAbilities()
{
	GASComponent = PDWPlayerController->GetGASComponent();
	if (!IsValid(GASComponent)) return;
	BasicAttributeSet = GASComponent->GetSet<UPDWAttributeSet>();
	GASComponent->GetGameplayAttributeValueChangeDelegate(BasicAttributeSet->GetCharacterAccelerationAttribute()).AddUObject(this, &APDWCharacter::OnAccelerationChanged);
	GASComponent->GetGameplayAttributeValueChangeDelegate(BasicAttributeSet->GetCharacterSpeedAttribute()).AddUObject(this, &APDWCharacter::OnTopSpeedChanged);
	GASComponent->GetGameplayAttributeValueChangeDelegate(BasicAttributeSet->GetCharacterTurningAttribute()).AddUObject(this, &APDWCharacter::OnSteeringChanged);

	FOnAttributeChangeData Placeholder;
	Placeholder.NewValue = BasicAttributeSet->GetCharacterTurning();
	OnSteeringChanged(Placeholder);
	Placeholder.NewValue = BasicAttributeSet->GetCharacterAcceleration();
	OnAccelerationChanged(Placeholder);
	Placeholder.NewValue = BasicAttributeSet->GetCharacterSpeed();
	OnTopSpeedChanged(Placeholder);
}

void APDWCharacter::Hide(bool bHide,bool bSkipCollision /*= false*/)
{
	if(!bSkipCollision)
		SetActorEnableCollision(!bHide);

	SetActorHiddenInGame(bHide);
}

void APDWCharacter::OnSteeringChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->RotationRate.Roll = Data.NewValue;
}

void APDWCharacter::OnAccelerationChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxAcceleration = Data.NewValue;
}

void APDWCharacter::OnTopSpeedChanged(const FOnAttributeChangeData& Data)
{
	// m/s to cm/s
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue * 100.0f;
}


UPDWBreadCrumbsComponent* APDWCharacter::GetBreadCrumbsComponent()
{
	return PDWPlayerController->GetBreadCrumbsComponent();
}

void APDWCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PDWPlayerController = Cast<APDWPlayerController>(NewController);
	PDWPlayerController->OnSwapVehicle.AddUniqueDynamic(this,&APDWCharacter::DisableLogicWhileDriving);
	InitCharacterAbilities();
	
	//Init customizationDA
	const FGameplayTag CurrentPup = PDWPlayerController->GetPDWPlayerState()->GetCurrentPup();
	if (Customizations.Contains(CurrentPup))
	{
		UCustomizationDataAsset* CustomizationDA = Customizations[CurrentPup];
		NebulaGraphicsCustomizationComponent = FindComponentByClass<UNebulaGraphicsCustomizationComponent>();
		ensureMsgf(NebulaGraphicsCustomizationComponent, TEXT("APDWCharacter::CustomizePup - NebulaGraphicsCustomizationComponent is null, this should not happen!"));
		if (NebulaGraphicsCustomizationComponent)
		{
			NebulaGraphicsCustomizationComponent->SetCustomizationDataAsset(CustomizationDA);
		}
	}

	OnPlayerPossesed.Broadcast();
}

void APDWCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(bDIsableLogicsWhileDriving)
	{
		return;
	}

	if ((GetCharacterMovement()->IsMovingOnGround() && !IsInsideGeyser) || StartJump)
	{
		AirTime = 0.f;
		JumpBufferingCount = 0.f;
	}
	else
	{
		AirTime += DeltaSeconds;
		if (bJumpBufferingActive)
		{
			JumpBufferingCount += DeltaSeconds;
			bJumpBufferingActive = JumpBufferingCount < JumpBufferingTime;
		}
	}

	if (!bForcedGravityActive)
	{
		if (AirTimeGravityScaleCurve)
			GetCharacterMovement()->GravityScale = AirTimeGravityScaleCurve->GetFloatValue(AirTime);
	}
	if (WaterLandCurve && WaterLand > 0.f)
	{
		SetFloatZParameter(-CurrentWaterLandMultiplier * WaterLandCurve->GetFloatValue(WaterLandDuration - WaterLand));
		WaterLand -= DeltaSeconds;
	}
}

void APDWCharacter::SetForcedGravityScale(bool bEnabled, float Value)
{
	bForcedGravityActive = bEnabled;
	if (bForcedGravityActive)
		GetCharacterMovement()->GravityScale = Value;
}

void APDWCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

bool APDWCharacter::CanJumpInternal_Implementation() const
{
	return true;
}

void APDWCharacter::Move(const FVector2D& MovementVector)
{
	if (Controller != nullptr)
	{
		if (AutomaticSpringArm && AutomaticSpringArm->GetAttachedCamera())
		{
			FVector ForwardMovementVector = FVector();
			FVector RightMovementVector = FVector();
			if (AutomaticSpringArm->GetCurrentCameraParameters()->ManualCamera)
			{
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);
				ForwardMovementVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				RightMovementVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			}
			else
			{
				ForwardMovementVector = AutomaticSpringArm->GetForwardMovementVector();
				RightMovementVector = AutomaticSpringArm->GetRightMovementVector();
			}

			AddMovementInput(ForwardMovementVector, MovementVector.Y);
			AddMovementInput(RightMovementVector, MovementVector.X);
		}
		OnMovementInput.Broadcast(MovementVector);
	}
}

void APDWCharacter::MiniGameMove(const FVector2D& MovementVector,const FVector& inForwardVector,const FVector& inRightVector)
{
	if (Controller != nullptr)
	{
		AddMovementInput(inForwardVector, MovementVector.Y);
		AddMovementInput(inRightVector, MovementVector.X);
		OnMovementInput.Broadcast(MovementVector);
	}
}

void APDWCharacter::Look(const FVector2D& LookAxisVector)
{
	if (Controller != nullptr)
	{
		// TODO: aggiungere moltiplicatore degli assi
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
	OnCameraInput.Broadcast(LookAxisVector);
}

void APDWCharacter::NotifyJump()
{
	JumpNotifyArrived = true;
}

APDWPlayerState* APDWCharacter::GetPDWPlayerState()
{
	if (PDWPlayerController)
	{
		return PDWPlayerController->GetPDWPlayerState();
	}
	return nullptr;
}

void APDWCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultJumpValue = GetCharacterMovement()->JumpZVelocity;
	CachedGravity = GetCharacterMovement()->GravityScale;
}

void APDWCharacter::CustomizePup(FGameplayTag PupTag)
{
	if (NebulaGraphicsCustomizationComponent && Customizations.Contains(PupTag))
	{
		const FPupCustomizationInfo& CustomizationInfo = UPDWGameSettings::GetPupCustomizationInfo(PupTag);
		UCustomizationDataAsset* CustomizationDA = Customizations[PupTag];
		FCustomizationData CustomizationData = UPDWDataFunctionLibrary::GetCustomizationData(this);
		FGameplayTag CustomizationTag = CustomizationData.PupCustomizations.Contains(PupTag) ? CustomizationData.PupCustomizations[PupTag] : CustomizationInfo.HatCustomizationTag;
		TArray<FNebulaGraphicsStaticMeshCustomization> AllSMCustomizations = CustomizationDA->StaticMeshCustomizationList;

		NebulaGraphicsCustomizationComponent->SetCustomizationDataAsset(CustomizationDA);
		for (FNebulaGraphicsStaticMeshCustomization& SMCustomization : AllSMCustomizations)
		{
			if (SMCustomization.ID == CustomizationTag)
			{
				NebulaGraphicsCustomizationComponent->CustomizeStaticMesh(CustomizationTag.GetSingleTagContainer());
				break;
			}
		}

		NebulaGraphicsCustomizationComponent->CustomizeSkeletalMesh(CustomizationInfo.CharacterCustomizationTag.GetSingleTagContainer());
		NebulaGraphicsCustomizationComponent->OnAllCustomizationCompleted.AddUniqueDynamic(this, &APDWCharacter::SkeletalCustomizationComplete);
	}
}

void APDWCharacter::SkeletalCustomizationComplete()
{
	NebulaGraphicsCustomizationComponent->OnAllCustomizationCompleted.RemoveDynamic(this, &APDWCharacter::SkeletalCustomizationComplete);
	const FPupCustomizationInfo& CustomizationInfo = UPDWGameSettings::GetPupCustomizationInfo(PDWPlayerController->GetPDWPlayerState()->GetCurrentPup());
	
	NebulaGraphicsCustomizationComponent->CustomizeAnimation(CustomizationInfo.CharacterCustomizationTag.GetSingleTagContainer());
	NebulaGraphicsCustomizationComponent->CustomizeMaterial(CustomizationInfo.CharacterCustomizationTag.GetSingleTagContainer());
	
	if (PDWPlayerController->GetPDWPlayerState()->GetIsOnVehicle())
	{
		if (APDWCharacter* Char = PDWPlayerController->GetPupInstance())
		{
			if (Char->GetAniminstance())
			{
				Char->GetAniminstance()->SetAnimationState(EAnimationState::Driving);
			}
		}
	}
	
	PDWPlayerController->OnPupCustomizationCompleted.Broadcast(PDWPlayerController);
	CollectDMIs();
}

void APDWCharacter::CollectOrCreateExistingDMIsFromMesh(UPrimitiveComponent* MeshComponent)
{
    if (!MeshComponent)
    {
        return;
    }

    int32 NumMaterials = MeshComponent->GetNumMaterials();

    for (int32 i = 0; i < NumMaterials; ++i)
    {
        UMaterialInterface* Material = MeshComponent->GetMaterial(i);

        UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);

        if (DynamicMaterial)
        {
            AllDynamicMaterials.Add(DynamicMaterial);
        }
		else
		{
			UMaterialInstanceDynamic* DMI = MeshComponent->CreateAndSetMaterialInstanceDynamic(i);
			AllDynamicMaterials.Add(DMI);
		}
    }
}

void APDWCharacter::CollectDMIs()
{
	AllDynamicMaterials = TArray<UMaterialInstanceDynamic*>();
    CollectOrCreateExistingDMIsFromMesh(GetMesh());

    TArray<USceneComponent*> AttachedComponents;
    GetMesh()->GetChildrenComponents(true, AttachedComponents);

    for (USceneComponent* Component : AttachedComponents)
    {
        if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Component))
        {
            CollectOrCreateExistingDMIsFromMesh(PrimitiveComp);
        }
    }
}

void APDWCharacter::Landed(const FHitResult& Hit)
{
	if (!Hit.GetActor()) return;
	
	float HitVelocity = FMath::Abs(GetVelocity().Z);
	if (HitVelocity > HeavyLandVelocity)
	{
		UPDWGameplayFunctionLibrary::PlayForceFeedback({PDWPlayerController}, UPDWGameplayTagSettings::GetPupLandVibrationTag());		
	}
	if (Hit.GetComponent()->GetCollisionObjectType() == SwimmingChannel) // Swimming plane
	{
		WaterLanded(Hit);
	}
	Super::Landed(Hit);
}

void APDWCharacter::WaterLanded(const FHitResult& Hit)
{
	float FallVelocity = GetCharacterMovement()->GetLastUpdateVelocity().Z;
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, *FString::Printf(TEXT("Fall %f"), FallVelocity));

	FCollisionQueryParams TraceParams(FName(TEXT("RayCastWaterDepth")), false);

	TraceParams.AddIgnoredActor(this);
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + (-FVector::UpVector * MaxWaterLandDistance);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		WaterLandTrace,
		TraceParams
	);
	
	CurrentWaterLandMultiplier = FMath::Min((-FallVelocity / WaterLandVerticalSpeedToGetMaxDistance), 1.0f) * MaxWaterLandDistance;
	if (bHit)
	{
		CurrentWaterLandMultiplier = FMath::Min(CurrentWaterLandMultiplier, HitResult.Distance - MinWaterLandDistanceFromUnder);
		//DrawDebugLine(GetWorld(), StartLocation, HitResult.Location, FColor::Red, false, 2.0f, (uint8)0U, 5.f);
	}
	else
	{
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 2.0f, (uint8)0U, 5.f);
	}
	WaterLand = WaterLandDuration;
}


void APDWCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PDWPlayerController)
	{
		PDWPlayerController->OnSwapVehicle.RemoveDynamic(this,&APDWCharacter::DisableLogicWhileDriving);
	}
	Super::EndPlay(EndPlayReason);
	
}

//TODO
//void APDWCharacter::ChangeOverlapEvents(bool IsActive)
//{
//	if (GetCapsuleComponent() && GetInteractionComponent())
//	{
//		GetInteractionComponent()->SetGenerateOverlapEvents(IsActive);
//		GetCapsuleComponent()->SetGenerateOverlapEvents(IsActive);
//	}
//}

void APDWCharacter::CheckJumpInput(float DeltaTime)
{
	JumpCurrentCountPreJump = JumpCurrentCount;

	if (GetCharacterMovement())
	{
		if ((bPressedJump && CheckJumpConditions()) || (bJumpBufferingActive && GetCharacterMovement()->IsMovingOnGround() && CheckJumpConditions()))
		{
			StartJump = true;	
			bJumpBufferingActive = false;
		}

		if (StartJump)
		{
			const bool bDidJump = (JumpNotifyArrived || !WaitForJumpNotify || CoyoteJumping) && CheckJumpConditions() && GetCharacterMovement()->DoJump(bClientUpdating, DeltaTime);
			if (bDidJump)
			{
				StartJump = false;
				JumpNotifyArrived = false;
				CoyoteJumping = false;

				// Transition from not (actively) jumping to jumping.
				if (!bWasJumping)
				{
					JumpCurrentCount++;
					JumpForceTimeRemaining = GetJumpMaxHoldTime();
					OnJumped();
				}
			}

			bWasJumping = bDidJump;
		}
	}
}

bool APDWCharacter::CheckJumpConditions()
{
	if (bIsCrouched)
		return false;

	bool bJumpIsAllowed = false;

	// Ensure JumpHoldTime and JumpCount are valid.
	if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
	{
		if (JumpCurrentCount == 0 && !GetCharacterMovement()->IsMovingOnGround() && !IsInsideGeyser)
		{
			if (AirTime < CoyoteTime)
			{
				bJumpIsAllowed = true;
				CoyoteJumping = true;
			}
			else
			{
				bJumpIsAllowed = false;
			}
		}
		else if (!GetCharacterMovement()->IsMovingOnGround() && !IsInsideGeyser)
		{
			bJumpBufferingActive = true;
			JumpBufferingCount = 0.f;
		}
		else
		{
			bJumpIsAllowed = JumpCurrentCount < JumpMaxCount || IsInsideGeyser;
		}
	}
	else
	{
		// Only consider JumpKeyHoldTime as long as:
		// A) The jump limit hasn't been met OR
		// B) The jump limit has been met AND we were already jumping
		const bool bJumpKeyHeld = (bPressedJump && JumpKeyHoldTime < GetJumpMaxHoldTime());
		bJumpIsAllowed = bJumpKeyHeld &&
			((JumpCurrentCount < JumpMaxCount) || (bWasJumping && JumpCurrentCount == JumpMaxCount));
	}
	
	return bJumpIsAllowed;
}

void APDWCharacter::DisableLogicWhileDriving(bool bInVehicleMode, APDWPlayerController* inController)
{
	if (inController != PDWPlayerController) return;

	bDIsableLogicsWhileDriving = bInVehicleMode;
	//GetCharacterMovement()->bAlwaysCheckFloor = !bInVehicleMode;
}

void APDWCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = FGameplayTagContainer();
	if (PDWPlayerController && PDWPlayerController->GetPDWPlayerState())
	{
		TagContainer = PDWPlayerController->GetPDWPlayerState()->GetPlayerStates();
	}

}

UPDWAnimInstance* APDWCharacter::GetAniminstance()
{
	return Cast<UPDWAnimInstance>(GetMesh()->GetAnimInstance());
}

float APDWCharacter::GetCachedGravityScale()
{
	return CachedGravity;
}

void APDWCharacter::EnableBouncingJumpForce(bool bEnable, float inValue /*= 0*/)
{
	if (bEnable)
	{
		GetCharacterMovement()->JumpZVelocity = inValue;
	}
	else
	{
		GetCharacterMovement()->JumpZVelocity = DefaultJumpValue; // Reset to default value
	}
}

void APDWCharacter::SetFloatZParameter(float NewValue)
{
	const FName ParameterName = TEXT("FloatZ");
	FMaterialParameterInfo ParameterInfo(
		ParameterName,
		EMaterialParameterAssociation::LayerParameter,
		0
	);

	for (UMaterialInstanceDynamic* DynamicMaterial : AllDynamicMaterials)
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValueByInfo(ParameterInfo, NewValue);
		}
	}
	ParameterInfo.Index = 1;

	for (UMaterialInstanceDynamic* DynamicMaterial : AllDynamicMaterials)
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValueByInfo(ParameterInfo, NewValue);
		}
	}	
}
