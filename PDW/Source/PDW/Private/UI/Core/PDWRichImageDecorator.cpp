// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/PDWRichImageDecorator.h"

#include "Framework/Text/ITextDecorator.h"
#include "Fonts/FontMeasure.h"
#include "EnhancedInput/Public/InputAction.h"
#include "Misc/DefaultValueHelper.h"
#include "Widgets/SCanvas.h"

class SPDWRichInLineWidget : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SPDWRichInLineWidget)
        {}
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs, const FSlateBrush* Brush, const FTextBlockStyle& TextStyle, TOptional<int32> Width, TOptional<int32> Height, const FPDWRichTextImageRow* rawData)//FMargin Padding, FVector2D GlyphSize, FVector2D ImagePosition)
    {
        check(Brush);
        const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
        float IconHeight = FMath::Min((float)FontMeasure->GetMaxCharacterHeight(TextStyle.Font, 1.0f), Brush->ImageSize.Y);

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
                    .HeightOverride(rawData->GlyphSize.X)
                    .WidthOverride(rawData->GlyphSize.Y)
                    .Padding(rawData->Padding)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SCanvas)
                        + SCanvas::Slot()
                        .Size(FVector2D(IconWidth,IconHeight))
                        .Position(rawData->ImagePosition)
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Center)
                        [
                            SNew(SImage)
                            .Image(Brush)
                        ]
                    ]
            ];

    }
};

class FPDWRichInlineWidget : public FRichTextDecorator
{

public:
    FPDWRichInlineWidget(URichTextBlock* InOwner, UPDWRichImageDecorator* InDecorator)
        : FRichTextDecorator(InOwner)
        , Decorator(InDecorator)
    {
    }

    virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
    {
        if (RunParseResult.Name == TEXT("image") && RunParseResult.MetaData.Contains(TEXT("id")))
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
        const FSlateBrush* Brush = Decorator->GetImageSlateBrush(TagOrId, bWarnIfMissing);
        if (!Brush) return TSharedPtr<SWidget>();

        if (!Width.IsSet())
        {
            Width = Brush->ImageSize.X;
        }

        if (!Height.IsSet())
        {
            Height = Brush->ImageSize.Y;
        }

        FMargin Padding = Decorator->GetImagePadding(TagOrId, bWarnIfMissing);
        FVector2D size = Decorator->GetGlyphSize(TagOrId, bWarnIfMissing);
        FVector2D imagePosition = Decorator->GetImagePosition(TagOrId, bWarnIfMissing);

        const FPDWRichTextImageRow* data = Decorator->GetRawData(TagOrId, bWarnIfMissing);

        return SNew(SPDWRichInLineWidget, Brush, TextStyle, Width, Height, data); //Padding, size, imagePosition);

    }

private:
    UPDWRichImageDecorator* Decorator;

};

UPDWRichImageDecorator::UPDWRichImageDecorator(const FObjectInitializer& ObjectInitializer)    : Super(ObjectInitializer){
}

TSharedPtr<ITextDecorator> UPDWRichImageDecorator::CreateDecorator(URichTextBlock* InOwner)
{
    return MakeShareable(new FPDWRichInlineWidget(InOwner, this));
}

FPDWRichTextImageRow* UPDWRichImageDecorator::FindImageRow(FName TagOrId, bool bWarnIfMissing)
{
    if (!RichImageTable) return nullptr;

    FString ContextString;
    return RichImageTable->FindRow<FPDWRichTextImageRow>(TagOrId, ContextString, bWarnIfMissing);
}

const FSlateBrush* UPDWRichImageDecorator::GetImageSlateBrush(FName TagOrId, bool bWarnIfMissing)
{
    const FPDWRichTextImageRow* WidgetRow = FindImageRow(TagOrId, bWarnIfMissing);
    if (!WidgetRow) return nullptr;
    
    return &WidgetRow->Brush;
}

const FMargin UPDWRichImageDecorator::GetImagePadding(FName TagOrId, bool bWarnIfMissing)
{
    const FPDWRichTextImageRow* WidgetRow = FindImageRow(TagOrId, bWarnIfMissing);
    if (!WidgetRow) return FMargin(0.0f);

    return WidgetRow->Padding;
}

const FVector2D UPDWRichImageDecorator::GetGlyphSize(FName TagOrId, bool bWarnIfMissing)
{
    const FPDWRichTextImageRow* WidgetRow = FindImageRow(TagOrId, bWarnIfMissing);
    if (!WidgetRow) return FVector2D(0.0f);

    return WidgetRow->GlyphSize;
}

const FVector2D UPDWRichImageDecorator::GetImagePosition(FName TagOrId, bool bWarnIfMissing)
{
    const FPDWRichTextImageRow* WidgetRow = FindImageRow(TagOrId, bWarnIfMissing);
    if (!WidgetRow) return FVector2D(0.0f);

    return WidgetRow->ImagePosition;
}

const FPDWRichTextImageRow* UPDWRichImageDecorator::GetRawData(FName TagOrId, bool bWarnIfMissing)
{
    const FPDWRichTextImageRow* WidgetRow = FindImageRow(TagOrId, bWarnIfMissing);
    return WidgetRow;
}