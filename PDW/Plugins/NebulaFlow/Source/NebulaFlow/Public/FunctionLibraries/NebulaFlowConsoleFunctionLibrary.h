#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "NebulaFlowConsoleFunctionLibrary.generated.h"

UCLASS()
class NEBULAFLOW_API UNebulaFlowConsoleFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/* WINDOWS */

	/* XBOX */

	/* XSX */

	/* PS4 */

	/* PS5 */

	/* SWITCH */

	static bool bIsJoyConHorizontal;
		
	static void SetIsJoyconHorizontal();

	UFUNCTION(BlueprintPure, Category = "NebulaFlowConsoleLibrary|Switch")
	static bool IsJoyconHorizontal(const int32 InID);

	UFUNCTION(BlueprintPure, Category = "NebulaFlowConsoleLibrary|Switch")
	static bool IsJoyconDualGrip(const int32 InID);

	UFUNCTION(BlueprintPure, Category = "NebulaFlowConsoleLibrary|Switch")
	static bool IsJoyconHandheld(const int32 InID);

	UFUNCTION(BlueprintPure, Category = "NebulaFlowConsoleLibrary|Switch")
	static bool IsJoyconLeft(const int32 InID);
	
	UFUNCTION(BlueprintPure, Category = "NebulaFlowConsoleLibrary|Switch")
	static bool IsJoyconRight(const int32 InID);

	UFUNCTION(BlueprintPure, Category = "NebulaFlowConsoleLibrary|Switch")
	static bool IsSwitchProController(const int32 InID);

	UFUNCTION(BlueprintCallable, Category = "NebulaFlowConsoleLibrary|Switch")
	static void ShowRemapJoycons();

	UFUNCTION(BlueprintCallable, Category = "NebulaFlowConsoleLibrary|Switch")
	static void SetEnableSingleJoycon(const bool InIsEnable);
	
	UFUNCTION(BlueprintCallable, Category = "NebulaFlowConsoleLibrary|Switch")
	static void SetSwitchMinControllers(const int32 InNum);

	UFUNCTION(BlueprintCallable, Category = "NebulaFlowConsoleLibrary|Switch")
	static void SetSwitchMaxControllers(const int32 InNum);

	UFUNCTION(BlueprintCallable, Category = "NebulaFlowConsoleLibrary|Switch")
	static void ResetHeartBeatDurationMultiplier();

	UFUNCTION(BlueprintPure, Category = "NebulaFlowConsoleLibrary|Switch")
	static bool IsJoyconConnected(const int32 InID);
};