// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PDWGameSettings.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "UI/Widgets/PDWMinigameNavButton.h"
#include "Data/GameOptionsData.h"
#include "FunctionLibraries/NebulaFlowConsoleFunctionLibrary.h"

//#include UE_INLINE_GENERATED_CPP_BY_NAME(UPDWGameSettings)

UPDWGameSettings::UPDWGameSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Initialize default values for the game settings here if needed
}

UPDWGameSettings* UPDWGameSettings::Get()
{
	return CastChecked<UPDWGameSettings>(UPDWGameSettings::StaticClass()->GetDefaultObject());
}

void UPDWGameSettings::GetEntityConfig(TMap<FGameplayTag, FEntity>& OutConfig)
{
	for (auto& [Tag, PupAsset] : Get()->EntityConfig)
	{
		OutConfig.Add(Tag,PupAsset);
	}
}

FEntity UPDWGameSettings::GetEntityConfigForTag(const FGameplayTag& EntityTag)
{
	return Get()->EntityConfig.FindRef(EntityTag);
}

TSubclassOf<UPDWBaseHUD> UPDWGameSettings::GetHUDClass(EHUDType HudType)
{
	return ensureMsgf(Get()->HUDClasses.Num() > 0 && Get()->HUDClasses.Contains(HudType), TEXT("Missing HUD Class Configuration")) ? Get()->HUDClasses[HudType] : nullptr;
}

TSubclassOf<UNebulaFlowNavbarButton> UPDWGameSettings::GetNavButtonClassReference()
{
	return ensureMsgf(Get()->NavButtonClassReference, TEXT("Missing NavButton Class Configuration")) ? Get()->NavButtonClassReference : nullptr;
}

TSubclassOf<UPDWMinigameNavButton> UPDWGameSettings::GetMinigameNavButtonClassReference()
{
	return ensureMsgf(Get()->MinigameNavButtonClassReference, TEXT("Missing Minigame NavButton Class Configuration")) ? Get()->MinigameNavButtonClassReference : nullptr;
}

UInputMappingContext* UPDWGameSettings::GetMappinContextByTag(const FGameplayTag& MapTag)
{
	UInputMappingContext* Result = nullptr;
	TMap<FGameplayTag, FSoftObjectPath> ListOfIMC;
	if (UNebulaFlowConsoleFunctionLibrary::IsJoyconHorizontal(0))
	{
		ListOfIMC = Get()->ListOfMappingContextByTagSwitchSingleJoycon;
	}
	else
	{
		ListOfIMC = Get()->ListOfMappingContextByTag;
	}
	if (ensureMsgf(ListOfIMC.Num(), TEXT("Missing Input List Configuration")))
	{
		if (ensureMsgf(ListOfIMC.Contains(MapTag), TEXT("Missing Map")))
		{
			Result = Cast<UInputMappingContext>(ListOfIMC[MapTag].TryLoad());
		}
	}

	return Result;
}

UInputAction* UPDWGameSettings::GetInputActionByTag(const FGameplayTag& ActionTag)
{
	UInputAction* Result = nullptr;
	if (ensureMsgf(Get()->ListOfInputActions.Num(), TEXT("Missing Input List Configuration")))
	{
		if (ensureMsgf(Get()->ListOfInputActions.Contains(ActionTag), TEXT("Missing Action")))
		{
			Result = Cast<UInputAction>(Get()->ListOfInputActions[ActionTag].TryLoad());
		}
	}

	return Result;
}

bool UPDWGameSettings::GetWrongFaceButtons(const FGameplayTag& CorrectButton,FGameplayTagContainer& WrongButtons)
{
	if(FaceButtonsList.HasAny(CorrectButton.GetSingleTagContainer()))
	{
		for (const FGameplayTag& Tag : FaceButtonsList)
		{
			if (CorrectButton != Tag)
			{
				WrongButtons.AddTag(Tag);
			}
		}
		return true;
	}
	else
	{
		if (HoldMap.Contains(CorrectButton))
		{
			return GetWrongFaceButtons(HoldMap[CorrectButton],WrongButtons);
		}
	}
	
	return false;
}

TSubclassOf<APDWCharacter> UPDWGameSettings::GetPupClass(const FGameplayTag& inTag)
{
	return Get()->EntityConfig[inTag].PupClass;
}

TSubclassOf<APDWVehiclePawn> UPDWGameSettings::GetPupVehicleClass(const FGameplayTag& inTag)
{
	return Get()->EntityConfig[inTag].PupVehicle;
}

TSubclassOf<UPDWGASAbility> UPDWGameSettings::GetBasicSkill(const EBaseSkillType& inSkillType)
{
	ensureMsgf(Get()->BasicSkills.Contains(inSkillType), TEXT("Missing Configuration"));
	return Get()->BasicSkills[inSkillType];
}

TSubclassOf<UPDWGASAbility> UPDWGameSettings::GetEntitySkill(const FGameplayTag& inPupTag)
{
	ensureMsgf(Get()->EntityConfig.Contains(inPupTag),TEXT("Missing Configuration"));
	return Get()->EntityConfig[inPupTag].PupBaseSkill;
}

FGameplayTagContainer& UPDWGameSettings::GetEntitySkillTypeContainer(const FGameplayTag& inPupTag)
{
	ensureMsgf(Get()->EntityConfig.Contains(inPupTag),TEXT("Missing Configuration"));
	return Get()->EntityConfig[inPupTag].PupSkillType;
}

FGameplayTagContainer& UPDWGameSettings::GetEntityInteractionsContainer(const FGameplayTag& inPupTag)
{
	ensureMsgf(Get()->EntityConfig.Contains(inPupTag),TEXT("Missing Configuration"));
	return Get()->EntityConfig[inPupTag].PupInteractions;
}

const int32 UPDWGameSettings::GetAllPupTags(TArray<FGameplayTag>& outPupsTags)
{
	ensureMsgf(Get()->EntityConfig.IsEmpty(),TEXT("Entity Config is empty"));
	Get()->EntityConfig.GetKeys(outPupsTags);
	return Get()->EntityConfig.Num();
}

const FPupUIInfo& UPDWGameSettings::GetPupInfo(const FGameplayTag& inPupTag)
{
	ensureMsgf(Get()->EntityConfig.Contains(inPupTag),TEXT("Missing Configuration"));
	return Get()->EntityConfig[inPupTag].PupUIInfo;
}

const FPupCustomizationInfo& UPDWGameSettings::GetPupCustomizationInfo(const FGameplayTag& inPupTag)
{
	ensureMsgf(Get()->EntityConfig.Contains(inPupTag), TEXT("Missing Configuration"));
	return Get()->EntityConfig[inPupTag].CustomizationInfo;

}

const FName UPDWGameSettings::GetMiniGameComponentTag()
{
	ensureMsgf(!Get()->MiniGameComponentTag.IsNone(), TEXT("MiniGameComponentTag is not set in PDWGameSettings"));
	return Get()->MiniGameComponentTag;
}

const FName UPDWGameSettings::GetMiniGameStartintPositionTag()
{
	ensureMsgf(!Get()->MiniGameStartingPositionTag.IsNone(), TEXT("MiniGameStartingPositionTag is not set in PDWGameSettings"));
	return Get()->MiniGameStartingPositionTag;
}

const FName UPDWGameSettings::GetMiniGameBehaviourPreventTag()
{
	ensureMsgf(!Get()->MiniGameBehaviourPreventTag.IsNone(), TEXT("MiniGameBehaviourPreventTag is not set in PDWGameSettings"));
	return Get()->MiniGameBehaviourPreventTag;
}

const FName UPDWGameSettings::GetMiniGameMovingMeshTag()
{
	return Get()->MiniGameMovingMeshTag;
}

FCollisionProfileName UPDWGameSettings::GetInteractionReceiverProfileName()
{
	return Get()->InteractionReceiverProfileName;
}

FCollisionProfileName UPDWGameSettings::GetInteractionProfileName()
{
	return Get()->InteractionProfileName;
}

FCollisionProfileName UPDWGameSettings::GetAutoSwapAreaProfileName()
{
	return Get()->AutoSwapAreaProfileName;
}

FCollisionProfileName UPDWGameSettings::GetExclusionVolumeProfileName()
{
	return Get()->ExclusionVolumeProfileName;
}

FCollisionProfileName UPDWGameSettings::GetOutlineCollisionProfileName()
{
	return Get()->OutlineCollisionProfileName;
}

FCollisionProfileName UPDWGameSettings::GetAudioEnvironmentAreaProfileName()
{
	return Get()->AudioEnvironmentAreaProfileName;
}


