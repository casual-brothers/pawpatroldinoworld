// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once



#include "Engine/StreamableManager.h"
#include "Blueprint/UserWidget.h"
#include "NebulaFlowNavigationIcon.generated.h"

class UTexture;
class UTextBlock;
class UImage;
class UOverlay;
class UInputAction;
class UNebulaFlowUIConstants;


UCLASS()
class NEBULAFLOW_API UNebulaFlowNavigationIcon : public UUserWidget
{
	
	GENERATED_BODY()

public:
	
	UNebulaFlowNavigationIcon(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void InitNavigationIcon(FName InActionName, bool bInIsAxis = false, float InScale = 0.0f);

	//UFUNCTION(BlueprintCallable)
	void InitNavigationIcon(const UInputAction* InAction, bool bInIsAxis = false, float InScale = 0.0f , FName OverridedActionName = FName());

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	bool bIsFirstPlayerOnly = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	bool bUseMulticonIfPresent = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly ,Category = "UI")
	FName ActionName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly ,Category = "UI")
	FName OverridedActionName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	bool bIsAxis = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float AxisScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FName MaterialTextureParameterName = "Texture";

	void UpdateOwnerPlayerController(APlayerController* inController, bool bForceUpdate = false);

	virtual void UpdateImage();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	UImage* NavigationImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	UTextBlock* NavigationText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	UOverlay* NavigationOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	UTexture2D* BaseTexture;

	//Even if the key is not found, the text is left visible, with the last key recorded
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	bool bShowTextOnKeyNotFound = false;

	UPROPERTY()
	UNebulaFlowUIConstants* UIConstants{};

	UFUNCTION()
	virtual void SetNavigationImageTexture(UTexture2D* Texture);

	void OnInputTypeSwitched(const bool bInIsUsingGamepad);

	void OnAnyKeyPressed(FKey Key, bool bIsGamepad, APlayerController* Sender);

	FString GetIconPath(FString& OutKeyName, bool bSearchMultiIconPath = false);

	bool bFirstSynchronize = false;

	bool bIsUsingGamePad = true;

	UPROPERTY()
	UTexture2D* CurrentTexture;

	FDelegateHandle InputHandler;
	FDelegateHandle RemapInputHandler;

	//override Logics

	void CheckOverriddenIcon(FName ActionName);

	FString OverriddenKeyboardIcon = FString("");
	FString OverriddenPadIcon = FString("");
	bool bIsOverriddenIcon = false;

	void Init_Internal();

	UFUNCTION()
	void OnRebuildInputs();
};