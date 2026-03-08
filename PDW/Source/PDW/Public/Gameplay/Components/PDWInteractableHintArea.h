// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Managers/PDWDialogueSubSystem.h"
#include "Components/CapsuleComponent.h"
#include "PDWInteractableHintArea.generated.h"

class UPDWTagComponent;
class UPDWInteractionReceiverComponent;
/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWInteractableHintArea : public UCapsuleComponent
{
	GENERATED_BODY()
	

public:
	UPDWInteractableHintArea();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag,FConversation> ConversationPreset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag,FGameplayTag> ConversationToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float DelayBeforePlay = 3.0f;

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type Reason) override;

protected:
	
	//UFUNCTION()
	//void OnQuestTagAdded(const FGameplayTagContainer& UpdatedContainer, const FGameplayTag& NewTagAdded);
	//
	//UFUNCTION()
	//void OnQuestTagRemoved(const FGameplayTagContainer& UpdatedContainer, const FGameplayTag& TagRemoved);

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex);

	UFUNCTION()
	void OnInteractionStateChange(UPDWInteractionReceiverComponent* Component, const FGameplayTag& ChangedTag, const FGameplayTag& Tag);

	UFUNCTION()
	void TriggerConversation();

	FTimerHandle ConversationTriggerDelayHandle;
	
	UPROPERTY()
	UPDWTagComponent* TagComp = nullptr;

	UPROPERTY()
	UPDWInteractionReceiverComponent* InteractionReceiver = nullptr;

private:

	FGameplayTag CurrentState;
	//FGameplayTag CurrentTimerTag; 
	FGameplayTag QuestTargetTag;
};
