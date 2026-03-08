// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Components/PDWAIMoveComponent.h"
#include "PDWDinoAIMoveComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PDW_API UPDWDinoAIMoveComponent : public UPDWAIMoveComponent
{
	GENERATED_BODY()
	
protected:

	bool CanMove() override;

};
