#pragma once

#include "Engine/DataTable.h"
#include "FMODEvent.h"
#include "NebulaFlowWidgetDataStructures.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct NEBULAFLOW_API FPlatformVisibility
{
	GENERATED_USTRUCT_BODY()

public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Info")
		bool HideOnPc = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Info")
		bool HideOnXBox = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Info")
		bool HideOnPS = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Info")
		bool HideOnSwitch = false;
};


USTRUCT(Blueprintable, BlueprintType)
struct NEBULAFLOW_API FAudioEventsTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:


	UPROPERTY(EditAnywhere, Category = "UI")
		UFMODEvent* AudioEvent = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
		FString ProgrammerName = FString("");

	UPROPERTY(EditAnywhere, Category = "UI")
		FString ParameterName = FString("");

	UPROPERTY(EditAnywhere, Category = "UI")
		float ParameterValue = 0.f;

	UPROPERTY(EditAnywhere, Category = "UI")
		FString ParameterNameB = FString("");

	UPROPERTY(EditAnywhere, Category = "UI")
		float ParameterValueB = 0.f;
};

USTRUCT(Blueprintable, BlueprintType)
struct NEBULAFLOW_API FUIAudioEvents
{
	GENERATED_USTRUCT_BODY()

public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Audio")
		FName OnFocus {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Audio")
		FName OnFocusVoiceOver {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Audio")
		FName OnPressed {};


};