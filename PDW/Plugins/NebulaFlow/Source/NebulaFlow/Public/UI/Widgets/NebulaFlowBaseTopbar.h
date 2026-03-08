// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/NebulaFlowBasePage.h"
#include "NebulaFlowBaseTopbar.generated.h"

class UNebulaFlowNavbarButton;
class UTextBlock;

UCLASS()
class NEBULAFLOW_API UNebulaFlowBaseTopbar : public UUserWidget
{
	GENERATED_BODY()
	
public:
		
	UNebulaFlowBaseTopbar(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void SetTitle(FText InTitle);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void SetTitlePosition(ENavElementPosition InPosition);

	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void InitializeTopBar();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowUserName(bool bShow);

	virtual int32 NavigateTo(int32 InDestinationIndex);

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget, OptionalWidget = true))
	UTextBlock* TopbarTitle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget, OptionalWidget = true))
	UTextBlock* TopbarUserLabel;

	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta = (BindWidget, OptionalWidget = true))
	UNebulaFlowNavbarButton* LeftNavButton;

	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta = (BindWidget, OptionalWidget = true))
	UNebulaFlowNavbarButton* RightNavButton;

	UPROPERTY(EditAnywhere, Category = "UI|Topbar settings")
	bool bEnableNavigation = false;

	UPROPERTY(EditAnywhere, Category = "UI|Topbar settings")
	bool bHideLabelWhenNavigationEnabled = true;
};
