// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "PDWTagComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPDWTagAdded, const FGameplayTagContainer&, UpdatedContainer, const FGameplayTag&, NewTagAdded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPDWTagRemoved, const FGameplayTagContainer&, UpdatedContainer, const FGameplayTag&, TagRemoved);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PDW_API UPDWTagComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPDWTagComponent();

	UPROPERTY(EditAnywhere)
	int32 QuestTargetID = -1;

	UPROPERTY(BlueprintAssignable)
	FOnPDWTagAdded OnPDWTagAdded;

	UPROPERTY(BlueprintAssignable)
	FOnPDWTagRemoved OnPDWTagRemoved;

	UFUNCTION(BlueprintCallable)
	void AddTag(const FGameplayTag& NewTag);
	
	UFUNCTION(BlueprintCallable)
	void RemoveTag(const FGameplayTag& TagToRemove);

	UFUNCTION(BlueprintCallable)
	bool HasTag(const FGameplayTag& TagToCheck) const;

	UFUNCTION()
	FGameplayTagContainer& GetCurrentTags() {return TagContainer;};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTagContainer TagContainer;
		
};
