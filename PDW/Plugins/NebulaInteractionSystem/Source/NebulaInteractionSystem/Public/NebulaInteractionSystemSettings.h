#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "NebulaInteractionSystemSettings.generated.h"

class UNebulaInteractionBehaviour;

UCLASS(Config = NebulaInteractionSystem, defaultconfig)
class NEBULAINTERACTIONSYSTEM_API UNebulaInteractionSystemSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

	static UNebulaInteractionSystemSettings* Get();

public:
		
	UFUNCTION(BlueprintPure)
	static FGameplayTag& GetInteractionActionTag();

	UFUNCTION(BlueprintPure)
	static FCollisionProfileName GetInteractionCollisionProfileName();

	UFUNCTION(BlueprintPure)
	static FCollisionProfileName GetInteractionReceiverCollisionProfileName();

	UPROPERTY(Config, EditAnywhere, Category = "Definitions")
	TSubclassOf<UNebulaInteractionBehaviour> DefaultInteractionBehaviourClass = nullptr;

protected:

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Tags ")
	FGameplayTag DefaultInteractionActionTag = FGameplayTag::EmptyTag;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Collision Profiles ")
	FCollisionProfileName InteractionCollisionProfileName;

	UPROPERTY(Config, EditAnywhere, Category = "Definitions | Collision Profiles ")
	FCollisionProfileName InteractionReceiverCollisionProfileName;
};