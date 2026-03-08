
#pragma once

#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "NebulaFlowDialogDataStructures.h"
#include "NebulaFlowDialog.generated.h"


class UNebulaFlowDialogButton;
class UTextBlock;
class UImage;
class ANebulaFlowPlayerController;
class UTexture2D;
class UWidgetAnimation;

UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowDialog : public UUserWidget
{

	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName OpenSoundID = "UI_PopUpOpen";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ResponsePositiveSoundID = "UI_PopUpVotePositive";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ResponseNegativeSoundID = "UI_PopUpVoteNegative";

	UNebulaFlowDialog(const FObjectInitializer& ObjectInitializer);

	void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SendDialogResponse(FString Response,const ANebulaFlowPlayerController* Sender = nullptr, int32 ResponsePriority = 0);

	virtual void InitDialog(APlayerController* InControllerOwner, FName DialogID, TFunction<void(FString)> InCallBack, const TArray<ANebulaFlowPlayerController*>& SyncControllers = {}, const TArray<FText>& Params = {}, UTexture2D* Image = nullptr, bool ibForceFocus = false, TObjectPtr<UInputMappingContext> InputMappingContext = {});

	void UninitDialog();

	void SetDialogText(FText inText);

	void SetAdditionImage(TSoftObjectPtr<UTexture2D> Image);

	UPROPERTY()
		UWidgetAnimation* IntroAnimation = nullptr;

protected:


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true))
	UTextBlock* DialogText{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true))
	UTextBlock* SecondaryDialogText {};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true))
	UPanelWidget* ButtonsContainer{};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true))
	UImage* AdditionalImage {};

	UPROPERTY(BlueprintReadOnly, Category = "Dialog")
	TMap<ANebulaFlowPlayerController*, FString> CurrentSyncResponses{};
		
	UPROPERTY(BlueprintReadOnly, Category = "Dialog")
	TMap<ANebulaFlowPlayerController*, int32> CurrentSyncResponsePriorities{};

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialog")
		void OnResponsesUpdated(const ANebulaFlowPlayerController* ActualPlayer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialog")
		void OnResponsed(const FString & Response);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialog")
		void ShowFormattedText(const FText &Message, const FText &Arg);
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;


private:

	FDialogConfiguration CachedDialogConfiguration;

	UPROPERTY()
	APlayerController* ControllerOwner{};

	TFunction<void(FString)> DialogCallBack;

	void RestoreFocus();

	bool bIsSyncDialog = false;

	bool bForceFocus = false;

	FString CheckSyncResponses();

	FText RetrieveDialogText(bool bIsMainText = true);

};