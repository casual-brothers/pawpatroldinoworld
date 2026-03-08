// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Decorators/NebulaFlowNavIconDecorator.h"
#include "UI/Widgets/NebulaFlowNavigationIcon.h"
#include "Framework/Text/ITextDecorator.h"
#include "Fonts/FontMeasure.h"
#include "EnhancedInput/Public/InputAction.h"
#include "Misc/DefaultValueHelper.h"


class SNebulaRichInlineNavIcon : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SNebulaRichInlineNavIcon)
		{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UNebulaFlowNavigationIcon* NavigationIconWidget, const FTextBlockStyle& TextStyle, TOptional<int32> Width, TOptional<int32> Height, FMargin Padding)
	{
		check(NavigationIconWidget);
		const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		float IconHeight = FMath::Min((float)FontMeasure->GetMaxCharacterHeight(TextStyle.Font, 1.0f), NavigationIconWidget->GetDesiredSize().Y);

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
						NavigationIconWidget->TakeWidget()
					]
			];

	}
};

class FNebulaRichInlineNavIcon : public FRichTextDecorator
{

public:
	FNebulaRichInlineNavIcon(URichTextBlock* InOwner, UNebulaFlowNavIconDecorator* InDecorator)
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

		return SNew(SNebulaRichInlineNavIcon, NavIconWidget, TextStyle, Width, Height, Padding);
	}

private:
	UNebulaFlowNavIconDecorator* Decorator;

};

UNebulaFlowNavIconDecorator::UNebulaFlowNavIconDecorator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSharedPtr<ITextDecorator> UNebulaFlowNavIconDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShareable(new FNebulaRichInlineNavIcon(InOwner, this));
}

FNebulaRichNavigationIconRow* UNebulaFlowNavIconDecorator::FindNavIconRow(FName TagOrId, bool bWarnIfMissing)
{
	if (!RichNavIconTable) return nullptr;

	FString ContextString;
	return RichNavIconTable->FindRow<FNebulaRichNavigationIconRow>(TagOrId, ContextString, bWarnIfMissing);
}

const TSubclassOf<UNebulaFlowNavigationIcon> UNebulaFlowNavIconDecorator::GetNavIconClass(FName TagOrId, bool bWarnIfMissing)
{
	const FNebulaRichNavigationIconRow* NavIconRow = FindNavIconRow(TagOrId, bWarnIfMissing);
	if (!NavIconRow) return nullptr;

	return NavIconRow->NavIconClass;
}

const FVector2D UNebulaFlowNavIconDecorator::GetNavIconSize(FName TagOrId, bool bWarnIfMissing)
{
	const FNebulaRichNavigationIconRow* NavIconRow = FindNavIconRow(TagOrId, bWarnIfMissing);
	if (!NavIconRow) return FVector2D(64.0f, 64.0f);

	return NavIconRow->DefaultSize;
}

const UInputAction* UNebulaFlowNavIconDecorator::GetNavIconAction(FName TagOrId, bool bWarnIfMissing)
{
	const FNebulaRichNavigationIconRow* NavIconRow = FindNavIconRow(TagOrId, bWarnIfMissing);
	if (!NavIconRow) return nullptr;

	return NavIconRow->Action;
}

const FMargin UNebulaFlowNavIconDecorator::GetNavIconPadding(FName TagOrId, bool bWarnIfMissing)
{
	const FNebulaRichNavigationIconRow* NavIconRow = FindNavIconRow(TagOrId, bWarnIfMissing);
	if (!NavIconRow) return FMargin(0.0f);

	return NavIconRow->Padding;
}
