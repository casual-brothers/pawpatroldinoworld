// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/CLDebugWidget.h"

#include "Components/TextBlock.h"
#include "BuildPreprocess.h"
#include "../PDW.h"

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

#define TOSTRING(x) TEXT(STRINGIFY(x))


void UCLDebugWidget::ShowCL()
{
	#ifdef BUILD_TIMESTAMP
    FText InternalUseText = FText::FromString(TEXT("INTERNAL USE"));

    // Turn raw macros into proper FText
    FText ProjectNameText = FText::FromString(TOSTRING(PROJECT_NAME));
    FText BuildDataText = FText::FromString(TOSTRING(BUILD_DATA));
    FText TimestampText = FText::FromString(TOSTRING(BUILD_TIMESTAMP));

    FFormatNamedArguments Args;
    Args.Add(TEXT("name"), ProjectNameText);
    Args.Add(TEXT("timestamp"), TimestampText);
    Args.Add(TEXT("infodatatext"), BuildDataText);
    Args.Add(TEXT("disclaimer"), InternalUseText);

    FText Pattern = FText::FromString(TEXT("{name} {timestamp} {infodatatext} {disclaimer}"));
    FText FinalText = FText::Format(Pattern, Args);

    if (ClLabel)
    {
        ClLabel->SetText(FinalText);
        ClLabel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }
#endif
}

void UCLDebugWidget::HideCL()
{
	ClLabel->SetVisibility(ESlateVisibility::Collapsed);
}

void UCLDebugWidget::NativeConstruct()
{

	Super::NativeConstruct();

#ifdef CLWIDGETVISIBILITY
	ShowCL();
#else
	HideCL();
#endif

}
