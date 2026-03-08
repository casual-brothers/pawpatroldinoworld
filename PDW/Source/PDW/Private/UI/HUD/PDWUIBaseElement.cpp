// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/PDWUIBaseElement.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Managers/PDWUIManager.h"

void UPDWUIBaseElement::SaveWidgetData()
{
#if WITH_EDITOR
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

	OnCurrentElementDataChange.Broadcast(ElementID, CurrentElementData);
#endif
}

#if WITH_EDITOR
void UPDWUIBaseElement::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() != "WidgetVisualizationDataAsset")
	{
		SaveWidgetData();
	}
}

void UPDWUIBaseElement::PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation)
{
	Super::PostEditUndo(TransactionAnnotation);
	SaveWidgetData();
}
#endif

void UPDWUIBaseElement::ApplyCurrentConfiguration(FUIElementData Configuration)
{
	//Canvas Slot
	UCanvasPanelSlot* SlotAsCanvasSlot = Cast<UCanvasPanelSlot>(Slot);
	if (SlotAsCanvasSlot)
	{
		SlotAsCanvasSlot->SetLayout(Configuration.LayoutData);
		SlotAsCanvasSlot->SetAutoSize(Configuration.bAutoSize);
		SlotAsCanvasSlot->SetZOrder(Configuration.ZOrder);
		SlotAsCanvasSlot->SetPosition(Configuration.Position);
	}

	//Overlay Slot
	UOverlaySlot* SlotAsOverlaySlot = Cast<UOverlaySlot>(Slot);
	if (SlotAsOverlaySlot)
	{
		SlotAsOverlaySlot->SetPadding(Configuration.Padding);
		SlotAsOverlaySlot->SetHorizontalAlignment(Configuration.HorizontalAlignment);
		SlotAsOverlaySlot->SetVerticalAlignment(Configuration.VerticalAlignment);
	}

	//Horizontal slot
	UHorizontalBoxSlot* SlotAsHorizontalSlot = Cast<UHorizontalBoxSlot>(Slot);
	if (SlotAsHorizontalSlot)
	{
		SlotAsHorizontalSlot->SetPadding(Configuration.Padding);
		SlotAsHorizontalSlot->SetHorizontalAlignment(Configuration.HorizontalAlignment);
		SlotAsHorizontalSlot->SetVerticalAlignment(Configuration.VerticalAlignment);
	}

	//Vertical slot
	UVerticalBoxSlot* SlotAsVerticalSlot = Cast<UVerticalBoxSlot>(Slot);
	if (SlotAsVerticalSlot)
	{
		SlotAsVerticalSlot->SetPadding(Configuration.Padding);
		SlotAsVerticalSlot->SetHorizontalAlignment(Configuration.HorizontalAlignment);
		SlotAsVerticalSlot->SetVerticalAlignment(Configuration.VerticalAlignment);
	}
	
	//Visibility
#if WITH_EDITOR
	if (GetWorld()->WorldType == EWorldType::EditorPreview)
	{
		Visibility = Configuration.Visibility;
	}
	else
#endif
	{
		SetVisibility(Configuration.Visibility);
	}
	
	SetRenderOpacity(Configuration.RenderOpacity);

	//Render Transform
	SetRenderTransform(Configuration.Transform);
	SetRenderTransformPivot(Configuration.Pivot);
}

void UPDWUIBaseElement::ChangeCurrentConfiguration(EVisualizationType NewType, FUIElementData NewConfiguration)
{
	if (CurrentVisualization != NewType)
	{
		CurrentVisualization = NewType;
		ApplyCurrentConfiguration(NewConfiguration);
	}
}