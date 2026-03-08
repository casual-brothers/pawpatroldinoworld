// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "GameplayTagContainer.h"
#include "Data/PDWGameplayStructures.h"
#include "Engine/StreamableManager.h"
#include "PDWCutsceneFSMState.generated.h"

class ALevelSequenceActor;

UCLASS()
class PDW_API UPDWCutsceneFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()
	
public:

	void OnFSMStateEnter_Implementation(const FString& InOption = FString("")) override;

	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;

	void OnFSMStateExit_Implementation() override;

	

protected:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UNebulaFlowBasePage> DialogueCutscene;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UNebulaFlowBasePage> DialogueCinematic;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> CinematicOriginStreamingSource = nullptr;

	void StartCutscene();

	void PlayNextSequence();

	void OnSequenceLoaded();

	UFUNCTION()
	void OnCurrentSequenceFinished();

	UFUNCTION()
	virtual void EndCutscene();

	UFUNCTION()
	virtual void OnTransitionInEnd();

	UPROPERTY()
	FGameplayTag CutsceneTag {};

	UPROPERTY()
	ALevelSequenceActor* SequenceActor;

	UPROPERTY()
	int32 CurrentSequenceIndex = -1;

	UPROPERTY()
	FPDWSequenceCollection CurrentSequenceCollection;

	UPROPERTY()
	FPDWSequenceDataElement CurrentSequenceData;

	TSharedPtr<FStreamableHandle> LoadSoftObjsHandle;

private:

	UPROPERTY()
	AActor* CinematicStreamingSource = nullptr;

	UFUNCTION()
	void HandleFramedActor();

	UFUNCTION()
	void HandlePlayersVisibility(bool IsHidden);
	void SetUpCameraViewport();
};
