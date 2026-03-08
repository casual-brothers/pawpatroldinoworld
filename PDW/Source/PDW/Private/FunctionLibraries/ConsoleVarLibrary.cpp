// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "FunctionLibraries/ConsoleVarLibrary.h"
#include "HAL/IConsoleManager.h"

static IConsoleVariable* FindConsoleVar(const FName& Name)
{
    return IConsoleManager::Get().FindConsoleVariable(*Name.ToString());
}

// -------- GET ---------
float UConsoleVarLibrary::GetConsoleFloat(FName VarName)
{
    if (IConsoleVariable* CVar = FindConsoleVar(VarName))
        return CVar->GetFloat();
    return 0.f;
}

int32 UConsoleVarLibrary::GetConsoleInt(FName VarName)
{
    if (IConsoleVariable* CVar = FindConsoleVar(VarName))
        return CVar->GetInt();
    return 0;
}

FString UConsoleVarLibrary::GetConsoleString(FName VarName)
{
    if (IConsoleVariable* CVar = FindConsoleVar(VarName))
        return CVar->GetString();
    return TEXT("");
}

// -------- SET ---------
void UConsoleVarLibrary::SetConsoleFloat(FName VarName, float NewValue)
{
    if (IConsoleVariable* CVar = FindConsoleVar(VarName))
        CVar->Set(NewValue, ECVF_SetByCode);
}

void UConsoleVarLibrary::SetConsoleInt(FName VarName, int32 NewValue)
{
    if (IConsoleVariable* CVar = FindConsoleVar(VarName))
        CVar->Set(NewValue, ECVF_SetByCode);
}

void UConsoleVarLibrary::SetConsoleString(FName VarName, const FString& NewValue)
{
    if (IConsoleVariable* CVar = FindConsoleVar(VarName))
        CVar->Set(*NewValue, ECVF_SetByCode);
}
