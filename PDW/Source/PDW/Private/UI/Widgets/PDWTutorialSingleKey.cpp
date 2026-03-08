// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PDWTutorialSingleKey.h"
#include "UI/Widgets/PDWTutorialNavigationIcon.h"
#include "Components/OverlaySlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/AssetManager.h"

void UPDWTutorialSingleKey::InitKey(FPDWPlatformIconConfiguration& IconConfiguration)
{
	CurrentIconConfiguration = IconConfiguration;
	SetNewWidgetData(CurrentIconConfiguration.ElementData);
	
#pragma region AsyncLoadImage
	SetVisibility(ESlateVisibility::Collapsed);
	TSoftObjectPtr<UTexture2D> SoftTexture = CurrentIconConfiguration.KeyTexture;
	if (!SoftTexture.ToSoftObjectPath().IsValid())
	{
		OnIconLoad.Broadcast();
		return;
	}
	TWeakObjectPtr<UImage> WeakImage(KeyImage);
	TWeakObjectPtr<UPDWTutorialSingleKey> WeakThis(this);
	UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftTexture.ToSoftObjectPath(),
		[WeakImage, WeakThis, SoftTexture]() {
			if (UImage* StrongImage = WeakImage.Get())
			{
				StrongImage->SetBrushFromTexture(SoftTexture.Get(), false);
			}
			if (UPDWTutorialSingleKey* StrongThis = WeakThis.Get())
			{
				StrongThis->SetVisibility(ESlateVisibility::HitTestInvisible);
				StrongThis->OnIconLoad.Broadcast();
			}
		}
	);
	
	KeyImage->SetDesiredSizeOverride(CurrentIconConfiguration.KeyImageSize);
	KeyImage->SetRenderTransform(CurrentIconConfiguration.KeyImageTransform);
#pragma endregion
#pragma region Material
	Material->SetVisibility(ESlateVisibility::Hidden);
	if (!CurrentIconConfiguration.MaterialInstance.ToSoftObjectPath().IsValid())
	{
		return;
	}
	Material->SetBrushFromSoftMaterial(CurrentIconConfiguration.MaterialInstance);
	Material->SetDesiredSizeOverride(CurrentIconConfiguration.MaterialSize);
	Material->SetRenderTransform(CurrentIconConfiguration.MaterialTransform);
	UOverlaySlot* WidgetSlot = Cast<UOverlaySlot>(Material->Slot);
	if (WidgetSlot)
	{
		FMargin MaterialPadding;
		MaterialPadding.Bottom = MaterialPadding.Top = (CurrentIconConfiguration.MaterialSize.Y/2) * -1.f;
		MaterialPadding.Left = MaterialPadding.Right = (CurrentIconConfiguration.MaterialSize.X/2) * -1.f;
		WidgetSlot->SetPadding(MaterialPadding);
	}
#pragma endregion
}

#if WITH_EDITOR
void UPDWTutorialSingleKey::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnPostEditChangeTrigger.Broadcast();
}

FUIElementData UPDWTutorialSingleKey::GetNewWidgetData()
{
	FUIElementData CurrentElementData;

	//Canvas Slot
	UCanvasPanelSlot* SlotAsCanvasSlot = Cast<UCanvasPanelSlot>(Slot);
	if (SlotAsCanvasSlot)
	{
		CurrentElementData.LayoutData = SlotAsCanvasSlot->GetLayout();
		CurrentElementData.bAutoSize = SlotAsCanvasSlot->GetAutoSize();
		CurrentElementData.ZOrder = SlotAsCanvasSlot->GetZOrder();
		CurrentElementData.Position = SlotAsCanvasSlot->GetPosition();
	}

	//Overlay Slot
	UOverlaySlot* SlotAsOverlaySlot = Cast<UOverlaySlot>(Slot);
	if (SlotAsOverlaySlot)
	{
		CurrentElementData.Padding = SlotAsOverlaySlot->GetPadding();
		CurrentElementData.HorizontalAlignment = SlotAsOverlaySlot->GetHorizontalAlignment();
		CurrentElementData.VerticalAlignment = SlotAsOverlaySlot->GetVerticalAlignment();
	}

	//Horizontal slot
	UHorizontalBoxSlot* SlotAsHorizontalSlot = Cast<UHorizontalBoxSlot>(Slot);
	if (SlotAsHorizontalSlot)
	{
		CurrentElementData.Padding = SlotAsHorizontalSlot->GetPadding();
		CurrentElementData.HorizontalAlignment = SlotAsHorizontalSlot->GetHorizontalAlignment();
		CurrentElementData.VerticalAlignment = SlotAsHorizontalSlot->GetVerticalAlignment();
	}

	//Vertical slot
	UVerticalBoxSlot* SlotAsVerticalSlot = Cast<UVerticalBoxSlot>(Slot);
	if (SlotAsVerticalSlot)
	{
		CurrentElementData.Padding = SlotAsVerticalSlot->GetPadding();
		CurrentElementData.HorizontalAlignment = SlotAsVerticalSlot->GetHorizontalAlignment();
		CurrentElementData.VerticalAlignment = SlotAsVerticalSlot->GetVerticalAlignment();
	}
	
	//Visibility
	CurrentElementData.Visibility = Visibility;
	CurrentElementData.RenderOpacity = GetRenderOpacity();

	//Render Transform
	CurrentElementData.Transform = GetRenderTransform();
	CurrentElementData.Pivot = GetRenderTransformPivot();

	return CurrentElementData;
}
#endif

void UPDWTutorialSingleKey::SetNewWidgetData(const FUIElementData& NewData)
{
	//Canvas Slot
	UCanvasPanelSlot* SlotAsCanvasSlot = Cast<UCanvasPanelSlot>(Slot);
	if (SlotAsCanvasSlot)
	{
		SlotAsCanvasSlot->SetLayout(NewData.LayoutData);
		SlotAsCanvasSlot->SetAutoSize(NewData.bAutoSize);
		SlotAsCanvasSlot->SetZOrder(NewData.ZOrder);
		SlotAsCanvasSlot->SetPosition(NewData.Position);
	}

	//Overlay Slot
	UOverlaySlot* SlotAsOverlaySlot = Cast<UOverlaySlot>(Slot);
	if (SlotAsOverlaySlot)
	{
		SlotAsOverlaySlot->SetPadding(NewData.Padding);
		SlotAsOverlaySlot->SetHorizontalAlignment(NewData.HorizontalAlignment);
		SlotAsOverlaySlot->SetVerticalAlignment(NewData.VerticalAlignment);
	}

	//Horizontal slot
	UHorizontalBoxSlot* SlotAsHorizontalSlot = Cast<UHorizontalBoxSlot>(Slot);
	if (SlotAsHorizontalSlot)
	{
		SlotAsHorizontalSlot->SetPadding(NewData.Padding);
		SlotAsHorizontalSlot->SetHorizontalAlignment(NewData.HorizontalAlignment);
		SlotAsHorizontalSlot->SetVerticalAlignment(NewData.VerticalAlignment);
	}

	//Vertical slot
	UVerticalBoxSlot* SlotAsVerticalSlot = Cast<UVerticalBoxSlot>(Slot);
	if (SlotAsVerticalSlot)
	{
		SlotAsVerticalSlot->SetPadding(NewData.Padding);
		SlotAsVerticalSlot->SetHorizontalAlignment(NewData.HorizontalAlignment);
		SlotAsVerticalSlot->SetVerticalAlignment(NewData.VerticalAlignment);
	}
	
	//Visibility
#if WITH_EDITOR
	if (GetWorld()->WorldType == EWorldType::EditorPreview)
	{
		Visibility = NewData.Visibility;
	}
	else
#endif
	{
		SetVisibility(NewData.Visibility);
	}
	
	SetRenderOpacity(NewData.RenderOpacity);

	//Render Transform
	SetRenderTransform(NewData.Transform);
	SetRenderTransformPivot(NewData.Pivot);
}