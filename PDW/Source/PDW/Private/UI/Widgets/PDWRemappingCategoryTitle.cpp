// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#include "UI/Widgets/PDWRemappingCategoryTitle.h"
#include "Internationalization/Text.h"
#include "Components/TextBlock.h"



void UPDWRemappingCategoryTitle::InitializeWidget(FText& Title)
{
	TitleText->SetText(Title);
}
