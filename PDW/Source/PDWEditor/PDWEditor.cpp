// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDWEditor.h"
#include "PropertyEditorModule.h"

//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, PPREditor, "PPREditor" );
// 
// void FDefaultGameModuleImpl::StartupModule()
// {
// 
// }

void FPDWEditorModule::StartupModule()
{

}

void FPDWEditorModule::ShutdownModule()
{

}

void FPDWEditorModule::RegisterCustomClassLayout(FName ClassName, FOnGetDetailCustomizationInstance DetailLayoutDelegate )
{
	check(ClassName != NAME_None);

	//RegisteredClassNames.Add(ClassName);

	static FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	PropertyModule.RegisterCustomClassLayout( ClassName, DetailLayoutDelegate );
}



IMPLEMENT_MODULE(FPDWEditorModule, PDWEditor)