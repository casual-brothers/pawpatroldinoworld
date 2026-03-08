// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/NebulaFlowLocalPlayer.h"
#include "PDWLocalPlayer.generated.h"

class UPDWPersistentUser;

/**
 * 
 */
UCLASS()
class PDW_API UPDWLocalPlayer : public UNebulaFlowLocalPlayer
{
	GENERATED_UCLASS_BODY()
public:
	virtual FString GetDefaultSlotName() const override;

	void PlayerAdded(class UGameViewportClient* InViewportClient, int32 InControllerID) override;

protected:
	void OnPersistentUserLoaded(const bool& bNewPersistentUser) override;

	UPROPERTY(BlueprintReadOnly)
	UPDWPersistentUser* PDWPersistentUser;
};
