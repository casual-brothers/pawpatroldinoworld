// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/NebulaFlowBasePage.h"
#include "UI/NebulaFlowDialog.h"
#include "GameFramework/PlayerController.h"
#include "NebulaFlowUIFunctionLibrary.generated.h"

class UNebulaFlowBaseUIFSMState;
class UNebulaFlowBasePage;
class UNebulaFlowUIConstants;
class ANebulaFlowPlayerController;
class UTexture2D;

UCLASS()
class NEBULAFLOW_API UNebulaFlowUIFunctionLibrary :  public UBlueprintFunctionLibrary
{

	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static UNebulaFlowBasePage* CreateNewPage(UObject* WorldContextObject, TSubclassOf<UNebulaFlowBasePage> PageClass , UNebulaFlowBaseFSMState* StateOwner, ANebulaFlowPlayerController* OwnerPlayerController = nullptr, EAddToScreenType AddToScreenType = EAddToScreenType::EAddToViewPort);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static void RemovePage(UObject* WorldContextObject, UNebulaFlowBasePage* InPageToRemove);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static UNebulaFlowUIConstants* GetUIConstants(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static UNebulaFlowBasePage* GetCurrentPage(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static FString GetInputIconPath(UObject* WorldContextObject,FString IconName, bool bIsUsingKeyboard = false , APlayerController* Owner = nullptr);
	
	UFUNCTION()
	static void SetFocusToGameViewport(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static bool IsGameArabic(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "UI", meta = (WorldContext = "WorldContextObject"))
	static void SetUserFocus(UObject* WorldContextObject , UWidget* InFocusWidget, APlayerController* PlayerController = nullptr, bool bPlaySound = true);	

	static UNebulaFlowDialog* ShowDialog(UObject* WorldContextObject, FName DialogID, TFunction<void(FString)> InCallBack, APlayerController* OwnerController = nullptr, TArray<ANebulaFlowPlayerController*> SyncControllers = {}, TArray<FText> Params = {}, UTexture2D* Image = nullptr);

	static bool HideDialog(UObject* WorldContextObject, UNebulaFlowDialog* DialogToClose);

	static bool HideGameDialog(UObject* WorldContextObject);

	static bool HideSystemDialog(UObject* WorldContextObject);

};