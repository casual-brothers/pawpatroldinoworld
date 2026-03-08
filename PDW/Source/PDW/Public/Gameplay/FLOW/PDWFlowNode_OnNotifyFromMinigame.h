// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDWFlowNode_OnNotifyFromActor.h"
#include "Gameplay/Components/PDWMinigameConfigComponent.h"
#include "PDWFlowNode_OnNotifyFromMinigame.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Wait for minigame"))
class PDW_API UPDWFlowNode_OnNotifyFromMinigame : public UPDWFlowNode_OnNotifyFromActor
{
	GENERATED_BODY()
	
public:

	UPDWFlowNode_OnNotifyFromMinigame(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere)
	TMap<TObjectPtr<UDataLayerAsset>, bool> MinigameInitCompleteLayersInfo {};

	UPROPERTY(EditAnywhere)
	TMap<TObjectPtr<UDataLayerAsset>, bool> MinigameLeftOrCompletedLayersInfo {};

protected:

	void OnEventReceived() override;

	UFUNCTION()
	void ContinueToNextNode();

	void ObserveActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;

	void ForgetActor(TWeakObjectPtr<AActor> Actor, TWeakObjectPtr<UFlowComponent> Component) override;

	UFUNCTION()
	void OnMinigameComplete(const FMiniGameEventSignature& inSignature);

	UFUNCTION()
	void OnMinigameLeft(const FMiniGameEventSignature& inSignature);

	UFUNCTION()
	void OnMinigameInitComplete(const TArray<APDWPlayerController*>& Controllers);
};
