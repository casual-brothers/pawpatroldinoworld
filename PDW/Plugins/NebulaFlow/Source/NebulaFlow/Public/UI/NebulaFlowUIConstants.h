
#pragma once

#include "UI/NebulaFlowUIDelegates.h"
#include "Engine/DataAsset.h"
#include "NebulaFlowNavbarDataStructures.h"
#include "NebulaFlowUIConstants.generated.h"

class UDataTable;
class UNebulaFlowNavigationIcons;
class UNebulaFlowDialog;
class UNebulaFlowLoadingPage;
class UNebulaFlowSaveIcon;

UENUM(BlueprintType)
enum class EUILayers : uint8
{
	EBackGround,
	EPage,
	EOverlay,
	EGameDialog,
	ESystemDialog,
	ELoading,
	ESaveGameIcon
};


UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowUIConstants : public UDataAsset
{
	GENERATED_BODY()

public:

	UNebulaFlowUIConstants(const FObjectInitializer& ObjectInitializer);

	const UDataTable* GetNavbarButtonsTable() const {return NavbarButtonsTable; }

	const UDataTable* GetDialogConfigurationsTable() const { return DialogConfigurationsTable; }

	const UDataTable* GetUIAudioTable() const { return UIAudioTable; }

	const UNebulaFlowNavigationIconsAsset* GetNavigationIconDataAssets() const {return  NavigationIconsDataAsset;}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI")
	TMap<EUILayers,int> UILayerZOrderMap{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI")
	TSubclassOf<UNebulaFlowSaveIcon> SaveIconWidgetClass{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI|Loading")
	bool bUseAsyncLoadingPage = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI|Loading")
	TSubclassOf<UNebulaFlowLoadingPage> DefaultLoadingPageClass{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI|Loading")
	TMap<FName,TSubclassOf<UNebulaFlowLoadingPage>> MapToLoadingPage{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI|Mapping")
		UInputMappingContext* UIInputMapping{};
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI|Dialog")
	//TSubclassOf<UNebulaFlowDialog> GameDialogClass{};

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI|Dialog")
	//TSubclassOf<UNebulaFlowDialog> SyncGameDialogClass{};

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI|Dialog")
	//TSubclassOf<UNebulaFlowDialog> SystemDialogClass{};

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI|Dialog")
	//TSubclassOf<UNebulaFlowDialog> PopupClass{};

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI")
	UDataTable* NavbarButtonsTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI|Dialog")
	UDataTable* DialogConfigurationsTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI|Dialog")
	UDataTable* UIAudioTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FLOW|UI")
	UNebulaFlowNavigationIconsAsset* NavigationIconsDataAsset = nullptr;

};

