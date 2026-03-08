#include "Gameplay/GAS/PDWGAS_BaseSwapAbility.h"

#include "Components/NebulaGraphicsCustomizationComponent.h"
#include "Data/PDWPlayerState.h"
#include "GameFramework/MovementComponent.h"
#include "Gameplay/Pawns/PDWCharacter.h"
#include "Gameplay/Pawns/PDWPlayerController.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Data/PDWGameSettings.h"
#include "NebulaVehicle/PDWVehiclePawn.h"
#include "Gameplay/Animation/PDWAnimInstance.h"
#include "FunctionLibraries/PDWDataFunctionLibrary.h"

APawn* UPDWGAS_BaseSwapAbility::SwapCharacterCustomization(const bool bRequestVehicle,const EPupRequestType& inPupRequest)
{
	const FGameplayTag& NextPup = PlayerController->GetNextPupForSwap();
	const FGameplayTag& CurrentPup = PlayerController->GetCurrentPup();
	PupTag = (inPupRequest == EPupRequestType::Next) ? PlayerController->GetNextPupForSwap() : PlayerController->GetCurrentPup();
	APDWCharacter* PupInstance = PlayerController->GetPupInstance();
	if (PupInstance)
	{
		PupInstance->CustomizePup(PupTag);
	}

	return PlayerController->GetPawn();
}

void UPDWGAS_BaseSwapAbility::CompleteCharacterSwap()
{
	PlayerController->HandleSwapEnd();
}

void UPDWGAS_BaseSwapAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	PlayerController->bDoingSwappingAction = true;
	
}
