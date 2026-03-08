// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "Blueprint/UserWidget.h"
#include "UI/NebulaFlowBasePage.h"
#include "NebulaFlowBaseNavbar.generated.h"

class UPanelWidget;
class UNebulaFlowNavbarButton;
class UNebulaFlowProfileSwap;
class UTextBlock;


USTRUCT(BlueprintType)
struct FNavButtonPosition
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(VisibleAnywhere)
	ENavElementPosition NavbarPosition{ ENavElementPosition::CENTER };

	UPROPERTY(VisibleAnywhere)
	int32 Index{ 0 };

	FNavButtonPosition() {};

	FNavButtonPosition(ENavElementPosition In_Position , int32 In_Index)
	{
		NavbarPosition = In_Position;
		Index = In_Index;
	}
};

UCLASS()
class NEBULAFLOW_API UNebulaFlowBaseNavbar : public UUserWidget
{

	GENERATED_BODY()

public:

	UNebulaFlowBaseNavbar(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UNebulaFlowProfileSwap> ProfileSwapClass{ nullptr };

	void DefineNavbarButtons(TMap<FName, ENavElementPosition> InNavbarConfig);
	void AddButtonsToNavbarWidget();
	void DefineNavbarLabel(FText InText);


	UFUNCTION(BlueprintCallable, Category = "UI")
	void AddNavbarButton(const FName& ButtonName, ENavElementPosition Position);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void RemoveNavbarButton(const FName& ButtonName);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void EnableButton(FName& ButtonName);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void DisableButton(FName& ButtonName);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetupSwapProfileLayout();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget, OptionalWidget = true))
	TObjectPtr<UPanelWidget> ProfileSwapContainer{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget, OptionalWidget = true))
	TObjectPtr<UPanelWidget> NavbarLeftContainer{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget, OptionalWidget = true))
	TObjectPtr<UPanelWidget> NavbarRightContainer{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget, OptionalWidget = true))
	TObjectPtr<UPanelWidget> NavbarCenterContainer{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget, OptionalWidget = true))
	TObjectPtr<UTextBlock> NavbarTextblock{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TMap<FName, FNavButtonPosition> NavbarButtons{};
	
	void ResetNavbar();

	UNebulaFlowNavbarButton* GetButtonByName(FName& ButtonName);
};
