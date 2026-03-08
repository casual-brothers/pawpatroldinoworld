// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/PDWFlipbook.h"
#include "PaperSprite.h"
#include "Components/Image.h"
#include "PaperSpriteBlueprintLibrary.h"
#include "../Plugins/2D/Paper2D/Source/Paper2D/Classes/PaperFlipbook.h"

void UPDWFlipbook::Reset()
{
	TimeAccumulator = 0;
	Playing = true;	
}

FSlateBrush UPDWFlipbook::MakeBrushFromSprite(UPaperSprite* Sprite)
{
	//const FSlateAtlasData SpriteAtlasData = Sprite->GetSlateAtlasData();
	//const FVector2D SpriteSize = SpriteAtlasData->GetSourceDimensions();
	FSlateBrush Brush;
	Brush.SetResourceObject(Sprite);
	if (WantedSize.X != 0)
	{
		Brush.ImageSize = WantedSize;
	}
	
	//Brush.ImageSize = FVector2D(512, 512);
	return Brush;
}

void UPDWFlipbook::SetPlaying(bool StartPlay)
{
	Playing = StartPlay;
}

void UPDWFlipbook::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (RelatedFlipbook && FlipbookImage)
	{
		UPaperSprite* Sprite = RelatedFlipbook->GetSpriteAtFrame(0);
		FSlateBrush Brush = MakeBrushFromSprite(Sprite);
		FlipbookImage->SetBrush(Brush);
	}

}

void UPDWFlipbook::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPDWFlipbook::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry ,InDeltaTime);
	if (Playing)
	{
		if (RelatedFlipbook)
		{
			TimeAccumulator += InDeltaTime;
			UPaperSprite* Sprite = RelatedFlipbook->GetSpriteAtTime(TimeAccumulator, true);
			FSlateBrush Brush = MakeBrushFromSprite(Sprite);
			FlipbookImage->SetBrush(Brush);

			if (TimeAccumulator >= RelatedFlipbook->GetTotalDuration())
			{
				TimeAccumulator = 0;

				if (!Loop)
				{
					Playing = false;
					if (ResetInitialStateAtEnd)
					{
						FlipbookImage->SetBrush(MakeBrushFromSprite(RelatedFlipbook->GetSpriteAtTime(0)));
					}
				}
			}
		}
	}
}
