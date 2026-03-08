// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/PDWRichTextBlockWidgetDecorator.h"
#include "UI/Widgets/NebulaFlowNavigationIcon.h"
#include "Components/RichTextBlock.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Framework/Text/ITextDecorator.h"
#include "Fonts/FontMeasure.h"
#include "Math/UnrealMathUtility.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Misc/DefaultValueHelper.h"
#include "EnhancedInput/Public/InputAction.h"


class SRichInlineNavIcon : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRichInlineNavIcon)
		{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UNebulaFlowNavigationIcon* InNavigationIconWidget, const FTextBlockStyle& TextStyle, TOptional<int32> Width, TOptional<int32> Height, FMargin Padding)
	{
		check(InNavigationIconWidget);
		const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		float IconHeight = FMath::Min((float)FontMeasure->GetMaxCharacterHeight(TextStyle.Font, 1.0f), InNavigationIconWidget->GetDesiredSize().Y);

		if (Height.IsSet())
		{
			IconHeight = static_cast<float>(Height.GetValue());
		}

		float IconWidth = IconHeight;
		if (Width.IsSet())
		{
			IconWidth = static_cast<float>(Width.GetValue());
		}

		ChildSlot
			[
				SNew(SBox)
					.HeightOverride(IconHeight)
					.WidthOverride(IconWidth)
					.Padding(Padding)
					[
						InNavigationIconWidget->TakeWidget()
					]
			];

	}
};

class FRichInlineNavIcon : public FRichTextDecorator
{

public:
	FRichInlineNavIcon(URichTextBlock* InOwner, UPDWRichTextBlockWidgetDecorator* InDecorator)
		: FRichTextDecorator(InOwner)
		, Decorator(InDecorator)
	{
	}

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		if (RunParseResult.Name == TEXT("navicon") && RunParseResult.MetaData.Contains(TEXT("id")))
		{
			return true;
		}
		return false;
	}


protected:
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const override
	{
		const bool bWarnIfMissing = true;

		TOptional<int32> Width;
		if (const FString* WidthString = RunInfo.MetaData.Find(TEXT("width")))
		{
			int32 WidthTemp;
			if (FDefaultValueHelper::ParseInt(*WidthString, WidthTemp))
			{
				Width = WidthTemp;
			}
		}

		TOptional<int32> Height;
		if (const FString* HeightString = RunInfo.MetaData.Find(TEXT("height")))
		{
			int32 HeightTemp;
			if (FDefaultValueHelper::ParseInt(*HeightString, HeightTemp))
			{
				Height = HeightTemp;
			}
		}

		if (!Decorator) return TSharedPtr<SWidget>();

		FName TagOrId = *RunInfo.MetaData[TEXT("id")];
		TSubclassOf<UNebulaFlowNavigationIcon> NavIconClass = Decorator->GetNavIconClass(TagOrId, bWarnIfMissing);
		if (!NavIconClass) return TSharedPtr<SWidget>();

		UNebulaFlowNavigationIcon* NavIconWidget = CreateWidget<UNebulaFlowNavigationIcon>(Decorator->GetWorld(), NavIconClass);
		if (!NavIconWidget) return TSharedPtr<SWidget>();

		const UInputAction* Action = Decorator->GetNavIconAction(TagOrId, bWarnIfMissing);
		NavIconWidget->InitNavigationIcon(Action);

		if (!Width.IsSet())
		{
			Width = Decorator->GetNavIconSize(TagOrId, bWarnIfMissing).X;
		}

		if (!Height.IsSet())
		{
			Height = Decorator->GetNavIconSize(TagOrId, bWarnIfMissing).Y;
		}

		FMargin Padding = Decorator->GetNavIconPadding(TagOrId, bWarnIfMissing);

		return SNew(SRichInlineNavIcon, NavIconWidget, TextStyle, Width, Height, Padding);
	}

private:
	UPDWRichTextBlockWidgetDecorator* Decorator;

};

UPDWRichTextBlockWidgetDecorator::UPDWRichTextBlockWidgetDecorator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSharedPtr<ITextDecorator> UPDWRichTextBlockWidgetDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShareable(new FRichInlineNavIcon(InOwner, this));
}

FRichNavigationIconRow* UPDWRichTextBlockWidgetDecorator::FindNavIconRow(FName TagOrId, bool bWarnIfMissing)
{
	if (!RichNavIconTable) return nullptr;

	FString ContextString;
	return RichNavIconTable->FindRow<FRichNavigationIconRow>(TagOrId, ContextString, bWarnIfMissing);
}

const TSubclassOf<UNebulaFlowNavigationIcon> UPDWRichTextBlockWidgetDecorator::GetNavIconClass(FName TagOrId, bool bWarnIfMissing)
{
	const FRichNavigationIconRow* NavIconRow = FindNavIconRow(TagOrId, bWarnIfMissing);
	if (!NavIconRow) return nullptr;

	return NavIconRow->NavIconClass;
}

const FVector2D UPDWRichTextBlockWidgetDecorator::GetNavIconSize(FName TagOrId, bool bWarnIfMissing)
{
	const FRichNavigationIconRow* NavIconRow = FindNavIconRow(TagOrId, bWarnIfMissing);
	if (!NavIconRow) return FVector2D(64.0f, 64.0f);

	return NavIconRow->DefaultSize;
}

const UInputAction* UPDWRichTextBlockWidgetDecorator::GetNavIconAction(FName TagOrId, bool bWarnIfMissing)
{
	const FRichNavigationIconRow* NavIconRow = FindNavIconRow(TagOrId, bWarnIfMissing);
	if (!NavIconRow) return nullptr;

	return NavIconRow->Action;
}

const FMargin UPDWRichTextBlockWidgetDecorator::GetNavIconPadding(FName TagOrId, bool bWarnIfMissing)
{
	const FRichNavigationIconRow* NavIconRow = FindNavIconRow(TagOrId, bWarnIfMissing);
	if (!NavIconRow) return FMargin(0.0f);

	return NavIconRow->Padding;
}
