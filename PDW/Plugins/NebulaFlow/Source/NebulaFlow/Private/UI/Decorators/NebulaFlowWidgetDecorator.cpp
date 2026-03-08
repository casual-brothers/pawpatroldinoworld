// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Decorators/NebulaFlowWidgetDecorator.h"
#include "Framework/Text/ITextDecorator.h"
#include "Fonts/FontMeasure.h"
#include "Misc/DefaultValueHelper.h"
#include "Blueprint/UserWidget.h"

class SNebulaRichInlineWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SNebulaRichInlineWidget)
		{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UUserWidget* InUserWidget, const FTextBlockStyle& TextStyle, TOptional<int32> Width, TOptional<int32> Height, FMargin Padding)
	{
		check(InUserWidget);
		const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		float IconHeight = FMath::Min((float)FontMeasure->GetMaxCharacterHeight(TextStyle.Font, 1.0f), InUserWidget->GetDesiredSize().Y);

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
						InUserWidget->TakeWidget()
					]
			];

	}
};

class FNebulaRichInlineWidget : public FRichTextDecorator
{

public:
	FNebulaRichInlineWidget(URichTextBlock* InOwner, UNebulaFlowWidgetDecorator* InDecorator)
		: FRichTextDecorator(InOwner)
		, Decorator(InDecorator)
	{
	}

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		if (RunParseResult.Name == TEXT("widget") && RunParseResult.MetaData.Contains(TEXT("id")))
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
		TSubclassOf<UUserWidget> WidgetClass = Decorator->GetWidgetClass(TagOrId, bWarnIfMissing);
		if (!WidgetClass) return TSharedPtr<SWidget>();

		UUserWidget* NewWidget = CreateWidget<UUserWidget>(Decorator->GetWorld(), WidgetClass);
		if (!NewWidget) return TSharedPtr<SWidget>();

		if (!Width.IsSet())
		{
			Width = Decorator->GetWidgetSize(TagOrId, bWarnIfMissing).X;
		}

		if (!Height.IsSet())
		{
			Height = Decorator->GetWidgetSize(TagOrId, bWarnIfMissing).Y;
		}

		FMargin Padding = Decorator->GetWidgetPadding(TagOrId, bWarnIfMissing);

		return SNew(SNebulaRichInlineWidget, NewWidget, TextStyle, Width, Height, Padding);

	}

private:
	UNebulaFlowWidgetDecorator* Decorator;

};

UNebulaFlowWidgetDecorator::UNebulaFlowWidgetDecorator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSharedPtr<ITextDecorator> UNebulaFlowWidgetDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShareable(new FNebulaRichInlineWidget(InOwner, this));
}

FNebulaRichWidgetRow* UNebulaFlowWidgetDecorator::FindWidgetRow(FName TagOrId, bool bWarnIfMissing)
{
	if (!RichWidgetTable) return nullptr;

	FString ContextString;
	return RichWidgetTable->FindRow<FNebulaRichWidgetRow>(TagOrId, ContextString, bWarnIfMissing);
}

const TSubclassOf<UUserWidget> UNebulaFlowWidgetDecorator::GetWidgetClass(FName TagOrId, bool bWarnIfMissing)
{
	const FNebulaRichWidgetRow* WidgetRow = FindWidgetRow(TagOrId, bWarnIfMissing);
	if (!WidgetRow) return nullptr;

	return WidgetRow->WidgetClass;
}

const FVector2D UNebulaFlowWidgetDecorator::GetWidgetSize(FName TagOrId, bool bWarnIfMissing)
{
	const FNebulaRichWidgetRow* WidgetRow = FindWidgetRow(TagOrId, bWarnIfMissing);
	if (!WidgetRow) return FVector2D(64.0f, 64.0f);

	return WidgetRow->DefaultSize;
}

const FMargin UNebulaFlowWidgetDecorator::GetWidgetPadding(FName TagOrId, bool bWarnIfMissing)
{
	const FNebulaRichWidgetRow* WidgetRow = FindWidgetRow(TagOrId, bWarnIfMissing);
	if (!WidgetRow) return FMargin(0.0f);

	return WidgetRow->Padding;
}
