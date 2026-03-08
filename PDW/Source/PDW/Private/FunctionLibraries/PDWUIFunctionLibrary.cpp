// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/PDWUIFunctionLibrary.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Managers/PDWEventSubsytem.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/PDWUIManager.h"
#include "Data/GameOptionsDeveloperSettings.h"
#include "Blueprint/UserWidget.h"
#include "Modes/PDWGameplayGameMode.h"
#include "Gameplay/FSMFlow/Helpers/PDWFsmHelper.h"

UNebulaFlowDialog* UPDWUIFunctionLibrary::ShowModal(UObject* WorldContextObject, FName ModalID, TFunction<void(FString)> InCallBack, APlayerController* OwnerController, TArray<ANebulaFlowPlayerController*> SyncControllers, TArray<FText> Params, UTexture2D* Texture)
{
	return ShowDialog(WorldContextObject, ModalID, InCallBack, OwnerController, SyncControllers, Params, Texture);
}

UNebulaFlowDialog* UPDWUIFunctionLibrary::BP_ShowModal(UObject* WorldContextObject, FName ModalID)
{
	return ShowModal(WorldContextObject, ModalID, [=](FString Response){
		UPDWEventSubsytem::Get(WorldContextObject)->OnModalResponseEvent(Response);
	});
}

void UPDWUIFunctionLibrary::HideModal(UObject* WorldContextObject, UNebulaFlowDialog* ModalToClose)
{
	HideDialog(WorldContextObject, ModalToClose);
}

#if WITH_EDITOR
TArray<FName> UPDWUIFunctionLibrary::GetAllDTSpeakersRows()
{
    TArray<FName> Options;
    UDataTable* Table = nullptr;
    UWorld* World = nullptr;

    Table = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/UI/DT_CharacterInfo.DT_CharacterInfo"));

    if (Table)
    {
        Options = Table->GetRowNames();
    }

    return Options;
}
#endif

FCharacterInfo UPDWUIFunctionLibrary::GetCharacterInfoByTag(UObject* WorldContextObject, FGameplayTag Tag)
{
    return GetCharacterInfoByName(WorldContextObject, Tag.GetTagLeafName());
}

FCharacterInfo UPDWUIFunctionLibrary::GetCharacterInfoByName(UObject* WorldContextObject, FName Id)
{
    UNebulaFlowUIManager* NebulaManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowUIManager(WorldContextObject);
    if (NebulaManager)
    {
        if (UPDWUIManager* UIManager = Cast<UPDWUIManager>(NebulaManager))
        {
            UDataTable* Table = UIManager->GetCharacterInfoDT();
            if (Table)
            {
                FCharacterInfoRow* Row = Table->FindRow<FCharacterInfoRow>(Id, "");
                if (Row)
                {
                    return Row->CharacterInfo;
                }
            }
        }
    }

    return FCharacterInfo();
}

UWidget* UPDWUIFunctionLibrary::GetWidgetByName(const UUserWidget* WidgetToSearch, FName WidgetName)
{
    return WidgetToSearch->GetWidgetFromName(WidgetName);
}

TArray<FName> UPDWUIFunctionLibrary::GetTrasitionOption()
{
	return TArray<FName>{"Transition In", "Transition Out"};
}

void UPDWUIFunctionLibrary::SetPendingReview(UObject* WorldContextObject, FPDWUnlockableContainer UnlockableContainers)
{
	APDWGameplayGameMode::Get(WorldContextObject)->FSMHelper->PendingRewards = UnlockableContainers;
}

FString UPDWUIFunctionLibrary::InsertSpacesBeforeUppercase(const FString& Input)
{
    FString Result;
    Result.Reserve(Input.Len() * 2);

    for (int32 Index = 0; Index < Input.Len(); ++Index)
    {
        const TCHAR Char = Input[Index];

        if (Index > 0 && FChar::IsUpper(Char))
        {
            Result.AppendChar(TEXT(' '));
        }

        Result.AppendChar(Char);
    }

    return Result;
}
