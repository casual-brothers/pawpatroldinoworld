// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PDWGameplayTagSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDWGameplayTagSettings)

UPDWGameplayTagSettings::UPDWGameplayTagSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UPDWGameplayTagSettings* UPDWGameplayTagSettings::Get()
{
	return CastChecked<UPDWGameplayTagSettings>(UPDWGameplayTagSettings::StaticClass()->GetDefaultObject());
}

FGameplayTag& UPDWGameplayTagSettings::GetMinigameInteractionTag()
{
	return UPDWGameplayTagSettings::Get()->Interaction_Minigame;
}

FGameplayTag& UPDWGameplayTagSettings::GetInteractionActionTag()
{
	return UPDWGameplayTagSettings::Get()->Interaction_Action;
}

FGameplayTag& UPDWGameplayTagSettings::GetInteractionSkillTag()
{
	return UPDWGameplayTagSettings::Get()->Interaction_Skill;
}

FGameplayTag& UPDWGameplayTagSettings::GetInteractionAreaTag()
{
	return UPDWGameplayTagSettings::Get()->Interaction_Area;
}

FGameplayTag& UPDWGameplayTagSettings::GetQuestTargetTag()
{
	return UPDWGameplayTagSettings::Get()->Quest_Target;
}

FGameplayTag& UPDWGameplayTagSettings::GetInputPressedTag()
{
	return UPDWGameplayTagSettings::Get()->Input_Pressed;
}

FGameplayTag& UPDWGameplayTagSettings::GetInputReleasedTag()
{
	return UPDWGameplayTagSettings::Get()->Input_Released;
}

FGameplayTag& UPDWGameplayTagSettings::GetMinigameTransitionToComplete()
{
	return UPDWGameplayTagSettings::Get()->Minigame_Transition_To_Complete;
}

FGameplayTag& UPDWGameplayTagSettings::GetMinigameNotifyOverlap()
{
	return UPDWGameplayTagSettings::Get()->Minigame_Notify_Overlap_Success;
}

FGameplayTag& UPDWGameplayTagSettings::GetMinigameNotifyDestroy()
{
	return UPDWGameplayTagSettings::Get()->Minigame_Notify_Destroy_Success;
}

FGameplayTag& UPDWGameplayTagSettings::GetMinigameNotifySpawn()
{
	return UPDWGameplayTagSettings::Get()->Minigame_Notify_Spawn_Success;
}

FGameplayTag& UPDWGameplayTagSettings::GetInteractionDefaultState()
{
	return UPDWGameplayTagSettings::Get()->Interaction_DefaultState;
}

FGameplayTag& UPDWGameplayTagSettings::GetHappyDino()
{
	return UPDWGameplayTagSettings::Get()->HappyDino;
}

FGameplayTag& UPDWGameplayTagSettings::GetHungryDino()
{
	return UPDWGameplayTagSettings::Get()->HungryDino;
}

FGameplayTag& UPDWGameplayTagSettings::GetHungryDinoWithItem()
{
	return UPDWGameplayTagSettings::Get()->HungryDinoWithItem;
}

FGameplayTag& UPDWGameplayTagSettings::GetThirstyDino()
{
	return UPDWGameplayTagSettings::Get()->ThirstyDino;
}

FGameplayTag& UPDWGameplayTagSettings::GetDirtyDino()
{	
	return UPDWGameplayTagSettings::Get()->DirtyDino;
}

FGameplayTag& UPDWGameplayTagSettings::GetHurtDino()
{
	return UPDWGameplayTagSettings::Get()->HurtDino;
}

FGameplayTag& UPDWGameplayTagSettings::GetBoredDino()
{
	return UPDWGameplayTagSettings::Get()->BoredDino;
}

TArray<FGameplayTag> UPDWGameplayTagSettings::GetDinoNeeds()
{
	TArray<FGameplayTag> NeedsList = {};
	NeedsList.Add(UPDWGameplayTagSettings::GetHungryDino());
	NeedsList.Add(UPDWGameplayTagSettings::GetThirstyDino());
	NeedsList.Add(UPDWGameplayTagSettings::GetDirtyDino());
	NeedsList.Add(UPDWGameplayTagSettings::GetHurtDino());
	NeedsList.Add(UPDWGameplayTagSettings::GetBoredDino());

	return NeedsList;
}

FGameplayTag& UPDWGameplayTagSettings::GetSOUseEvent()
{
	return UPDWGameplayTagSettings::Get()->SOUseEvent;
}

FGameplayTag& UPDWGameplayTagSettings::GetPupTreatTag()
{
	return UPDWGameplayTagSettings::Get()->PupTreatTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetAutoSwapRefreshTag()
{
	return UPDWGameplayTagSettings::Get()->AutoSwapAreaRefreshSkillTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetAutoSwapStopRefreshTag()
{
	return UPDWGameplayTagSettings::Get()->AutoSwapStopRefreshTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetVehicleBoostVibrationTag()
{
	return UPDWGameplayTagSettings::Get()->VehicleBoostVibrationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetVehicleImpactVibrationTag()
{
	return UPDWGameplayTagSettings::Get()->VehicleImpactVibrationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetVehicleLandVibrationTag()
{
	return UPDWGameplayTagSettings::Get()->VehicleLandVibrationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetPupLandVibrationTag()
{
	return UPDWGameplayTagSettings::Get()->PupLandVibrationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetSwapPupVibrationTag()
{
	return UPDWGameplayTagSettings::Get()->SwapPupVibrationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetChangeToVehicleVibrationTag()
{
	return UPDWGameplayTagSettings::Get()->ChangeToVehicleVibrationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetChangeToPupVibrationTag()
{
	return UPDWGameplayTagSettings::Get()->ChangeToPupVibrationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetRewardVibrationTag()
{
	return UPDWGameplayTagSettings::Get()->RewardVibrationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetCollectEggVibrationTag()
{
	return UPDWGameplayTagSettings::Get()->CollectEggVibrationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetHornVibrationTag()
{
	return UPDWGameplayTagSettings::Get()->HornVibrationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetBarkVibrationTag()
{
	return UPDWGameplayTagSettings::Get()->BarkVibrationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetTrunkImpactTag()
{
	return UPDWGameplayTagSettings::Get()->TrunkImpactTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetInteractionInactiveState()
{
	return UPDWGameplayTagSettings::Get()->Interaction_InactiveState;
}

FGameplayTag& UPDWGameplayTagSettings::GetPupCustomizationTag()
{
	return UPDWGameplayTagSettings::Get()->PupCustomizationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetDinoCustomizationTag()
{
	return UPDWGameplayTagSettings::Get()->DinoCustomizationTag;
}

FGameplayTag& UPDWGameplayTagSettings::GetDinoPenCustomizationTag()
{
	return UPDWGameplayTagSettings::Get()->DinoPenCustomizationTag;
}	

FGameplayTag& UPDWGameplayTagSettings::GetEggTag()
{
	return UPDWGameplayTagSettings::Get()->EggTag;
}
