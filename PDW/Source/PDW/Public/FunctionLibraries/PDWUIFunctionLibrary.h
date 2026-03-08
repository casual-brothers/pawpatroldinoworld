// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Managers/PDWDialogueSubSystem.h"
#include "Data/PDWGameplayStructures.h"
#include "PDWUIFunctionLibrary.generated.h"

class UPDWInputsData;

/**
 * 
 */
UCLASS()
class PDW_API UPDWUIFunctionLibrary : public UNebulaFlowUIFunctionLibrary
{
	GENERATED_BODY()

public:

	static UNebulaFlowDialog* ShowModal(UObject* WorldContextObject, FName ModalID, TFunction<void(FString)> InCallBack, APlayerController* OwnerController = nullptr, TArray<ANebulaFlowPlayerController*> SyncControllers = {}, TArray<FText> Params = {}, UTexture2D* Texture = nullptr);
	
	UFUNCTION(BlueprintCallable)
	static UNebulaFlowDialog* BP_ShowModal(UObject* WorldContextObject, FName ModalID);
	
	UFUNCTION(BlueprintCallable)
	static void HideModal(UObject* WorldContextObject, UNebulaFlowDialog* ModalToClose);

#if WITH_EDITOR
	UFUNCTION()
	static TArray<FName> GetAllDTSpeakersRows();
#endif

	UFUNCTION(BlueprintCallable)
	static FCharacterInfo GetCharacterInfoByTag(UObject* WorldContextObject, FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	static FCharacterInfo GetCharacterInfoByName(UObject* WorldContextObject, FName Id);

	UFUNCTION(BlueprintCallable)
	static UWidget* GetWidgetByName(const UUserWidget* WidgetToSearch, FName WidgetName);

	UFUNCTION()
	static TArray<FName> GetTrasitionOption();

	UFUNCTION(BlueprintCallable)
	static void SetPendingReview(UObject* WorldContextObject, FPDWUnlockableContainer UnlockableContainers);

	UFUNCTION(BlueprintCallable)
	static FString InsertSpacesBeforeUppercase(const FString& Input);
};
