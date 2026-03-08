// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetsComponent/PDWIconComponent.h"
#include "GameplayTagContainer.h"
#include "UI/Widgets/PDWIconWidget.h"
#include "Data/PDWGameSettings.h"
#include "Managers/PDWEventSubsytem.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Gameplay/Components/PDWTagComponent.h"

UPDWIconComponent::UPDWIconComponent()
{
	TickMode = ETickMode::Disabled;
}

void UPDWIconComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, this, &ThisClass::HandleMultiplayerVisibility,.3f);

	SetOnlyOwnerSee(true);
	DefaultWidgetClass = GetWidgetClass();
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		EventSubsystem->OnMultiplayerStateChange.AddUniqueDynamic(this, &UPDWIconComponent::OnMultiplayerStateChange);
		EventSubsystem->OnGameplayStateEnter.AddUniqueDynamic(this, &UPDWIconComponent::OnGameplayStateEnter);
		EventSubsystem->OnGameplayStateExit.AddUniqueDynamic(this, &UPDWIconComponent::OnGameplayStateExit);
	}

	PlayerNearStates.Empty();
    PlayerNearStates.Add(0, false);
    PlayerNearStates.Add(1, false);
	
	PlayerFarStates.Empty();
    PlayerFarStates.Add(0, true);
    PlayerFarStates.Add(1, true);

	UPDWTagComponent* TagComponent = GetOwner()->FindComponentByClass<UPDWTagComponent>();
	if(TagComponent)
	{
		TagComponent->OnPDWTagAdded.AddUniqueDynamic(this, &UPDWIconComponent::OnTagAdded);
		TagComponent->OnPDWTagRemoved.AddUniqueDynamic(this, &UPDWIconComponent::OnTagRemoved);
		OnTagAdded(TagComponent->GetCurrentTags(), FGameplayTag());
	}
		
#if WITH_EDITOR
	if (!IconDebugTag.IsEmpty())
	{
		OnTagAdded(IconDebugTag, FGameplayTag());
	}
#endif

	UpdateIcon();
}

void UPDWIconComponent::EndPlay(EEndPlayReason::Type Reason)
{
	UPDWEventSubsytem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPDWEventSubsytem>();
	if (EventSubsystem)
	{
		if (EventSubsystem->OnMultiplayerStateChange.IsBound())
		{
			EventSubsystem->OnMultiplayerStateChange.RemoveDynamic(this, &UPDWIconComponent::OnMultiplayerStateChange);
		}

		EventSubsystem->OnGameplayStateEnter.RemoveDynamic(this, &UPDWIconComponent::OnGameplayStateEnter);
		EventSubsystem->OnGameplayStateExit.RemoveDynamic(this, &UPDWIconComponent::OnGameplayStateExit);
	}
	
	UPDWTagComponent* TagComponent = GetOwner()->FindComponentByClass<UPDWTagComponent>();
	if(TagComponent)
	{
		TagComponent->OnPDWTagAdded.RemoveDynamic(this, &UPDWIconComponent::OnTagAdded);
		TagComponent->OnPDWTagRemoved.RemoveDynamic(this, &UPDWIconComponent::OnTagRemoved);
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	Super::EndPlay(Reason);
}

void UPDWIconComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bIsInGameplayState || (!bCheckForFarDistance && !bCheckForNearDistance && !bCheckForDistanceHeight))
	{
		return;
	}

    FVector MyLocation = GetComponentLocation();

    //int32 MaxPlayers = GetWorld()->GetNumPlayerControllers();

    //for (int32 i = 0; i < MaxPlayers; i++)
    //{
        if (PlayerControllerOwner && PlayerControllerOwner->GetPawn())
        {
            float CurrentDistance = FVector::Distance(MyLocation, PlayerControllerOwner->GetPawn()->GetActorLocation());
			int32 i = 0;

			if (bCheckForFarDistance)
			{
				//Far
				bool* bFarPtr = PlayerFarStates.Find(i);
				bool bCurrentlyFar = bFarPtr ? *bFarPtr : false;

				if (bCurrentlyFar && CurrentDistance <= FarRange)
				{
					OnPlayerFar(i, false);
				}
				else if (!bCurrentlyFar && CurrentDistance >= FarRange)
				{
					OnPlayerFar(i, true);
				}
			}
			
			if (bCheckForNearDistance)
			{
				//Near
				bool* bNearPtr = PlayerNearStates.Find(i);
				bool bCurrentlyNear = bNearPtr ? *bNearPtr : false;

				if (bCurrentlyNear && CurrentDistance >= NearRange)
				{
					OnPlayerNear(i, false);
				}
				else if (!bCurrentlyNear && CurrentDistance <= NearRange)
				{
					OnPlayerNear(i, true);
				}
			}
			
			if (bCheckForDistanceHeight)
			{
				TArray<class USceneComponent*> Parents;
				GetParentComponents(Parents);
				FVector2D ActorScreenPosition {};
				UGameplayStatics::ProjectWorldToScreen(PlayerControllerOwner, Parents[0]->GetComponentLocation(), ActorScreenPosition);
	
				FVector2D ComponentScreenPosition {};
				UGameplayStatics::ProjectWorldToScreen(PlayerControllerOwner, GetComponentLocation(), ComponentScreenPosition);
	
				float LineSize = FMath::Abs(ActorScreenPosition.Y - ComponentScreenPosition.Y);

				UPDWIconWidget* IconWidget = Cast<UPDWIconWidget>(GetWidget());
				if (IconWidget)
				{
					IconWidget->BP_DistanceFromCenterToWidget(LineSize);
				}
			}
        }
    //}
}

void UPDWIconComponent::OnTagAdded(const FGameplayTagContainer& TagContainer,const FGameplayTag& AddedTag)
{
	if ((!IconTags.HasTag(AddedTag)) && (HaveThisConfigurationTags(TagContainer)))
	{
		IconTags = TagContainer;
		SetComponentTickEnabled(true);
		TickMode = ETickMode::Enabled;
		UpdateIcon();
		OnIconUpdated.Broadcast(true);
	}
}


void UPDWIconComponent::OnTagRemoved(const FGameplayTagContainer& TagContainer,const FGameplayTag& RemovedTag)
{
	if (IconTags.HasTag(RemovedTag))
	{
		IconTags.RemoveTag(RemovedTag);
		if (IconTags.IsEmpty())
		{
			SetComponentTickEnabled(false);
			TickMode = ETickMode::Disabled;
		}

		UpdateIcon();
		OnIconUpdated.Broadcast(false);
	}
}

void UPDWIconComponent::OnPlayerNear(int32 PlayerIndex, bool bIsNear)
{
	PlayerNearStates[PlayerIndex] = bIsNear;

	BP_OnPlayerNear(PlayerIndex, bIsNear);
	UPDWIconWidget* IconWidget = Cast<UPDWIconWidget>(GetWidget());
	if (IconWidget)
	{
		IconWidget->BP_OnPlayerNear(PlayerIndex, bIsNear);
	}
}

void UPDWIconComponent::OnPlayerFar(int32 PlayerIndex, bool bIsFar)
{
	PlayerFarStates[PlayerIndex] = bIsFar;

	BP_OnPlayerFar(PlayerIndex, bIsFar);
	UPDWIconWidget* IconWidget = Cast<UPDWIconWidget>(GetWidget());
	if (IconWidget)
	{
		IconWidget->BP_OnPlayerFar(PlayerIndex, bIsFar);
	}
}

void UPDWIconComponent::UpdateIcon()
{
	FPDWIconData IconData;
	bool bFound = GetIconDataByCurrentTags(IconData);
	if (IconTags.IsEmpty() || (!bFound))
	{
		ChangeWidgetVisibility(true);
		bCheckForFarDistance = false;
		bCheckForNearDistance = false;
		bCheckForDistanceHeight = false;
	}
	else
	{
		if (IconData.CustomWidgetIcon.Get())
		{
			if (IconData.CustomWidgetIcon != GetWidgetClass())
			{
				SetWidgetClass(IconData.CustomWidgetIcon);
			}
		}
		else
		{
			if (DefaultWidgetClass != GetWidgetClass())
			{
				SetWidgetClass(DefaultWidgetClass);
			}
		}

		UPDWIconWidget* IconWidget = Cast<UPDWIconWidget>(GetWidget());
		if (IconWidget)
		{
			SetDrawSize(IconData.ImageSize);
			IconWidget->SetIconsTag(IconTags);
			IconWidget->BP_UpdateIcon(IconData.IconTexture, IconData.ImageSize);
		}
		
		if (!IconData.bCheckForFarDistance)
		{
			ChangeWidgetVisibility(false);
		}
		else
		{
			FVector MyLocation = GetComponentLocation();

			int32 MaxPlayers = GetWorld()->GetNumPlayerControllers();
			bool bIsFar = false;
			//for (int32 i = 0; i < MaxPlayers; i++)
			//{
				if (PlayerControllerOwner && PlayerControllerOwner->GetPawn())
				{
					float CurrentDistance = FVector::Distance(MyLocation, PlayerControllerOwner->GetPawn()->GetActorLocation());

					if (CurrentDistance <= FarRange)
					{
						ChangeWidgetVisibility(false);
					}
					else if (CurrentDistance >= FarRange)
					{
						ChangeWidgetVisibility(true);
					}
				}
				
			//}
		}

		bCheckForFarDistance = IconData.bCheckForFarDistance;
		bCheckForNearDistance = IconData.bCheckForNearDistance;
		bCheckForDistanceHeight = IconData.bCheckHeightFromWidgetToActor;
	}
}

void UPDWIconComponent::OnGameplayStateEnter()
{
	bIsInGameplayState = true;
	SetHiddenInGame(StayHidden);
	if (!StayHidden)
	{
		UpdateIcon();
	}
}

void UPDWIconComponent::OnGameplayStateExit()
{
	bIsInGameplayState = false;
	SetHiddenInGame(true);
	ChangeWidgetVisibility(true);
}

void UPDWIconComponent::ChangeWidgetVisibility(bool Hide)
{
	UPDWIconWidget* IconWidget = Cast<UPDWIconWidget>(GetWidget());
	if (IconWidget)
	{
		IconWidget->BP_OnChangeVisibilityWidget(!Hide);
	}
}

bool UPDWIconComponent::GetIconDataByCurrentTags(FPDWIconData& StructToFill)
{
	for (FGameplayTag Tag : IconTags)
	{
		if (IconsConfiguration.Contains(Tag))
		{
			StructToFill = IconsConfiguration[Tag];
			return true;
		}
	}

	return false;
}

bool UPDWIconComponent::HaveThisConfigurationTags(const FGameplayTagContainer StructToCheck)
{
	for (FGameplayTag Tag : StructToCheck)
	{
		if (IconsConfiguration.Contains(Tag))
		{
			return true;
		}
	}

	return false;
}

void UPDWIconComponent::OnMultiplayerStateChange()
{
	HandleMultiplayerVisibility();
}

void UPDWIconComponent::HandleMultiplayerVisibility()
{
	bool IsMultiPlayerOn = UPDWGameplayFunctionLibrary::IsMultiplayerOn(this);

	if (IsPlayer2Icon)
	{
		SetHiddenInGame(!IsMultiPlayerOn);
		StayHidden = !IsMultiPlayerOn;

		if (IsMultiPlayerOn)
		{		
			APDWPlayerController* P2Controller = UPDWGameplayFunctionLibrary::GetPlayerControllerTwo(this);
			if (!P2Controller) return;
			PlayerControllerOwner = P2Controller;
			SetOwnerPlayer(P2Controller->GetLocalPlayer());
		}
	}
	else
	{
		APDWPlayerController* P1Controller = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
		if(!P1Controller) return;
		PlayerControllerOwner = P1Controller;
		SetOwnerPlayer(P1Controller->GetLocalPlayer());
	}
}

FGameplayTagContainer UPDWIconComponent::GetIconTag() const
{
	return IconTags;
}
