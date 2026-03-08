// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/RichTextBlockDecorator.h"
#include "PDWRichImageDecorator.generated.h"

class Texture2D;

USTRUCT(Blueprintable, BlueprintType)
struct FPDWRichTextImageRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	FVector2D GlyphSize{ FVector2D::ZeroVector };

	UPROPERTY(EditAnywhere)
	FVector2D ImagePosition{ FVector2D::ZeroVector };

	UPROPERTY(EditAnywhere)
	FMargin Padding;

	UPROPERTY(EditAnywhere)
	FSlateBrush Brush;
};

UCLASS()
class PDW_API UPDWRichImageDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:

	UPDWRichImageDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	virtual const FSlateBrush* GetImageSlateBrush(FName TagOrId, bool bWarnIfMissing);

	virtual const FMargin GetImagePadding(FName TagOrId, bool bWarnIfMissing);

	virtual const FVector2D GetGlyphSize(FName TagOrId, bool bWarnIfMissing);

	virtual const FVector2D GetImagePosition(FName TagOrId, bool bWarnIfMissing);

	virtual const FPDWRichTextImageRow* GetRawData(FName TagOrId, bool bWarnIfMissing);
protected:
	FPDWRichTextImageRow* FindImageRow(FName TagOrId, bool bWarnIfMissing);

	UPROPERTY(EditAnywhere, meta = (RequiredAssetDataTags = "RowStructure=/Script/PDW.PDWRichTextImageRow"))
	TObjectPtr<class UDataTable> RichImageTable{};
};
