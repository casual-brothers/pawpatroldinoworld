#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GAS/PDWGASAbility.h"
#include "PDWGAS_BaseSwapAbility.generated.h"


class APDWCharacter;
class APDWPlayerController;
class UNebulaGraphicsCustomizationComponent;

UCLASS()
class PDW_API UPDWGAS_BaseSwapAbility : public UPDWGASAbility
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	APawn* SwapCharacterCustomization(const bool bRequestVehicle,const EPupRequestType& inPupRequest);
	UFUNCTION(BlueprintCallable)
	void CompleteCharacterSwap();

	void SetUpSwap();

	UPROPERTY(EditAnywhere,Category = "CameraConfig")
	float BlendTime = 0.1f;

	UPROPERTY(EditAnywhere,Category = "CameraConfig")
	TEnumAsByte<EViewTargetBlendFunction> BlendFunction = VTBlend_EaseInOut;

	UPROPERTY(EditAnywhere,Category = "CameraConfig")
	float BlendExp = 2.0f;

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<APawn> CharacterClass;

	UPROPERTY(EditAnywhere)
	FVector SpawnPositionOffset = FVector::ZeroVector;
	UPROPERTY()
	TObjectPtr<APawn> OldCharacter;

	UPROPERTY()
	TObjectPtr<APawn> NewCharacter;

	FGameplayTag PupTag;

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
