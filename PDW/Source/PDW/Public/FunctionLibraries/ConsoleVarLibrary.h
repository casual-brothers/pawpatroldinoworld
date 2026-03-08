// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ConsoleVarLibrary.generated.h"

UCLASS()
class UConsoleVarLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    // -------- GET ---------
    UFUNCTION(BlueprintPure, Category = "Console Variables")
    static float GetConsoleFloat(FName VarName);

    UFUNCTION(BlueprintPure, Category = "Console Variables")
    static int32 GetConsoleInt(FName VarName);

    UFUNCTION(BlueprintPure, Category = "Console Variables")
    static FString GetConsoleString(FName VarName);

    // -------- SET ---------
    UFUNCTION(BlueprintCallable, Category = "Console Variables")
    static void SetConsoleFloat(FName VarName, float NewValue);

    UFUNCTION(BlueprintCallable, Category = "Console Variables")
    static void SetConsoleInt(FName VarName, int32 NewValue);

    UFUNCTION(BlueprintCallable, Category = "Console Variables")
    static void SetConsoleString(FName VarName, const FString& NewValue);
};
