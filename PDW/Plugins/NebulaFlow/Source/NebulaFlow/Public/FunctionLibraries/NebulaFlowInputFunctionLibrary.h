// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/Object.h"
#include "NebulaFlowInputFunctionLibrary.generated.h"

USTRUCT(BlueprintType)
struct NEBULAFLOW_API FRemappableAction
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName ActionName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName RelatedAction = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FText ActionText {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bIsAxis = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float AxisValue = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<FKey> Inputs{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bIsPCOnly = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName Context = NAME_None;
};

UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UDefaultRemappableActions : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
		TArray<FRemappableAction> Actions{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
		TArray<FKey> NotRemappableKeys{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
		FKey JapanesAcceptKey{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
		FKey JapanesBackKey {};
};

UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UJapaneseInputSetup : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
		FKey JapanesAcceptKey {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
		FKey JapanesBackKey {};
};

UCLASS()
class NEBULAFLOW_API UNebulaFlowInputFunctionLibrary :  public UBlueprintFunctionLibrary
{

	GENERATED_BODY()

public:

	
	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static bool RemapActionToKey(const UObject* WorldContextObject,FName ActionContext, FName ActionName, FKey NewKey, bool IsPadAssignment = false);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static void ApplyAssignmentToInputs(const UObject* WorldContextObject, bool bIsPadAssignment = false);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static void ApplyDefaultInputs(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static bool IsKeyRemappable(const UObject* WorldContextObject, FKey Key);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static void SetupPS4JapaneseInputs(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static void SetMouseInputEnabled(const UObject* WorldContextObject, bool NewValue);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static bool IsUsingEnhancedInput(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static void AddMappingInput(const UObject* WorldContextObject, UInputMappingContext* InContext, int InPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static void RemoveMappingInput(const UObject* WorldContextObject, UInputMappingContext* InContext);
private:

	static void RemoveActionAssignment(const UObject* WorldContextObject,FName ActionName, FKey Key);

	static void AddActionAssignment(const UObject* WorldContextObject, FName ActionName, FKey Key);

	static FName FindCurrentActionFromKey(const UObject* WorldContextObject, FKey Key, FName ActionContext);

	static TArray<FKey> FindCurrentKeyFromAction(const UObject* WorldContextObject, FName ActionName, FName ActionContext, bool& bKeyFound, bool IsPad = false);

	static void CleanInputs(const UObject* WorldContextObject, bool bIsPadAssignment);

};