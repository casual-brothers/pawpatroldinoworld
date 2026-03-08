#include "Managers/NebulaFlowCommandManager.h"

#include "DeviceProfiles/DeviceProfile.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "Misc/ConfigUtilities.h"

UNebulaFlowCommandManager::UNebulaFlowCommandManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UNebulaFlowCommandManager::ApplyCommands(const TCHAR* InSectionName, const TCHAR* InIniFilename /*= *GDeviceProfilesIni*/)
{
	if (!NebulaFlowSectionCommandsBackupList.ContainsByPredicate([=](const FNebulaFlowSectionCommandsBackup& InItem) { return InItem.SectionName == InSectionName; }))
	{
		FNebulaFlowSectionCommandsBackup NebulaFlowSectionCommandsBackup = {};
		NebulaFlowSectionCommandsBackup.SectionName = InSectionName + FString(" ") + UDeviceProfileManager::Get().GetActiveProfile()->DeviceType;

		// Backup
		TFunction<void(IConsoleVariable*, const FString&, const FString&)> Func = [&](IConsoleVariable* CVar, const FString& KeyString, const FString& ValueString)
		{
			if (!NebulaFlowSectionCommandsBackup.CVarBackupMap.Contains(CVar))
			{
				NebulaFlowSectionCommandsBackup.CVarBackupMap.Add(CVar, CVar->GetString());
			}
		};

		UE::ConfigUtilities::ForEachCVarInSectionFromIni(*NebulaFlowSectionCommandsBackup.SectionName, InIniFilename, Func);

		NebulaFlowSectionCommandsBackupList.Add(NebulaFlowSectionCommandsBackup);

		// Apply
		UE::ConfigUtilities::ApplyCVarSettingsFromIni(*NebulaFlowSectionCommandsBackup.SectionName, InIniFilename, ECVF_SetByCode);
	}
}

void UNebulaFlowCommandManager::RestoreCommands(const TCHAR* InSectionName)
{
	FString SectionName = InSectionName + FString(" ") + UDeviceProfileManager::Get().GetActiveProfile()->DeviceType;
	const FNebulaFlowSectionCommandsBackup* NebulaFlowSectionCommandsBackup = NebulaFlowSectionCommandsBackupList.FindByPredicate([=](const FNebulaFlowSectionCommandsBackup& InItem)
	{
		return InItem.SectionName == SectionName;
	});
	
	if (NebulaFlowSectionCommandsBackup)
	{
		// Restore backup
		for (auto& CVarBackup : NebulaFlowSectionCommandsBackup->CVarBackupMap)
		{
			CVarBackup.Key->Set(*CVarBackup.Value);
		}

		const int32 SectionCommandsIndex = NebulaFlowSectionCommandsBackupList.IndexOfByPredicate([=](const FNebulaFlowSectionCommandsBackup& InItem)
		{
			return InItem.SectionName == SectionName;
		});

		// Empty backup
		NebulaFlowSectionCommandsBackupList.RemoveAt(SectionCommandsIndex);
	}
}