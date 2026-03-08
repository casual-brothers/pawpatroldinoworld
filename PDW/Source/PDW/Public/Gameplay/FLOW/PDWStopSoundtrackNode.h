// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/FLOW/PDWSoundtrackNode.h"
#include "PDWStopSoundtrackNode.generated.h"

/**
 * 
 */
UCLASS()
class PDW_API UPDWStopSoundtrackNode : public UPDWSoundtrackNode
{
	GENERATED_BODY()

protected:

	void PlaySoundtrack() override;
	
};
