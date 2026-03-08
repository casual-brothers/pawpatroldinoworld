#include "FunctionLibraries/NebulaFlowLocaleFunctionLibrary.h"

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Internationalization/Internationalization.h"
#include "Core/NebulaFlowGameInstance.h"
#include "FunctionLibraries/NebulaFlowAudioBanksFunctionLibrary.h"

UNebulaFlowGameInstance* UNebulaFlowLocaleFunctionLibrary::GInstance = 0;


/**
 * This function simplify the Locale from it-IT form to a more simple form if is possible.
 *
 * @param	InCultureName	The culture to be simplified
 *
 * @return	The simplified culture
 */
FString UNebulaFlowLocaleFunctionLibrary::AdjustComplexLocale(FString InCultureName, TArray<FString> AcceptedCultures)
{
	if (AcceptedCultures.Num() <= 0)
	{
		check(false);
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("AcceptedCultures empty array fix it!")));
		return "en";
	}

	FString LocaleToUse = InCultureName;
	FString Left;
	FString Right;
	// it could be "it-IT" or like "fr"
	InCultureName.Split(FString("-"), &Left, &Right);

	if ((InCultureName.Compare(FString("es-MX"), ESearchCase::IgnoreCase) == 0) || (InCultureName.Compare(FString("es-CO"), ESearchCase::IgnoreCase) == 0) || 
		(InCultureName.Compare(FString("es-CL"), ESearchCase::IgnoreCase) == 0) || (InCultureName.Compare(FString("es-AR"), ESearchCase::IgnoreCase) == 0))
	{
		LocaleToUse = FString("es-419");
	}

	if (InCultureName.Compare(FString("zh-SG"), ESearchCase::IgnoreCase) == 0)
	{
		LocaleToUse = FString("zh-Hans");
	}

	if (InCultureName.Compare(FString("zh-TW"), ESearchCase::IgnoreCase) == 0)
	{
		LocaleToUse = FString("zh-Hant");
	}

	if (InCultureName.Compare(FString("zh-CN"), ESearchCase::IgnoreCase) == 0)
	{
		LocaleToUse = FString("zh-Hans");
	}

	//nb-NO -> no
	if (InCultureName.Compare(FString("nb-NO"), ESearchCase::IgnoreCase) == 0)
	{
		LocaleToUse = Right.ToLower();
	}
	//ja-JP -> ja
	if (InCultureName.Compare(FString("ja-JP"), ESearchCase::IgnoreCase) == 0)
	{
		LocaleToUse = Left.ToLower();
	}
	//ar-SA,ar-AE -> ar
	if (InCultureName.Contains(FString("ar-")))
	{
		LocaleToUse = Left.ToLower();
	}

	//The language is the main language es it-IT -> it
	if (InCultureName.Split(FString("-"), &Left, &Right) && Right.ToLower() == Left.ToLower())
	{
		LocaleToUse = Left.ToLower();
	}

	bool bFound = false;
	for (int i = 0; i < AcceptedCultures.Num(); i++)
	{
		if (AcceptedCultures[i].Compare(LocaleToUse, ESearchCase::IgnoreCase) == 0)
		{
			bFound = true;
		}
	}

	bool promoteCountryPair = true;
	//hijacked culture from a language_country pair into a language if the lang_country pair is not accepted.
	//fr-CA -> fr , if fr-CA is not supported 
	if (!bFound && promoteCountryPair)
	{
		for (int i = 0; i < AcceptedCultures.Num(); i++)
		{
			if (AcceptedCultures[i].Compare(LocaleToUse, ESearchCase::IgnoreCase) != 0)
			{
				LocaleToUse.Split(FString("-"), &Left, &Right);
				if (AcceptedCultures[i].Contains(Left))
				{
					bFound = true;
					//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("culture %s not supported, using %s"), *LocaleToUse, *Left));
					LocaleToUse = Left.ToLower();
					break;
				}
			}
		}
	}

	//Still not found then use the default AcceptedCultures[0]
	if (!bFound)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("culture %s not supported, using %s"), *LocaleToUse, *AcceptedCultures[0]));
		LocaleToUse = AcceptedCultures[0];
	}


	return LocaleToUse;
}

/**
 * This function will ask the DefaultLocale to the system and will change FInternationalization Culture if the language is supported
 */
void UNebulaFlowLocaleFunctionLibrary::InitializeLocale(UNebulaFlowGameInstance* GameInstance)
{
	GInstance = GameInstance;
	TArray<FString> AcceptedCultures = GetCultures(TEXT("AcceptedCultures"));

	FString LocaleToUse = UKismetSystemLibrary::GetDefaultLocale();

	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Detected DefaultLocale %s"), *LocaleToUse));

	LocaleToUse = AdjustComplexLocale(LocaleToUse, AcceptedCultures);

	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Changed Locale into %s"), *LocaleToUse));
	SetCurrentCulture(LocaleToUse);
}

void UNebulaFlowLocaleFunctionLibrary::SetCurrentCulture(FString LocaleToUse)
{
	FInternationalization::Get().SetCurrentCulture(LocaleToUse);
	if (GInstance) 
	{
		UNebulaFlowAudioBanksFunctionLibrary::LoadLocalizedBanks(GInstance, LocaleToUse);
	}
}

TArray<FString> UNebulaFlowLocaleFunctionLibrary::GetCultures(const TCHAR* key)
{
	FString cultures;
	GConfig->GetString(TEXT("/Script/Clouds.GeneralProjectSettings"), key, cultures, GGameIni);
	TArray<FString> Out;
	cultures.ParseIntoArray(Out, TEXT(","), true);
	return Out;
}

