// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/NebulaFlowScrollingText.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/OverlaySlot.h"
#include "Layout/Margin.h"
#include "Components/ScaleBox.h"
#include "Components/Overlay.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"

void UNebulaFlowScrollingText::InitTextes(FText NewText)
{
	Reset();
	SetupTextBasedOnWidth(false);
	if (FirstText)
	{
		FirstText->SetText(NewText);
	}
	if (SecondText)
	{
		SecondText->SetText(NewText);
		//SecondText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UNebulaFlowScrollingText::Reset()
{
	CurrentTimer = StartingTimeDelay;
	TextLocalWidth = 0;
	bEnableSecondText = false;
	ForceStart = false;
	bFrameSkippedToUpdateGeometry = false;
	FirstText->SetRenderTranslation(FVector2D(0, 0));
	SecondText->SetVisibility(ESlateVisibility::Hidden);
	
}

void UNebulaFlowScrollingText::CheckCorrectTextSize()
{
	if (TextLocalWidth >= SizeBoxSize.X)
	{
		if (!bEnableSecondText)
		{
			SetupTextBasedOnWidth(true);
			bCanBeContained = false;
		}
	}
	else
	{
		bCanBeContained = true;
		bEnableSecondText = false;
		return;
	}
}

void UNebulaFlowScrollingText::SetupTextBasedOnWidth(bool TooBig) // setup of anchors to allow correct calculation of our moving text
{

	FAnchors BigTextAnchors{};

	BigTextAnchors.Minimum = FVector2D(TooBig ? bIsArabicLanguage ? 1 : 0 : 0.5, 0.5);
	BigTextAnchors.Maximum = FVector2D(TooBig ? bIsArabicLanguage ? 1 : 0 : 0.5, 0.5);

	UCanvasPanelSlot* FirstOverlayTextSlot = Cast<UCanvasPanelSlot>(FirstTextOverlay->Slot);
	UCanvasPanelSlot* SecondOverlayTextSlot = Cast<UCanvasPanelSlot>(SecondTextOverlay->Slot);

	FirstOverlayTextSlot->SetAnchors(BigTextAnchors);
	SecondOverlayTextSlot->SetAnchors(BigTextAnchors);

	FirstOverlayTextSlot->SetPosition(FVector2D(0, 0));
	SecondOverlayTextSlot->SetPosition(FVector2D(0, 0));

	FirstOverlayTextSlot->SetAlignment(FVector2D(TooBig ? bIsArabicLanguage ? 1 : 0 : 0.5, 0.5));
	SecondOverlayTextSlot->SetAlignment(FVector2D(TooBig ? bIsArabicLanguage ? 1 : 0 : 0.5, 0.5));

	FirstOverlayTextSlot->SetAutoSize(TooBig ? true : false);
	SecondOverlayTextSlot->SetAutoSize(TooBig ? true : false);
}

void UNebulaFlowScrollingText::NativePreConstruct()
{
	Super::NativePreConstruct();

	MovingSizeBox->SetWidthOverride(SizeBoxSize.X);
	MovingSizeBox->SetHeightOverride(SizeBoxSize.Y);
	UCanvasPanelSlot* FirstOverlayTextSlot = Cast<UCanvasPanelSlot>(FirstTextOverlay->Slot);
	UCanvasPanelSlot* SecondOverlayTextSlot = Cast<UCanvasPanelSlot>(SecondTextOverlay->Slot);
	FirstOverlayTextSlot->SetSize(FVector2D(SizeBoxSize.X, SizeBoxSize.Y));
	SecondOverlayTextSlot->SetSize(FVector2D(SizeBoxSize.X, SizeBoxSize.Y));
}

void UNebulaFlowScrollingText::NativeConstruct()
{
	Super::NativeConstruct();

	bIsArabicLanguage = UNebulaFlowUIFunctionLibrary::IsGameArabic(this);

	if (bIsArabicLanguage)
	{
		FirstText->SetJustification(ETextJustify::Right);
		SecondText->SetJustification(ETextJustify::Right);
	}

	if (!DoubleText)
	{
		SecondText->RemoveFromParent(); // we get rid of the unused text
	}

	if (!DefaultText.IsEmpty())
	{
		InitTextes(DefaultText);
	}

	Reset();

}

void UNebulaFlowScrollingText::NativeDestruct()
{
	Super::NativeDestruct();
}

void UNebulaFlowScrollingText::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ForceStart || AutoStart) // if we want to start from beginning or call a start from outside
	{
		CurrentTimer -= InDeltaTime;

		if (CurrentTimer <= 0)
		{
			CurrentTimer = 0; // to avoid current timer going - infinite

			if (DoubleText)
			{

				TextLocalWidth = FirstText->GetCachedGeometry().GetLocalSize().X;

				if (!bFrameSkippedToUpdateGeometry) // we skip a frame to assure that the geometry is calculated correctly
				{
					CheckCorrectTextSize();
					bFrameSkippedToUpdateGeometry = true; 
					return;
				}
				
				if (!bEnableSecondText && !bCanBeContained)
				{
					if (FirstText)
					{
						if (SecondText)
						{
							SecondText->SetVisibility(ESlateVisibility::HitTestInvisible);
							SecondText->SetRenderTranslation(FVector2D(bIsArabicLanguage ? -TextLocalWidth - SpaceBetweenTextes : TextLocalWidth + SpaceBetweenTextes, 0));
							bEnableSecondText = true;
						}
					}
				}

				float FrameMovement = InDeltaTime * Speed;

				if (!bCanBeContained)
				{
					if (!bIsArabicLanguage)
					{
						FirstText->SetRenderTranslation(FirstText->GetRenderTransform().Translation + FVector2D(FrameMovement * -1, 0));
						SecondText->SetRenderTranslation(SecondText->GetRenderTransform().Translation + FVector2D(FrameMovement * -1, 0));

						if (TextLocalWidth > 0 && FirstText->GetRenderTransform().Translation.X * -1 > TextLocalWidth)// if 1 text is completely out
						{
							FirstText->SetRenderTranslation(FVector2D(TextLocalWidth + SpaceBetweenTextes * 2, 0));
						}

						if (TextLocalWidth > 0 && SecondText->GetRenderTransform().Translation.X * -1 > TextLocalWidth)// if 1 text is completely out
						{
							SecondText->SetRenderTranslation(FVector2D(TextLocalWidth + SpaceBetweenTextes * 2, 0));
						}
					}
					else
					{
						FirstText->SetRenderTranslation(FirstText->GetRenderTransform().Translation - FVector2D(FrameMovement * -1, 0));
						SecondText->SetRenderTranslation(SecondText->GetRenderTransform().Translation - FVector2D(FrameMovement * -1, 0));

						if (TextLocalWidth > 0 && FirstText->GetRenderTransform().Translation.X > TextLocalWidth)// if 1 text is completely out
						{
							FirstText->SetRenderTranslation(FVector2D(-TextLocalWidth - SpaceBetweenTextes * 2, 0));
						}

						if (TextLocalWidth > 0 && SecondText->GetRenderTransform().Translation.X > TextLocalWidth)// if 1 text is completely out
						{
							SecondText->SetRenderTranslation(FVector2D(-TextLocalWidth - SpaceBetweenTextes * 2, 0));
						}
					}
		
				}
			}
		}
	}
}
