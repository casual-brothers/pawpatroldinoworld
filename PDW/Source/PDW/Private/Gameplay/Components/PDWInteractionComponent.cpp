// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWInteractionComponent.h"
#include "Gameplay/Components/PDWInteractionReceiverComponent.h"
#include "Data/PDWGameplayTagSettings.h"
#include "Algo/Sort.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Data/PDWGameSettings.h"
#include "Data/PDWPlayerState.h"
#include "FunctionLibraries/PDWGameplayTagsFunctionLibrary.h"

UPDWInteractionComponent::UPDWInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetCollisionProfileName(UPDWGameSettings::GetInteractionProfileName().Name);
	bCanEverAffectNavigation =false;
}

void UPDWInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &UPDWInteractionComponent::BeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UPDWInteractionComponent::EndOverlap);
	APDWPlayerController* Controller = GetOwner()->GetInstigatorController<APDWPlayerController>();
	if (Controller)
	{
		Controller->OnChangeCharacter.AddUniqueDynamic(this,&UPDWInteractionComponent::OnChangeCharacterEvent);
		AllowedInteractions = UPDWGameSettings::GetEntityInteractionsContainer(Controller->GetPDWPlayerState()->GetCurrentPup());
		SkillTypeInteractions = UPDWGameSettings::GetEntitySkillTypeContainer(Controller->GetPDWPlayerState()->GetCurrentPup());
	}
	else
	{
		IPDWPlayerInterface* PlayerInterface = Cast<IPDWPlayerInterface>(GetOwner());
		if (PlayerInterface)
		{
			PlayerInterface->OnPlayerPossesed.AddUniqueDynamic(this,&UPDWInteractionComponent::OnCharacterPossessed);
		}
	}
}

void UPDWInteractionComponent::EndPlay(EEndPlayReason::Type Reason)
{
	APDWPlayerController* Controller = GetOwner()->GetInstigatorController<APDWPlayerController>();
	if (Controller)
	{
		Controller->OnChangeCharacter.RemoveDynamic(this,&UPDWInteractionComponent::OnChangeCharacterEvent);
	}
	OnComponentBeginOverlap.RemoveDynamic(this, &UPDWInteractionComponent::BeginOverlap);
	OnComponentEndOverlap.RemoveDynamic(this, &UPDWInteractionComponent::EndOverlap);
	Super::EndPlay(Reason);
}

void UPDWInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	EvaluateClosestInteraction();
}

void UPDWInteractionComponent::BeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult)
{
	if (UPDWInteractionReceiverComponent* InteractionReceiverComp = Cast<UPDWInteractionReceiverComponent>(InOtherComp))
	{
		const uint8 InteractionConfiguration = InteractionReceiverComp->GetInteractionConfiguration();
		if (InteractionConfiguration & (uint8)EInteractionCheckConfig::WorkOnClosest)
		{
			ReceiverComponents.Emplace(InteractionReceiverComp);
		
			if (ReceiverComponents.Num() > 1)
			{
				bEvaluateClosestInteraction = true;
			}
			else
			{
				SetCurrentInteractionReceiver(InteractionReceiverComp);
				bool bAutoSwap = false;
				InteractAction(bAutoSwap,EInteractionType::Overlap);
			}
		}
		else
		{
			//#TODO THINK WHEN WE HAVE NECESSITY TO TRIGGER ALL OVERLAPPED STUFF, RIGHT NOW CURRENT INTERACTER IS ONE, THINK IF NEEDED -> TURN CURRENT IN ARRAY OF CURRENTS.
			/*SetCurrentInteractionReceiver(InteractionReceiverComp);
			bool bAutoSwap = false;
			InteractAction(bAutoSwap,EInteractionType::Overlap);*/
		}
	}
}

void UPDWInteractionComponent::EndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex)
{
	if (UPDWInteractionReceiverComponent* InteractingComponent = Cast<UPDWInteractionReceiverComponent>(InOtherComp))
	{
		ReceiverComponents.Remove(InteractingComponent);
		if (CurrentReceiverComponent == InteractingComponent)
		{
			SetCurrentInteractionReceiver(nullptr);
		}
		
		if (ReceiverComponents.Num() <= 1)
		{
			bEvaluateClosestInteraction = false;
		}
	}
}

void UPDWInteractionComponent::OnChangeCharacterEvent(const FGameplayTag& inNewPup,APDWPlayerController* inController)
{
	SkillTypeInteractions = UPDWGameSettings::GetEntitySkillTypeContainer(inNewPup);
	AllowedInteractions = UPDWGameSettings::GetEntityInteractionsContainer(inNewPup);
}

void UPDWInteractionComponent::OnCharacterPossessed()
{
	IPDWPlayerInterface* PlayerInterface = Cast<IPDWPlayerInterface>(GetOwner());
	if (PlayerInterface)
	{
		APDWPlayerController* Controller = PlayerInterface->GetPDWPlayerController();
		Controller->OnChangeCharacter.AddUniqueDynamic(this, &UPDWInteractionComponent::OnChangeCharacterEvent);
		AllowedInteractions = UPDWGameSettings::GetEntityInteractionsContainer(Controller->GetPDWPlayerState()->GetCurrentPup());
		SkillTypeInteractions = UPDWGameSettings::GetEntitySkillTypeContainer(Controller->GetPDWPlayerState()->GetCurrentPup());
		PlayerInterface->OnPlayerPossesed.RemoveDynamic(this,&UPDWInteractionComponent::OnCharacterPossessed);
	}
}

bool UPDWInteractionComponent::InteractAction(bool& bIsAutoSwap,EInteractionType InteractiontType /*= EInteractionType::Overlap*/)
{
	if (InteractiontType == EInteractionType::InputReceived)
	{
		AddOwnerInteractionStates(UPDWGameplayTagSettings::GetInputPressedTag().GetSingleTagContainer());
		if (IsValid(CurrentReceiverComponent))
		{
			CurrentReceiverComponent->OnInputReceivedInteractionSuccess();
		}
	}
	FPDWInteractionPayload Payload;
	GenerateInteractionPayload(Payload);
	if (IsValid(CurrentReceiverComponent))
		return CurrentReceiverComponent->ProcessInteraction(Payload,bIsAutoSwap);
	return false;
}

void UPDWInteractionComponent::SetOwnerInteractionStates(const FGameplayTagContainer& inContainer)
{
	APDWPlayerController* Controller = GetOwner()->GetInstigatorController<APDWPlayerController>();
	if (Controller)
	{
		Controller->GetPDWPlayerState()->SetPlayerStates(inContainer);
	}
}

void UPDWInteractionComponent::AddOwnerInteractionStates(const FGameplayTagContainer& inContainer)
{
	APDWPlayerController* Controller = GetOwner()->GetInstigatorController<APDWPlayerController>();
	if (Controller)
	{
		Controller->GetPDWPlayerState()->AddPlayerStates(inContainer);
	}
	else
	{
		InteractionComponentState.AppendTags(inContainer);
	}
	//write saves with new states
}

void UPDWInteractionComponent::RemoveinteractionStates(const FGameplayTagContainer& inContainer)
{
	APDWPlayerController* Controller = GetOwner()->GetInstigatorController<APDWPlayerController>();
	if (Controller)
	{
		Controller->GetPDWPlayerState()->RemovePlayerStates(inContainer);
	}
	else
	{
		InteractionComponentState.RemoveTags(inContainer);
	}
	OnInteractionStateRemoved.Broadcast(inContainer);
}

FGameplayTagContainer UPDWInteractionComponent::GetOwnerStates() const
{
	APDWPlayerController* Controller = GetOwner()->GetInstigatorController<APDWPlayerController>();
	if (Controller)
	{
		return Controller->GetPDWPlayerState()->GetPlayerStates();
	}
	return InteractionComponentState;
}

void UPDWInteractionComponent::AddAllowedInteraction(const FGameplayTagContainer& inContainer)
{
	AllowedInteractions.AppendTags(inContainer);
}

void UPDWInteractionComponent::RemoveAllowedInteraction(const FGameplayTagContainer& inContainer)
{
	AllowedInteractions.RemoveTags(inContainer);
}

void UPDWInteractionComponent::SetCurrentInteractionReceiver(UPDWInteractionReceiverComponent* inReceiver)
{
	if (inReceiver)
	{
		inReceiver->AddInteracter(this,AllowedInteractions);
	}
	else
	{
		UPDWInteractionReceiverComponent* Temp = CurrentReceiverComponent;
		CurrentReceiverComponent = inReceiver;
		Temp->RemoveInteracter(this);
	}
	CurrentReceiverComponent = inReceiver;
}

void UPDWInteractionComponent::SetAllowedInteraction(const FGameplayTagContainer& inContainer)
{
	AllowedInteractions = inContainer;
}

FGameplayTagContainer UPDWInteractionComponent::GetAllowedInteractions() const
{
	return AllowedInteractions;
}

FGameplayTagContainer UPDWInteractionComponent::GetSkillTypeInteractions() const
{
	return SkillTypeInteractions;
}

UPDWInteractionReceiverComponent* UPDWInteractionComponent::GetCurrentInteractionReceiver()
{
	return CurrentReceiverComponent;
}

//************************************
// Method:    GenerateInteractionPayload
// FullName:  UPDWInteractionComponent::GenerateInteractionPayload
// Access:    private 
// Returns:   void
// Qualifier: Get first interaction tag from AllowedInteractions that matches the interaction action tag and generate the payload
// Parameter: FPDWInteractionPayload & outPayload
//************************************
void UPDWInteractionComponent::GenerateInteractionPayload(FPDWInteractionPayload& outPayload)
{
	if (!CurrentReceiverComponent)
	{
		UE_LOG(LogTemp,Warning,TEXT("InteractionComponent - GeneratePayload: Missing CUrrentReceiverComponent"))
		return;
	}
	FGameplayTagContainer InteractionReceiverAllowedInteraction = CurrentReceiverComponent->GetAllowedInteraction();
	const FGameplayTagContainer FitleredContainer = AllowedInteractions.Filter(InteractionReceiverAllowedInteraction);

	outPayload.Executioner = GetOwner();
	outPayload.OwnerStates = GetOwnerStates();
	if (FitleredContainer.Num() == 1)
	{
		outPayload.InteractionType = FitleredContainer.GetByIndex(0);
	}
	else
	{
		TArray<FGameplayTag> AllowedInteractionsTagArray;
		AllowedInteractions.GetGameplayTagArray(AllowedInteractionsTagArray);
		for (int i = 0; i < AllowedInteractions.Num(); i++)
		{
			if (AllowedInteractionsTagArray[i].MatchesTag(UPDWGameplayTagSettings::Get()->GetInteractionActionTag()))
			{
				outPayload.InteractionType = AllowedInteractionsTagArray[i];
				outPayload.InteractionInfo = UPDWGameplayTagsFunctionLibrary::GetRootTagFromTag(AllowedInteractionsTagArray[i]).GetSingleTagContainer();
				break;
			}
		}
	}
}

void UPDWInteractionComponent::EvaluateClosestInteraction()
{
	if(!bEvaluateClosestInteraction)
		return;

	Algo::Sort(ReceiverComponents, [this](const UPDWInteractionReceiverComponent* A, const UPDWInteractionReceiverComponent* B)
		{
			return FVector::DistSquared(GetComponentLocation(), A->GetOwner()->GetActorLocation()) <
				FVector::DistSquared(GetComponentLocation(), B->GetOwner()->GetActorLocation());
		});


	if (IsValid(ReceiverComponents[0]) && CurrentReceiverComponent != ReceiverComponents[0])
	{
		if (IsValid(CurrentReceiverComponent))
		{
			SetCurrentInteractionReceiver(nullptr);
		}
		SetCurrentInteractionReceiver(ReceiverComponents[0]);
		bool bAutoSwap = false;
		InteractAction(bAutoSwap);
	}
}
