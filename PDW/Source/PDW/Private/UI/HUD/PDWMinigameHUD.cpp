// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/PDWMinigameHUD.h"
#include "Data/PDWMinigameConfigData.h"
#include "FunctionLibraries/PDWGameplayFunctionLibrary.h"
#include "Gameplay/MiniGames/PDWMiniGameInputBehaviour.h"
#include "Components/PanelWidget.h"
#include "Data/PDWGameSettings.h"
#include "Managers/PDWEventSubsytem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/OverlaySlot.h"
#include "UI/Widgets/Dialogue/PDWDialogueBaseWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "UI/Widgets/PDWMinigamePlayersNavButton.h"
#include "Gameplay/MiniGames/ConditionsCheck/PDWQuantityConditionCheck.h"
#include "UI/Widgets/PDWProgressBar.h"
#include "GameplayTagContainer.h"
#include "Gameplay/MiniGames/PDWMovingObjectBehaviour.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/PDWPlayerIndicator.h"
#include "Gameplay/Components/PDWMinigameMovingMesh.h"
#include "FunctionLibraries/NebulaFlowAudioFunctionLibrary.h"


void UPDWMinigameHUD::InitWithMinigameData(const UPDWMinigameConfigData* MinigameData)
{
	ActionContainer->ClearChildren();
	CurrentMinigameTag = MinigameData->MiniGameIdentifierTag;
	
	if (MinigameProgress)
	{
		//Get the Max Quantity
		TArray<FGameplayTag> ObjectivesKeys;
		MinigameData->MiniGameObjectiveConfig.GetKeys(ObjectivesKeys);
		TargetQuantity = MinigameData->MiniGameObjectiveConfig[ObjectivesKeys[0]]->TargetQuantity;
		CurrentQuantity = 0;
		if (TargetQuantity <= 1)
		{
			MinigameProgress->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	for (FPDWMinigameButtonInputData& InputData : MinigameInputsData)
	{
		if (InputData.MinigameId == MinigameData->MiniGameIdentifierTag)
		{
			for (FPDWMinigameInputData InputToAdd : InputData.InputsData)
			{
				UPDWMinigamePlayersNavButton* NewButton = CreateWidget<UPDWMinigamePlayersNavButton>(this, PlayersNavButton);
				if (NewButton)
				{
					NewButton->InitMinigamePlayersButton(InputToAdd);
					ActionContainer->AddChild(NewButton);
				}
			}
		}	
	}

	StartTutorialDialogue(CurrentMinigameTag);

	UPDWEventSubsytem::Get(this)->OnShowHint.AddUniqueDynamic(this, &ThisClass::OnShowHint);
	UPDWEventSubsytem::Get(this)->OnTargetDeactivation.AddUniqueDynamic(this, &ThisClass::OnTargetDeactivation);
	UPDWEventSubsytem::Get(this)->OnHintRequest.AddUniqueDynamic(this, &ThisClass::OnHintRequest);
	if (MinigameProgress)
	{
		UPDWEventSubsytem::Get(this)->BindToInteractionEvents(MinigameData->MiniGameIdentifierTag)->AddUniqueDynamic(this, &ThisClass::OnNotifySuccessFromMinigameActor);
	}

	if (UPDWGameplayFunctionLibrary::IsMultiplayerOn(this) && !(MinigamePlayerIndicatorExpection.Contains(CurrentMinigameTag)))
	{
		const FMiniGameBehaviourConfigStruct* CurrentMinigameConfig = MinigameData->MiniGameConfig.Find(EMiniGamePlayerConfiguration::MultiPlayer);
		if (CurrentMinigameConfig)
		{
			if (CurrentMinigameConfig->MiniGameBehaviour.Num() > 1)
			{
				for (auto MinigameInputList : CurrentMinigameConfig->MiniGameBehaviour)
				{
					for (UPDWMiniGameInputBehaviour* MinigameBeahviour : MinigameInputList.Value.InputBehaviour)
					{
						if (UPDWMovingObjectBehaviour* MovingObject = Cast<UPDWMovingObjectBehaviour>(MinigameBeahviour))
						{
							AActor* PupToPosses = MovingObject->GetPossesedPupInstance();
							UPDWMinigameMovingMesh* ObjectToMove = MovingObject->GetMovingMeshComponent();

 							if (!PlayerIndicatorWidgetClass)
							{
								return;
							}
							UPDWPlayerIndicator* NewWidget = CreateWidget<UPDWPlayerIndicator>(this, PlayerIndicatorWidgetClass);
							bool bFoundObject = false;

							switch (MovingObject->GetMinigamePossesRule())
							{
							case EMiniGameActorSource::PossesPup:
								if (PupToPosses)
								{
									ActorPossesedObjects.Add(PupToPosses, NewWidget);
									bFoundObject = true;
								}
								break;
							case EMiniGameActorSource::PossesSceneComponent:
								if (ObjectToMove)
								{
									MovingMeshPossesedObjects.Add(ObjectToMove, NewWidget);
									if (MovingObject->HaveMinigameMultiplePosses())
									{
										MovingObject->OnObjectChange.AddUniqueDynamic(this, &ThisClass::OnMovingMeshChange);
									}
									bFoundObject = true;
								}
								break;
							}
							
							if (bFoundObject)
							{
								NewWidget->AddToViewport(1);
								NewWidget->InitIndicator(MinigameInputList.Key);
								bStartMoveInidcator = true;
							}
						}
					}
				}
			}
		}
	}

	BP_InitWithMinigameData(MinigameData);
}

void UPDWMinigameHUD::OnNotifySuccessFromMinigameActor(const FInteractionEventSignature& inEvent)
{
	CurrentQuantity++;
	if (MinigameProgress)
	{
		MinigameProgress->ChangeProgressPercentage(float(CurrentQuantity));
	}
	if (SmellTargetPanel->IsVisible())
	{
		UMaterialInstanceDynamic* Material = SmellTargetImage->GetDynamicMaterial();
		if (Material)
		{
			Material->SetScalarParameterValue(SmellProgressionParameterName, 1-(float(CurrentQuantity) / float(TargetQuantity)));
		}
	}

	UNebulaFlowAudioFunctionLibrary::PlayUIEvent(GetWorld(), StepSuccesSound);
}

void UPDWMinigameHUD::NativeDestruct()
{
	UPDWEventSubsytem::Get(this)->OnShowHint.RemoveDynamic(this, &ThisClass::OnShowHint);
	UPDWEventSubsytem::Get(this)->OnTargetDeactivation.RemoveDynamic(this, &ThisClass::OnTargetDeactivation);
	TutorialComment->OnEndDialogue.RemoveDynamic(this, &ThisClass::OnEndTutorialDialogue);
	if (MinigameProgress)
	{
		UPDWEventSubsytem::Get(this)->BindToInteractionEvents(CurrentMinigameTag)->RemoveDynamic(this, &ThisClass::OnNotifySuccessFromMinigameActor);
	}
	if (ActiveHintWidgets.Num())
	{
		TArray<FPDWMinigameHintWidgets> WidgetsToRemove{};
		for (FPDWMinigameHintWidgets& WidgetStruct : ActiveHintWidgets)
		{
			WidgetStruct.HintWidget->RemoveFromParent();
			WidgetsToRemove.Add(WidgetStruct);
		}

		for (FPDWMinigameHintWidgets& WidgetStructToRemove : WidgetsToRemove)
		{
			ActiveHintWidgets.Remove(WidgetStructToRemove);
		}
	}
	
	bStartMoveInidcator = false;
	if (ActorPossesedObjects.Num())
	{
		for (auto PossesedObjects : ActorPossesedObjects)
		{
			PossesedObjects.Value->RemoveFromParent();
		}
		ActorPossesedObjects.Empty();
	}
	if (MovingMeshPossesedObjects.Num())
	{
		for (auto PossesedObjects : MovingMeshPossesedObjects)
		{
			PossesedObjects.Value->RemoveFromParent();
		}
		MovingMeshPossesedObjects.Empty();
	}

	Super::NativeDestruct();
}

void UPDWMinigameHUD::OnShowHint(USceneComponent* Owner, const FGameplayTag& inMinigameID)
{
	if (!Owner)
	{
		return;
	}

	for (FPDWMinigameHintWidgets& WidgetStruct : ActiveHintWidgets)
	{
		if (WidgetStruct.MinigameComp == Owner)
		{
			return;
		}
	}

	UUserWidget* HintWidget = CreateWidget<UUserWidget>(this, HintWidgetClass);
	HintWidget->AddToViewport(2);

	SetHintWidgetTranslation(HintWidget, Owner->GetComponentLocation(), inMinigameID);
	FPDWMinigameHintWidgets NewHint;
	NewHint.HintWidget = HintWidget;
	NewHint.MinigameComp = Owner;
	NewHint.MinigameId = inMinigameID;
	ActiveHintWidgets.Add(NewHint);
}

void UPDWMinigameHUD::OnTargetDeactivation(AActor* ActorDeactivated)
{
	FPDWMinigameHintWidgets WidgetToRemove {};
	bool bFound = false;
	for (FPDWMinigameHintWidgets& WidgetStruct : ActiveHintWidgets)
	{
		if (WidgetStruct.MinigameComp->GetOwner() == ActorDeactivated)
		{
			WidgetStruct.HintWidget->RemoveFromParent();
			WidgetToRemove = WidgetStruct;
			bFound = true;
			break;
		}
	}
	if (bFound)
	{
		ActiveHintWidgets.Remove(WidgetToRemove);
	}
}

void UPDWMinigameHUD::OnHintRequest(const FGameplayTag& inMinigameID)
{
	if (MinigameSequenceTags.Contains(inMinigameID))
	{
		for (UWidget* MinigameButton : NavButtonActionContainer->GetAllChildren())
		{
			if (UPDWMinigameNavButton* MinigameNavButton = Cast<UPDWMinigameNavButton>(MinigameButton))
			{
				MinigameNavButton->ActiveHint();
			}
		}
	}
}

void UPDWMinigameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	TArray<FPDWMinigameHintWidgets> WidgetsToRemove{};
	for (FPDWMinigameHintWidgets& WidgetStruct : ActiveHintWidgets)
	{
		SetHintWidgetTranslation(WidgetStruct.HintWidget, WidgetStruct.MinigameComp->GetComponentLocation(), WidgetStruct.MinigameId);
		WidgetStruct.CurrentLifeTime += InDeltaTime;
			
		if (WidgetStruct.CurrentLifeTime >= HintTime)
		{
			WidgetStruct.HintWidget->RemoveFromParent();
			WidgetsToRemove.Add(WidgetStruct);
		}
	}

	for (FPDWMinigameHintWidgets& WidgetStructToRemove : WidgetsToRemove)
	{
		ActiveHintWidgets.Remove(WidgetStructToRemove);
	}

	if (bStartCount)
	{
		CurrentTime += InDeltaTime;
		if (CurrentTime >= LoopTime)
		{
			if (MinigameSequenceTags.Contains(CurrentMinigameTag) && !(NavButtonActionContainer->GetAllChildren().Num()))
			{
				return;
			}
			bStartCount = false;
			CurrentTime = 0.f;
			StartTutorialDialogue(CurrentMinigameTag);
		}
	}

	if (bStartMoveInidcator)
	{
		if (ActorPossesedObjects.Num())
		{
			for (auto PossesedObjects : ActorPossesedObjects)
			{
				MovePlayerIndicator(PossesedObjects.Key->GetTargetLocation(), PossesedObjects.Value);
			}
		}
		if (MovingMeshPossesedObjects.Num())
		{
			for (auto PossesedObjects : MovingMeshPossesedObjects)
			{
				MovePlayerIndicator(PossesedObjects.Key->GetComponentLocation(), PossesedObjects.Value);
			}
		}
	}
}

void UPDWMinigameHUD::SetHintWidgetTranslation(UUserWidget* WidgetToSet, const FVector3d& WorldPos, const FGameplayTag& inMinigameID)
{
	if (!WidgetToSet) return;

    APlayerController* PC = UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this);
    if (!PC) return;

    FVector2D ScreenPos;
    UGameplayStatics::ProjectWorldToScreen(PC, WorldPos, ScreenPos);

	ScreenPos = FVector2D(ScreenPos.X - (WidgetToSet->GetDesiredSize().X/2),ScreenPos.Y - (WidgetToSet->GetDesiredSize().Y/2));
    if (HintOffset.Contains(inMinigameID))
    {
        ScreenPos += HintOffset[inMinigameID];
    }
	WidgetToSet->SetPositionInViewport(ScreenPos);

    // --- DEBUG --- //
    //if (GEngine)
    //{
    //    FString DebugMsg = FString::Printf(TEXT("WorldPos=(%.1f, %.1f, %.1f)  ScreenPos=(%.1f, %.1f)"),
    //        WorldPos.X, WorldPos.Y, WorldPos.Z, ScreenPos.X, ScreenPos.Y);

    //    GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Green, DebugMsg);
    //}

    //DrawDebugSphere(GetWorld(), WorldPos, 30.f, 12, FColor::Red, false, 0.f);
}

void UPDWMinigameHUD::StartTutorialDialogue(FGameplayTag MinigameId)
{
	if (bHUDCommentIsPlaying)
	{
		return;
	}
	if (DialoguesHintConfiguration.Contains(MinigameId))
	{
		if (MinigameSequenceTags.Contains(CurrentMinigameTag) && !(NavButtonActionContainer->GetAllChildren().Num()))
		{
			bStartCount = true;
			return;
		}
		TutorialComment->StartDialogue(DialoguesHintConfiguration[MinigameId]);
		TutorialComment->OnEndDialogue.AddUniqueDynamic(this, &ThisClass::OnEndTutorialDialogue);
	}
	else
	{
		TutorialComment->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UPDWMinigameHUD::OnEndTutorialDialogue()
{
	TutorialComment->OnEndDialogue.RemoveDynamic(this, &ThisClass::OnEndTutorialDialogue);
	bStartCount = true;
}

void UPDWMinigameHUD::ShowSmellTarget(FGameplayTagContainer MinigameID)
{
	if (!SmellTargetImage && !SmellTargetPanel)
	{
		return;
	}
	
	for (auto SmellConfig : MinigameSmellTargetConfiguration)
	{
		if (MinigameID.HasTag(SmellConfig.Key))
		{
			SmellTargetPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
			UMaterialInstanceDynamic* Material = SmellTargetImage->GetDynamicMaterial();
			if (Material)
			{
				Material->SetTextureParameterValue(SmellTextureParameterName, SmellConfig.Value.TargetTexture.LoadSynchronous());
				Material->SetScalarParameterValue(SmellProgressionParameterName, 1);
			}
			else
			{
				SmellTargetImage->SetBrushFromSoftTexture(SmellConfig.Value.TargetTexture);
			}
		}
	}
}

void UPDWMinigameHUD::ManageTriggerDialogue(const FConversation& Conversation)
{
	Super::ManageTriggerDialogue(Conversation);
	TutorialComment->EndDialogue();
}

void UPDWMinigameHUD::MovePlayerIndicator(FVector WorldTransform, UPDWPlayerIndicator* WidgetToMove)
{
	FVector2D NewWidgetPosition = FVector2D::Zero();
	UGameplayStatics::ProjectWorldToScreen(UPDWGameplayFunctionLibrary::GetPlayerControllerOne(this),WorldTransform, NewWidgetPosition);
	NewWidgetPosition = FVector2D(NewWidgetPosition.X - (WidgetToMove->GetDesiredSize().X/2),NewWidgetPosition.Y - (WidgetToMove->GetDesiredSize().Y/2));
	if (PlayerIndicatorOffset.Contains(CurrentMinigameTag))
    {
		if (PlayerIndicatorOffset[CurrentMinigameTag].OffsetByPlayer.Contains(WidgetToMove->GetAssinedPlayer()))
		{
			NewWidgetPosition += PlayerIndicatorOffset[CurrentMinigameTag].OffsetByPlayer[WidgetToMove->GetAssinedPlayer()];
		}
    }
	WidgetToMove->SetPositionInViewport(NewWidgetPosition);
}

void UPDWMinigameHUD::OnMovingMeshChange(UPDWMinigameMovingMesh* OldMovingMesh, UPDWMinigameMovingMesh* NewMovingMesh)
{
	if (MovingMeshPossesedObjects.Contains(OldMovingMesh))
	{
		UPDWPlayerIndicator* PlayerIndicator = MovingMeshPossesedObjects[OldMovingMesh];
		MovingMeshPossesedObjects.Remove(OldMovingMesh);
		MovingMeshPossesedObjects.Add(NewMovingMesh, PlayerIndicator);
	}
}