// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WPLoadingHandlerComponent.h"
#include "WPHelperSubsystem.h"
#include "Components/ShapeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

#define LOCTEXT_NAMESPACE "WPPluginWarning"

UWPLoadingHandlerComponent::UWPLoadingHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


//************************************
// Method:    OnLoadingComplete
// FullName:  UWPLoadingHandlerComponent::OnLoadingComplete
// Access:    virtual public 
// Returns:   void
// Qualifier: Will turn on logic that should be active only after cell is loaded.
//************************************
void UWPLoadingHandlerComponent::OnLoadingComplete()
{
	if (SetUpBehavior & (uint8)ESetUpBehavior::Gravity)
	{
		EnableGravity();
	}
	if (SetUpBehavior & (uint8)ESetUpBehavior::AILogic)
	{
		EnableAILogic();
	}

	CurrentState = EEntityState::Enabled;
	OnEntityEnabledDelegate.Broadcast();
}

//************************************
// Method:    EnableGravity
// FullName:  UWPLoadingHandlerComponent::EnableGravity
// Access:    virtual protected 
// Returns:   void
// Qualifier: USE IT ON ACTOR WITH STATIC MESH (useless on character, AI LOGIC WILL DO IT FOR U) to enable Gravity interaction.
//************************************
void UWPLoadingHandlerComponent::EnableGravity()
{	
	USceneComponent* RootComp = GetOwner()->GetRootComponent();
	UMeshComponent* MeshComponent = Cast<UMeshComponent>(RootComp);
	if(MeshComponent)
	{
		MeshComponent->SetEnableGravity(true);
	}
	else
	{
		UShapeComponent* ShapeComp = Cast<UShapeComponent>(RootComp);
		if (ShapeComp)
		{
			ShapeComp->SetEnableGravity(true);
		}
	}
}

//************************************
// Method:    EnableAILogic
// FullName:  UWPLoadingHandlerComponent::EnableAILogic
// Access:    virtual protected 
// Returns:   void
// Qualifier: Use it on AICharacter. Once ai logic is active BT and gravity should start to work unless different config on AIController
//************************************
void UWPLoadingHandlerComponent::EnableAILogic()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	ensureMsgf(OwnerPawn,TEXT("This owner is not a pawn, disable the mask on ailogic"));
	UCharacterMovementComponent* MovementComp = OwnerPawn->FindComponentByClass<UCharacterMovementComponent>();
	if (MovementComp)
	{
		MovementComp->SetComponentTickEnabled(true);
		MovementComp->GravityScale = 1.0f;
	}
}

// Called when the game starts
void UWPLoadingHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = EEntityState::Disabled;
	UWPHelperSubsystem::Get(this)->RegisterEntity(this,GetOwner()->GetActorLocation());
}

#undef LOCTEXT_NAMESPACE