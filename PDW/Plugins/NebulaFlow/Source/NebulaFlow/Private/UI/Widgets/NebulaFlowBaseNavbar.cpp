#include "UI/Widgets/NebulaFlowBaseNavbar.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "FunctionLibraries/NebulaFlowUIFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "UI/NebulaFlowUIConstants.h"
#include "UI/NebulaFlowNavbarDataStructures.h"
#include "UI/Widgets/NebulaFlowNavbarButton.h"
#include "NebulaFlow.h"
#include "UI/Widgets/NebulaFlowProfileSwap.h"



UNebulaFlowBaseNavbar::UNebulaFlowBaseNavbar(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UNebulaFlowBaseNavbar::DefineNavbarButtons(TMap<FName, ENavElementPosition> InNavbarConfig)
{
	NavbarButtons.Empty();
	for (auto Elem : InNavbarConfig)
	{
 		FNavButtonPosition NavPositionOrdered(Elem.Value , NavbarButtons.Num());
 
 		NavbarButtons.Emplace(Elem.Key , NavPositionOrdered);
	}

	AddButtonsToNavbarWidget();
}

void UNebulaFlowBaseNavbar::AddButtonsToNavbarWidget()
{
	UNebulaFlowUIConstants* UIConstants = UNebulaFlowUIFunctionLibrary::GetUIConstants(this);
	ResetNavbar();
	ensure(UIConstants && (NavbarCenterContainer || NavbarRightContainer || NavbarLeftContainer));
	if (UIConstants && (NavbarCenterContainer || NavbarRightContainer || NavbarLeftContainer))
	{
		const UDataTable* NavBarButtonTable = UIConstants->GetNavbarButtonsTable();
		ensure(NavBarButtonTable);
		if (NavBarButtonTable)
		{
			for (int i= 0 ; i < NavbarButtons.Num() ; i++)
			{
				for (auto currentButtonName : NavbarButtons)
				{
					if (currentButtonName.Value.Index != i)
					{
						continue;
					}
				
					FNavbarButtonDataTableRow* currentRow = NavBarButtonTable->FindRow<FNavbarButtonDataTableRow>(currentButtonName.Key, FString("GENERAL"));
					ensure(currentRow && currentRow->NavbarButtonClass);
					if (currentRow && currentRow->NavbarButtonClass)
					{
						UNebulaFlowNavbarButton* NewButton = CreateWidget<UNebulaFlowNavbarButton>(this, currentRow->NavbarButtonClass);
						NewButton->SetOwningPlayer(GetOwningPlayer());

						if (currentButtonName.Value.NavbarPosition == ENavElementPosition::LEFT && NavbarLeftContainer)
						{
							NavbarLeftContainer->AddChild(NewButton);
						}
						else if (currentButtonName.Value.NavbarPosition == ENavElementPosition::CENTER && NavbarCenterContainer)
						{
							NavbarCenterContainer->AddChild(NewButton);
						}
						else if (currentButtonName.Value.NavbarPosition == ENavElementPosition::RIGHT && NavbarRightContainer)
						{
							NavbarRightContainer->AddChild(NewButton);
						}
						NewButton->SetButtonName(currentButtonName.Key);
						NewButton->InitializeButton(currentRow->NavbarButtonData);
					}
					else
					{
						UE_LOG(LogNebulaFlowUI, Warning, TEXT("No Class Button Defined"));
					}
				}
			}
		}
		else
		{
			UE_LOG(LogNebulaFlowUI, Warning, TEXT("No Navbar Buttons Table Found"));
		}
	}
	else
	{
		UE_LOG(LogNebulaFlowUI, Warning, TEXT("No UI Constants Defined or NavbarContainerFound"));
	}
	if (NavbarButtons.Num() > 0)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UNebulaFlowBaseNavbar::DefineNavbarLabel(FText InText)
{
	ensure(NavbarTextblock);
	if (NavbarTextblock == nullptr)
	{
		UE_LOG(LogNebulaFlowUI, Warning, TEXT("No navbar textblock for the label found"));
		return;
	}

	NavbarTextblock->SetText(InText);
}

void UNebulaFlowBaseNavbar::ResetNavbar()
{
	ensure(NavbarLeftContainer || NavbarRightContainer || NavbarCenterContainer);

	if (NavbarLeftContainer) {
		NavbarLeftContainer->ClearChildren();
	}

	if (NavbarRightContainer) {
		NavbarRightContainer->ClearChildren();
	}

	if (NavbarCenterContainer) {
		NavbarCenterContainer->ClearChildren();
	}
}

UNebulaFlowNavbarButton* UNebulaFlowBaseNavbar::GetButtonByName(FName& ButtonName)
{	
	if (NavbarButtons.Contains(ButtonName))
	{	
		TArray<UWidget*> childs = NavbarLeftContainer->GetAllChildren();
		childs.Append(NavbarRightContainer->GetAllChildren());
		childs.Append(NavbarCenterContainer->GetAllChildren());
		for (UWidget* currentChild : childs)
		{
			UNebulaFlowNavbarButton* currentButton = Cast<UNebulaFlowNavbarButton>(currentChild);
			if (currentButton && currentButton->GetButtonName().IsEqual(ButtonName))
			{
				return currentButton;
			}
		}
	}	

	return nullptr;
}

void UNebulaFlowBaseNavbar::AddNavbarButton(const FName& ButtonName, ENavElementPosition Position)
{
	if (!NavbarButtons.Contains(ButtonName))
	{	
		FNavButtonPosition ButtonPosition(Position , NavbarButtons.Num());
		NavbarButtons.Emplace(ButtonName, ButtonPosition);
		AddButtonsToNavbarWidget();
	}
}

void UNebulaFlowBaseNavbar::RemoveNavbarButton(const FName& ButtonName)
{
	if (NavbarButtons.Contains(ButtonName))
	{
		FNavButtonPosition* NavPositionInfo = NavbarButtons.Find(ButtonName);
		for (auto &Elem : NavbarButtons)
		{
			if (Elem.Value.Index > NavPositionInfo->Index)
			{
				--Elem.Value.Index ;
			}
		}
		NavbarButtons.Remove(ButtonName);
		AddButtonsToNavbarWidget();
	}
}

void UNebulaFlowBaseNavbar::EnableButton(FName& ButtonName)
{
	if (UNebulaFlowNavbarButton* FoundButton = GetButtonByName(ButtonName))
	{
		FoundButton->ChangeButtonEnabled(true);
	}
}

void UNebulaFlowBaseNavbar::DisableButton(FName& ButtonName)
{
	if (UNebulaFlowNavbarButton* FoundButton = GetButtonByName(ButtonName))
	{
		FoundButton->ChangeButtonEnabled(false);
	}
}

void UNebulaFlowBaseNavbar::SetupSwapProfileLayout()
{
	ProfileSwapContainer->ClearChildren();
	UNebulaFlowProfileSwap* ProfileSwapWidget = CreateWidget<UNebulaFlowProfileSwap>(this, ProfileSwapClass);
	ProfileSwapWidget->SetOwningPlayer(GetOwningPlayer());
	ProfileSwapContainer->AddChild(ProfileSwapWidget);
}
