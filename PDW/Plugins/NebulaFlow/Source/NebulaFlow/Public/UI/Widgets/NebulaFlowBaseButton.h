// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once



#include "NebulaFlowBaseWidget.h"
#include "Input/NavigationReply.h"
#include "Components/Button.h"
#include "Engine/LocalPlayer.h"
#include "NebulaFlowBaseButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonFocused, UNebulaFlowBaseButton*, BaseButton);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonUnFocused, UNebulaFlowBaseButton*, BaseButton);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonClick, UNebulaFlowBaseButton*, BaseButton);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserButtonClick, UNebulaFlowBaseButton*, BaseButton, int32, UserIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonHovered, UNebulaFlowBaseButton*, BaseButton);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonUnHovered, UNebulaFlowBaseButton*, BaseButton);

UCLASS()
class NEBULAFLOW_API UNebulaFlowBaseButton : public UNebulaFlowBaseWidget
{
	
	GENERATED_BODY()

public:
	
	UNebulaFlowBaseButton(const FObjectInitializer& ObjectInitializer);

	////////////////// DELEGATES /////////////////////

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Button Delegates")
		FOnButtonFocused ButtonFocused;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Button Delegates")
		FOnButtonUnFocused ButtonUnFocused;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Button Delegates")
		FOnButtonClick ButtonClicked;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Button Delegates")
		FOnUserButtonClick UserButtonClicked;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Button Delegates")
		FOnButtonHovered ButtonHovered;
		
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Button Delegates")
		FOnButtonUnHovered ButtonUnHovered;


	////////////////// PLATFORM VISIBILITY /////////////////////

	UPROPERTY(EditAnywhere, Category = "Platforms")
		bool HideOnPC = false;

	UPROPERTY(EditAnywhere, Category = "Platforms")
		bool HideOnXBox = false;

	UPROPERTY(EditAnywhere, Category = "Platforms")
		bool HideOnPS5 = false;

	UPROPERTY(EditAnywhere, Category = "Platforms")
		bool HideOnSwitch = false;

	UPROPERTY(EditAnywhere, Category = "Platforms")
		bool CollapseOnPC = false;

	UPROPERTY(EditAnywhere, Category = "Platforms")
		bool CollapseOnXBox = false;

	UPROPERTY(EditAnywhere, Category = "Platforms")
		bool CollapseOnPS5 = false;

	UPROPERTY(EditAnywhere, Category = "Platforms")
		bool CollapseOnSwitch = false;

	UFUNCTION()
		void UpdatePlatformButtonVisibility();

	/////////////// BUTTON PROPERTIES //////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
		FString ButtonParameter = FString("");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
		FString ButtonActionClicked = FString("ButtonClicked");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
		FString ButtonActionFocus = FString("ButtonFocused");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Properties")
		bool IsButtonStartFocusable = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Properties")
		bool IsButtonStartHoverable = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Properties")
		bool bIsLocked = false;

	UFUNCTION()
		bool GetButtonIsFocusable() { return Button->GetIsFocusable(); };
		
	UFUNCTION()
		void SetButtonIsHoverable(bool IsButtonHoverable) { bIsButtonHoverable = IsButtonHoverable; };

	UFUNCTION()
		bool GetButtonIsHoverable() { return bIsButtonHoverable; };

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* Button = nullptr;

	UPROPERTY(Transient)
		bool bIsButtonHoverable = false;

	UFUNCTION()
		virtual void OnUserButtonClicked(const int32 UserIndex);

	UFUNCTION()
		virtual void OnButtonClicked();

	UFUNCTION()
		virtual void OnButtonFocused();

	UFUNCTION()
		virtual void OnButtonUnFocused();

	UFUNCTION()
		virtual void OnButtonHovered();

	UFUNCTION()
		virtual void OnButtonUnHovered();

	virtual void NativeConstruct()override;
	virtual void NativeDestruct()override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
};
