// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "UI/Pages/PDWVideoPage.h"

#include "Input/Reply.h"
#include "Framework/Application/SlateApplication.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

FReply UPDWVideoPage::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	KeyDown();

	return FReply::Handled();
}

void UPDWVideoPage::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	
#if PLATFORM_SWITCH
	UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(SwitchMaterial, this);
	VideoImage->SetBrushFromMaterial(DynMat);
#else
	UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(DefaultMaterial, this);
	VideoImage->SetBrushFromMaterial(DynMat);
#endif
}

void UPDWVideoPage::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// User Focus Must always be on this page
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!this->HasAnyUserFocus())
	{
		FSlateApplication::Get().SetAllUserFocus(this->TakeWidget());
	}
}

void UPDWVideoPage::KeyDown()
{
	TriggerAction("KeyDown", FString(""));
}



