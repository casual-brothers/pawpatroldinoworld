#include "FunctionLibraries/NebulaFlowAudioBanksFunctionLibrary.h"
#include "FMODBlueprintStatics.h"

#include "CoreMinimal.h"
#include "Core/NebulaFlowGameInstance.h"
#include "FMODSettings.h"
#include "FMODBank.h"

#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"
#include "Managers/NebulaFlowAudioManager.h"

void UNebulaFlowAudioBanksFunctionLibrary::LoadZoneBanks(UNebulaFlowGameInstance* GInstance, FString fromZone, FString toZone)
{
	if (fromZone.Compare(toZone, ESearchCase::IgnoreCase) == 0) return;

	UNebulaFlowAudioManager* AudioManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(GInstance);
	if (!AudioManager) return;
	auto zoneBanks = AudioManager->FMODZone;

	for (int i = 0; i < zoneBanks.Num(); i++)
	{
		UFMODBank* bank = zoneBanks[i];
		FString name = bank->GetName();
		name = name.Replace(TEXT("Zone"), TEXT("Map"), ESearchCase::IgnoreCase);
		auto zone = GetZoneNum(name);

		if (fromZone.Compare(zone, ESearchCase::IgnoreCase) == 0)
		{
			if (UFMODBlueprintStatics::IsBankLoaded(bank))
			{
				UFMODBlueprintStatics::UnloadBank(bank);
			}
		}

		if (toZone.Compare(zone, ESearchCase::IgnoreCase) == 0)
		{
			if (!UFMODBlueprintStatics::IsBankLoaded(bank))
			{
				UFMODBlueprintStatics::LoadBank(bank, true, true);
			}
		}
	}
}

void UNebulaFlowAudioBanksFunctionLibrary::LoadDefaultBanks(UNebulaFlowGameInstance* GInstance)
{
	UNebulaFlowAudioManager* AudioManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(GInstance);
	if (!IsValid(AudioManager)) return;
	LoadBanks(AudioManager->FMODDefaultBanks);
}

void UNebulaFlowAudioBanksFunctionLibrary::DebugLoadAllBanks(UNebulaFlowGameInstance* GInstance)
{
	UNebulaFlowAudioManager* AudioManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(GInstance);
	if (!AudioManager) return;
	auto banks = AudioManager->FMODDefaultBanks;
	bool bAlreadyLoaded = true;
	for (int i = 0; i < banks.Num(); i++)
	{
		UFMODBank* bank = banks[i];
		if (!UFMODBlueprintStatics::IsBankLoaded(bank))
		{
			bAlreadyLoaded = false;
		}
	}
	if (!bAlreadyLoaded)
	{
		LoadBanks(AudioManager->FMODDefaultBanks);
		LoadBanks(AudioManager->FMODMenuVOBanks);
		LoadBanks(AudioManager->FMODZone);
	}
}

void UNebulaFlowAudioBanksFunctionLibrary::LoadAllBanks(UNebulaFlowGameInstance* GInstance)
{
	UNebulaFlowAudioManager* AudioManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(GInstance);
	if (!IsValid(AudioManager)) return;
	LoadBanks(AudioManager->FMODDefaultBanks);
	LoadBanks(AudioManager->FMODMenuVOBanks);
	LoadBanks(AudioManager->FMODZone);
}
void UNebulaFlowAudioBanksFunctionLibrary::LoadBanks(TArray<UFMODBank*> Banks)
{
	for (int i = 0; i < Banks.Num(); i++)
	{
		UFMODBank* bank = Banks[i];
		if (!UFMODBlueprintStatics::IsBankLoaded(bank))
		{
			UFMODBlueprintStatics::LoadBank(bank, true, true);
		}
	}
}

void UNebulaFlowAudioBanksFunctionLibrary::LoadLocalizedBanks(UNebulaFlowGameInstance* GInstance,
	FString LocaleToUse)
{
	UNebulaFlowAudioManager* AudioManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAudioManager(GInstance);
	if (!AudioManager) return;
	auto banks = AudioManager->FMODMenuVOBanks;
	//FString prevLocale = IFMODStudioModule::Get().GetLocale(); do note use this, will return DEFAULTLANGUAGE see fmodstudiomodule.cpp getlocale()
	FString prevLocale = AudioManager->GetCurrentAudioLanguage();
	FString newLocaleCode;

	bool bSameAsOld = false;
	const UFMODSettings &Settings = *GetDefault<UFMODSettings>();
	bool foundCulture = false;
	for (const FFMODProjectLocale& Locale : Settings.Locales)
	{
		if (Locale.LocaleName == LocaleToUse)
		{
			foundCulture = true;
			newLocaleCode = Locale.LocaleCode;
			AudioManager->SetCurrentAudioLanguage(newLocaleCode);
			break;
		}
	}
	
	if (prevLocale == newLocaleCode) bSameAsOld = true;

	if (!foundCulture)
	{
		AudioManager->SetCurrentAudioLanguage(LocaleToUse);
	}
	for (int i = 0; i < banks.Num(); i++)
	{
		UFMODBank* bank = banks[i];
		if (bSameAsOld)
		{
			if (!UFMODBlueprintStatics::IsBankLoaded(bank))
			{
				bSameAsOld = false;//it's the same of before but for some reason was not loaded...
			}
		}
	}

	if (bSameAsOld) return; //As no work needs to be done

	for (int i = 0; i < banks.Num(); i++)
	{
		UFMODBank* bank = banks[i];
		if (UFMODBlueprintStatics::IsBankLoaded(bank))
		{
			UFMODBlueprintStatics::UnloadBank(bank);
		}
	}


	if (foundCulture == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("FMOD culture %s is not availablle"), *LocaleToUse));
		
		//in case there are no locales , needs to be added in DefaultEngine ini
		ensure(Settings.Locales.Num());
		if(Settings.Locales.Num())
		{
			LocaleToUse = Settings.Locales[0].LocaleName;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("FMOD using culture %s"), *LocaleToUse));
	UFMODBlueprintStatics::SetLocale(LocaleToUse);
	for (int i = 0; i < banks.Num(); i++)
	{
		UFMODBank* bank = banks[i];
		if (!UFMODBlueprintStatics::IsBankLoaded(bank))
		{
			UFMODBlueprintStatics::LoadBank(bank, true, true);
		}
	}


}

FString UNebulaFlowAudioBanksFunctionLibrary::GetZoneNum(const FString& Text)
{
	FString result = "-1";
	if (Text.Contains("Map_"))
	{
		int32 firstIdx = Text.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::Type::FromEnd, Text.Len());
		FString num = Text.Mid(firstIdx + 1, Text.Len());
		result = num;
	}
	return result;
}


//FMOD related stuff
//for (int i = 0; i < FMODLocalizedBanks.Num(); i++)
//{
//	UFMODBlueprintStatics::UnloadBank(FMODLocalizedBanks[i]);
//}
//const UFMODSettings &Settings = *GetDefault<UFMODSettings>();
//bool foundCulture = false;
//for (const FFMODProjectLocale& Locale : Settings.Locales)
//{
//	if (Locale.LocaleName == LocaleToUse)
//	{
//		foundCulture = true;
//	}
//	// 		if (LocaleToUse == InCultureName && LocaleToUse.StartsWith(Locale.LocaleName))
//	// 		{
//	// 			LocaleToUse = Locale.LocaleName;
//	// 		}
//}
//if (InCultureName != LocaleToUse)
//{
//	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("FMOD IN culture %s becomes  %s"), *InCultureName, *LocaleToUse));
//}

//if (foundCulture == false && LocaleToUse == InCultureName)
//{
//	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("FMOD culture %s is not availablle"), *InCultureName));
//	LocaleToUse = "en";
//}
//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("FMOD using culture %s"), *LocaleToUse));
//UFMODBlueprintStatics::SetLocale(LocaleToUse);
//for (int i = 0; i < FMODLocalizedBanks.Num(); i++)
//{
//	UFMODBlueprintStatics::LoadBank(FMODLocalizedBanks[i], true, true);
//}



