#include "Factory/NebulaInteractionBehaviourFactory.h"
#include "Behaviours/NebulaInteractionBehaviour.h"
#include "Kismet2/SClassPickerDialog.h"
#include "NebulaInteractionSystemSettings.h"
#include "Blueprint/InteractionBehaviourBlueprint.h"

#include "BlueprintEditorSettings.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "Editor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "SlateOptMacros.h"
#include "Templates/SubclassOf.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SWindow.h"

#define LOCTEXT_NAMESPACE "InteractionBehaviourFactory"


// ------------------------------------------------------------------------------
// Dialog to configure creation properties
// ------------------------------------------------------------------------------
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

class SInteractionBehaviourBPCreationDialog final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInteractionBehaviourBPCreationDialog) {}
		SLATE_ARGUMENT(TSubclassOf<UNebulaInteractionBehaviour>, ParentClass)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs)
	{
		bOkClicked = false;
		ParentClass = InArgs._ParentClass.Get();

		ChildSlot
		[
			SNew(SBorder)
				.Visibility(EVisibility::Visible)
				.BorderImage(FAppStyle::GetBrush("Menu.Background"))
				[
					SNew(SBox)
						.Visibility(EVisibility::Visible)
						.WidthOverride(500.0f)
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
								.FillHeight(1)
								[
									SNew(SBorder)
										.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
										.Content()
										[
											SAssignNew(ParentClassContainer, SVerticalBox)
										]
								]
							+ SVerticalBox::Slot()
								.AutoHeight()
								.HAlign(HAlign_Right)
								.VAlign(VAlign_Bottom)
								.Padding(8)
								[
									SNew(SUniformGridPanel)
										.SlotPadding(FAppStyle::GetMargin("StandardDialog.SlotPadding"))
										.MinDesiredSlotWidth(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
										.MinDesiredSlotHeight(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
										+ SUniformGridPanel::Slot(0, 0)
											[
												SNew(SButton)
													.HAlign(HAlign_Center)
													.ContentPadding(FAppStyle::GetMargin("StandardDialog.ContentPadding"))
													.OnClicked(this, &SInteractionBehaviourBPCreationDialog::OkClicked)
													.Text(LOCTEXT("CreateInteractionBehaviourBlueprintOk", "OK"))
											]
										+ SUniformGridPanel::Slot(1, 0)
											[
												SNew(SButton)
													.HAlign(HAlign_Center)
													.ContentPadding(FAppStyle::GetMargin("StandardDialog.ContentPadding"))
													.OnClicked(this, &SInteractionBehaviourBPCreationDialog::CancelClicked)
													.Text(LOCTEXT("CreateInteractionBehaviourBlueprintCancel", "Cancel"))
											]
								]
						]
				]
		];

			MakeParentClassPicker();
		}

	/** Sets properties for the supplied NebulaInteractionBehaviourFactory */
	bool ConfigureProperties(const TWeakObjectPtr<UNebulaInteractionBehaviourFactory> InInteractionBehaviourBlueprintFactory)
	{
		InteractionBehaviourBlueprintFactory = InInteractionBehaviourBlueprintFactory;

		const TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(LOCTEXT("CreateInteractionBehaviourBlueprintOptions", "Pick Parent Class"))
			.ClientSize(FVector2D(400, 700))
			.SupportsMinimize(false).SupportsMaximize(false)
			[
				AsShared()
			];

		PickerWindow = Window;
		GEditor->EditorAddModalWindow(Window);
		
		InteractionBehaviourBlueprintFactory.Reset();
		return bOkClicked;
	}

private:
	class FInteractionBehaviourBlueprintParentFilter final : public IClassViewerFilter
	{
	public:
		/** All children of these classes will be included unless filtered out by another setting. */
		TSet<const UClass*> AllowedChildrenOfClasses;

		FInteractionBehaviourBlueprintParentFilter() {}

		virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			// If it appears on the allowed child-of classes list (or there is nothing on that list)
			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
		}

		virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			// If it appears on the allowed child-of classes list (or there is nothing on that list)
			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
		}
	};

	/** Creates the combo menu for the parent class */
	void MakeParentClassPicker()
	{
		// Load the Class Viewer module to display a class picker
		FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

		FClassViewerInitializationOptions Options;
		Options.Mode = EClassViewerMode::ClassPicker;
		Options.DisplayMode = EClassViewerDisplayMode::TreeView;
		Options.bIsBlueprintBaseOnly = true;

		const TSharedPtr<FInteractionBehaviourBlueprintParentFilter> Filter = MakeShareable(new FInteractionBehaviourBlueprintParentFilter);

		// All child classes of ParentClass are valid
		if (UClass* ParentClassObject = ParentClass.Get())
		{
			Filter->AllowedChildrenOfClasses.Add(ParentClassObject);
		}
		Options.ClassFilters = {Filter.ToSharedRef()};

		ParentClassContainer->ClearChildren();
		ParentClassContainer->AddSlot()
			[
				ClassViewerModule.CreateClassViewer(Options, FOnClassPicked::CreateSP(this, &SInteractionBehaviourBPCreationDialog::OnClassPicked))
			];
	}

	/** Handler for when a parent class is selected */
	void OnClassPicked(UClass* ChosenClass)
	{
		if (ChosenClass)
		{
			ParentClass = ChosenClass;
		}
	}

	/** Handler for when ok is clicked */
	FReply OkClicked()
	{
		if (InteractionBehaviourBlueprintFactory.IsValid())
		{
			InteractionBehaviourBlueprintFactory->ParentClass = ParentClass.Get();
		}

		CloseDialog(true);

		return FReply::Handled();
	}

	void CloseDialog(bool bWasPicked = false)
	{
		bOkClicked = bWasPicked;
		if (PickerWindow.IsValid())
		{
			PickerWindow.Pin()->RequestDestroyWindow();
		}
	}

	/** Handler for when cancel is clicked */
	FReply CancelClicked()
	{
		CloseDialog();
		return FReply::Handled();
	}

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			CloseDialog();
			return FReply::Handled();
		}
		return SWidget::OnKeyDown(MyGeometry, InKeyEvent);
	}

private:
	/** The factory for which we are setting up properties */
	TWeakObjectPtr<UNebulaInteractionBehaviourFactory> InteractionBehaviourBlueprintFactory;

	/** A pointer to the window that is asking the user to select a parent class */
	TWeakPtr<SWindow> PickerWindow;

	/** The container for the Parent Class picker */
	TSharedPtr<SVerticalBox> ParentClassContainer;

	/** The selected class */
	TWeakObjectPtr<UClass> ParentClass;

	/** True if Ok was clicked */
	bool bOkClicked = false;
};

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

UNebulaInteractionBehaviourFactory::UNebulaInteractionBehaviourFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    SupportedClass = UInteractionBehaviourBlueprint::StaticClass();
	DefaultParentClass = UNebulaInteractionBehaviour::StaticClass();
	ParentClass = DefaultParentClass;
    bCreateNew = true;
	bEditAfterNew = true;
}

bool UNebulaInteractionBehaviourFactory::ConfigureProperties()
{
	const TSharedRef<SInteractionBehaviourBPCreationDialog> Dialog =
		SNew(SInteractionBehaviourBPCreationDialog)
		.ParentClass(ParentClass);

	return Dialog->ConfigureProperties(this);
}


UObject* UNebulaInteractionBehaviourFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

UObject* UNebulaInteractionBehaviourFactory::FactoryCreateNew(UClass* BlueprintClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(BlueprintClass->IsChildOf(SupportedClass));

	if (ParentClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass) || !ParentClass->IsChildOf(DefaultParentClass))
	{
		ShowCannotCreateBlueprintDialog();

		return nullptr;
	}

	UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, InName, BPTYPE_Normal, BlueprintClass, UBlueprintGeneratedClass::StaticClass(), CallingContext);
	return NewBP;
}

void UNebulaInteractionBehaviourFactory::ShowCannotCreateBlueprintDialog()
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("DefaultClassName"), DefaultParentClass ? FText::FromString(DefaultParentClass->GetName()) : LOCTEXT("Null", "(null)"));
	Args.Add(TEXT("ClassName"), ParentClass ? FText::FromString(ParentClass->GetName()) : LOCTEXT("Null", "(null)"));
	FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("CannotCreateInteractionBehaviourBlueprint", "Cannot create a {DefaultClassName} Blueprint based on the class '{ClassName}'."), Args));
}

#undef LOCTEXT_NAMESPACE
