// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/PDWCharacterSelector.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Data/PDWPlayerState.h"
#include "Data/PDWGameSettings.h"
#include "Components/RichTextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Components/PanelWidget.h"
#include "UI/HUD/PDWCharacterSelectorElement.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Components/Button.h"
#include "FunctionLibraries/NebulaFlowCoreFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "BlueprintGameplayTagLibrary.h"


void UPDWCharacterSelector::InitializeCharacterPicker(APlayerController* Instigator)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	APDWPlayerController* PlayerController = Cast<APDWPlayerController>(Instigator);
	SetOwningPlayer(PlayerController);
	CurrentPup = PlayerController->GetPDWPlayerState()->GetCurrentPup();
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (UIManager)
	{
		PupOrder = UIManager->GetDefaultAllowedPups();
		CurrentPupAllowed = UPDWGameplayFunctionLibrary::IsSecondPlayer(PlayerController) ? UIManager->GetCurrentAllowedPupsP2() : UIManager->GetCurrentAllowedPupsP1();
		CurrentPupsHighlighted = UIManager->GetCurrentHighlithedPups();
		UIManager->OnAllowedPupsChanged.AddUniqueDynamic(this, &ThisClass::OnAllowedPupsChanged);
	}
	if (PupOrder.Num() <= 0)
	{
		UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UPDWGameSettings::GetActionBackToGameplay().ToString(), "", GetOwningPlayer());
	}
	UpdateCharacters();
	PlayAnimation(Intro);
	UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), OpeningSoundID);
}

void UPDWCharacterSelector::UpdateCharacters()
{
	int32 CurrentIndex = PupOrder.Find(CurrentPup);
	TArray<UWidget*> CharactersWidgets = CharactersContainer->GetAllChildren();
	bool bCurrentPupIsBlocked = false;
	for (UWidget* CharacterImage : CharactersWidgets)
	{
		UPDWCharacterSelectorElement* SelectorElement = Cast<UPDWCharacterSelectorElement>(CharacterImage);
		if (SelectorElement)
		{
			SelectorElement->ChangePupInfo(PupOrder[GetRightIndex(CurrentIndex + SelectorElement->GetPositionIndex())]);
			SelectorElement->SetIsBlocked(!CurrentPupAllowed.Contains(PupOrder[GetRightIndex(CurrentIndex + SelectorElement->GetPositionIndex())]));
			SelectorElement->SetIsHighlighted(CurrentPupsHighlighted.Contains(PupOrder[GetRightIndex(CurrentIndex + SelectorElement->GetPositionIndex())]));
			if (PupOrder[GetRightIndex(CurrentIndex + SelectorElement->GetPositionIndex())] == CurrentPup)
			{
				bCurrentPupIsBlocked = SelectorElement->IsBlocked();
			}
		}
	}

	ConfirmNavButton->ChangeButtonEnabled(!bCurrentPupIsBlocked);
	ConfirmNavButton->SetVisibility(bCurrentPupIsBlocked ? ESlateVisibility::Hidden : ESlateVisibility::HitTestInvisible);
}

int32 UPDWCharacterSelector::GetRightIndex(int32 InIndex)
{
	if (PupOrder.Num() == 1)
	{
		return 0;
	}
	//InIndex can be positive or negative, so we use always +
	if (InIndex >= PupOrder.Num())
	{
		return InIndex - PupOrder.Num();
	}
	else if(InIndex < 0)
	{
		return InIndex + PupOrder.Num();
	}
	else
	{
		return InIndex;
	}
}

void UPDWCharacterSelector::ManageMovementRequest(bool IsRight)
{
	if (IsAnimationPlaying(Outro) || IsAnimationPlaying(OutroWithSelection))
	{
		return;
	}
	else
	{
		int32 CurrentIndex = PupOrder.Find(CurrentPup);
		if (IsRight)
		{
			if (IsAnimationPlaying(Reverse))
			{
				return;
			}

			if (IsAnimationPlaying(Forward))
			{
				PlayAnimation(Forward, Forward->GetEndTime() - GetAnimationCurrentTime(Forward), 1, EUMGSequencePlayMode::Reverse, SwapCharacterAnimationSpeed, true);
			}
			else
			{
				PlayAnimation(Reverse, 0, 1, EUMGSequencePlayMode::Forward, SwapCharacterAnimationSpeed, true);
			}
			CurrentPup = PupOrder[GetRightIndex(CurrentIndex + 1)];
		}
		else
		{

			if (IsAnimationPlaying(Forward))
			{
				return;
			}

			if (IsAnimationPlaying(Reverse))
			{
				PlayAnimation(Reverse, Reverse->GetEndTime() - GetAnimationCurrentTime(Reverse), 1, EUMGSequencePlayMode::Reverse, SwapCharacterAnimationSpeed, true);
			}
			else
			{
				PlayAnimation(Forward, 0, 1, EUMGSequencePlayMode::Forward, SwapCharacterAnimationSpeed, true);
			}
			CurrentPup = PupOrder[GetRightIndex(CurrentIndex - 1)];
		}
	}
	
	UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(),SelectSoundID);
	//if (GameplayConstants)
	//{
	//	FPPAPupData* PupDataRow = GameplayConstants->PupDataTable->FindRow<FPPAPupData>(UPPAGameplayFunctionLibrary::GetPupIdByEnum(this, CurrentPup), FString("GENERAL"));
	//	UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), PupDataRow->PupNameVoicehoverID);
	//}
}

void UPDWCharacterSelector::CloseWithoutSelection()
{
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (UIManager)
	{
		UIManager->OnAllowedPupsChanged.RemoveDynamic(this, &ThisClass::OnAllowedPupsChanged);
	}

	if (IsAnimationPlaying(OutroWithSelection) || IsAnimationPlaying(Outro))
	{
		return;
	}
	else
	{
		PlayAnimation(Outro , 0 , 1, EUMGSequencePlayMode::Forward , 1 , true);
	}
}

void UPDWCharacterSelector::CloseWithSelection()
{
	if (IsAnimationPlaying(Outro) || IsAnimationPlaying(OutroWithSelection))
	{
		return;
	}
	
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (UIManager)
	{
		UIManager->OnAllowedPupsChanged.RemoveDynamic(this, &ThisClass::OnAllowedPupsChanged);
	}
	
	APDWPlayerController* PlayerController = Cast<APDWPlayerController>(GetOwningPlayer());
	if (UBlueprintGameplayTagLibrary::IsGameplayTagValid(CurrentPup) && PlayerController->GetPDWPlayerState()->GetCurrentPup() != CurrentPup)
	{	
		UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), ConfirmSoundID);
		PlayerController->ChangeCharacter(CurrentPup);
		PlayAnimation(OutroWithSelection);
	}
	else
	{
		UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), CloseSoundID);
		CloseWithoutSelection();
	}

}

void UPDWCharacterSelector::NativeConstruct()
{
	Super::NativeConstruct();

	IntroAnimationFinished.BindUFunction(this, FName("ManageIntroAnimationFinished"));
	this->BindToAnimationFinished(Intro, IntroAnimationFinished);

	OutroAnimationFinished.BindUFunction(this, FName("ManageOutroAnimationFinished"));
	this->BindToAnimationFinished(Outro, OutroAnimationFinished);

	OutroAnimationFinished.BindUFunction(this, FName("ManageOutroAnimationFinished"));
	this->BindToAnimationFinished(OutroWithSelection, OutroAnimationFinished);

	MovementAnimationFinished.BindUFunction(this, FName("ManageMovementAnimationFinished"));
	this->BindToAnimationFinished(Forward, MovementAnimationFinished);

	MovementAnimationFinished.BindUFunction(this, FName("ManageMovementAnimationFinished"));
	this->BindToAnimationFinished(Reverse, MovementAnimationFinished);

	TArray<UWidget*> CharactersWidgets = CharactersContainer->GetAllChildren();
	for (UWidget* CharacterImage : CharactersWidgets)
	{
		UPDWCharacterSelectorElement* SelectorElement = Cast<UPDWCharacterSelectorElement>(CharacterImage);
		if (SelectorElement)
		{
			SelectorElement->OnDirectionPressed.AddUniqueDynamic(this, &ThisClass::ManageMovementRequest);
			//SelectorElement->GetCharButton()->OnClicked.AddUniqueDynamic(this, &ThisClass::CloseWithSelection);
		}
	}
}

void UPDWCharacterSelector::NativeDestruct()
{
	UPDWUIManager* UIManager = UPDWGameplayFunctionLibrary::GetPDWUIManager(this);
	if (UIManager)
	{
		UIManager->OnAllowedPupsChanged.RemoveDynamic(this, &ThisClass::OnAllowedPupsChanged);
	}

	TArray<UWidget*> CharactersWidgets = CharactersContainer->GetAllChildren();
	for (UWidget* CharacterImage : CharactersWidgets)
	{
		UPDWCharacterSelectorElement* SelectorElement = Cast<UPDWCharacterSelectorElement>(CharacterImage);
		if (SelectorElement)
		{
			SelectorElement->OnDirectionPressed.RemoveDynamic(this, &ThisClass::ManageMovementRequest);
			//SelectorElement->GetCharButton()->OnClicked.RemoveDynamic(this, &ThisClass::CloseWithSelection);
		}
	}
	Super::NativeDestruct();
}

void UPDWCharacterSelector::OnAllowedPupsChanged()
{
	InitializeCharacterPicker(GetOwningPlayer());
}

void UPDWCharacterSelector::ManageIntroAnimationFinished()
{
	TArray<UWidget*> CharactersWidgets = CharactersContainer->GetAllChildren();
	for (UWidget* CharacterImage : CharactersWidgets)
	{
		UPDWCharacterSelectorElement* SelectorElement = Cast<UPDWCharacterSelectorElement>(CharacterImage);
		if (SelectorElement->GetPositionIndex() == 0)
		{
			SelectorElement->SetOwningPlayer(GetOwningPlayer());
			UNebulaFlowUIFunctionLibrary::SetUserFocus(this, SelectorElement->GetCharButton(), GetOwningPlayer());
			break;
		}
	}

	//UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), OpenSoundVoId);
}

void UPDWCharacterSelector::ManageOutroAnimationFinished()
{
	SetVisibility(ESlateVisibility::Hidden);
	UNebulaFlowCoreFunctionLibrary::TriggerAction(this, UPDWGameSettings::GetActionBackToGameplay().ToString(), "", GetOwningPlayer());
}

void UPDWCharacterSelector::ManageMovementAnimationFinished()
{
	UpdateCharacters();
}