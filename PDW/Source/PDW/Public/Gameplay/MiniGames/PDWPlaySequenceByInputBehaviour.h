// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MiniGames/PDWButtonSequenceInputBehaviour.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "../../../../../../../Source/Runtime/LevelSequence/Public/DefaultLevelSequenceInstanceData.h"
#include "PDWPlaySequenceByInputBehaviour.generated.h"

class ULevelSequence;
class ALevelSequenceActor;
class ULevelSequencePlayer;
USTRUCT(BlueprintType)
struct PDW_API FSequenceConfiguration : public FButtonSequenceConfiguration
{

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration")
	ULevelSequence* SequenceToPlay = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta =(ToolTip = "if true, will use minigame origin."), Category = "Configuration")
	bool bOverrideSequenceData = false;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FMovieSceneSequencePlaybackSettings LevelSequenceSettings;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Configuration")
	FName BindingTagID = NAME_None;

};

UCLASS()
class PDW_API UPDWPlaySequenceByInputBehaviour : public UPDWButtonSequenceInputBehaviour
{
	GENERATED_BODY()
	
public:

	void ExecuteBehaviour(const FInputActionInstance& inInputInstance) override;


	void InitializeBehaviour(APDWPlayerController* inController, UPDWMinigameConfigComponent* inMiniGameComp) override;


	void UninitializeBehaviour() override;

protected:

	UFUNCTION()
	void OnSequenceEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_BeforeUninit();

	UPROPERTY()
	ALevelSequenceActor* LevelSequenceActor = nullptr;

	UPROPERTY()
	ULevelSequencePlayer* SequencePlayer = nullptr;
};
