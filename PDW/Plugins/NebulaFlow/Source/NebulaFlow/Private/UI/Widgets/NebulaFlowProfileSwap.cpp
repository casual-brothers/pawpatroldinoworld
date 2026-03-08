// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/NebulaFlowProfileSwap.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

void UNebulaFlowProfileSwap::NativeConstruct()
{
	Super::NativeConstruct();
#if (PLATFORM_XBOXONE || PLATFORM_XSX)

    FString XboxActiveUser = "";
    FString GamerTag = "";
    FString TagSuffix = "";

    if (UNebulaFlowLocalPlayer* PlayerOwner = UNebulaFlowCoreFunctionLibrary::GetCurrentOwnerPlayer(this))
    {
        XboxActiveUser = FText::AsCultureInvariant(PlayerOwner->GetNickname()).ToString();
    }

    if (!XboxActiveUser.Split("#", &GamerTag, &TagSuffix, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
    {
        GamerTag = XboxActiveUser;
    }

    LabelGamerTag->SetText(FText::FromString(GamerTag));

    if (!TagSuffix.IsEmpty())
    {
        LabelTagSuffix->SetText(FText::FromString("#" + TagSuffix));
        LabelTagSuffix->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }

    PanelUser->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

#else

    PanelUser->SetVisibility(ESlateVisibility::Collapsed);

#endif
}