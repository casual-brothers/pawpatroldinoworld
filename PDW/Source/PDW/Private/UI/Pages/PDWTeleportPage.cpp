// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Pages/PDWTeleportPage.h"

#include "UI/Widgets/PDWTeleportLocationWidget.h"

void UPDWTeleportPage::NativeConstruct()
{
	Super::NativeConstruct();

	// Setup navigation between areas

	// Dino Planes
	BtnAreaDinoPlanes->SetNavigationRuleExplicit(EUINavigation::Up, BtnAreaJungle);
	BtnAreaDinoPlanes->SetNavigationRuleExplicit(EUINavigation::Left, BtnAreaVolcano);
	BtnAreaDinoPlanes->SetNavigationRuleExplicit(EUINavigation::Right, BtnAreaShores);
	BtnAreaDinoPlanes->SetNavigationRuleBase(EUINavigation::Down, EUINavigationRule::Stop);
	BtnAreaDinoPlanes->SetNavigationRuleBase(EUINavigation::Previous, EUINavigationRule::Stop);
	BtnAreaDinoPlanes->SetNavigationRuleBase(EUINavigation::Next, EUINavigationRule::Stop);

	// Dino Shores
	BtnAreaShores->SetNavigationRuleExplicit(EUINavigation::Up, BtnAreaJungle);
	BtnAreaShores->SetNavigationRuleExplicit(EUINavigation::Left, BtnAreaDinoPlanes);
	BtnAreaShores->SetNavigationRuleExplicit(EUINavigation::Down, BtnAreaDinoPlanes);
	BtnAreaShores->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Stop);
	BtnAreaShores->SetNavigationRuleBase(EUINavigation::Previous, EUINavigationRule::Stop);
	BtnAreaShores->SetNavigationRuleBase(EUINavigation::Next, EUINavigationRule::Stop);

	// Jungle
	BtnAreaJungle->SetNavigationRuleExplicit(EUINavigation::Down, BtnAreaDinoPlanes);
	BtnAreaJungle->SetNavigationRuleExplicit(EUINavigation::Left, BtnAreaVolcano);
	BtnAreaJungle->SetNavigationRuleExplicit(EUINavigation::Right, BtnAreaShores);
	BtnAreaJungle->SetNavigationRuleBase(EUINavigation::Up, EUINavigationRule::Stop);
	BtnAreaJungle->SetNavigationRuleBase(EUINavigation::Previous, EUINavigationRule::Stop);
	BtnAreaJungle->SetNavigationRuleBase(EUINavigation::Next, EUINavigationRule::Stop);

	// Volcano
	BtnAreaVolcano->SetNavigationRuleExplicit(EUINavigation::Up, BtnAreaJungle);
	BtnAreaVolcano->SetNavigationRuleExplicit(EUINavigation::Down, BtnAreaDinoPlanes);
	BtnAreaVolcano->SetNavigationRuleExplicit(EUINavigation::Right, BtnAreaJungle);
	BtnAreaVolcano->SetNavigationRuleBase(EUINavigation::Left, EUINavigationRule::Stop);
	BtnAreaVolcano->SetNavigationRuleBase(EUINavigation::Previous, EUINavigationRule::Stop);
	BtnAreaVolcano->SetNavigationRuleBase(EUINavigation::Next, EUINavigationRule::Stop);
}
