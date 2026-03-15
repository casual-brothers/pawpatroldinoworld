// Copyright Epic Games, Inc. All Rights Reserved.
using AutomationTool;
using EpicGames.Core;
using Gauntlet;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using NintendoPackagingHelpers;
using NintendoTm;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Xml.Linq;
using System.Xml.XPath;
using UnrealBuildBase;
using UnrealBuildTool;

public abstract class NintendoPlatform : AutomationTool.Platform
{
	class AutoPatchDefragmentData
	{
		public int AutoPatchDefragmentBlockSize { get; set; }
	}

	public NintendoPlatform(UnrealTargetPlatform TargetPlatform)
		: base(TargetPlatform)
	{
	}

	public string RuntimeSettingsName => $"/Script/{TargetPlatformType}RuntimeSettings.{TargetPlatformType}RuntimeSettings";
	public string EditorSettingsName => $"/Script/{TargetPlatformType}PlatformEditor.{TargetPlatformType}TargetSettings";

	protected abstract bool IsDeviceTypeValidForPlatform(string DeviceType);

	static public string GetPlatformIdentifierFromTargetPlatform(UnrealTargetPlatform TargetPlatform)
	{ 
		if ( GetPlatform(TargetPlatform) is NintendoPlatform PlatformAutomation)
		{
			return PlatformAutomation.NintendoSDKPlatformIdentifier;
		}

		throw new Exception("NintendoPlatform::GetPlatformIdentifierFromTargetPlatform() only supports Nintendo platforms.");
	}
	static public string GetPlatformIdentifierFromPlatformName(string PlatformName)
	{
		if (UnrealTargetPlatform.TryParse(PlatformName, out UnrealTargetPlatform TargetPlatform))
		{
			return GetPlatformIdentifierFromTargetPlatform(TargetPlatform);
		}
		throw new Exception("NintendoPlatform::GetPlatformIdentifierFromTargetPlatform() only supports Nintendo platforms.");
	}

	static public UnrealTargetPlatform GetTargetPlatformFromIdentifier(string Identifier)
	{
		foreach ( NintendoPlatform PlatformAutomation in Platforms.Values.OfType<NintendoPlatform>())
		{
			if (PlatformAutomation.NintendoSDKPlatformIdentifier == Identifier)
			{
				return PlatformAutomation.PlatformType;
			}
		}

		throw new Exception("NintendoPlatform::GetTargetPlatformFromIdentifier() only supports Nintendo platform identifiers.");
	}

	protected abstract UnrealTargetPlatform TargetPlatform { get; }
	
	protected abstract string NintendoSDKPlatformIdentifier { get; }
	protected abstract string AuthoringToolPlatformArg { get; }
	protected abstract string PatchAnalyzerPlatformArg { get; }
	protected abstract string TargetToolsArch { get; }
	
	protected static readonly string CodeSpecifiedVersionSuffix = " (from installed SDK)";

	public abstract void WriteDefaultIOStorageValues(string MetaFilePath, string UpdateMetaContents, string StageDirectory, string ShortProjectName);

	public override StagedFileReference Remap(StagedFileReference Dest)
	{
		// subpath defined by Nintendo's AuthoringTool.exe
		const string NspdDynamicLibSubpath = ".nspd/program0.ncd/data/";

		if (Dest.Name.EndsWith("nrr"))
		{
			if (Dest.Name.Contains(NspdDynamicLibSubpath))
			{
				// to support staging multiple compile configurations, don't remap .nrr files if they are in the .nspd directory structure
				return Dest;
			}

			// If there is an .nrr file then it MUST be in /.nrr and it MUST be the only one.
			String Filename = Path.Combine(".nrr", Path.GetFileName(Dest.Name));
			return new StagedFileReference(Filename);
		}

		if (Dest.Name.EndsWith(".nro"))
		{
			if (Dest.Name.Contains(NspdDynamicLibSubpath))
			{
				// to support staging multiple compile configurations, don't remap .nro files if they are in the .nspd directory structure
				return Dest;
			}

			// This is an arbitrary directory name.  It needs to contain all the .nro files referenced by the .nrr file.
			// It will be excluded from the creatensp call but added using --nro
			String Filename = Path.Combine("nro", Path.GetFileName(Dest.Name));
			return new StagedFileReference(Filename);
		}

		if (Dest.Name.EndsWith(".nrs"))
		{
			if (Dest.Name.Contains(NspdDynamicLibSubpath))
			{
				// to support staging multiple compile configurations, don't remap .nrs files if they are in the .nspd directory structure
				return Dest;
			}

			// This is an arbitrary directory name.  It needs to contain all the .nrs files associated with .nro files
			// It will be filtered from creatensp but the files will be referenced using --nrs on platforms that need it
			String Filename = Path.Combine("nrs", Path.GetFileName(Dest.Name));
			return new StagedFileReference(Filename);
		}

		if (Dest.Name.EndsWith(".modules"))
		{
			// We want to move the modules file to the root NRO directory, because that's the modules directory
			String Filename = Path.Combine("nro", Path.GetFileName(Dest.Name));
			return new StagedFileReference(Filename);
		}

		return Dest;
	}


	protected override string GetPlatformExeExtension()
	{
		return ".nspd_root";
	}

	public override string GetPlatformPakCommandLine(ProjectParams Params, DeploymentContext SC)
	{
		// If bUseDefaultPatchAlignment is enabled, ignore the platform-specific value
		bool bUseDefaultPatchAlignment = false;
		ConfigHierarchy GameIni = ConfigCache.ReadHierarchy(ConfigHierarchyType.Game, DirectoryReference.FromFile(SC.RawProjectPath), TargetIniPlatformType);
		GameIni.GetBool("/Script/UnrealEd.ProjectPackagingSettings", $"b{TargetPlatformType}UseDefaultPatchAlignment", out bUseDefaultPatchAlignment);
		// If generate chunks is disabled then bypass the list.
		if (bUseDefaultPatchAlignment)
		{
			return " -patchpaddingalign=16 -iostorepatchpaddingalign=65536";
		}
		else
		{
			return " -patchpaddingalign=16";
		}
	}

	private string GetTargetNameFromDeviceName(string DeviceName)
	{
		// Device is in the form Platform@SERIALNUMBER
		string[] Tokens = DeviceName.Split('@');
		if (Tokens.Length == 2)
		{
			return Tokens[1];
		}

		return DeviceName;
	}

	public override bool GetPlatformPatchesWithDiffPak(ProjectParams Params, DeploymentContext SC)
	{
		ConfigHierarchy PlatformGameConfig = null;
		if (!Params.EngineConfigs.TryGetValue(SC.StageTargetPlatform.PlatformType, out PlatformGameConfig))
		{
			Logger.LogInformation("Did not read GenerateDiffPakPatch setting from .ini setting, not using diff pak method.");
			return false;
		}

		bool GenerateDiffPakPatch = false;
		if (PlatformGameConfig.GetBool(EditorSettingsName, "GenerateDiffPakPatch", out GenerateDiffPakPatch))
		{
			Logger.LogInformation("Read GenerateDiffPakPatch setting from platform config, {Arg0}using diff pak method.", GenerateDiffPakPatch ? "" : "not ");
			return GenerateDiffPakPatch;
		}

		Logger.LogInformation("Did not read GenerateDiffPakPatch setting from .ini setting, not using diff pak method.");
		return false;
	}

	protected string GetProjectDir(ProjectParams Params)
	{
		return Path.GetDirectoryName(Path.GetFullPath(Params.RawProjectPath.FullName));
	}

	protected string GetPackageOutputDirectory(ProjectParams Params, DeploymentContext SC)
	{
		// location where packages will go
		string BinariesLocation = Path.Combine(GetProjectDir(Params), "Saved", "Packages", GetCookPlatform(false, Params.Client));

		// legacy support. (this is inconsistent with other console platforms).
		Params.EngineConfigs.TryGetValue(SC.StageTargetPlatform.PlatformType, out ConfigHierarchy PlatformGameConfig);
		if (PlatformGameConfig != null && 
			PlatformGameConfig.GetBool(EditorSettingsName, "bCreatePackagesInBinariesFolder", out bool bCreatePackagesInBinariesFolder) && 
			bCreatePackagesInBinariesFolder)
		{
			BinariesLocation = Path.Combine(GetProjectDir(Params), "Binaries", TargetPlatformType.ToString());
		}

		// override if params say so 
		if (Params.Prebuilt)
		{
			BinariesLocation = Path.Combine(Params.BaseStageDirectory, TargetPlatformType.ToString());
		}
		else if (Params.HasCreateReleaseVersion)
		{
			BinariesLocation = Params.GetCreateReleaseVersionPath(SC, Params.Client);
		}

		return BinariesLocation;
	}
	protected string GetNspName(ProjectParams Params, DeploymentContext SC, UnrealTargetConfiguration TargetConfiguration, string AppendName = "")
	{
		string FullNspName = "UnrealGame";

		// handle code projects
		if (Params.IsCodeBasedProject)
		{
			// strip off any extension of the executable
			FullNspName = Path.GetFileNameWithoutExtension(Params.GetProjectExeForPlatform(TargetPlatformType).ToString());
		}

		// Allow forcing the configuration name to always be affixed
		ConfigHierarchy GameIni = ConfigCache.ReadHierarchy(ConfigHierarchyType.Game, (SC.ProjectRoot == null) ? null : SC.ProjectRoot, TargetPlatformType);
		String IniPath = "/Script/UnrealEd.ProjectPackagingSettings";
		bool bAlwaysAffixConfiguration = false;
		GameIni.GetBool(IniPath, "bAlwaysAffixConfiguration", out bAlwaysAffixConfiguration);

		// append config if not development 
		if (bAlwaysAffixConfiguration || TargetConfiguration != UnrealTargetConfiguration.Development)
		{
			FullNspName += "-" + PlatformType.ToString() + "-" + TargetConfiguration.ToString();
		}

		if (!string.IsNullOrEmpty(AppendName))
		{
			FullNspName += AppendName;
		}

		if (Params.HasDLCName)
		{
			// Add suffix for DLC
			FullNspName += "-" + Params.DLCFile.GetFileNameWithoutExtension();
		}

		return FullNspName + ".nsp";
	}

	public override void PreStage(ProjectParams Params, DeploymentContext SC)
	{
		if (!CommandUtils.IsBuildMachine)
		{
			// terminate local running app in case it helps
			if (Params.DeviceNames.Count() > 0)
			{
				bool bKilledDefault = false;

				foreach (string DeviceName in Params.DeviceNames)
				{
					if (!String.IsNullOrWhiteSpace(DeviceName))
					{
						KillCurrentRunningProcess(GetTargetNameFromDeviceName(DeviceName));
					}
					else if (bKilledDefault == false)
					{
						KillCurrentRunningProcess(null);
						bKilledDefault = true;
					}
				}
			}
			else
			{
				KillCurrentRunningProcess(null);
			}
		}
	}

	private bool GetPackagingOption(string OptionalPackagingOptions, DirectoryReference ProjectPath, string OptionName, ref string OptionValue)
	{
		if (GetOptionValue(OptionalPackagingOptions, OptionName, ref OptionValue))
		{
			return true;
		}
		else if (ProjectPath != null)
		{
			ConfigHierarchy EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, ProjectPath, TargetIniPlatformType);
			if (EngineConfig.GetString("Packaging", OptionName, out OptionValue))
			{
				return true;
			}
		}

		return false;
	}

	private bool GetPackagingOption(string OptionalPackagingOptions, DirectoryReference ProjectPath, string OptionName, ref bool OptionValue)
	{
		string OptionString = null;
		if (GetOptionValue(OptionalPackagingOptions, OptionName, ref OptionString))
		{
			return bool.TryParse(OptionString, out OptionValue);
		}
		else if (ProjectPath != null)
		{
			ConfigHierarchy EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, ProjectPath, TargetIniPlatformType);
			if (EngineConfig.GetBool("Packaging", OptionName, out OptionValue))
			{
				return true;
			}
		}

		return false;
	}

	private bool GetPackagingOption(string OptionalPackagingOptions, DirectoryReference ProjectPath, string OptionName, ref int OptionValue)
	{
		string OptionString = null;
		if (GetOptionValue(OptionalPackagingOptions, OptionName, ref OptionString))
		{
			return int.TryParse(OptionString, out OptionValue);
		}
		else if (ProjectPath != null)
		{
			ConfigHierarchy EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, ProjectPath, TargetIniPlatformType);
			if (EngineConfig.GetInt32("Packaging", OptionName, out OptionValue))
			{
				return true;
			}
		}

		return false;
	}

	private bool GetPackagingOption(string OptionalPackagingOptions, DirectoryReference ProjectPath, string OptionName, ref float OptionValue)
	{
		string OptionString = null;
		if (GetOptionValue(OptionalPackagingOptions, OptionName, ref OptionString))
		{
			return float.TryParse(OptionString, out OptionValue);
		}
		else if (ProjectPath != null)
		{
			ConfigHierarchy EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, ProjectPath, TargetIniPlatformType);
			if (EngineConfig.GetString("Packaging", OptionName, out OptionString))
			{
				return float.TryParse(OptionString, out OptionValue);
			}
		}

		return false;
	}

	private bool HasPackagingOption(string OptionalPackagingOptions, DirectoryReference ProjectPath, string OptionName)
	{
		bool OptionValue = false;

		if (HasOption(OptionalPackagingOptions, OptionName))
		{
			OptionValue = true;
		}
		else if (ProjectPath != null)
		{
			ConfigHierarchy EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, ProjectPath, TargetIniPlatformType);
			EngineConfig.GetBool("Packaging", OptionName, out OptionValue);
		}

		return OptionValue;
	}

	static public string GetOptionValue(string OptionsString, string OptionName)
	{
		string OptionDesignator = OptionName + "=";

		string[] OptionsArray = OptionsString.TrimStart('-').Split(" -");
		foreach (string Option in OptionsArray)
		{
			if (Option.StartsWith(OptionDesignator, StringComparison.InvariantCultureIgnoreCase))
			{
				return Option.Remove(0, OptionDesignator.Length).Trim().TrimStart('\"').TrimEnd('\"').Trim();
			}
		}

		return "";
	}
	static public bool GetOptionValue(string OptionsString, string OptionName, ref string OptionValue)
	{
		OptionValue = GetOptionValue(OptionsString, OptionName);

		return !string.IsNullOrEmpty(OptionValue);
	}

	static private bool HasOption(string Options, string NeededOption)
	{
		bool bTrimStartingDash = !NeededOption.StartsWith("-");

		foreach (string Option in Options.Split(" "))
		{
			string TrimmedOption = bTrimStartingDash ? Option.TrimStart('-') : Option;
			 
			// check for value or -value to be safe
			if (TrimmedOption.Equals(NeededOption, StringComparison.InvariantCultureIgnoreCase))
			{
				return true;
			}
		}

		return false;
	}

	public bool ShouldDefragmentPatch(string AdditionalPackageOptions, DirectoryReference ProjectRoot, string ApplicationVersionString)
	{
		// Example arguments passed to RunUAT.bat
		// -AdditionalPackageOptions="-DefragmentMajorPatches -DefragmentPatchVersions=1.10,1.20"

		if (HasPackagingOption(AdditionalPackageOptions, ProjectRoot, "DefragmentPatch"))
		{
			return true;
		}
		else if (HasPackagingOption(AdditionalPackageOptions, ProjectRoot, "DefragmentMajorPatches"))
		{
			// If -DefragmentMajorPatches is specified and not forced with -DefragmentPatch, only defragment if the new .nsp version is an integer.

			// Only defragment if this is a major build, e.g. 4.0 and not 4.0.1 or 4.1 or 4.0.0.1
			// Check <= as values will be -1 if they are not specified in the version string.
			Version FullVersion = new Version(ApplicationVersionString);
			if (FullVersion.Minor <= 0 && FullVersion.Build <= 0 && FullVersion.Revision <= 0)
			{
				return true;
			}
		}
		else
		{
			string DefragmentVersions = null;
			if (GetPackagingOption(AdditionalPackageOptions, ProjectRoot, "DefragmentPatchVersions", ref DefragmentVersions))
			{
				if (!string.IsNullOrEmpty(DefragmentVersions))
				{
					Version CurrentVersion = new Version(ApplicationVersionString);

					foreach (string DefragmentPatchVersion in DefragmentVersions.Split(','))
					{
						// Only defragment if this ApplicationVersionString is contained in the list.
						Version DefragmentVersion = new Version(DefragmentPatchVersion);
						if (CurrentVersion == DefragmentVersion)
						{
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	// Assumes that only the filename without a path is passed in.
	// Returns the base chunk index a file should be allocated to
	// Content not otherwise assigned a chunk index always returns 0
	private int GetChunkIndexForFile(String FileName)
	{
		//Return a default of 0 if we fail to parse
		int ChunkID = 0;

		//Skip work and just return 0 for any filetype we don't expect to have it's ChunkID in the filename
		if (!IsChunkIndexExpectedInNameForFileType(FileName))
		{
			return ChunkID;
		}

		//if this is a global file it won't have a ChunkID in the file (IE: global.pak / global.ucas / etc )
		if (FileName.StartsWith("global"))
		{
			return ChunkID;
		}

		bool bWasParseSuccess = false;
		var PakFileMatch = Regex.Match(FileName, $".*pakchunk(Early)?([0-9]+)?[a-zA-Z]*-{TargetPlatformType}(Client)?\\.(pak|utoc|ucas)", RegexOptions.IgnoreCase);
		if (PakFileMatch.Success)
		{
			//Group 1 will succeed if this was an early pak, if so it's expected to not find a chunk ID in the filename.
			//Just treat this as the default chunkID
			//IE pakChunkEarly-PLATFORM_NAME.pak
			if (PakFileMatch.Groups[1].Success)
			{
				bWasParseSuccess = true;
				Logger.LogInformation("Parsed early pakchunk file: {FileName}. Using default ChunkID: {ChunkID}", FileName, ChunkID);
			}
			//Group 2 should be the pak chunk ID for non-early paks IE: pakchunk0-PLATFORM_NAME.pak (0) or pakchunk1003optional-PLATFORM_NAME.pak (1003)
			else if (PakFileMatch.Groups[2].Success)
			{
				int ParsedChunkID = 0;
				if (int.TryParse(PakFileMatch.Groups[2].Value, out ParsedChunkID))
				{
					bWasParseSuccess = true;
					ChunkID = ParsedChunkID;
					Logger.LogInformation("Parsed pakchunk file: {FileName} with ChunkID: {ChunkID}.", FileName, ChunkID);
				}
			}
		}

		if (false == bWasParseSuccess)
		{
			Logger.LogInformation("Couldn't parse chunkID from filename: {FileName}", FileName);
		}

		return ChunkID;
	}

	//Returns true if FileName is a filetype that we expect to include the ChunkID in the filename for
	private static bool IsChunkIndexExpectedInNameForFileType(String FileName)
	{
		return (FileName.EndsWith(".pak")
			 || FileName.EndsWith(".utoc")
			 || FileName.EndsWith(".ucas"));
	}

	private class AddOnContentChunk
	{
		public int AddOnContentId;
		public string Tag;
		public List<string> FilterRules;
	}

	private static bool HasNrrFile(DirectoryReference StagingRootDirectory)
	{
		return DirectoryExists(DirectoryReference.Combine(StagingRootDirectory, ".nrr").FullName);
	}

	// Recursively adds FilterRules
	private void RecurseStagingDirectory(DirectoryReference CurrentDirectory, DirectoryReference StagingRootDirectory, Dictionary<int, AddOnContentChunk> AddOnContentChunks, List<int> InitialChunks, List<String> InitialChunkFilterRules)
	{
		foreach (FileReference File in DirectoryReference.EnumerateFiles(CurrentDirectory))
		{
			string RelativePath = File.MakeRelativeTo(StagingRootDirectory).Replace(@"\", @"/");
			int FileChunkIndex = GetChunkIndexForFile(File.GetFileName());
			if (AddOnContentChunks.ContainsKey(FileChunkIndex))
			{
				// Add the exclusion rule to the initial chunk
				InitialChunkFilterRules.Add("-\"^" + RelativePath + "\"");

				// Add the inclusion rule to this chunk
				AddOnContentChunks[FileChunkIndex].FilterRules.Add("+\"^" + RelativePath + "\"");
			}
			else if (!InitialChunks.Contains(FileChunkIndex))
			{
				// If this chunk will be in the base/initial nsp then add it to the initial chunk list if it is not already there
				InitialChunks.Add(FileChunkIndex);
			}
		}

		foreach (DirectoryReference SubDirectory in DirectoryReference.EnumerateDirectories(CurrentDirectory))
		{
			RecurseStagingDirectory(SubDirectory, StagingRootDirectory, AddOnContentChunks, InitialChunks, InitialChunkFilterRules);
		}
	}

	private Dictionary<int, AddOnContentChunk> GetAddOnContentChunks(DeploymentContext SC, ref List<string> InitialChunkFilterRules)
	{
		Dictionary<int, AddOnContentChunk> AddOnContentChunks = new Dictionary<int, AddOnContentChunk>();

		ConfigHierarchy GameIni = ConfigCache.ReadHierarchy(ConfigHierarchyType.Game, DirectoryReference.FromFile(SC.RawProjectPath), TargetIniPlatformType);

		bool bGenerateChunks = false;
		GameIni.GetBool("/Script/UnrealEd.ProjectPackagingSettings", "bGenerateChunks", out bGenerateChunks);
		// If generate chunks is disabled then bypass the list.
		if (!bGenerateChunks)
		{
			return AddOnContentChunks;
		}

		ConfigHierarchy EngineIni = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, DirectoryReference.FromFile(SC.RawProjectPath), TargetIniPlatformType);

		List<string> AddOnContentChunkList;
		EngineIni.GetArray(RuntimeSettingsName, "AddOnContentChunks", out AddOnContentChunkList);

		if (AddOnContentChunkList != null && AddOnContentChunkList.Count != 0)
		{
			// Remove parentheses
			AddOnContentChunkList = AddOnContentChunkList.Select(Chunk => Chunk.Trim("()".ToCharArray())).ToList();

			foreach (var Chunk in AddOnContentChunkList)
			{
				AddOnContentChunk ChunkEntry = new AddOnContentChunk();
				int ChunkId = -1;

				ChunkEntry.FilterRules = new List<string>();
				ChunkEntry.FilterRules.Add("-\".*\"");

				string[] ChunkProperties = Chunk.Split(",".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
				foreach (string ChunkProperty in ChunkProperties)
				{
					string[] ChunkPropertyPair = ChunkProperty.Split("=".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);

					if (ChunkPropertyPair.Length == 2)
					{
						switch (ChunkPropertyPair[0].ToLower())
						{
							case "chunkid":
								int.TryParse(ChunkPropertyPair[1].Trim("\"".ToCharArray()), out ChunkId);
								break;
							case "addoncontentid":
								int.TryParse(ChunkPropertyPair[1].Trim("\"".ToCharArray()), out ChunkEntry.AddOnContentId);
								break;
							case "tag":
								ChunkEntry.Tag = ChunkPropertyPair[1].Trim("\"".ToCharArray());
								break;
						}
					}
				}

				AddOnContentChunks.Add(ChunkId, ChunkEntry);
			}
		}

		List<int> InitialChunks = new List<int>();

		// Build up the filter rules and chunk lists.
		RecurseStagingDirectory(SC.StageDirectory, SC.StageDirectory, AddOnContentChunks, InitialChunks, InitialChunkFilterRules);

		if (InitialChunks.Count > 0)
		{
			File.WriteAllText(Path.Combine(SC.StageDirectory.FullName, "__chunks"), string.Join(",", InitialChunks.ToArray()));
		}

		// Create the chunk "manifests" for each AddOnContent from the base nsp
		foreach (KeyValuePair<int, AddOnContentChunk> AddOnContent in AddOnContentChunks)
		{
			// Create the manifest (currently one chunk per AddOnContent).
			File.WriteAllText(Path.Combine(SC.StageDirectory.FullName, "__chunks_" + AddOnContent.Value.AddOnContentId.ToString()), AddOnContent.Key.ToString());

			// Add the exclusion rule for this AddOnContent's chunk manifest to the base nsp
			InitialChunkFilterRules.Add("-\"^__chunks_" + AddOnContent.Value.AddOnContentId.ToString() + "\"");

			// Add the inclusion rule for this AddOnContent's chunk manifest
			AddOnContent.Value.FilterRules.Add("+\"^__chunks_" + AddOnContent.Value.AddOnContentId.ToString() + "\"");
		}

		return AddOnContentChunks;
	}

	/// <summary>
	/// Makes sure that the project meta file is configured to allow enough persistent local storage
	/// for IoStoreOnDemand caching. If the project is not set up for this or has not requested enough
	/// space then the meta file will be modified to satisfy our requirements.
	/// Since we need to know the sizes for cache storage and journaling at runtime, if we change these
	/// values we need to be able to read those changes at runtime which we do so by writing our a
	/// generated config file to the staging directory.
	/// </summary>
	/// <param name="Params">Parameters for the current project</param>
	/// <param name="MetaFilePath">Path to the .meta file to be modified</param>
	/// <param name="StageDirectory">Path to the staging directory</param>
	private void ApplyIoStoreOnDemandSettings(ProjectParams Params, string MetaFilePath, string StageDirectory)
	{
		Func<XDocument,string> ModifyContent = MetaDoc =>
		{
			XElement ApplicationNode = MetaDoc.Root.Element("Application");

			if (ApplicationNode == null)
			{
				return string.Empty;
			}

			{
				XElement IndexNode = ApplicationNode.Element("CacheStorageIndexMax");
				XElement SizeNode = ApplicationNode.Element("CacheStorageDataAndJournalSizeMax");
				if (IndexNode != null && SizeNode != null)
				{
					if (Convert.ToInt32(SizeNode.Value, 16) < 0x10400000)
					{
						SizeNode.Value = "0x10400000";
					}

					return MetaDoc.ToString();
				}
			}

			{
				XElement SizeNode = ApplicationNode.Element("CacheStorageSize");
				XElement JournalNode = ApplicationNode.Element("CacheStorageJournalSize");
				if (SizeNode != null && JournalNode != null)
				{
					if (Convert.ToInt32(SizeNode.Value, 16) < 0x10000000)
					{
						SizeNode.Value = "0x10000000";
					}

					if (Convert.ToInt32(JournalNode.Value, 16) < 0xD0000)
					{
						JournalNode.Value = "0x400000";
					}

					return MetaDoc.ToString();
				}
			}

			ApplicationNode.SetElementValue("CacheStorageIndexMax", "0x1");
			ApplicationNode.SetElementValue("CacheStorageDataAndJournalSizeMax", "0x10400000");

			return MetaDoc.ToString();
		};

		try
		{
			string UpdateMetaContents = ModifyContent(XDocument.Load(MetaFilePath));

			// ModifyContent will only return data if we need to modify the .meta file
			if (!string.IsNullOrEmpty(UpdateMetaContents))
			{
				WriteDefaultIOStorageValues(MetaFilePath, UpdateMetaContents, StageDirectory, Params.ShortProjectName);
				Logger.LogInformation("IoStoreOnDemand applied changes to persistent local storage settings to the project");
			}
		}
		catch (Exception Ex)
		{
			Logger.LogWarning("Error encountered while trying to apply persistent download data settings, IoStoreOnDemand may not work! [{Msg}]", Ex.Message);
		}
	}

	public override void Package(ProjectParams Params, DeploymentContext SC, int WorkingCL)
	{
		// get the project's ProgramId
		ConfigHierarchy Ini = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, (SC.ProjectRoot == null) ? null : SC.ProjectRoot, TargetIniPlatformType);
		string ProgramId = null;
		string ApplicationVersionString = null;
		string ApplicationVersion = null;
		string RuntimeAddOnContentInstall = null;
		string AddOnContentVersion = null;
		Ini.GetString(RuntimeSettingsName, "ProgramId", out ProgramId);
		Ini.GetString(RuntimeSettingsName, "ApplicationVersionString", out ApplicationVersionString);
		Ini.GetString(RuntimeSettingsName, "ApplicationVersion", out ApplicationVersion);
		Ini.GetString(RuntimeSettingsName, "RuntimeAddOnContentInstall", out RuntimeAddOnContentInstall);
		Ini.GetString(RuntimeSettingsName, "AddOnContentVersion", out AddOnContentVersion);
		if (string.IsNullOrEmpty(ProgramId))
		{
			throw new AutomationException("Unable to find ProgramId .ini setting");
		}

		// Set the AddOnContentVersion to the ApplicationVersion if AddOnContentVersion is not specified.
		if(string.IsNullOrEmpty(AddOnContentVersion))
		{
			AddOnContentVersion = ApplicationVersion;
		}

		// place an empty, magic file in the data directory to let the packaged game know this is a fully packaged rom image
		string RomFile = Path.Combine(SC.StageDirectory.FullName, "__rom");
		File.WriteAllText(RomFile, Params.ShortProjectName);

		string DescFile = NintendoExports.LocateDescFile(TargetPlatformType, Params.RawProjectPath);

		// Scan for chunk based AddOnContent
		List<string> InitialChunkFilterRules = new List<string>();
		Dictionary<int, AddOnContentChunk> AddOnContentChunks = GetAddOnContentChunks(SC, ref InitialChunkFilterRules);
		if (AddOnContentChunks.Count > 0 && Params.HasDLCName)
		{
			throw new BuildException("Unable to mix chunk based AddOnContent and plugin based AddOnContent in the same project.");
		}

		bool bRuntimeAddOnContentInstall = !string.IsNullOrEmpty(RuntimeAddOnContentInstall) && RuntimeAddOnContentInstall.Equals("AllowAppend", StringComparison.InvariantCultureIgnoreCase);

		bool bSucceeded = true;
		Parallel.For(0, SC.StageTargetConfigurations.Count, TargetConfigurationIdx =>
		{
			if (!bSucceeded) return;

			string LatestPatchNsp = "";

			if (Params.IsGeneratingPatch)
			{
				string PreviousPatch = "";
				if (GetOptionValue(Params.AdditionalPackageOptions, "PreviousPatch", ref PreviousPatch))
				{
					Logger.LogInformation("Looking for previous patch at {PreviousPatch} (from 'AdditionalPackageOptions').", PreviousPatch);
					if (File.Exists(PreviousPatch))
					{
						LatestPatchNsp = PreviousPatch;
					}
				}

				if (string.IsNullOrEmpty(LatestPatchNsp))
				{
					string PreviousPatchDir = "";

					if (!GetOptionValue(Params.AdditionalPackageOptions, "PreviousPatchDir", ref PreviousPatchDir))
					{
						PreviousPatchDir = CombinePaths(Params.GetBasedOnReleaseVersionPath(SC, Params.Client), "LatestPatch");
					}

					string LatestPatchNspName = GetNspName(Params, SC, SC.StageTargetConfigurations[TargetConfigurationIdx], "-patch");
					Logger.LogInformation("Looking for previous patch {LatestPatchNspName} in {PreviousPatchDir}.", LatestPatchNspName, PreviousPatchDir);

					if (FileExists(Path.Combine(PreviousPatchDir, LatestPatchNspName)))
					{
						LatestPatchNsp = Path.Combine(PreviousPatchDir, LatestPatchNspName);
					}
					else
					{
						Logger.LogInformation("Looking for TinyApp in {PreviousPatchDir}.", PreviousPatchDir);
						LatestPatchNspName = "TinyApp.nsp";

						if (!FileExists(Path.Combine(PreviousPatchDir, LatestPatchNspName)))
						{
							LatestPatchNsp = "";
							if (DirectoryExists(PreviousPatchDir))
							{
								Logger.LogInformation("Looking for possible patch NSPs under directory {PreviousPatchDir}.", PreviousPatchDir);

								foreach (String PkgFile in Directory.GetFiles(PreviousPatchDir, "*.nsp", SearchOption.AllDirectories))
								{
									LatestPatchNsp = PkgFile;
									break;
								}
							}
						}
					}
				}

				if (string.IsNullOrEmpty(LatestPatchNsp))
				{
					// this block searches the location where patch NSPs are stored when release versions are created.
					string PreviousPatchDir = PreviousPatchDir = Params.GetBasedOnReleaseVersionPath(SC, Params.Client);

					string LatestPatchNspName = GetNspName(Params, SC, SC.StageTargetConfigurations[TargetConfigurationIdx], "-patch");
					Logger.LogInformation("Looking for previous patch {LatestPatchNspName} in {PreviousPatchDir}.", LatestPatchNspName, PreviousPatchDir);

					if (FileExists(Path.Combine(PreviousPatchDir, LatestPatchNspName)))
					{
						LatestPatchNsp = Path.Combine(PreviousPatchDir, LatestPatchNspName);
					}
				}
				
				if (string.IsNullOrEmpty(LatestPatchNsp))
				{
					Logger.LogInformation("Did not find previous patch NSP in the above locations (expected if this is the first patch)");
				}
				else
				{
					Logger.LogInformation("Found previous patch NSP at {LatestPatchNsp}", LatestPatchNsp);
				}
			}

			string BaseNspName = GetNspName(Params, SC, SC.StageTargetConfigurations[TargetConfigurationIdx]);
			string OutputNsp = Path.Combine(GetPackageOutputDirectory(Params, SC), BaseNspName);

			int SuggestedApplicationVersion = !string.IsNullOrEmpty(LatestPatchNsp) ? GetNextReleaseVersion(LatestPatchNsp) : -1;
			string MetaFile = NintendoExports.GenerateMetaFile(TargetPlatformType, Params.RawProjectPath, SC.StageTargetConfigurations[TargetConfigurationIdx], false, SuggestedApplicationVersion);

			if (Params.ApplyIoStoreOnDemand)
			{
				ApplyIoStoreOnDemandSettings(Params, MetaFile, SC.StageDirectory.ToString());
			}

			// Capture this as we now need to wait until we rename the package during the merge packages step if we're doing it.
			bool bCreateNSPSucceeded = CreateNSP(SC, Params, TargetConfigurationIdx, ProgramId, MetaFile, DescFile, OutputNsp, Params.HasDLCName, bRuntimeAddOnContentInstall, ApplicationVersion, AddOnContentChunks, InitialChunkFilterRules);
			bSucceeded &= bCreateNSPSucceeded;

			if (AddOnContentChunks.Count > 0)
			{
				// Create the chunk based AddOnContent NSP at the same time as the main NSP
				string AOCOutputNsp = Path.Combine(GetPackageOutputDirectory(Params, SC), GetNspName(Params, SC, SC.StageTargetConfigurations[TargetConfigurationIdx], "_AOC"));
				
				bool bCreateAOCSucceeded = CreateNSP(SC, Params, TargetConfigurationIdx, ProgramId, MetaFile, DescFile, AOCOutputNsp, true, bRuntimeAddOnContentInstall, AddOnContentVersion, AddOnContentChunks, InitialChunkFilterRules);

				if (bCreateAOCSucceeded)
				{
					CreateAddOnContentInstallBatchFile(Params, OutputNsp, ProgramId, bRuntimeAddOnContentInstall);
				}
				bSucceeded &= bCreateAOCSucceeded;
			}

			if (bSucceeded)
			{
				// Copy the .desc file to the output folder so we have it if we need to run AuthoringTool at a later time, then ensure it's not read-only
				string DestinationDestFile = Path.Combine(GetPackageOutputDirectory(Params, SC), Path.GetFileNameWithoutExtension(BaseNspName) + ".desc");
				CopyFile(DescFile, DestinationDestFile, false);
				SetFileAttributes(DestinationDestFile, false);

				// Copy the .meta file to the output folder so we have it if we need to run AuthoringTool at a later time, then ensure it's not read-only
				string DestinationMetaFile = Path.Combine(GetPackageOutputDirectory(Params, SC), Path.GetFileNameWithoutExtension(BaseNspName) + ".meta");
				CopyFile(MetaFile, DestinationMetaFile, false);
				SetFileAttributes(DestinationMetaFile, false);

				Console.Write(MetaFile);
				// Export any files referenced in the .meta file. We'll need them if we rebuild the monolithic package (such as when we need to update the ApplicationVersion to do a BuildDiff against an arbitrary build).
				ExportResources(Params, SC, MetaFile, TargetConfigurationIdx);

				// copy the nss file to the package output directory if the package isn't being generated in the binaries folder
				string PackageRoot = Path.Combine(GetProjectDir(Params), "Saved", "Packages");
				if (OutputNsp.StartsWith(PackageRoot))
				{
					string InputNss = Path.Combine(Path.GetDirectoryName(Params.GetProjectExeForPlatform(TargetPlatformType).ToString()), SC.StageExecutables[TargetConfigurationIdx] + ".nss");
					string OutputNss = Path.Combine(Path.GetDirectoryName(OutputNsp), Path.GetFileName(InputNss));
					CopyFile(InputNss, OutputNss);
				}

				// Set up to generate JSON metadata if requested
				// @todo: Move this into PackagingParameters class
				JObject MetadataRootObject = null;

				if (HasPackagingOption(Params.AdditionalPackageOptions, SC.ProjectRoot, "GenerateMetadata"))
				{
					const string MetadataVersion = "2";

					Logger.LogInformation($"Generating version {MetadataVersion} metadata.");
					MetadataRootObject = new JObject();

					MetadataRootObject.Add("version", MetadataVersion);

					MetadataRootObject.Add("build_info", new JObject
					{
						{ "branch", GetOptionValue(Params.AdditionalPackageOptions, "Branch") },
						{ "changelist", GetOptionValue(Params.AdditionalPackageOptions, "Changelist") },
					});
				}

				// Merge multiple .nsp files into a single .nsp file if requested
				bool bMergedNsp = false;
				string MergedNspDescString = "";
				UnrealTargetPlatform MergedNspPlatform = TargetPlatform;

				// Build up information about all the .nsp that are requested to be merged
				string MergeInput = "";

				if (GetOptionValue(Params.AdditionalPackageOptions, "MergeNsps", ref MergeInput))
				{
					Logger.LogInformation("Beginning MergeNsps");

					UnrealTargetPlatform SwitchPlatform = UnrealTargetPlatform.Parse("Switch");
					MergedNspDescString = NintendoExports.LocateDescFile(SwitchPlatform, SC.ProjectRoot);
					MergedNspPlatform = SwitchPlatform;
					List<string> MergedNsps = new();

					string MergedNspPlatformInput = "";
					if (GetOptionValue(Params.AdditionalPackageOptions, "MergedNspPlatform", ref MergedNspPlatformInput))
					{
						MergedNspPlatform = UnrealTargetPlatform.Parse(MergedNspPlatformInput);

						List<(string MergeNspName, UnrealTargetPlatform MergeNspPlatform)> MergeNspsInfo = new();

						string[] MergeNspsAndPlatforms = MergeInput.Split(";");
						foreach (string entry in MergeNspsAndPlatforms)
						{
							string[] NspAndPlatform = entry.Split(",");
							MergeNspsInfo.Add((NspAndPlatform[0], UnrealTargetPlatform.Parse(NspAndPlatform[1])));
						}

						if (MergeNspsInfo.Count > 0)
						{
							foreach (var entry in MergeNspsInfo)
							{
								MergedNsps.Add(entry.Item1);
								MergedNspDescString += $@",{NintendoExports.LocateDescFile(entry.Item2, SC.ProjectRoot)}";
							}

							string InputNsp = Path.Combine(GetPackageOutputDirectory(Params, SC), BaseNspName.Replace("Switch", "SwitchOnly"));
							if (File.Exists(InputNsp))
							{
								File.Delete(InputNsp);
							}
							File.Move(OutputNsp, InputNsp);

							// Create install batch files for the original build
							CreateInstallBatchFile(SC, InputNsp, NintendoSDKPlatformIdentifier);
							CreateRunBatchFile(SC, InputNsp, NintendoSDKPlatformIdentifier);

							bMergedNsp = MergeNSP(OutputNsp, MergedNspPlatform, InputNsp, MergedNsps);
							if (bMergedNsp)
							{
								CreateInstallBatchFile(SC, OutputNsp, GetPlatformIdentifierFromTargetPlatform(MergedNspPlatform));
								CreateRunBatchFile(SC, OutputNsp, GetPlatformIdentifierFromTargetPlatform(MergedNspPlatform));
							}
							bSucceeded &= bMergedNsp;

							if (bMergedNsp && MetadataRootObject != null)
							{
								Logger.LogInformation("Adding merged_nsp info to metadata.");

								MetadataRootObject.Add("primary_nsp", new JObject
								{
									{ "name", Path.GetFileName(InputNsp) },
									{ "platform", TargetPlatform.ToString() },
									{ "desc_file", NintendoExports.LocateDescFile(TargetPlatform, SC.ProjectRoot) }
								});

								JArray MergedNspsEntries = new JArray(
									MergeNspsInfo.Select(t => new JObject
									{
									{ "name", t.Item1 },
									{ "platform", t.Item2.ToString() },
									{ "desc_file", NintendoExports.LocateDescFile(t.Item2, SC.ProjectRoot) }
									})
								);

								MetadataRootObject.Add("merged_nsps", MergedNspsEntries);
								MetadataRootObject.Add("package_platform", MergedNspPlatform.ToString());
							}
						}
						else
						{
							CreateInstallBatchFile(SC, OutputNsp, GetPlatformIdentifierFromTargetPlatform(MergedNspPlatform));
							CreateRunBatchFile(SC, OutputNsp, GetPlatformIdentifierFromTargetPlatform(MergedNspPlatform));
						}
					}
					else
					{
						Logger.LogWarning("-MergeNsps and -MergedNspPlatform must be provided to merge multiple nsps into a single package.");
					}
				}
				else
				{
					if (MetadataRootObject != null)
					{
						MetadataRootObject.Add("primary_nsp", new JObject
						{
							{ "name", Path.GetFileName(OutputNsp) },
							{ "platform", TargetPlatform.ToString() },
							{ "desc_file", NintendoExports.LocateDescFile(TargetPlatform, SC.ProjectRoot) }
						});

						MetadataRootObject.Add("package_platform", TargetPlatform.ToString());
					}

					CreateInstallBatchFile(SC, OutputNsp, GetPlatformIdentifierFromTargetPlatform(MergedNspPlatform));
					CreateRunBatchFile(SC, OutputNsp, GetPlatformIdentifierFromTargetPlatform(MergedNspPlatform));
				}

				if (Params.IsGeneratingPatch)
				{
					string ReleaseVersionNsp;
					string OriginalReleaseNSPPathSetting = "OriginalReleaseNSPPath";
					string OriginalReleaseNSPPathSettingSection = EditorSettingsName;

					if (Ini.GetString(OriginalReleaseNSPPathSettingSection, OriginalReleaseNSPPathSetting, out ReleaseVersionNsp))
					{
						Logger.LogInformation("Using Original Release NSP path from .ini setting: {ReleaseVersionNsp}", ReleaseVersionNsp);
						if (!Path.IsPathFullyQualified(ReleaseVersionNsp))
						{
							ReleaseVersionNsp = Path.GetFullPath(ReleaseVersionNsp, GetProjectDir(Params));
							Logger.LogInformation("Original Release NSP path is a relative path, however only fully qualified paths are supported by the underlying tool.  Prepended current Project directory: {ReleaseVersionNsp}", ReleaseVersionNsp);
						}
					}
					else
					{
						bool bFoundReleaseVersionNsp = false;

						ReleaseVersionNsp = CombinePaths(Params.GetBasedOnReleaseVersionPath(SC, Params.Client), "OriginalRelease");
						Logger.LogInformation("Searching BasedOnReleaseVersion path under the 'OriginalRelease' directory for Original Release NSP: {ReleaseVersionNsp}", ReleaseVersionNsp);

						if (Directory.Exists(ReleaseVersionNsp))
						{
							foreach (String PkgFile in Directory.GetFiles(ReleaseVersionNsp, "*.nsp", SearchOption.AllDirectories))
							{
								ReleaseVersionNsp = PkgFile;
								bFoundReleaseVersionNsp = true;
								break;
							}
						}

						if (!bFoundReleaseVersionNsp)
						{
							ReleaseVersionNsp = Params.GetOriginalReleaseVersionPath(SC, Params.Client);
							Logger.LogInformation("Searching OriginalReleaseVersionPath path for Original Release NSP: {ReleaseVersionNsp}", ReleaseVersionNsp);

							if (Directory.Exists(ReleaseVersionNsp))
							{
								foreach (String PkgFile in Directory.GetFiles(ReleaseVersionNsp, "*.nsp", SearchOption.AllDirectories))
								{
									ReleaseVersionNsp = PkgFile;
									bFoundReleaseVersionNsp = true;
									break;
								}
							}
						}

						if (!bFoundReleaseVersionNsp)
						{
							throw new AutomationException("Failed to automatically locate the Original Release NSP, consider setting Original Release Version" +
								" in Launch Profile or the setting {0} in the Platform Project Settings", OriginalReleaseNSPPathSetting);
						}
						else
						{
							Logger.LogInformation("Found candidate Original Release NSP at {ReleaseVersionNsp}", ReleaseVersionNsp);
						}

					}

					string PatchOutputNsp = Path.Combine(GetPackageOutputDirectory(Params, SC), GetNspName(Params, SC, SC.StageTargetConfigurations[TargetConfigurationIdx], "-patch"));

					string BaseOptions = string.Empty;

					if (HasOption(Params.AdditionalPackageOptions, "UsePatchingCache"))
					{
						string BuildStorageRootDir = CommandUtils.RootBuildStorageDirectory();
						string CacheDir = Path.Combine(BuildStorageRootDir, "Automation", Params.ShortProjectName, "Platforms", TargetPlatformType.ToString(), "PatchCache");

						BaseOptions += String.Format(" --cache-directory {0}", CacheDir);
					}

					// determine matching block size
					ConfigHierarchy GameIni = ConfigCache.ReadHierarchy(ConfigHierarchyType.Game, DirectoryReference.FromFile(SC.RawProjectPath), TargetPlatformType);
					int PatchBlockMatchingSize = -1;
					if (GetPackagingOption(Params.AdditionalPackageOptions, SC.ProjectRoot, "PatchMinimumMatchingSize", ref PatchBlockMatchingSize) && PatchBlockMatchingSize > 0)
					{
						BaseOptions += String.Format(" --minimum-matching-size {0}", PatchBlockMatchingSize);
					}

					string AdditionalOptions = BaseOptions;
					string DefragmentOptions = string.Empty;

					// determine defragmentation options
					bool Defragment = ShouldDefragmentPatch(Params.AdditionalPackageOptions, SC.ProjectRoot, ApplicationVersionString);
					// We will warn below if the defragment block size isn't set somewhere but will use a reasonable value during this pass
					int DefragmentBlockSize = 0;
					if (Defragment)
					{
						Logger.LogInformation("Patch defragmentation enabled.");

						bool bUseAutoDefragBlockSize = false;
						GetPackagingOption(Params.AdditionalPackageOptions, SC.ProjectRoot, "UseAutoPatchDefragmentBlockSize", ref bUseAutoDefragBlockSize);
						if (bUseAutoDefragBlockSize)
						{
							string JSONFilePath = Path.Combine(SC.ProjectRoot.GetDirectoryName(), "Platforms", TargetPlatformType.ToString(), "Build", "AutoDefrag.json");
							if (File.Exists(JSONFilePath))
							{
								string FileContents = File.ReadAllText(JSONFilePath);

								try
								{
									AutoPatchDefragmentData AutoPatchDefragmentData = JsonConvert.DeserializeObject<AutoPatchDefragmentData>(FileContents);
									DefragmentBlockSize = AutoPatchDefragmentData.AutoPatchDefragmentBlockSize;
									Logger.LogInformation("Using AutoPatchDefragment block size of {DefragmentBlockSize}.", DefragmentBlockSize);
								}
								catch (Exception Ex)
								{
									Logger.LogWarning("Exception {Ex} reading {JSONFilePath}. This must be fixed. Will use default value for this pass.", Ex, JSONFilePath);
								}
							}
							else
							{
								Logger.LogWarning("UseAutoDefragmentBlockSize was specified but {JSONFilePath} was not found. This must be fixed. Will use default value for this pass.", JSONFilePath);
							}
						}
						else
						{
							if (!GetPackagingOption(Params.AdditionalPackageOptions, SC.ProjectRoot, "PatchDefragmentBlockSize", ref DefragmentBlockSize))
							{
								Logger.LogWarning("PatchDefragmentBlockSize not found in {Platform}Engine.ini. This must be fixed.", TargetPlatformType.ToString());
							}
							else
							{
								Logger.LogInformation("Using PatchDefragmentBlockSize of {DefragmentBlockSize} from {Platform}Engine.ini", DefragmentBlockSize, TargetPlatformType.ToString());
							}
						}

						if (DefragmentBlockSize > 0)
						{
							AdditionalOptions += string.Format(" --defragment --defragment-size {0}", DefragmentBlockSize);
						}
						else
						{
							Logger.LogInformation("DefragmentBlockSize was set to 0. Patch will not be defragmented.");
							Defragment = false;
						}
					}

					// If we're merging .nsps the package and patch may not be the same platform type as the original package
					UnrealTargetPlatform PatchPlatform = bMergedNsp ? MergedNspPlatform : TargetPlatform;
					string PatchDesc = bMergedNsp ? MergedNspDescString : DescFile;

					if (CreatePatchNSP(SC, Params, TargetConfigurationIdx, ProgramId, PatchDesc, OutputNsp, ReleaseVersionNsp, LatestPatchNsp, PatchOutputNsp, PatchPlatform, AdditionalOptions))
					{
						CreateInstallAsPatchBatchFile(SC, PatchOutputNsp, ReleaseVersionNsp, GetPlatformIdentifierFromTargetPlatform(PatchPlatform));

						float FragmentationPct = 0.0f;
						GetPatchFragmentation(PatchOutputNsp, ReleaseVersionNsp, ref FragmentationPct, GetPlatformIdentifierFromTargetPlatform(PatchPlatform));

						float FragmentationPctThreshold = 0.0f;
						GetPackagingOption(Params.AdditionalPackageOptions, SC.ProjectRoot, "AutoPatchDefragmentFragmentationThreshold", ref FragmentationPctThreshold);

						// if AutoDefragment is requested check if the current fragmentation pct is above PatchAutoDefragmentFragmentationThreshold (if provided).
						// If so re-run the automatic defragmentation blocksize calculation. If a new defragmentation blocksize is generated rebuild the patch using that.
						if (HasPackagingOption(Params.AdditionalPackageOptions, SC.ProjectRoot, "AutoDefragment"))
						{
							int NewDefragmentBlockSize = 0;

							if (FragmentationPctThreshold == 0.0f)
							{
								Logger.LogWarning("AutoDefragment is enabled but AutoPatchDefragmentFragmentationThreshold not supplied. Skipping.");
							}
							else if (FragmentationPct > FragmentationPctThreshold)
							{
								Logger.LogInformation("AutoDefragment is enabled and fragmentation of {FragmentationPct}% is above threshold of {FragmentationPctThreshold}%. Checking if defragmentation block size {DefragmentBlockSize} needs increasing.", FragmentationPct, FragmentationPctThreshold, DefragmentBlockSize);

								if (GetDefragmentBlockSizeForTarget(FragmentationPctThreshold, OutputNsp, LatestPatchNsp, ReleaseVersionNsp, SC.ProjectRoot.ToString(), BaseOptions, ref NewDefragmentBlockSize))
								{
									if (NewDefragmentBlockSize != DefragmentBlockSize)
									{
										Logger.LogInformation("Autodefragment produced new defragment block size of {DefragmentBlockSize}.", NewDefragmentBlockSize);
										AdditionalOptions = BaseOptions;

										if (NewDefragmentBlockSize > 0)
										{
											AdditionalOptions += string.Format(" --defragment --defragment-size {0}", NewDefragmentBlockSize);
										}
										else
										{
											Defragment = false;
										}

										if (CreatePatchNSP(SC, Params, TargetConfigurationIdx, ProgramId, PatchDesc, OutputNsp, ReleaseVersionNsp, LatestPatchNsp, PatchOutputNsp, PatchPlatform, AdditionalOptions))
										{
											// recheck fragmentation pct
											GetPatchFragmentation(PatchOutputNsp, ReleaseVersionNsp, ref FragmentationPct, GetPlatformIdentifierFromTargetPlatform(PatchPlatform));
										}

										if (HasPackagingOption(Params.AdditionalPackageOptions, SC.ProjectRoot, "AutoDefragmentUpdateIni"))
										{
											// Save changed defragment block size to the platform's Engine.ini
											List<Tuple<string, string>> Options = new List<Tuple<string, string>> { new Tuple<string, string>("PatchDefragmentBlockSize", NewDefragmentBlockSize.ToString()) };
											UpdatePackagingOptions(SC.ProjectRoot, Options);
										}

										DefragmentBlockSize = NewDefragmentBlockSize;
									}
									else
									{
										Logger.LogInformation("AutoDefragment produced same defragmention block size of {DefragmentBlockSize}.", DefragmentBlockSize);
									}
								}
								else
								{
									Logger.LogWarning("AutoDefragment failed!");
								}
							}
						}

						if ((FragmentationPctThreshold > 0 && FragmentationPct > FragmentationPctThreshold) && P4Env.Branch.StartsWith("//Fortnite/Release"))
						{
							Logger.LogWarning("Patch NSP {PatchNsp} is {1:N2}% fragmented, above target of {1.N2}%.", PatchOutputNsp, FragmentationPct, FragmentationPctThreshold);
						}
						else
						{
							Logger.LogInformation("Patch NSP {PatchNsp} is {1:N2}% fragmented.", PatchOutputNsp, FragmentationPct);
						}

						if (MetadataRootObject != null)
						{
							Logger.LogInformation("Adding patch info to metadata.");

							string PatchCommandLine = "";

							if (Defragment)
							{
								PatchCommandLine += "--defragment";
								if (DefragmentBlockSize > 0)
								{
									PatchCommandLine += $@" --defragment-size {DefragmentBlockSize.ToString()}";
								}
							}

							if (PatchBlockMatchingSize > 0)
							{
								if (PatchCommandLine.Length > 0)
								{
									PatchCommandLine += " ";
								}
								PatchCommandLine += $@"--minimum-matching-size {PatchBlockMatchingSize.ToString()}";
							}

							JObject PatchSection = new JObject();
							PatchSection.Add("command_line", PatchCommandLine);

							string AdditionalPatchMetadata = new string("");
							if (GetOptionValue(Params.AdditionalPackageOptions, "AdditionalPatchMetadata", ref AdditionalPatchMetadata))
							{
								var AdditionalPatchMetadataKVs = JsonConvert.DeserializeObject<Dictionary<string, string>>(AdditionalPatchMetadata);
								AdditionalPatchMetadataKVs.ToList().ForEach(x => PatchSection.Add(x.Key, x.Value));
							}

							string PackageBaseName = GetFilenameWithoutAnyExtensions(LatestPatchNsp);

							if (FindAndLoadPackagingParameters(LatestPatchNsp, out PackagingParameters LatestPatchMetadata))
							{
								PatchSection.Add("previous_patch_hash", GetPropertyValue(LatestPatchNsp, "Digest", GetPlatformIdentifierFromTargetPlatform(LatestPatchMetadata.PackagePlatform)));

								PatchSection.Add("previous_patch_build_info", new JObject
								{
									{ "branch", LatestPatchMetadata.PrimaryBuildInfo != null ? LatestPatchMetadata.PrimaryBuildInfo.branch : "Unknown" },
									{ "changelist", LatestPatchMetadata.PrimaryBuildInfo != null ? LatestPatchMetadata.PrimaryBuildInfo.changelist : "00000000" },
								});
							}
							else
							{
								Logger.LogWarning($"Unable to find packaging data for {LatestPatchNsp}. Metadata section will be incomplete!");
							}

							MetadataRootObject.Add("patch", PatchSection);

							if (HasPackagingOption(Params.AdditionalPackageOptions, SC.ProjectRoot, "GeneratePatchInfo"))
							{
								// Generate PatchInfo metadata
								Dictionary<string, double> PatchInfo = new Dictionary<string, double>();
								if (DefragmentBlockSize > 0)
								{
									PatchInfo.Add("DefragmentBlockSize", DefragmentBlockSize);
								}
								PatchInfo.Add("FragmentationPercentage", FragmentationPct);

								string JsonContent = JsonConvert.SerializeObject(PatchInfo);
								string MetadataFile = Path.Combine(GetPackageOutputDirectory(Params, SC), Path.GetFileNameWithoutExtension(BaseNspName) + "-PatchInfo.json");
								Logger.LogInformation($"Generating defragmentation info file {MetadataFile}.");
								File.WriteAllText(MetadataFile, JsonContent);
							}
						}
					}
					else
					{
						bSucceeded = false;
					}
				}

				if (MetadataRootObject != null)
				{
					MetadataRootObject.Add("nintendo_sdk_root", NintendoExports.GetSDKInstallLocation());

					string AdditionalMetadata = new string("");
					if (GetOptionValue(Params.AdditionalPackageOptions, "AdditionalMetadata", ref AdditionalMetadata))
					{
						var AdditionalMetadataKVs = JsonConvert.DeserializeObject<Dictionary<string, string>>(AdditionalMetadata);
						AdditionalMetadataKVs.ToList().ForEach(x => MetadataRootObject.Add(x.Key, x.Value));
					}

					string MetadataFile = Path.Combine(GetPackageOutputDirectory(Params, SC), Path.GetFileNameWithoutExtension(BaseNspName) + "-PackagingParameters.json");
					Logger.LogInformation("Generating Metadata file {MetadataFile}.", MetadataFile);
					File.WriteAllText(MetadataFile, MetadataRootObject.ToString());
				}
				else
				{
					Logger.LogInformation("No Metadata to write.");
				}
			}

			PrintRunTime();

			if (!bSucceeded)
			{
				return;
			}
		});

		File.Delete(RomFile);

		if (!bSucceeded)
		{
			throw new AutomationException("One or more authoring steps failed");
		}
	}

	private void ExportResources(ProjectParams Params, DeploymentContext SC, string MetadataFile, int TargetConfigurationIdx)
	{
		string BuildConfigName = SC.StageTargetConfigurations[TargetConfigurationIdx].ToString();
		string TargetResourceDirectory = Path.Combine(GetPackageOutputDirectory(Params, SC), "Resources", BuildConfigName);

		// Go through the .meta file and copy any files or directory referenced in it.
		XDocument Metadata = XDocument.Load(MetadataFile);
		XElement PathNode = null;

		PathNode = Metadata.XPathSelectElement("//Application/LegalInformationFilePath");
		if (PathNode != null)
		{
			string LegalInformationFile = PathNode.Value;
			if (FileExists(LegalInformationFile))
			{
				string DestinationAccessibleUrlsFile = Path.Combine(TargetResourceDirectory, "LegalInformation.zip");

				CopyFile(LegalInformationFile, DestinationAccessibleUrlsFile, false);
				SetFileAttributes(DestinationAccessibleUrlsFile, false);
			}
			else
			{
				Logger.LogInformation("{0} doesn't exist.", LegalInformationFile);
			}
		}

		PathNode = Metadata.XPathSelectElement("//Application/AccessibleUrlsFilePath");
		if (PathNode != null)
		{
			string AccessibleUrlsFilePath = PathNode.Value;
			if (FileExists(AccessibleUrlsFilePath))
			{
				string DestinationAccessibleUrlsFile = Path.Combine(TargetResourceDirectory, "AccessibleUrls.txt");

				CopyFile(AccessibleUrlsFilePath, DestinationAccessibleUrlsFile, false);
				SetFileAttributes(DestinationAccessibleUrlsFile, false);
			}
			else
			{
				Logger.LogInformation("{0} doesn't exist.", AccessibleUrlsFilePath);
			}
		}

		PathNode = Metadata.XPathSelectElement("//Application/HtmlDocumentPath");
		if (PathNode != null)
		{
			string HTMLDocumentsDirectory = PathNode.Value;
			if (Directory.Exists(HTMLDocumentsDirectory))
			{
				string DestinationHTMLDocumentsDirectory = Path.Combine(TargetResourceDirectory, "HTMLDocuments");
				CloneDirectory(HTMLDocumentsDirectory, DestinationHTMLDocumentsDirectory);
			}
			else
			{
				Logger.LogInformation("Directory {0} doesn't exist.", HTMLDocumentsDirectory);
			}
		}

		IEnumerable<XElement> PathNodes = Metadata.XPathSelectElements("//Application/Icon");
		Logger.LogInformation("Copying {0} Icon files.",PathNodes.Count());
		if (PathNodes.Count() > 0)
		{
			string IconFilesDestinationDirectory = Path.Combine(TargetResourceDirectory, "Icons");
			if(!Directory.Exists(IconFilesDestinationDirectory))
			{
				Directory.CreateDirectory(IconFilesDestinationDirectory);
			}

			foreach (XElement Node in PathNodes)
			{
				XElement IconPathNode = Node.Element("IconPath");
				string IconFilename = IconPathNode.Value;
				if (File.Exists(IconFilename))
				{
					string DestinationIconFile = Path.Combine(TargetResourceDirectory,"Icons", Path.GetFileName(IconFilename));
					if (!File.Exists(DestinationIconFile))
					{
						CopyFile(IconFilename, DestinationIconFile, false);
					}
					else
					{
						Logger.LogInformation("{0} already exists.", DestinationIconFile);
					}
				}
				else
				{
					Logger.LogInformation("{0} doesn't exists.", IconFilename);
				}
			}
		}

	}

	public static bool MergeNSP(string OutputNsp, UnrealTargetPlatform MergePlatform, string PrimaryNsp, List<string> MergedNsps)
	{
		string MergedNspPlatformName = GetPlatformIdentifierFromTargetPlatform(MergePlatform);

		string Command = Path.Combine(NintendoExports.GetSDKInstallLocation(), @"Tools\CommandLineTools\AuthoringTool\AuthoringTool.exe");
		string CommandLine = $@"mergensp -o {OutputNsp} --platform {MergedNspPlatformName} {PrimaryNsp} " + String.Join(" ", MergedNsps.ToArray());
		
		return TEMP_RunViaCmdAndCaptureOutput(Command, CommandLine);
	}

	private bool CreateNSP(DeploymentContext SC, ProjectParams Params, int TargetConfigurationIdx,
		string ProgramId, string MetaFile, string DescFile, string OutputNsp, bool bAddOnContentNSP, bool bRunTimeAddOnContentInstall,
		string ApplicationVersion, Dictionary<int, AddOnContentChunk> AddOnContentChunks, List<string> InitialChunkFilterRules)
	{
		string FilterRulesFile = "";

		// Output to project directory (input may be Engine/Binaries)
		string InputNspd = Path.Combine(Path.GetDirectoryName(Params.GetProjectExeForPlatform(TargetPlatformType).ToString()), SC.StageExecutables[TargetConfigurationIdx] + ".nspd");

		// If packaging DLC (AddOnContent in Nintendo parlance), a different meta is needed file
		if (bAddOnContentNSP)
		{
			Dictionary<int, string> AddOnContentList = new Dictionary<int, string>();
			FileReference OverrideMetaFile = null;
			FileReference BaseFilterRules = null;
			DirectoryReference StageRoot = null;
			string AddOnContentVersion = "";
			if (Params.HasDLCName)
			{
				PluginDescriptor AddOnContentPluginDescriptor = PluginDescriptor.FromFile(Params.DLCFile);
				AddOnContentVersion = AddOnContentPluginDescriptor.Version.ToString();
				OverrideMetaFile = FileReference.Combine(Params.DLCFile.Directory, $@"Resources\{TargetPlatformType}\Override-AddOnContent.nmeta");

				// Defaulting to Add-On Content index 1.  This can be changed per DLC in the PluginName/Resources/PLATFORM_NAME/Override-AddOnContent.nmeta file.
				AddOnContentList.Add(1, Params.DLCFile.GetFileNameWithoutExtension());
			}
			else
			{
				AddOnContentVersion = ApplicationVersion;
				StageRoot = SC.StageDirectory;
				BaseFilterRules = FileReference.Combine(SC.ProjectRoot, $@"Intermediate\Build\{TargetPlatformType}\AOCFilterRules");

				foreach (KeyValuePair<int, AddOnContentChunk> AddOnContentChunk in AddOnContentChunks)
				{
					AddOnContentList.Add(AddOnContentChunk.Value.AddOnContentId, AddOnContentChunk.Value.Tag);
					File.WriteAllLines(BaseFilterRules.FullName + "_" + AddOnContentChunk.Value.AddOnContentId.ToString(), AddOnContentChunk.Value.FilterRules);
				}
			}
			MetaFile = NintendoExports.GenerateAddOnContentMetaFile(TargetPlatformType, Params.RawProjectPath, AddOnContentList, 
				OverrideMetaFile, AddOnContentVersion, Path.GetFileNameWithoutExtension(OutputNsp), StageRoot, BaseFilterRules);
		}
		else
		{
			// Write the initial chunk filter rules
			// for now, the files needed to create the .nsp with code and content are in a .nspd directory
			List<string> FilterRules = new List<string>();

			// create an exclusion for a staged binaries directory if it exists
			FilterRules.Add("-\"^" + SC.RelativeProjectRootForStage.ToString() + "/Binaries" + "\"");

			// create an exclusion for the Build directory where debug symbols are located if it exists
			FilterRules.Add("-\"^" + SC.RelativeProjectRootForStage.ToString() + "/Build" + "\"");

			// staged binaries and Build directory end up in a different location for BP only projects
			FilterRules.Add("-\"^" + "Engine/Binaries" + "\"");
			FilterRules.Add("-\"^" + "Engine/Build" + "\"");

			// exclude nrs folder
			FilterRules.Add("-\"^nrs\"");

			// exclude module info not used by this build config.
			FilterRules.Add("-\"ModuleInfo-\\w*?\\.nrr\"");
			FilterRules.Add($"+\"ModuleInfo-{SC.StageTargetConfigurations[TargetConfigurationIdx].ToString()}\\.nrr\"");

			// if not packaging a shipping build create an inclusion for the debug symbols if they exist
			if (SC.StageTargetConfigurations[TargetConfigurationIdx] != UnrealTargetConfiguration.Shipping)
			{
				FilterRules.Add("+\"^" + SC.RelativeProjectRootForStage.ToString() + $"/Build/{TargetPlatformType}/Symbols/" + SC.StageTargetConfigurations[TargetConfigurationIdx].ToString() + "-Symbols.bin" + "\"");
			}

			// add any user defined filter rules with the ProjectRootForStage as the base
			FileReference UserFilterRulesFilename = FileReference.Combine(SC.ProjectRoot, "Platforms", SC.PlatformDir, "Build", string.Format("FilterRules-{0}.txt", SC.StageTargetConfigurations[TargetConfigurationIdx].ToString()));
			if (!FileReference.Exists(UserFilterRulesFilename))
			{
				UserFilterRulesFilename = FileReference.Combine(SC.ProjectRoot, "Build", SC.PlatformDir, string.Format("FilterRules-{0}.txt", SC.StageTargetConfigurations[TargetConfigurationIdx].ToString()));
			}
			if (FileReference.Exists(UserFilterRulesFilename))
			{
				Logger.LogInformation("Applying user filter exclusion rules file {UserFilterRulesFilename}", UserFilterRulesFilename);
				string[] UserFilterRules = FileReference.ReadAllLines(UserFilterRulesFilename);
				foreach (string UserFilterRule in UserFilterRules)
				{
					if (!string.IsNullOrEmpty(UserFilterRule))
					{
						FilterRules.Add(string.Format("-\"^{0}/{1}\"", SC.RelativeProjectRootForStage.ToString(), UserFilterRule));
					}
				}
			}

			// Copy over the rules to exclude any AddOnContent chunks from the initial/base NSP.
			FilterRules.AddRange(InitialChunkFilterRules);

			FilterRulesFile = Path.GetTempFileName();
			File.WriteAllLines(FilterRulesFile, FilterRules);
		}

		// make sure it can be written, and not in use
		if (File.Exists(OutputNsp))
		{
			try
			{
				File.Delete(OutputNsp);
			}
			catch (Exception)
			{
				// terminate running app in case it helps!
				foreach (string DeviceName in Params.DeviceNames)
				{
					KillCurrentRunningProcess(GetTargetNameFromDeviceName(DeviceName));
				}

				// and try again (letting this throw all the way out)
				File.Delete(OutputNsp);
			}
		}

		// update the .npdm file that is inside the .nspd - this is especially important for content only projects to update
		string NpdmFile = Path.Combine(InputNspd, "program0.ncd\\code\\main.npdm");
		string BackupNpdmFile = Path.GetTempFileName();
		// back it up
		File.Delete(BackupNpdmFile);
		File.Copy(NpdmFile, BackupNpdmFile);

		// DLC (AddOnContent in Nintendo parlance) does not have any code, don't need to do this
		if (!bAddOnContentNSP)
		{
			// run MakeMeta to make a new .npdm
			string MakeMetaExe = Path.Combine(NintendoExports.GetSDKInstallLocation(), "Tools/CommandLineTools/MakeMeta/MakeMeta.exe");
			if (Run(MakeMetaExe, NintendoExports.GetMakeMetaCommandline(MetaFile, DescFile, NpdmFile, AuthoringToolPlatformArg)).ExitCode > 0)
			{
				// restore Npdm before we crap out
				File.Delete(NpdmFile);
				File.Copy(BackupNpdmFile, NpdmFile);
				Console.WriteLine("'MakeMeta.exe' failed to execute properly.");
				return false;
			}
		}

		// now package it up!
		string Command = Path.Combine(NintendoExports.GetSDKInstallLocation(), @"Tools\CommandLineTools\AuthoringTool\AuthoringTool.exe");
		string CommandLine;

		// Call Authoring tool creatensp with different parameters if this is DLC (AddOnContent in Nintendo parlance)
		if (bAddOnContentNSP)
		{
			CommandLine = string.Format("creatensp -o \"{0}\" --save-adf --type AddOnContent  --meta \"{1}\" {2}", OutputNsp, MetaFile, AuthoringToolPlatformArg);
		}
		else
		{
			string NssExtra = "";
			if (NintendoExports.HasRefactoredSDK())
			{
				NssExtra = $"--nss \"{Path.ChangeExtension(InputNspd, ".nss")}\"";
			}

			string NroExtra = "";
			if (HasNrrFile(SC.StageDirectory))
			{
				NroExtra = String.Format("--nro \"{0}\"", DirectoryReference.Combine(SC.StageDirectory, "nro"));

				// Add the remapped nrs files
				if (NintendoExports.GetSDKVersionInt() >= NintendoExports.VersionXYZToInt("18","2","0"))
				{
					DirectoryReference NrsDir =  DirectoryReference.Combine(SC.StageDirectory, "nrs");
					if (DirectoryReference.Exists(NrsDir))
					{
						foreach (FileReference NrsFile in DirectoryReference.EnumerateFiles(NrsDir, "*.nrs"))
						{
							NroExtra += $" --nrs \"{NrsFile.FullName}\"";
						}
					}
				}
			}

			CommandLine = $"creatensp -o \"{OutputNsp}\" --type Application {AuthoringToolPlatformArg} --desc \"{DescFile}\" ";
			CommandLine += $"--meta \"{MetaFile}\" --filter \"{FilterRulesFile}\" --program \"{InputNspd}\\program0.ncd\\code\" \"{SC.StageDirectory}\" ";
			CommandLine += $"{NssExtra} {NroExtra} ";

			{
				// Native SDK Rom Compression
				bool bUseNativeCompression = false;
				// when enabled, compress .pak and .ucas files by default
				string DefaultNativeCompressionConfig = "r:.*\\.pak:Lz4:16||r:.*\\.ucas:Lz4:16";
				string NativeCompressionConfig;

				ConfigHierarchy GameIni = ConfigCache.ReadHierarchy(ConfigHierarchyType.Game, DirectoryReference.FromFile(Params.RawProjectPath), TargetPlatformType);
				GameIni.TryGetValue("/Script/UnrealEd.ProjectPackagingSettings", "bUseNativeCompression", out bUseNativeCompression);

				if (!GameIni.TryGetValue("/Script/UnrealEd.ProjectPackagingSettings", "NativeCompressionConfig", out NativeCompressionConfig))
				{
					NativeCompressionConfig = DefaultNativeCompressionConfig;
				}

				if (bUseNativeCompression)
				{
					Logger.LogInformation("Packaging NSP with native ROM compression with configuration: \"{NativeCompressionConfig}\"", NativeCompressionConfig);

					bool bUsingEngineCompression = false;
					GameIni.TryGetValue("/Script/UnrealEd.ProjectPackagingSettings", "bCompressed", out bUsingEngineCompression);
					if (bUsingEngineCompression)
					{
						Logger.LogWarning("Using native ROM compression and Engine compression simultaneously");
					}

					CommandLine += string.Format(" --compression --compression-type None --compression-file-config \"{0}\"", NativeCompressionConfig);
				}
			}
		}

		if( CommandUtils.IsBuildMachine)
		{
			CommandLine += " --execute_parallel " + Math.Min(Utils.GetLogicalProcessorCount(), 8).ToString();
		}

		if (!TEMP_RunViaCmdAndCaptureOutput(Command, CommandLine))
		{
			// restore Npdm before we crap out
			File.Delete(NpdmFile);
			File.Copy(BackupNpdmFile, NpdmFile);
			Console.WriteLine("'AuthoringTool.exe creatensp' failed to execute properly.");
			return false;
		}


		// clean up temps
		File.Delete(NpdmFile);
		File.Copy(BackupNpdmFile, NpdmFile);
		if (!string.IsNullOrEmpty(FilterRulesFile))
		{
			File.Delete(FilterRulesFile);
		}

		return true;
	}

	private bool CreatePatchNSP(DeploymentContext SC, ProjectParams Params, int TargetConfigurationIdx,string ProgramId, string DescFile,
		string CurrentVersionNsp, string ReleaseVersionNsp, string LatestPatchNsp, string PatchNsp, UnrealTargetPlatform PatchPlatform, string AdditonalToolOptions)
	{
		// now generate a patch :)
		string Command = Path.Combine(NintendoExports.GetSDKInstallLocation(), @"Tools\CommandLineTools\AuthoringTool\AuthoringTool.exe");
		string PatchCommandLine = string.Format("makepatch -o \"{0}\" --desc \"{1}\" --original \"{2}\" --current \"{3}\" --platform {4}",
			PatchNsp, DescFile, ReleaseVersionNsp, CurrentVersionNsp, GetPlatformIdentifierFromTargetPlatform(PatchPlatform));

		if (!string.IsNullOrEmpty(LatestPatchNsp))
		{
			PatchCommandLine += string.Format(" --previous \"{0}\"", LatestPatchNsp);
		}

		if (!string.IsNullOrWhiteSpace(AdditonalToolOptions))
		{
			PatchCommandLine += $" {AdditonalToolOptions.Trim()}";
		}

		if (CommandUtils.IsBuildMachine)
		{
			PatchCommandLine += " --execute_parallel " + Math.Min(Utils.GetLogicalProcessorCount(), 8).ToString();
		}

		Logger.LogInformation("Generating patch with command line: {Command} {PatchCommandLine}", Command, PatchCommandLine);

		IProcessResult Result = Run(Command, PatchCommandLine);
		if (Result.ExitCode > 0)
		{
			File.Delete(PatchNsp);
			File.Delete(Path.ChangeExtension(PatchNsp, "intermediate.nsp"));

			// Detect these common errors and offer guidance on how to resolve
			if (Result.Output.Contains("release version number") && Result.Output.Contains("should be larger than that"))
			{
				Logger.LogError("'AuthoringTool.exe makepatch' failed with ReleaseVersion error.  The release version is read from the 'ApplicationVersion' setting"
					+ " in the Editor's Project Settings.  See output from AuthoringTool.exe above for more information.");
			}
			if (Result.Output.Contains("Original is not Application"))
			{
				Logger.LogError("'AuthoringTool.exe makepatch' failed as the NSP used for the original release version was not the original application.  Override the"
					+ " location of the Original version with the setting 'OriginalReleaseNSPPath' in the PLATFORM_NAME Platform Project Settings.  See output from"
					+ " AuthoringTool.exe above for more information.");
			}
			else
			{
				Logger.LogError("'AuthoringTool.exe makepatch' failed to execute properly (Exit code = {Arg0}).", Result.ExitCode);
			}
			return false;
		}

		return true;
	}

	
	private string GetPropertyValue(string NspName, string PropertyEntryName, string OverridePlatform = null)
	{
		string AuthoringToolPath = Path.Combine(NintendoExports.GetSDKInstallLocation(), @"Tools\CommandLineTools\AuthoringTool\AuthoringTool.exe");
		string Platform = OverridePlatform != null ? OverridePlatform : GetPlatformIdentifierFromTargetPlatform(TargetPlatform);
		string CommandLine = $"getproperty --xml --platform {Platform} {NspName}";

		int ExitCode;
		string Results = Utils.RunLocalProcessAndReturnStdOut(AuthoringToolPath, CommandLine, out ExitCode);

		if (ExitCode == 0)
		{
			XDocument ResultsAsXML = XDocument.Parse(Results);
			if (ResultsAsXML != null)
			{
				XElement PropertyElement = ResultsAsXML.Root.Element("Property");
				if (!PropertyElement.IsEmpty)
				{
					XElement PropertyEntryElement = PropertyElement.Element(PropertyEntryName);
					if (!PropertyEntryElement.IsEmpty)
					{
						return PropertyEntryElement.Value;
					}
					else
					{
						Logger.LogInformation($"Property element is empty.");
						Logger.LogInformation(Results);
						return "";
					}
				}
				else
				{
					Logger.LogInformation($"Property element not found.");
					Logger.LogInformation(Results);
					return "";
				}
			}
			else
			{
				Logger.LogInformation($"Failed to parse getproperty results.");
				Logger.LogInformation(Results);
				return "";
			}
		}
		else
		{
			Logger.LogInformation($"AuthoringTool failed to run getproperty on {NspName}");
			Logger.LogInformation(Results);
			return "";
		}
	}

	private int GetNextReleaseVersion(string NspName, string OverridePlatform = null)
	{
		if( string.IsNullOrEmpty(OverridePlatform))
		{
			PackagingParameters NspParameters;
			if(FindAndLoadPackagingParameters(NspName, out NspParameters))
			{
				OverridePlatform = GetPlatformIdentifierFromTargetPlatform(NspParameters.PackagePlatform);
			}
		}

		if(OverridePlatform == null)
		{
			Logger.LogWarning($"Package platform was not supplied or found for {NspName}. GetNextReleaseVersion will return -1.");
			return -1;
		}

		string ReleaseString = GetPropertyValue(NspName, "ReleaseVersion", OverridePlatform);
		return !string.IsNullOrEmpty(ReleaseString) ? int.Parse(ReleaseString) +1 : -1;
	}

	private void AnalyzePatchNSP(string PatchNsp, string ReleaseVersionNsp, string OverridePlatform)
	{
		float FragmentationPct = 0.0f;

		if( GetPatchFragmentation(PatchNsp, ReleaseVersionNsp, ref FragmentationPct, OverridePlatform) && FragmentationPct >= 12.0f)
		{
			Logger.LogWarning("Patch NSP {PatchNsp} is {1:N2}% fragmented. Consider defragmenting to improve I/O performance!", PatchNsp, FragmentationPct);
		}
	}

	public bool GetPatchFragmentation(string PatchNsp, string ReleaseVersionNsp, ref float FragmentationPct, string OverridePatchPlatform)
	{
		string Platform = !string.IsNullOrEmpty(OverridePatchPlatform) ? OverridePatchPlatform : NintendoSDKPlatformIdentifier;

		FragmentationPct = 0.0f;

		string PatchAnalyzerPath = CombinePaths(CmdEnv.LocalRoot, "Engine/Platforms/Nintendo/Binaries/DotNET/NintendoPatchAnalyzer.exe");
		string CommandLine = string.Format("-nsp=\"{0}\" -originalNsp=\"{1}\" -platform={2}", PatchNsp, ReleaseVersionNsp, Platform);

		try
		{
			IProcessResult Result = Run(PatchAnalyzerPath, CommandLine);

			if (Result.ExitCode != 0)
			{
				throw new Exception("NintendoPatchAnalyzer failed to execute properly.");
			}

			string FindStr = "Fragmentation percentage: ";

			int Index0 = Result.Output.IndexOf(FindStr);
			if (Index0 == -1)
			{
				throw new Exception("Fragmentation string not found.");
			}

			Index0 += FindStr.Length;
			int Index1 = Result.Output.IndexOf("%", Index0);
			if (Index1 == -1)
			{
				throw new Exception("Fragmentation string not found.");
			}

			string FragmentationPctString = Result.Output.Substring(Index0, Index1 - Index0);

			float.TryParse(FragmentationPctString, out FragmentationPct);
			CommandUtils.Telemetry.Add($"{TargetPlatformType}PatchFragmentation", FragmentationPct, TelemetryUnits.Percentage);
			return true;
		}
		catch (Exception Ex)
		{
			Logger.LogWarning("Error gathering fragmentation information. {0}.", Ex.Message);
			return false;
		}
	}

	public bool GetFragmentationForBlockSizes(string BuildPackage, string PreviousPatchPackage, string BaseBuildPackage, string ProjectRoot, string PackagingOptions, List<int> DefragmentBlockSizes, bool bIncludePatchSize, ref List<Tuple<int /*Block Size*/, float /*Fragmentation Pct*/, float /*Patch Size MB*/>> Results)
	{
		Results = new List<Tuple<int, float, float>>();

		// get the project's ProgramId
		ConfigHierarchy Ini = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, new DirectoryReference(ProjectRoot), PlatformType);
		string ProgramId = null;
		if (!Ini.TryGetValue(RuntimeSettingsName, "ProgramId", out ProgramId))
		{
			Logger.LogWarning("Unable to find ProgramId in Config.");
			return false;
		}

		string DescFile = NintendoExports.LocateDescFile(TargetPlatformType, new DirectoryReference(ProjectRoot));
		string PatchNameRoot = Path.GetFileNameWithoutExtension(BuildPackage);

		List<Tuple<int, float, float>> ResultsByBlockSize = new List<Tuple<int, float, float>>();

		Parallel.ForEach(DefragmentBlockSizes, (DefragmentBlockSize) =>
		{
			string AdditionalPackagingOptions = PackagingOptions;

			// add defragmentation options
			if (DefragmentBlockSize > 0)
			{
				AdditionalPackagingOptions += string.Format(" --defragment --defragment-size {0}", DefragmentBlockSize);
			}

			string TempPatchNsp = Path.Combine(Globals.TempDir, PatchNameRoot + "-patch-" + DefragmentBlockSize.ToString() + ".nsp");

			if (CreateTemporaryPatchNSP(TempPatchNsp, BuildPackage, PreviousPatchPackage, BaseBuildPackage, AdditionalPackagingOptions, ProgramId, DescFile))
			{
				float FragmentationPct = 0.0f;

				if (GetPatchFragmentation(TempPatchNsp, BaseBuildPackage, ref FragmentationPct, null))
				{
					float PatchSizeMB = 0.0f;
					if (bIncludePatchSize)
					{
						float BuildSizeMB = 0.0f;
						GetPatchAndBuildSize(TempPatchNsp, PreviousPatchPackage, BaseBuildPackage, ref PatchSizeMB, ref BuildSizeMB);
					}
					ResultsByBlockSize.Add(new Tuple<int, float, float>(DefragmentBlockSize, FragmentationPct, PatchSizeMB));
				}
			}
		
		});

		Results = ResultsByBlockSize;

		return true;
	}

	public bool GetDefragmentBlockSizeForTarget(float DefragmentThreshold, string BuildPackage, string PreviousPatchPackage, string BaseBuildPackage, string ProjectRoot, string PackagingOptions, ref int DefragmentBlockSize)
	{
		Logger.LogInformation("Attempting to find best DefragmentBlockSize to meet AutoDefragmentPatchThreshold of {0:N2}% fragmentation.", DefragmentThreshold);

		List<Tuple<int, float, float>> Results = new List<Tuple<int, float, float>>();
		
		List<int> DefragmentBlockSizes = new List<int>() { 0, 64, 128, 256, 512 };

		if (GetFragmentationForBlockSizes(BuildPackage, PreviousPatchPackage, BaseBuildPackage, ProjectRoot, PackagingOptions, DefragmentBlockSizes, false, ref Results))
		{
			Tuple<int, float, float> BestBlockSize = null;
			Results.RemoveAll((x) => x.Item2 > DefragmentThreshold);
			if (Results.Count > 0)
			{
				BestBlockSize = Results.OrderByDescending((x) => x.Item2).First();
			}
			else
			{
				Logger.LogWarning("No DefragmentBlockSize in the set ({%0}) yielded the {0:N2}% target fragmentation. Using build with smallest fragmentation.", DefragmentBlockSizes, DefragmentThreshold);
				BestBlockSize = Results.OrderByDescending((x) => x.Item2).First();
			}

			if (BestBlockSize != null)
			{
				DefragmentBlockSize = BestBlockSize.Item1;
				return true;
			}
		}

		return false;
	}

	private static bool GetPatchAndBuildSize(string PatchPackage, string PreviousPatchPackage, string BasePackage, ref float PatchSize, ref float BuildSize)
	{
		string AuthoringToolPath = Path.Combine(NintendoExports.GetSDKInstallLocation(), @"Tools\CommandLineTools\AuthoringTool\AuthoringTool.exe");
		string AuthoringToolParameters = string.Format("analyze-patch {0} --previous {1} --original {2}", PatchPackage, PreviousPatchPackage, BasePackage);

		int ReturnCode = 0;
		string AuthoringToolLog = UnrealBuildTool.Utils.RunLocalProcessAndReturnStdOut(AuthoringToolPath, AuthoringToolParameters, out ReturnCode, false);

		Logger.LogInformation(AuthoringToolLog);

		if (ReturnCode == 0)
		{
			if (string.IsNullOrEmpty(AuthoringToolLog))
			{
				Logger.LogWarning("AuthoringTool ran successfully, but created no output.");
				return false;
			}

			{
				string PatchSizeRegEx = @"Delta\ssize.*\n.*100.00%,\s(\d+)MiB";

				Match PatchSizeRegExMatch = Regex.Match(AuthoringToolLog, PatchSizeRegEx);
				if (PatchSizeRegExMatch.Success)
				{
					if (PatchSizeRegExMatch.Groups.Count != 2)
					{
						Logger.LogWarning("Found {0} instances of PatchRegEx '{0}' in AuthoringTool output. Expected only one.", PatchSizeRegExMatch.Groups.Count, PatchSizeRegEx);
						for (int i = 0; i <  PatchSizeRegExMatch.Groups.Count; i++)
						{
							Logger.LogWarning("Instance {0}: {1}", i, PatchSizeRegExMatch.Groups[i].Value);
						}

					}
					PatchSize = float.Parse(PatchSizeRegExMatch.Groups[1].Value);
				}
				else
				{
					Logger.LogWarning("Failed to find PatchRegEx '{0}' in AuthoringTool output.", PatchSizeRegEx);
					return false;
				}
			}
			{
				string BuildSizeRegEx = @"Patch\ssize.*\n.*100.00%,\s(\d+)MiB";

				Match BuildSizeRegExMatch = Regex.Match(AuthoringToolLog, BuildSizeRegEx);
				if (BuildSizeRegExMatch.Success)
				{
					if (BuildSizeRegExMatch.Groups.Count != 2)
					{
						Logger.LogWarning("Found {0} instances of BuildSizeRegEx '{0}' in AuthoringTool output. Expected only one.", BuildSizeRegExMatch.Groups.Count, BuildSizeRegEx);
						for (int i = 0; i < BuildSizeRegExMatch.Groups.Count; i++)
						{
							Logger.LogWarning("Instance {0}: {1}", i, BuildSizeRegExMatch.Groups[i].Value);
						}
					}
					BuildSize = float.Parse(BuildSizeRegExMatch.Groups[1].Value);
				}
				else
				{
					Logger.LogWarning("Failed to find BuildSizeRegEx '{0}' in AuthoringTool output.", BuildSizeRegEx);
					return false;
				}
			}
		}

		return true;
	}
	public override bool UpdatePatchPackagingParameters(string ProjectRoot, string BuildToUse, string AdditionalOptions)
	{
		ConfigHierarchy Ini = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, new DirectoryReference(ProjectRoot), PlatformType);

		bool UpdateDefragmentBlockSize = false;
		bool bGotValue = Ini.TryGetValue("Packaging", "RunAutoPatchDefragment", out UpdateDefragmentBlockSize);

		if (UpdateDefragmentBlockSize == false)
		{
			if (bGotValue)
			{
				Logger.LogInformation("RunAutoPatchDefragment is false. AutoDefragmentBlockSize will no be updated.");
			}
			else
			{
				Logger.LogInformation("RunAutoPatchDefragment not set. Default is false. AutoDefragmentBlockSize will no be updated.");
			}
			return true;
		}

		// get the project's ProgramId
		string ProgramId;
		if (!Ini.TryGetValue(RuntimeSettingsName, "ProgramId", out ProgramId))
		{
			throw new AutomationException("Unable to find ProgramId in Config.");
		}

		float DefragmentThreshold;
		if (!Ini.TryGetValue("Packaging", "AutoPatchDefragmentFragmentationThreshold", out DefragmentThreshold))
		{
			Logger.LogWarning("PatchDefragmentThreshold not set in {Plat}Engine.ini [Packaging] section using {Threshold}. This must be fixed.", PlatformType.ToString(), DefragmentThreshold);
			return false;
		}

		string BaseBuild = "";

		if (!GetOptionValue(AdditionalOptions, "BaseBuild", ref BaseBuild))
		{
			throw new AutomationException("BaseBuild not passed via AdditionalOptions.");
		}

		string PreviousPatch = "";

		if (!GetOptionValue(AdditionalOptions, "PreviousPatch", ref PreviousPatch))
		{
			if (string.IsNullOrEmpty(PreviousPatch))
			{
				throw new AutomationException("Unable to find a suitable build for PreviousPatch.");
			}
		}     

		string PackagingOptions = "";
		int MinimumMatchingSize;
		if (!Ini.TryGetValue("Packaging", "PatchBlockMatchingSize", out MinimumMatchingSize))
		{
			PackagingOptions += String.Format(" --minimum-matching-size {0}", MinimumMatchingSize);
		}

		List<Tuple<int, float, float>> Results = new List<Tuple<int, float, float>>();

		List<int> DefragmentBlockSizes = new List<int>() { 0, 64, 128, 256, 512 };

		if (GetFragmentationForBlockSizes(BuildToUse, PreviousPatch, BaseBuild, ProjectRoot, PackagingOptions, DefragmentBlockSizes, true, ref Results))
		{
			Results = Results.OrderBy((x) => x.Item1).ToList();

			Logger.LogInformation("Results by DefragmentBlockSize:");
			foreach (Tuple<int, float, float> Result in Results)
			{
				if (Result.Item1 == 0)
				{
					Logger.LogInformation("No \'--defragment\' option        Fragmentation: {1:N2}% PatchSize {2}MB.", Result.Item2, Result.Item3);
				}
				else
				{
					Logger.LogInformation("\'--defragment-size\': {03} Fragmentation: {1:N2}% PatchSize {2}MB.", Result.Item1, Result.Item2, Result.Item3);
				}
			}

			Tuple<int, float, float> BestBlockSize = null;

			Results.RemoveAll((x) => x.Item2 > DefragmentThreshold);
			if (Results.Count > 0)
			{
				BestBlockSize = Results.OrderByDescending((x) => x.Item2).First();
			}
			else
			{
				Logger.LogWarning("No DefragmentBlockSize in the set ({%0}) yielded the {0:N2}% target fragmentation. Using build with smallest fragmentation.", DefragmentBlockSizes, DefragmentThreshold);
				BestBlockSize = Results.OrderByDescending((x) => x.Item2).First();
			}

			if (BestBlockSize != null)
			{
				Logger.LogInformation("Submitting AutoDefragmentBlockSize of {0}", BestBlockSize.Item1);

				string Description = $"Automated update of {PlatformType} Packaging DefragmentBlockSize";

				try
				{
					string JSONFilePath = Path.Combine(ProjectRoot, "Platforms", TargetPlatformType.ToString(), "Build", "AutoDefrag.json");

					int Changelist = P4.CreateChange(P4Env.Client, Description);

					if (File.Exists(JSONFilePath))
					{
						Logger.LogInformation("Opening {0} for edit.", JSONFilePath);
						P4.Edit(Changelist, JSONFilePath);
					}
					else
					{
						string JSONFileDirectory = Path.GetDirectoryName(JSONFilePath);

						if (!Directory.Exists(JSONFileDirectory))
						{
							Directory.CreateDirectory(JSONFileDirectory);
						}

						Logger.LogInformation("Opening {0} for add.", JSONFilePath);
						P4.Add(Changelist, JSONFilePath);
					}

					int AutoPatchDefragmentBlockSize = BestBlockSize.Item1;

					string JSONContents = JsonConvert.SerializeObject(new { AutoPatchDefragmentBlockSize }, Newtonsoft.Json.Formatting.Indented);

					File.WriteAllText(JSONFilePath, JSONContents);
					Logger.LogInformation("Submitting CL{0} as:\n{1}", Changelist, JSONContents);
					P4.Submit(Changelist, true);
				}
				catch (Exception ex)
				{
					Logger.LogWarning("Unable to submit updated {Plat} Packaging DefragmentBlockSize. Exception {Ex}", PlatformType, ex.ToString());
				}
			}
		}

		return true;
	}
	static bool FindAndLoadPackagingParameters(string NspFilePath, out PackagingParameters PackagingParameters)
	{
		return FindAndLoadPackagingParameters(Path.GetDirectoryName(NspFilePath), Path.GetFileNameWithoutExtension(NspFilePath), out PackagingParameters);
	}

	static bool FindAndLoadPackagingParameters(string MetadataDirectory, string FileNameBase, out PackagingParameters PackagingParameters)
	{
		string MetadataFilePath = FindPackagingParameters(MetadataDirectory, FileNameBase);

		if (string.IsNullOrEmpty(MetadataFilePath))
		{
			PackagingParameters = null;
			return false;
		}
		else
		{
			string MetadataFileContents = File.ReadAllText(MetadataFilePath);
			PackagingParameters = new PackagingParameters(MetadataFileContents, null);
			return true;
		}
	}

	static string GetPackagingParametersFilename(string PackageBaseFileNameOrPath)
	{
		return Path.GetFileNameWithoutExtension(PackageBaseFileNameOrPath).Replace("-patch", null) + "-PackagingParameters.json";
	}

	static string FindPackagingParameters(string MetadataBaseDirectory, string MetadataBaseFilename)
	{
		string MetadataFilename = GetPackagingParametersFilename(MetadataBaseFilename);
		return FindMetadataFile(MetadataBaseDirectory, MetadataFilename);
	}
	static string FindPackagingParameters(string NspFilePath)
	{
		return FindPackagingParameters(Path.GetDirectoryName(NspFilePath), Path.GetFileName(NspFilePath));
	}

	static string FindMetadataFile(string MetadataBaseDirectory, string MetadataBaseFilename)
	{
		string MetadataFilePath = Path.Combine(MetadataBaseDirectory, MetadataBaseFilename);

		Logger.LogInformation($"Looking for packaging parameters metadata file {MetadataFilePath} from {MetadataBaseDirectory} {MetadataBaseFilename}");

		if (!File.Exists(MetadataFilePath))
		{
			MetadataFilePath = Path.Combine(MetadataBaseDirectory, "Metadata", MetadataBaseFilename);
			Logger.LogInformation($"Looking for packaging parameters metadata file {MetadataFilePath}");
			if (!File.Exists(MetadataFilePath))
			{
				Logger.LogWarning($@"Unable to find Switch packaging parameters metadatafile {MetadataBaseFilename} in {MetadataBaseDirectory} or {MetadataBaseDirectory}\Metadata");
				return null;
			}
		}

		return MetadataFilePath;
	}
	
	static void OverridePackagePlatformFromMetaData(string BuildRootDir, string BuildName, ref string PackagePlatform)
	{
		PackagingParameters Metadata = null;
		if (FindAndLoadPackagingParameters(BuildRootDir, BuildName, out Metadata))
		{
			PackagePlatform = NintendoPlatform.GetPlatformIdentifierFromTargetPlatform(Metadata.PackagePlatform);
		}
	}

	private static bool CreateTemporaryPatchNSP(string PatchNsp, string BuildNsp, string PreviousPatchNsp, string BaseBuildNsp, string AdditionalToolOptions, string ProgramId, string DescFile)
	{
		// now generate a patch :)
		string Command = Path.Combine(NintendoExports.GetSDKInstallLocation(), @"Tools\CommandLineTools\AuthoringTool\AuthoringTool.exe");
		string PatchCommandLine = string.Format("makepatch -o \"{0}\" --desc \"{1}\" --original \"{2}\" --current \"{3}\" {4}",
			PatchNsp, DescFile, BaseBuildNsp, BuildNsp, AdditionalToolOptions);

		if (!string.IsNullOrEmpty(PreviousPatchNsp))
		{
			PatchCommandLine += string.Format(" --previous \"{0}\"", PreviousPatchNsp);
		}

		if(CommandUtils.IsBuildMachine)
		{
			PatchCommandLine += " --execute_parallel " + Math.Max(Utils.GetLogicalProcessorCount(), 4).ToString();
		}

		IProcessResult Result = Run(Command, PatchCommandLine);

		return Result.ExitCode == 0;
	}

	public void UpdatePackagingOptions( DirectoryReference ProjectRoot, List<Tuple<string,string>> Options)
	{
		string Description = $"Automated update of {PlatformType} Packaging DefragmentBlockSize";

		try
		{
			int Changelist = P4.CreateChange(P4Env.Client, Description);

			FileReference NintendoEngineIni = ConfigCache.GetPlatformConfigFileReference(ConfigHierarchyType.Engine, ProjectRoot, PlatformType.ToString());

			if (File.Exists(NintendoEngineIni.FullName))
			{
				Logger.LogInformation("Opening {0} for edit.", NintendoEngineIni.FullName);
				P4.Edit(Changelist, NintendoEngineIni.FullName);
			}
			else
			{
				Logger.LogInformation("Opening {0} for add.", NintendoEngineIni.FullName);
				P4.Add(Changelist, NintendoEngineIni.FullName);
			}

			bool bSuccess = false;

			foreach( Tuple<string,string> Option in Options)
			{
				if( ConfigCache.WriteSettingToConfigFile(NintendoEngineIni, ConfigCache.ConfigDefaultUpdateType.SetValue, "Packaging", Option.Item1, Option.Item2, Logger))
				{
					Logger.LogInformation("Updated Packaging option {0} to {1}", Option.Item1, Option.Item2);
					bSuccess = true;
				}
				else
				{
					Logger.LogWarning("Failed to update Packaging option {0}", Option.Item1);
				}
			}

			if (bSuccess)
			{
				Logger.LogInformation("Submitting CL{0}", Changelist);
				P4.Submit(Changelist, false);
			}
			else
			{
				Logger.LogWarning("Failed to update {0}", NintendoEngineIni.FullName);
				P4.DeleteChange(Changelist);
			}
		}
		catch (Exception ex)
		{
			Logger.LogWarning("Unable to submit updated {Plat} Packaging options. Exception {Ex}", PlatformType, ex.ToString());
		}

	}


	void CreateRunBatchFile(DeploymentContext SC, string OutputNsp, string PackagePlatform)
	{
		string TemplateFileFQPN = FileReference.Combine(SC.EngineRoot, $@"Platforms\{TargetPlatformType}\Build\RunNSP.template").FullName;
		string OutputBatchFilename = Path.Combine(Path.GetDirectoryName(OutputNsp), "Run_" + Path.ChangeExtension(Path.GetFileName(OutputNsp), ".bat"));

		CreateBatchFileFromTemplate(TemplateFileFQPN, Path.GetFileName(OutputNsp), PackagePlatform, OutputBatchFilename);
	}
	void CreateInstallBatchFile(DeploymentContext SC, string OutputNsp, string PackagePlatform)
	{
		string TemplateFileFQPN = FileReference.Combine(SC.EngineRoot, $@"Platforms\{TargetPlatformType}\Build\InstallNSP.template").FullName;
		string OutputBatchFilename = Path.Combine(Path.GetDirectoryName(OutputNsp), "Install_" + Path.ChangeExtension(Path.GetFileName(OutputNsp), ".bat"));

		CreateBatchFileFromTemplate(TemplateFileFQPN, Path.GetFileName(OutputNsp), PackagePlatform, OutputBatchFilename);
	}

	void CreateInstallAsPatchBatchFile(DeploymentContext SC, string OutputNsp, string BaseNsp, string PackagePlatform)
	{
		string TemplateFileFQPN = FileReference.Combine(SC.EngineRoot, $@"Platforms\{TargetPlatformType}\Build\InstallNSPAsPatch.template").FullName;
		string OutputBatchFilename = Path.Combine(Path.GetDirectoryName(OutputNsp), "Install_" + Path.ChangeExtension(Path.GetFileName(OutputNsp), ".bat"));

		CreatePatchBatchFileFromTemplate(TemplateFileFQPN, Path.GetFileName(OutputNsp), Path.GetFileName(BaseNsp), PackagePlatform, OutputBatchFilename);
	}

	void CreateBatchFileFromTemplate(string TemplateFilename, string OutputNsp, string PackagePlatform, string OutputBatchFileName)
	{
		string NL = Environment.NewLine;

		string OutputBatchFileContents = "";

		foreach (string LineIn in File.ReadAllLines(TemplateFilename))
		{
			OutputBatchFileContents += LineIn.Replace("{InNSPFilename}", OutputNsp).Replace("{InPackagePlatform}", PackagePlatform) + NL;
		}

		string OutputBatchFile = Path.Combine(Path.GetDirectoryName(OutputNsp), OutputBatchFileName);
		File.WriteAllText(OutputBatchFile, OutputBatchFileContents);
	}
	void CreatePatchBatchFileFromTemplate(string TemplateFilename, string OutputNsp, string BaseNsp, string PackagePlatform, string OutputBatchFileName)
	{
		string NL = Environment.NewLine;

		string OutputBatchFileContents = "";

		foreach (string LineIn in File.ReadAllLines(TemplateFilename))
		{
			OutputBatchFileContents += LineIn.Replace("{InBaseNSPFilename}", BaseNsp).Replace("{InPatchNSPFilename}", OutputNsp).Replace("{InPackagePlatform}", PackagePlatform) + NL;
		}

		string OutputBatchFile = Path.Combine(Path.GetDirectoryName(OutputNsp), OutputBatchFileName);
		File.WriteAllText(OutputBatchFile, OutputBatchFileContents);
	}

	void CreateAddOnContentInstallBatchFile(ProjectParams Params, string OutputNsp, string ProgramId, bool bRunTimeAddOnContentInstall = false)
	{
		string NL = Environment.NewLine;

		string InstallBatchContents =
			"@echo off" + NL +
			"setlocal" + NL +
			"set NSP=%~dp0" + Path.GetFileName(OutputNsp) + NL +
			"set RUNONTARGET=\"%NINTENDO_SDK_ROOT%\\Tools\\CommandLineTools\\RunOnTarget.exe\"" + NL +
			"set DEVMENUCOMMAND=\"%NINTENDO_SDK_ROOT%\\TargetTools\\" + TargetToolsArch + "\\DevMenuCommand\\Release\\DevMenuCommand.nsp\"" + NL +
			"echo." + NL +
			"echo Installing %NSP%..." + NL +
			"%RUNONTARGET%" + ((bRunTimeAddOnContentInstall) ? " --suppress-auto-kill" : "") + " %DEVMENUCOMMAND% -- addoncontent install \"%NSP%\"" + ((bRunTimeAddOnContentInstall) ? " --dynamic" : "") + NL +
			"";

		string InstallBatchFilePath = Path.Combine(Path.GetDirectoryName(OutputNsp), "Install_" + Path.ChangeExtension(Path.GetFileName(OutputNsp), ".bat"));
		File.WriteAllText(InstallBatchFilePath, InstallBatchContents);
	}

	public override void GetFilesToArchive(ProjectParams Params, DeploymentContext SC)
	{
		if (SC.StageTargetConfigurations.Count != 1 && Params.Package)
		{
			Logger.LogInformation("Archiving with more than one executable. Only {Arg0} will be archived.", SC.StageExecutables[SC.StageExecutables.Count - 1]);
		}

		// if we packaged a build, archive that, instead of the raw staging directory
		if (Params.Package || Params.SkipPackage)
		{
			UnrealTargetConfiguration TargetConfigurationName = SC.StageTargetConfigurations[SC.StageExecutables.Count - 1];

			string OutputNsp = Path.Combine(GetPackageOutputDirectory(Params, SC), GetNspName(Params, SC, TargetConfigurationName));
			SC.ArchiveFiles(Path.GetDirectoryName(OutputNsp), Path.GetFileName(OutputNsp));
			SC.ArchiveFiles(Path.GetDirectoryName(OutputNsp), Path.ChangeExtension("Run_" + Path.GetFileName(OutputNsp), ".bat"));
			SC.ArchiveFiles(Path.GetDirectoryName(OutputNsp), Path.ChangeExtension(Path.GetFileName(OutputNsp), ".nss"));

			string PatchNspPath = Path.Combine(GetPackageOutputDirectory(Params, SC), GetNspName(Params, SC, TargetConfigurationName, "-patch"));
			SC.ArchiveFiles(Path.GetDirectoryName(PatchNspPath), Path.GetFileName(PatchNspPath));

			string SymbolsBinPath = Path.Combine(SC.ProjectRoot.ToString(), $"Build/{TargetPlatformType}/Symbols/", TargetConfigurationName.ToString() + "-Symbols.bin");
			SC.ArchiveFiles(Path.GetDirectoryName(SymbolsBinPath), Path.GetFileName(SymbolsBinPath));
		}
		else
		{
			// otherwise, just archive the staging directory
			base.GetFilesToArchive(Params, SC);
		}
	}

	public override void GetConnectedDevices(ProjectParams Params, out List<string> Devices)
	{
		Devices = new List<string>();
	}

	public override bool RetrieveDeployedManifests(ProjectParams Params, DeploymentContext SC, string DeviceName, out List<string> UFSManifests, out List<string> NonUFSManifests)
	{
		UFSManifests = null;
		NonUFSManifests = null;

		return false;
	}

	public override void Deploy(ProjectParams Params, DeploymentContext SC)
	{
		foreach (string DeviceName in Params.DeviceNames)
		{
			KillCurrentRunningProcess(GetTargetNameFromDeviceName(DeviceName));
		}

		if(Params.Package || Params.SkipPackage)
		{
			ConfigHierarchy Ini = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, (SC.ProjectRoot == null) ? null : SC.ProjectRoot, TargetIniPlatformType);
			string ProgramId = null;
			Ini.GetString(RuntimeSettingsName, "ProgramId", out ProgramId);

			if (ProgramId == null)
			{
				throw new AutomationException("Unable to find ProgramId .ini setting");
			}

			var Config = SC.StageTargetConfigurations.Last();
			string BaseNspName = GetNspName(Params, SC, Config);
			string PackageNsp = Path.Combine(GetPackageOutputDirectory(Params, SC), BaseNspName);

			foreach (string DeviceName in Params.DeviceNames)
			{
				string TargetName = GetTargetNameFromDeviceName(DeviceName);
				ResetRequiredVersion(TargetName, ProgramId);
				InstallPackage(PackageNsp, ProgramId, TargetName);
			}
		}
	}

	public void InstallPackage(string PackageNsp, string ProgramId, string TargetName = null)
	{
		string ToolDir = Path.Combine(NintendoExports.GetSDKInstallLocation(), "Tools", "CommandLineTools");
		string ControlTool = Path.Combine(ToolDir, "ControlTarget.exe");
		string RunTool = Path.Combine(ToolDir, "RunOnTarget.exe");
		string DevMenuCommandNsp = Path.Join(NintendoExports.GetSDKInstallLocation(), $"\\TargetTools\\{TargetToolsArch}\\DevMenuCommand\\Release\\DevMenuCommand.nsp");
		
		if (!File.Exists(PackageNsp))
		{
			throw new AutomationException(ExitCode.Error_UnknownDeployFailure, $"Package file not found: {PackageNsp}");
		}

		if(!File.Exists(DevMenuCommandNsp))
		{
			throw new AutomationException(ExitCode.Error_UnknownDeployFailure, $"Unable to find DevMenuCommand: {DevMenuCommandNsp}");
		}

		// If target name is empty, it is assumed that the default target will be used
		string DeviceTarget = string.Empty;
		if (!string.IsNullOrEmpty(TargetName))
		{
			DeviceTarget = $"-t {TargetName}";
		}

		Logger.LogInformation("Uninstalling application if it exists");
		string UninstallCommand = $"uninstall-application {ProgramId} {DeviceTarget}";

		IProcessResult UninstallResult = Run(ControlTool, UninstallCommand);
		UninstallResult.WaitForExit();

		Logger.LogInformation("Installing package");
		string DeployCommand = $"{DeviceTarget} {DevMenuCommandNsp} -- application install {PackageNsp}";
		IProcessResult DeployResult = Run(RunTool, DeployCommand);
		DeployResult.WaitForExit();

		if (DeployResult.HasExited)
		{
			if (DeployResult.ExitCode == 0 && !DeployResult.Output.ToLower().Contains("failure"))
			{
				Logger.LogInformation("Package installation succeeded.");
			}
			else
			{
				throw new AutomationException(ExitCode.Error_UnknownDeployFailure, $"Failure deploying package: {PackageNsp}");
			}
		}
	}

	private int ExtractFirmwareMajorVersionFromLog(string ControlToolLog)
	{
		int FirmwareMajorVersion = -1;
		string SDKVersionStringPattern = "NintendoSDK Firmware for NX ([\\d.-]+)";
		var RegexMatch = Regex.Match(ControlToolLog, SDKVersionStringPattern);

		if (RegexMatch.Groups.Count > 1)
		{
			string VersionString = RegexMatch.Groups[1].Value;
			string[] VersionStringSplit = VersionString.Split('.');
			if (VersionStringSplit.Length > 0)
			{
				if (!int.TryParse(VersionStringSplit[0], out FirmwareMajorVersion))
				{
					return -1;
				}
			}
		}
		return FirmwareMajorVersion;
	}

	protected string GetValidTargetName(string InTargetName)
	{
		// Make sure the ControlTarget executable is available.
		string ToolDir = Path.Combine(NintendoExports.GetSDKInstallLocation(), "Tools", "CommandLineTools");
		string ControlTool = Path.Combine(ToolDir, "ControlTarget.exe");
		if (!File.Exists(ControlTool))
		{
			Logger.LogError($"GetValidTargetName(): ControlTarget.exe is not available at '{ControlTool}'");
			return string.Empty;
		}

		// If TargetName is empty get the default for the platform.
		string TargetName = InTargetName;
		if (string.IsNullOrWhiteSpace(TargetName))
		{
			string CommandLine = NintendoExports.GetSDKVersionInt() >= NintendoExports.VersionXYZToInt("17", "5", "0") ?
				$"get-default --platform {NintendoSDKPlatformIdentifier}" :
				"get-default";
			TargetName = UnrealBuildTool.Utils.RunLocalProcessAndReturnStdOut(ControlTool, CommandLine);
			if (!string.IsNullOrWhiteSpace(TargetName))
			{
				TargetName = TargetName.Split()[0];
			}
			else
			{
				Logger.LogError($"GetValidTargetName(): {CommandLine} failed to return a valid name for the default devkit target.  Is an appropriate default devkit target connected?");
			}
		}

		// If TargetName is not empty make sure we are connected to it.
		if (!string.IsNullOrWhiteSpace(TargetName))
		{
			string CommandLine = NintendoExports.GetSDKVersionInt() >= NintendoExports.VersionXYZToInt("17", "5", "0") ?
				$"list-target --tag _{NintendoSDKPlatformIdentifier}" :
				"list-target";

			// "--detail" will print both the name and the serial number of the devkit to the log
  			CommandLine += " --detail";

			string TargetList = UnrealBuildTool.Utils.RunLocalProcessAndReturnStdOut(ControlTool, CommandLine);
			if (!string.IsNullOrWhiteSpace(TargetList) && TargetList.ToUpper().IndexOf(TargetName.ToUpper()) != -1)
			{
				return TargetName;
			}
			else
			{
				Logger.LogError($"GetValidTargetName(): '{CommandLine}' failed to find target named '{TargetName}', is target configured correctly?");
			}
		}

		Logger.LogError($"GetValidTargetName(): failed to get valid name for '{InTargetName}'");
		return string.Empty;
	}

	public override IProcessResult RunClient(ERunOptions ClientRunFlags, string ClientApp, string ClientCmdLine, ProjectParams Params, DeploymentContext SC)
	{
		// use some SDK tools
		string ToolDir = Path.Combine(NintendoExports.GetSDKInstallLocation(), "Tools", "CommandLineTools");
		string ControlTool = Path.Combine(ToolDir, "ControlTarget.exe");
		string RunTool = Path.Combine(ToolDir, "RunOnTarget.exe");
		ConfigHierarchy EngineIni = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, DirectoryReference.FromFile(Params.RawProjectPath), TargetIniPlatformType);
		bool bShouldRunPackage = (Params.Package || Params.SkipPackage);
		var TargetConfig = SC.StageTargetConfigurations.Last(); 

		// decorated exe name
		string DecoratedExeName = bShouldRunPackage ? Path.Combine(GetPackageOutputDirectory(Params, SC), GetNspName(Params, SC, TargetConfig)) : 
			Path.ChangeExtension(Params.GetProjectExeForPlatform(TargetPlatformType).ToString(), ".nspd");
		// If running a package, the executable will already be decorated with platform and config.
		if (TargetConfig != UnrealTargetConfiguration.Development && !bShouldRunPackage)
		{
			string Ext = Path.GetExtension(DecoratedExeName);
			DecoratedExeName = Path.Combine(Path.GetDirectoryName(DecoratedExeName), Path.GetFileNameWithoutExtension(DecoratedExeName));
			DecoratedExeName += $"-{TargetPlatformType}-{TargetConfig}{Ext}";
		}

		IProcessResult FinalResult = null;

		foreach (string DeviceName in Params.DeviceNames)
		{
			// Make sure target name is valid and connected, if none is given the default for the platform will be used.
			string TargetName = GetValidTargetName(DeviceName.Length > 0 ? GetTargetNameFromDeviceName(DeviceName) : String.Empty);
			if (string.IsNullOrWhiteSpace(TargetName))
			{
				continue;
			}

			// Terminate any running app.
			Run(ControlTool, $"terminate -t {TargetName}", null, ERunOptions.Default);

			string StageDir = Params.BaseStageDirectory.Replace('\\', '/') + "/" + GetCookPlatform(false, Params.Client);
			string CommandLine = String.Empty;

			// @todo turnkey - this can be handled with Turnkey version checking

			// Verify firmware version is compatible with SDK version
// 			CommandLine = string.Format("firmware-version {0}", TargetSetting);
// 			string ControlToolLog = RunAndLog(ControlTool, CommandLine);
// 			int FirmwareMajorVersion = ExtractFirmwareMajorVersionFromLog(ControlToolLog);
// 			int SDKMajorVersion = NintendoExports.GetSDKVersionMajor();
// 
// 			if (FirmwareMajorVersion != -1 && SDKMajorVersion != -1 && FirmwareMajorVersion < SDKMajorVersion)
// 			{
// 				LogError("Firmware version on target {0} is lower than SDK version.  Please update firmware on target.", DeviceName);
// 				continue;
// 			}

			// run
			CommandLine += string.Format("--target {0} ", TargetName);
			string ProcessName = Path.GetFileName(DecoratedExeName);
			bool bResetRequiredVersionBeforeLaunch = false;
			EngineIni.GetBool(EditorSettingsName, "bResetRequiredVersionBeforeLaunch", out bResetRequiredVersionBeforeLaunch);
			string ProgramId = null;
			EngineIni.GetString(RuntimeSettingsName, "ProgramId", out ProgramId);

			if (bResetRequiredVersionBeforeLaunch)
			{
				bool bResetResult = ResetRequiredVersion(TargetName, ProgramId);
				if (!bResetResult)
				{
					Logger.LogWarning("Continuing launch attempt of {0} on {1}.", ProcessName, TargetName);
				}
			}

			// special output message for the Unreal Editor
			Logger.LogInformation("Running Package@Device:{ProcessName}@{TargetName}", ProcessName, TargetName);

			// If packaged, run the packaged build on the platform
			if (bShouldRunPackage)
			{
				// To run the deployed/installed package, we need the Program ID
				if(ProgramId == null)
				{
					throw new AutomationException("Unable to find ProgramId .ini setting");
				}

				// We can simply use the Program ID to run the package.
				CommandLine = $"{CommandLine} {ProgramId}";
				FinalResult = Run(RunTool, CommandLine, null, ClientRunFlags | ERunOptions.NoWaitForExit);
			}
			else 
			{
				// Run the staged build 
				CommandLine += string.Format("--working-directory \"{0}\" \"{1}\"", StageDir, DecoratedExeName);
				if (!string.IsNullOrEmpty(ClientCmdLine))
				{
					CommandLine += string.Format(" -- {0}", ClientCmdLine);
				}
				// launch and return immediately while stdout is continously emitted to UAT
				FinalResult = Run(RunTool, CommandLine, null, ClientRunFlags | ERunOptions.NoWaitForExit);
			}

			int CrashDumpWaitTimeoutSeconds;
			if (!EngineIni.GetInt32(EditorSettingsName, "CrashDumpWaitTimeoutSeconds", out CrashDumpWaitTimeoutSeconds))
			{
				CrashDumpWaitTimeoutSeconds = 60;
			}

			// add a wrapper that takes care of killing not only RunOnTarget but also the client when uat is exiting
			FinalResult = new RunOnTargetCreatedProcess(this, FinalResult, ProcessName, DeviceName, CrashDumpWaitTimeoutSeconds);
		}
		return FinalResult;
	}

	public bool ResetRequiredVersion(string TargetName, string ProgramId)
	{
		string ToolDir = Path.Combine(NintendoExports.GetSDKInstallLocation(), "Tools", "CommandLineTools");
		string ControlTool = Path.Combine(ToolDir, "ControlTarget.exe");

		string DevMenuCommandLine = "devmenu";
		const int ControlToolWaitTimeMs = 60 * 1000; // 1min.
		if (!string.IsNullOrWhiteSpace(TargetName))
		{
			DevMenuCommandLine += $" -t {TargetName}";
		}
		else
		{
			TargetName = "*DEFAULT*";
		}

		string ResetCommandLine = $"{DevMenuCommandLine} -- application reset-required-version {ProgramId}";
		Logger.LogInformation($"Resetting required version for Target '{TargetName}' with program Id: '{ProgramId}'");
		IProcessResult ResetResult = Run(ControlTool, ResetCommandLine, null, ERunOptions.NoWaitForExit);
		ResetResult.ProcessObject.WaitForExit(ControlToolWaitTimeMs);
		if (ResetResult.HasExited)
		{
			if (ResetResult.ExitCode == 0 && !ResetResult.Output.ToLower().Contains("failure"))
			{
				Logger.LogInformation("Reset of required version succeeded.");
				return true;
			}
			else
			{
				Logger.LogWarning($"Reset of required version failed on target {TargetName}");
			}
		}
		else
		{
			// Kill is async, wait for the process to exit, if it fails throw an exception.
			Logger.LogWarning("Reset of required version timed out, killing reset process.");
			ResetResult.ProcessObject.Kill();
			ResetResult.ProcessObject.WaitForExit(ControlToolWaitTimeMs);
			if (ResetResult.HasExited == false)
			{
				throw new AutomationException("Failed to kill reset required version process.");
			}
		}

		return false;
	}
	public string GetROMRedirectionTarget(string TargetName)
	{
		string ToolDir = Path.Combine(NintendoExports.GetSDKInstallLocation(), "Tools", "CommandLineTools");
		string ControlTool = Path.Combine(ToolDir, "ControlTarget.exe");

		string DevMenuCommandLine = "devmenu";
		if (!string.IsNullOrWhiteSpace(TargetName))
		{
			DevMenuCommandLine += $" -t {TargetName}";
		}

		string CommandLine = $"{DevMenuCommandLine} -- debug get-host-romfs-redirection-target";
		IProcessResult Result = Run(ControlTool, CommandLine, null);
		if (Result.HasExited)
		{
			return Result.Output.Substring(0, Result.Output.IndexOf('\n')).Trim();
		}

		return string.Empty;
	}

	// Redirection Target can be 'builtin', 'sdcard', 'gamecard' or off
	public bool SetROMRedirectionTarget(string RedirectionTarget, string DeviceTargetName)
	{
		string ToolDir = Path.Combine(NintendoExports.GetSDKInstallLocation(), "Tools", "CommandLineTools");
		string ControlTool = Path.Combine(ToolDir, "ControlTarget.exe");

		string DevMenuCommandLine = "devmenu";
		if (!string.IsNullOrWhiteSpace(DeviceTargetName))
		{
			DevMenuCommandLine += $" -t {DeviceTargetName}";
		}

		string CommandLine = $"{DevMenuCommandLine} -- debug set-host-romfs-redirection-target {RedirectionTarget}";
		IProcessResult Result = Run(ControlTool, CommandLine, null);
		if (Result.HasExited)
		{
			return (Result.ExitCode == 0 && !Result.Output.ToLower().Contains("failure"));
		}

		return false;
	}

	public void KillCurrentRunningProcess(string TargetName)
	{
		// Make sure target name is valid and connected, if none is given the default for the platform will be used.
		TargetName = GetValidTargetName(TargetName);
		if (string.IsNullOrWhiteSpace(TargetName))
		{
			return;
		}

		// Terminate any running application.
		string ToolDir = Path.Combine(NintendoExports.GetSDKInstallLocation(), "Tools", "CommandLineTools");
		string ControlTool = Path.Combine(ToolDir, "ControlTarget.exe");
		Logger.LogInformation($"Stopping the running process on device '{TargetName}'");
		Run(ControlTool, $"terminate -t {TargetName}", null, ERunOptions.Default);
	}

	public override void GetFilesToDeployOrStage(ProjectParams Params, DeploymentContext SC)
	{
		// Stage all the build products
		foreach (StageTarget Target in SC.StageTargets)
		{
			SC.StageBuildProductsFromReceipt(Target.Receipt, Target.RequireFilesExist, Params.bTreatNonShippingBinariesAsDebugFiles);
		}


		DirectoryReference ProjectCloudPath = DirectoryReference.Combine(SC.ProjectRoot, $"Platforms/{TargetPlatformType}/Build/Cloud");
		if (DirectoryReference.Exists(ProjectCloudPath))
		{
			SC.StageFiles(StagedFileType.SystemNonUFS, ProjectCloudPath, StageFilesSearch.AllDirectories, new StagedDirectoryReference("Cloud"));
		}

	}

	public override string GetCookPlatform(bool bDedicatedServer, bool bIsClientOnly)
	{
		return TargetPlatformType.ToString() + (bIsClientOnly ? "Client" : "");
	}

	public override bool DeployLowerCaseFilenames(StagedFileType FileType)
	{
		return false;
	}

	public override string LocalPathToTargetPath(string LocalPath, string LocalRoot)
	{
		return LocalPath.Replace("\\", "/").Replace(LocalRoot, "../../..");
	}

	public override bool IsSupported { get { return true; } }

	public override List<string> GetDebugFileExtensions()
	{
		return new List<string> { ".nss" };
	}

	public override bool PublishSymbols(DirectoryReference SymbolStoreDirectory, List<FileReference> Files,
			bool bIndexSources, List<FileReference> SourceFiles,
			string Product, string Branch, int Change, string BuildVersion = null, string VFSMapping = null)
	{
		Logger.LogInformation("Publishing symbols to \"{SymbolStoreDirectory}\"", SymbolStoreDirectory);

		// Find the symbol store tool.
		string UtilPath = Path.Combine(NintendoExports.GetSDKInstallLocation(), "Tools", "CommandLineTools", "NXSymStore.exe");
		if (!File.Exists(UtilPath))
		{
			Logger.LogError("Couldn't find NXSymStore. Cannot upload symbols.");
			return false;
		}

		if (!Directory.Exists(SymbolStoreDirectory.FullName))
		{
			Directory.CreateDirectory(SymbolStoreDirectory.FullName);
		}

		DirectoryReference TempSymStoreDir = DirectoryReference.Combine(Unreal.RootDirectory, "Saved", "SymStore");
		DirectoryReference.CreateDirectory(TempSymStoreDir);
		DeleteDirectoryContents(TempSymStoreDir);

		bool bSuccess = true;
		foreach (var SymbolFile in Files.Where(x => x.HasExtension(".nss")))
		{
			IProcessResult RunSymStore = CommandUtils.Run(UtilPath, string.Format("add /f \"{0}\" /s \"{1}\" /compress", SymbolFile.FullName, TempSymStoreDir.FullName));
			if (RunSymStore.ExitCode != 0)
			{
				bSuccess = false;
				continue;
			}
		}
		foreach (DirectoryReference HashDir in DirectoryReference.EnumerateDirectories(TempSymStoreDir))
		{
			foreach (FileReference SymbolFile in DirectoryReference.EnumerateFiles(HashDir))
			{
				string RelativePath = SymbolFile.MakeRelativeTo(DirectoryReference.Combine(TempSymStoreDir));
				FileReference ActualDestinationFile = FileReference.Combine(SymbolStoreDirectory, RelativePath);

				// Try and add a version file.  Do this before checking to see if the symbol is there already in the case of exact matches (multiple builds could use the same pdb, for example)
				if (!string.IsNullOrWhiteSpace(BuildVersion))
				{
					FileReference BuildVersionFile = FileReference.Combine(ActualDestinationFile.Directory, string.Format("{0}.version", BuildVersion));
					// Attempt to create the file. Just continue if it fails.
					try
					{
						DirectoryReference.CreateDirectory(BuildVersionFile.Directory);
						// write a file pointer string to the symbol this version is referencing
						FileReference.WriteAllText(BuildVersionFile, ActualDestinationFile.GetFileNameWithoutExtension());
					}
					catch (Exception Ex)
					{
						Logger.LogWarning("Failed to write the version file, reason {Arg0}", Ex.ToString());
					}
				}

				// Don't bother copying the temp file if the destination file is there already.
				if (FileReference.Exists(ActualDestinationFile))
				{
					Logger.LogInformation("Destination file {Arg0} already exists, skipping", ActualDestinationFile.FullName);
					continue;
				}

				FileReference TempDestinationFile = new FileReference(ActualDestinationFile.FullName + Guid.NewGuid().ToString());
				try
				{
					CommandUtils.CopyFile(SymbolFile.FullName, TempDestinationFile.FullName);
				}
				catch (Exception Ex)
				{
					throw new AutomationException("Couldn't copy the symbol file to the temp store! Reason: {0}", Ex.ToString());
				}
				// Move the file in the temp store over.
				try
				{
					FileReference.Move(TempDestinationFile, ActualDestinationFile);
				}
				catch (Exception Ex)
				{
					// If the file is there already, it was likely either copied elsewhere (and this is an ioexception) or it had a file handle open already.
					// Either way, it's fine to just continue on.
					if (FileReference.Exists(ActualDestinationFile))
					{
						Logger.LogInformation("Destination file {Arg0} already exists or was in use, skipping.", ActualDestinationFile.FullName);
						continue;
					}
					// If it doesn't exist, we actually failed to copy it entirely.
					else
					{
						Logger.LogWarning("Couldn't move temp file {Arg0} to the symbol store at location {Arg1}! Reason: {Arg2}", TempDestinationFile.FullName, ActualDestinationFile.FullName, Ex.ToString());
					}
				}
				// Delete the temp one no matter what, don't want them hanging around in the symstore
				finally
				{
					FileReference.Delete(TempDestinationFile);
				}
			}
		}
		return bSuccess;
	}

	public override int GetExecutableSize(DirectoryReference BinariesDirectory, string ClientName, HashSet<FileReference> BuildProducts)
	{
		string BloatyExePath = Unreal.RootDirectory.ToString() + @"\Engine\Extras\ThirdPartyNotUE\Bloaty\bloaty.exe";

		if( File.Exists(BloatyExePath) )
		{
			int ExitCode = 0;

			string NSSPath = Path.Combine(BinariesDirectory.ToString(), ClientName + ".nss");

			if (File.Exists(NSSPath))
			{
				string Results = Utils.RunLocalProcessAndReturnStdOut(BloatyExePath, NSSPath, out ExitCode);

				if (ExitCode == 0)
				{
					// Get the results from Bloaty as an array of lines
					string[] LineArray = Results.Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);

					// Basic validation of results
					if (LineArray.Length >= 4)
					{
						// Get the last line as an array of space delimited entries
						string[] LineEntries = LineArray[LineArray.Length - 1].Split(' ', StringSplitOptions.RemoveEmptyEntries);

						// Basic validation of entries
						// Assume the lines is formatted like this:
						//  100.0%   3.99Gi 100.0% 295.19Mi    TOTAL
						if (LineEntries.Length == 5 && LineEntries[4] == "TOTAL")
						{
							string VMUsageEntry = LineEntries[3];

							// Basic validation of entry
							if (VMUsageEntry.Length > 2)
							{
								// Remove the Mi
								VMUsageEntry = VMUsageEntry.Substring(0, VMUsageEntry.Length - 2);

								return (int) float.Parse(VMUsageEntry, CultureInfo.InvariantCulture) * 1024 * 1024;
							}
						}
					}
				}
			}
		}

		return 0;
	}

	public override string[] SymbolServerDirectoryStructure
	{
		get
		{
			return new string[]
			{
				"*",			// Hash Directory		(e.g. A9, 2 bytes of full hash)
			};
		}
	}

	public override bool SymbolServerDeleteIndividualFiles
	{
		// Nintendo platforms store multiple symbols in the final directory.
		// Allow deleting individual symbol files which are out of date.
		get { return true; }
	}

	public override bool SymbolServerRequiresLock
	{
		// No lock file required on the symbols.
		get { return false; }
	}

	private string GetMegazarfPathFromTurnkeyCopyOutput(ITurnkeyContext TurnkeyContext)
	{
		// get SDK source (megazarf) file location
		string MegazarfDirectory = TurnkeyContext.GetVariable("CopyOutputPath");
		string[] MegazarfFilePaths = Directory.GetFiles(MegazarfDirectory, "*.megazarf");
		if (MegazarfFilePaths.Length == 0)
		{
			throw new AutomationException("No megazarf install files (*.megazarf) found in Turnkey download directory (CopyOutputPath): {0}", MegazarfDirectory);
		}
		else if (MegazarfFilePaths.Length > 1)
		{
			throw new AutomationException("Multiple megazarf install files (*.megazarf) found in Turnkey download directory (CopyOutputPath): {0}", MegazarfDirectory);
		}

		return MegazarfFilePaths[0];
	}

	private void CheckMegazarfInstallLocationAndOverrideIfNeededForNDICmd(ITurnkeyContext TurnkeyContext, out string OutAlternateRootDir, out string OutAlternateEnvName)
	{
		string AlternateRootDir = string.Empty;
		string AlternateEnvName = string.Empty;
		string MegazarfPath = GetMegazarfPathFromTurnkeyCopyOutput(TurnkeyContext);

		// check to see if overriding the install location has been disallowed
		bool bAllowOverridingMegazarfInstallPath;
		string ProjectPath = TurnkeyContext.GetVariable("ProjectPath");
		ConfigHierarchy EngineIni = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, DirectoryReference.FromString(ProjectPath), TargetPlatformType);
		if (!EngineIni.GetBool(EditorSettingsName, "AllowOverridingMegazarfInstallPath", out bAllowOverridingMegazarfInstallPath))
		{
			bAllowOverridingMegazarfInstallPath = true;
		}

		// The AlternateEnvName is technically not needed here as it is not changed from what is specified in the
		// megazarf.  However NDICmd fails to use the AlternateRootDir if the AlternateEnvName is not also overridden.
		// As a workaround, both are specified and the install location is overridden as expected.
		bool bUseAlternateInstallPath = bAllowOverridingMegazarfInstallPath
			&& NintendoExports.MegazarfNeedsAlternateRootDir(TargetPlatformType, false, MegazarfPath, out AlternateRootDir, out AlternateEnvName);

		if (bUseAlternateInstallPath && !string.IsNullOrWhiteSpace(AlternateRootDir) && !string.IsNullOrWhiteSpace(AlternateEnvName))
		{
			OutAlternateRootDir = AlternateRootDir;
			OutAlternateEnvName = AlternateEnvName;
		}
		else
		{
			OutAlternateRootDir = "NO_ALTERNATE_ROOT_DIR";
			OutAlternateEnvName = "NO_ALTERNATE_ENV_NAME";
		}
	}

	public void ResetAndConnectDevice(string TargetName)
	{
		string ToolDir = Path.Combine(NintendoExports.GetSDKInstallLocation(), "Tools", "CommandLineTools");
		string ControlTool = Path.Combine(ToolDir, "ControlTarget.exe");

		const string ResetCommand = "reset";
		const string ConnectCommand = "connect";

		string TargetOption = string.Empty;
		if(!string.IsNullOrEmpty(TargetName))
		{
			TargetOption = $"-t {TargetName}";
		}

		string CommandLine = $"{ResetCommand} {TargetOption}";
		IProcessResult Result = Run(ControlTool, CommandLine);
		if (Result.ExitCode != 0)
		{
			throw new AutomationException("Unable to restart device.");
		}

		CommandLine = $"{ConnectCommand} {TargetOption}";
		Result = Run(ControlTool, CommandLine);
		if (Result.ExitCode != 0)
		{
			throw new AutomationException("Unable to re-connect to device after restart.");
		}
	}

	public override bool UpdateDevicePrerequisites(DeviceInfo Device, BuildCommand Command, ITurnkeyContext TurnkeyContext, bool bVerifyOnly)
	{
		// GetROMRedirectionTarget() will implicitly connect the device, try and leave it in it's
		// originally state rather than keeping the implicit connection.
		Target Target = !String.IsNullOrWhiteSpace(Device.Name) ? GetTargetManagerDevice(Device.Name) : null;
		bool WasConnected = Target != null ? Target.GetIsConnected() : false;

		string CurrentROMRedirectTarget = GetROMRedirectionTarget(Device.Name);
		if(string.IsNullOrEmpty(CurrentROMRedirectTarget))
		{
			throw new AutomationException("Unable to get ROM Redirection Target.");
		}

		bool bIsRomRedirectOn = !CurrentROMRedirectTarget.ToLower().StartsWith("off");
		if(bIsRomRedirectOn)
		{
			Logger.LogInformation("ROM Redirection is ON. Setting it to off before proceeding.");

			// Store the redirection target so that we can set it back after SDK update. 
			Device.PlatformValues["ROMRedirectionTarget"] = CurrentROMRedirectTarget;
			if(SetROMRedirectionTarget("off", Device.Name))
			{
				// We need to reset the device after changing
				// ROM redirection target setting. 
				Logger.LogInformation("Restarting target to apply change.");
				ResetAndConnectDevice(Device.Name);
			}
		}

		// Disconnect if we didn't enter this method connected.
		if (Target != null && !WasConnected && Target.GetIsConnected())
		{
			Target.Disconnect();
		}

		// If ROM Redirection is off, we can just continue
		return true;
	}

	public override bool OnSDKInstallComplete(int ExitCode, ITurnkeyContext TurnkeyContext, DeviceInfo Device)
	{
		// Set ROM Redirection target back to original state if needed. 
		string ROMRedirectionTarget;
		if (Device != null && Device.PlatformValues.TryGetValue("ROMRedirectionTarget", out ROMRedirectionTarget)) 
		{
			Logger.LogInformation("Setting ROM Redirection back to original state.");
			if (!SetROMRedirectionTarget(ROMRedirectionTarget, Device.Name))
			{
				Logger.LogWarning("Failed to set ROM Redirection target back to original state. Set it back manually if required.");
			}
			else
			{
				Logger.LogInformation("Resetting target to apply change.");
				ResetAndConnectDevice(Device.Name);
			}
		}

		return ExitCode == 0;
	}

	public override bool GetSDKInstallCommand(out string Command, out string Params, ref bool bRequiresPrivilegeElevation, ref bool bCreateWindow, ITurnkeyContext TurnkeyContext, bool bSdkAlreadyInstalled)
	{
		string Version = TurnkeyContext.GetVariable("Version");

		// remove the existing copy of this environment, if any
		string EnvToDelete = "NO_ENV_TO_DELETE";
		if (bSdkAlreadyInstalled)
		{
			string NDIEnvironment = NintendoExports.GetNDIEnvironmentForSDKVersion(PlatformType, Version);
			if (!string.IsNullOrEmpty(NDIEnvironment))
			{
				EnvToDelete = $" {NDIEnvironment}";
			}
		}

		// see if we will be using NNPM or NDICmd to install the SDK
		string CopyOutputPath = TurnkeyContext.GetVariable("CopyOutputPath");
		bool bUseNNPM = Directory.Exists(CopyOutputPath) && Directory.EnumerateFiles(CopyOutputPath, "nnpm_setup-*").Any(); 

		// get the final command and parameters
		if (bUseNNPM)
		{
			// get install location. nnpm cannot use the location specified in the megazarf
			string MegazarfPath = GetMegazarfPathFromTurnkeyCopyOutput(TurnkeyContext);
			if (!NintendoExports.MegazarfNeedsAlternateRootDir(TargetPlatformType, true, MegazarfPath, out string AlternateRootDir, out string AlternateEnvName))
			{
				throw new AutomationException("Cannot find target installation directory for NNPM");
			}

			Command = "$(EngineDir)/Platforms/Nintendo/Build/BatchFiles/Turnkey/install_nintendo_sdk_nnpm.bat";
			Params = $"$(CopyOutputPath) {Version} {EnvToDelete} {AlternateRootDir}";
			bRequiresPrivilegeElevation = true;
		}
		else
		{
			// get alternate install location if the location specified in the megazarf is invalid
			CheckMegazarfInstallLocationAndOverrideIfNeededForNDICmd(TurnkeyContext, out string AlternateRootDir, out string AlternateEnvName);

			Command = "$(EngineDir)/Platforms/Nintendo/Build/BatchFiles/Turnkey/install_nintendo_sdk_ndicmd.bat";
			Params = $"$(CopyOutputPath) {EnvToDelete}  {AlternateRootDir} {AlternateEnvName}";
			bRequiresPrivilegeElevation = true;
		}


		TurnkeyContext.PauseForUser("Please close 'Nintendo Target Manager' and 'Visual Studio' before continuing (if applicable)");
		return true;
	}

	public override bool GetDeviceUpdateSoftwareCommand(out string Command, out string Params, ref bool bRequiresPrivilegeElevation, ref bool bCreateWindow, ITurnkeyContext TurnkeyContext, DeviceInfo Device)
	{
		// check for code-specified device software update command
		string Version = TurnkeyContext.GetVariable("Version");
		if (Version.EndsWith(CodeSpecifiedVersionSuffix))
		{
			Command = $"$(EngineDir)/Platforms/{PlatformType}/Build/BatchFiles/Turnkey/update_fw_and_devmenu_from_sdk.bat";
			Params = $"{Device.Name} {NintendoExports.GetSDKInstallLocation()}";
			return true;
		}

		// see if we will be using NNPM or NDICmd to install the SystemUpdater
		string CopyOutputPath = TurnkeyContext.GetVariable("CopyOutputPath");
		bool bUseNNPM = Directory.Exists(CopyOutputPath) && Directory.EnumerateFiles(CopyOutputPath, "nnpm_setup-*").Any(); 

		if (bUseNNPM)
		{
			// get install location. nnpm cannot use the location specified in the megazarf
			string MegazarfPath = GetMegazarfPathFromTurnkeyCopyOutput(TurnkeyContext);
			if (!NintendoExports.MegazarfNeedsAlternateRootDir(TargetPlatformType, true, MegazarfPath, out string AlternateRootDir, out string AlternateEnvName))
			{
				throw new AutomationException("Cannot find target installation directory for NNPM");
			}

			Command = "$(EngineDir)/Platforms/Nintendo/Build/BatchFiles/Turnkey/update_fw_and_devmenu_nnpm.bat";
			Params = $"$(CopyOutputPath) {Device.Name} $(Version) {PlatformType} {NintendoSDKPlatformIdentifier} {AlternateRootDir}";
			bRequiresPrivilegeElevation = true;
			return true;
		}
		else
		{
			// get alternate install location if the location specified in the megazarf is invalid
			CheckMegazarfInstallLocationAndOverrideIfNeededForNDICmd(TurnkeyContext, out string AlternateRootDir, out string AlternateEnvName);

			Command = "$(EngineDir)/Platforms/Nintendo/Build/BatchFiles/Turnkey/update_fw_and_devmenu_ndicmd.bat";
			Params = $"$(CopyOutputPath) {Device.Name} $(Version) {PlatformType} {NintendoSDKPlatformIdentifier} {AlternateRootDir} {AlternateEnvName}";
			bRequiresPrivilegeElevation = true;
			return true;
		}
	}

	public override string GetSDKCreationHelp()
	{
		StringBuilder Out = new StringBuilder();
		Out.AppendLine("To create an full SDK, use the Nintendo Dev Interface to create an offline installer by selecting \"Create My Own\" and");
		Out.AppendLine("\"Install to Archive\".  Use the format of \"SDK*versionnumber*\" (no asterisks) for the Export Filename and Environment name.");
		Out.AppendLine("Choose the \"Standard\" toolset and choose the appropriate version and SDK packages.  Place the megazarf and a copy of");
		Out.AppendLine("the Nintendo Dev Interface Installer (downloaded from Nintendo's developer site) in a directory named to correspond with the");
		Out.AppendLine("version of the SDK that is being installed.  I.e. \"a.b.c_full\" where a.b.c is the version of the SDK.");
		Out.AppendLine("See NintendoPlatformSDK::TryConvertVersionToInt() for more info on version number formatting.");
		Out.AppendLine();
		Out.AppendLine("Add an entry to the Turnkey manifest that points to the SDK directory");
		Out.AppendLine("Example:");
		Out.AppendLine("<FileSource>");
		Out.AppendLine("  <Platform>Platform1,Platform2</Platform>");
		Out.AppendLine("  <Type>Full</Type>");
		Out.AppendLine("  <Version>$(ExpVersion)</Version>");
		Out.AppendLine("  <Name>Nintendo Full Install version $(ExpVersion)</Name>");
		Out.AppendLine("  <Source HostPlatform=\"Win64\">fileexpansion:googledrive:/SdkInstallers/Installers/Nintendo/$[ExpVersion]_full/</Source>");
		Out.AppendLine("</FileSource>");
		Out.AppendLine();
		Out.AppendLine();
		Out.AppendLine("To create a flash SDK, use the Nintendo Dev Interface to create offline installer by selecting \"Create My Own\" and ");
		Out.AppendLine("\"Install to Archive\". Use the format SystemUpdater*versionnumber* (no asterisks) for the Export Filename and Environment");
		Out.AppendLine("name.  Next, choose the SystemUpdater toolset and choose the appropriate version.  After export is complete, place the");
		Out.AppendLine("megazarf and a copy of the Nintendo Dev Interface Installer (downloaded from Nintendo's developer site) in a directory");
		Out.AppendLine("named to correspond with the version of the SDK that is being installed.  I.e. a.b.c_d-e.");
		Out.AppendLine("See NintendoPlatformSDK::TryConvertVersionToInt() for more info on version number formatting.");
		Out.AppendLine();
		Out.AppendLine("<FileSource>");
		Out.AppendLine("  <Platform>PLATFORM_NAME</Platform>");
		Out.AppendLine("  <Type>Flash</Type>");
		Out.AppendLine("  <!-- Target Shell can update any device type -->");
		Out.AppendLine("  <AllowedFlashDeviceTypes>regex:.*</AllowedFlashDeviceTypes>");
		Out.AppendLine("  <Version>$(ExpVersion)</Version>");
		Out.AppendLine("  <Name>PLATFORM_NAME Update Flash to version $(ExpVersion)</Name>");
		Out.AppendLine("  <Source>fileexpansion:googledrive:/SdkInstallers/Flashes/PLATFORM_NAME/$[ExpVersion]/</Source>");
		Out.AppendLine("</FileSource>");
		Out.AppendLine();

		return Out.ToString();

	}

	public override DeviceInfo[] GetDevices()
	{
		if (TmAssembly.LoadDll(Logger))
		{
			return GetDevicesInternal();
		}
		return null;
	}

	public bool IsDeviceWritingDump(string DeviceName)
	{
		if (TmAssembly.LoadDll(Logger))
		{
			return IsDeviceWritingDumpInternal(DeviceName);
		}
		return false;
	}

	public string GetApplicationName(string DeviceName)
	{
		if (TmAssembly.LoadDll(Logger))
		{
			return GetApplicationNameInternal(DeviceName);
		}
		return "";
	}

	public bool IsDeviceRunningAnyApplication(string DeviceName)
	{
		if (TmAssembly.LoadDll(Logger))
		{
			return IsDeviceRunningAnyApplicationInternal(DeviceName);
		}
		return false;
	}

	private DeviceInfo[] GetDevicesInternal()
	{
		Target[] Targets;

		try
		{
			Targets = TargetManager.GetTargets();
		}
		catch (TargetInvocationException Ex)
		{
			Logger.LogError("Failed connecting to Nintendo TargetManager Service, please ensure it is running:\n{Arg0}", Ex.InnerException.ToString());
			return null;
		}

		List<DeviceInfo> Devices = new List<DeviceInfo>();
		foreach (Target Target in Targets)
		{
			string DeviceName;
			try
			{
				DeviceName = Target.GetName();
			}
			catch (TargetInvocationException Ex)
			{
				Logger.LogError($"Error communicating with Nintendo device:\n{Ex.InnerException.ToString()}");
				continue;
			}

			try
			{
				string DeviceType = Target.GetPlatform();
				if (DeviceType != NintendoSDKPlatformIdentifier)
				{
					continue;
				}

				DeviceInfo Device = new DeviceInfo(TargetPlatformType);
				Device.Name = DeviceName;

				bool ConnectedState = Target.GetIsConnected();
				if (!ConnectedState)
				{
					Logger.LogInformation($"Nintendo device '{Device.Name}' is disconnected, attempting connection");
					Target.Connect();

					// Sleep for a bit after connection or we will get a Nintendo.Tm.TmException:
					// (Error_28) The service that handles this isn't available.
					Thread.Sleep(1000);

					if (!Target.GetIsConnected())
					{
						Logger.LogInformation($"Failed to connect to Nintendo device '{Device.Name}'");
						continue;
					}
				}

				Target DefaultTarget;
				TargetManager.TryGetDefaultTarget(out DefaultTarget, DeviceType);

				Device.Id = Target.GetSerialNumber();
				Device.Type = Target.GetHardwareTypeName(Target.GetHardwareType());
				Device.bIsDefault = DefaultTarget != null ? DefaultTarget.GetSerialNumber().Equals(Device.Id) : false;

				// Get the firmware version number.  
				// The string from GetFirmwareVersion also includes a hash (ex: "10.0.2-1.0 (f72302ca14)"), parse out only the version number
				string FullFirmwareVersion = Target.GetFirmwareVersion();
				string[] SplitFWVersion = FullFirmwareVersion.Split();
				Device.SoftwareVersion = SplitFWVersion[0];
				Devices.Add(Device);

				if (!ConnectedState)
				{
					Logger.LogInformation($"Nintendo device '{Device.Name}' was originally disconnected, attempting disconnection");
					Target.Disconnect();
				}

				//Console.WriteLine($">>>>>>>>>>>>>>>> Device {DeviceType}: {Device.Id} {Device.Type} {Device.bIsDefault} {Device.SoftwareVersion}");
			}
			catch (TargetInvocationException Ex)
			{
				Logger.LogError($"Error communicating with Nintendo device '{DeviceName}':\n{Ex.InnerException.ToString()}");
				continue;
			}
		}

		return Devices.ToArray();
	}

	private Target GetTargetManagerDevice(string DeviceName)
	{
		Target SpecifiedDevice = null;
		Target[] Targets = null;

		try
		{
			if (String.IsNullOrWhiteSpace(DeviceName))
			{
				// If DeviceName is empty, use the default device.
				TargetManager.TryGetDefaultTarget(out SpecifiedDevice, NintendoSDKPlatformIdentifier);
			}
			else
			{
				Targets = TargetManager.GetTargets();

				foreach (Target PotentialTarget in Targets)
				{
					// Name and Serial Number are often used interchangably to reference Nintendo devices, look for both here.
					if (PotentialTarget.GetName().Equals(DeviceName, StringComparison.InvariantCultureIgnoreCase))
					{
						SpecifiedDevice = PotentialTarget;
						break;
					}

					if (PotentialTarget.GetSerialNumber().Equals(DeviceName, StringComparison.InvariantCultureIgnoreCase))
					{
						SpecifiedDevice = PotentialTarget;
						break;
					}
				}
			}
		}
		catch (TargetInvocationException Ex)
		{
			Logger.LogError("Error communicating with Nintendo device:\n{Arg0}", Ex.InnerException.ToString());
			return null;
		}

		if (SpecifiedDevice == null)
		{
			Logger.LogError("Unable to locate specified Nintendo device '{DeviceName}'.", DeviceName);
			if (Targets == null || Targets.Length == 0)
			{
				Logger.LogInformation("\tNo connected Nintendo devices were found");
			}
			else
			{
				string DeviceNames = "";
				foreach (Target PotentialTarget in Targets)
				{
					DeviceNames += PotentialTarget.GetName() + "(serial: " + PotentialTarget.GetSerialNumber() + ") ";
				}

				Logger.LogInformation("\tConnected devices include: {DeviceNames}", DeviceNames);
			}
			return null;
		}

		return SpecifiedDevice;
	}

	private bool IsDeviceRunningAnyApplicationInternal(string DeviceName)
	{
		Target TargetDevice = GetTargetManagerDevice(DeviceName);

		if (TargetDevice == null)
		{
			return false;
		}

		int State;
		try
		{
			State = TargetDevice.GetProgramState();
		}
		catch (TargetInvocationException Ex)
		{
			Logger.LogError("Error communicating with specified Nintendo device: '{DeviceName}'.  GetProgramState() failed: {Arg1}", DeviceName, Ex.InnerException.ToString());
			return false;
		}

		return State != TargetProgramState.NoProgram && State != TargetProgramState.Halted;
	}

	private bool IsDeviceWritingDumpInternal(string DeviceName)
	{
		Target TargetDevice = GetTargetManagerDevice(DeviceName);

		if (TargetDevice == null)
		{
			return false;
		}

		int State;
		try
		{
			State = TargetDevice.GetProgramState();
		}
		catch (TargetInvocationException Ex)
		{
			Logger.LogError("IsDeviceWritingDump(): Error communicating with specified Nintendo device: '{DeviceName}'.  GetProgramState() failed: {Arg1}", DeviceName, Ex.InnerException.ToString());
			return false;
		}

		if (State != TargetProgramState.Halted)
		{
			return false;
		}

		// note: experimentally, Target Manager always reports 'missing logs' so don't warn about it
		bool MissingLogs = false;
		String Log = TargetDevice.GetLog(ref MissingLogs);

		// Start and complete dump messages are documented in "Using the Dump Feature" in NintendoSDK Documentation
		int StartDumpMsgPostition = Log.LastIndexOf("] Start dumping to");
		int DumpCompleteMsgPosition = Log.LastIndexOf("] Dump completed");

		int StartAftermathDumpMsgPosition = Log.LastIndexOf("[AFTERMATH] - Writing crash dump file");
		if (StartAftermathDumpMsgPosition > StartDumpMsgPostition)
		{
			StartDumpMsgPostition = StartAftermathDumpMsgPosition;
			DumpCompleteMsgPosition = Log.LastIndexOf("[AFTERMATH] - Writing crash dump completed");
		}

		return StartDumpMsgPostition > DumpCompleteMsgPosition;

	}

	private string GetApplicationNameInternal(string DeviceName)
	{
		Target TargetDevice = GetTargetManagerDevice(DeviceName);

		if (TargetDevice == null)
		{
			return "";
		}

		string ApplicationName = null;
		try
		{
			ApplicationName = TargetDevice.GetProgramName();
		}
		catch (TargetInvocationException Ex)
		{
			Logger.LogError("Error communicating with specified Nintendo device: '{DeviceName}'.  GetProgramName() failed: {Arg1}", DeviceName, Ex.InnerException.ToString());
			return "";
		}

		return ApplicationName;
	}


	public class RunOnTargetCreatedProcess : AutomationTool.IProcessResult
	{
		private readonly object StopSyncObject = new object();
		NintendoPlatform Platform;
		IProcessResult HostRunProcess;
		string ProcessName;
		string DeviceName;
		bool bStopped = false;
		int CrashDumpWaitTimeoutSeconds;

		public RunOnTargetCreatedProcess(
				NintendoPlatform InPlatform,
				IProcessResult InHostRunProcess,
				string InProcessName,
				string InDeviceName,
				int InCrashDumpWaitTimeoutSeconds)
		{
			Platform = InPlatform;
			HostRunProcess = InHostRunProcess;
			ProcessName = InProcessName;
			DeviceName = InDeviceName;
			CrashDumpWaitTimeoutSeconds = InCrashDumpWaitTimeoutSeconds;
			ProcessManager.AddProcess(this);
		}

		~RunOnTargetCreatedProcess()
		{
			ProcessManager.RemoveProcess(this);
		}

		public void StopProcess(bool KillDescendants = true)
		{
			lock (StopSyncObject)
			{
				if (!bStopped)
				{
					Platform.KillCurrentRunningProcess(DeviceName);
					if (!HostRunProcess.HasExited)
					{
						HostRunProcess.StopProcess(KillDescendants);
					}
					bStopped = true;
				}
			}
		}

		public bool HasExited
		{
			get
			{
				// check if the game client has exited
				// the game client is always considered running while the RunOnTarget process is still active (at least during launch/startup),
				// and then if the process is still running on the device even after RunOnTarget may have been disconnected/killed.
				if (!bStopped && (HostRunProcess.HasExited && !IsProcessRunningOnDevice()))
				{
					// don't stop the process immediately if the device is writing a dump
					if (Platform.IsDeviceWritingDump(DeviceName))
					{
						Logger.LogInformation("Running process has stopped, but is writing a crash dump.  Waiting for completion before stopping process (timeout {CrashDumpWaitTimeoutSeconds} seconds)...", CrashDumpWaitTimeoutSeconds);
						DateTime TimeoutTime = DateTime.Now.AddSeconds(CrashDumpWaitTimeoutSeconds);

						do
						{
							if (DateTime.Now > TimeoutTime)
							{
								Logger.LogWarning("Timeout exceeded waiting for crash dump to complete, stopping process immediately.  Crash dump may be truncated.");
								break;
							}
							Thread.Sleep(1000);
						} while (Platform.IsDeviceWritingDump(DeviceName));
					}

					// run StopProcess for consistency and for setting bStopped
					StopProcess(false);
				}
				return bStopped;
			}
		}

		public string GetProcessName()
		{
			return String.Format("{0}@{1}", ProcessName, DeviceName);
		}

		public void OnProcessExited()
		{
		}

		public void DisposeProcess()
		{
			HostRunProcess.DisposeProcess();
		}

		public void StdOut(object sender, DataReceivedEventArgs e)
		{
		}

		public void StdErr(object sender, DataReceivedEventArgs e)
		{
		}

		public int ExitCode
		{
			get { return HostRunProcess.ExitCode; }
			set { HostRunProcess.ExitCode = value; }
		}

		public bool bExitCodeSuccess => ExitCode == 0;

		public string Output
		{
			get { return HostRunProcess.Output; }
		}

		public Process ProcessObject
		{
			get { return HostRunProcess.ProcessObject; }
		}

		public void WaitForExit()
		{
			// first wait for the RunOnTarget process to exit
			if (!HostRunProcess.HasExited)
			{
				HostRunProcess.WaitForExit();
			}
			// then also wait for the application to exit since it may be still be running
			// if RunOnTarget is disconnected/killed by another process.
			while (IsProcessRunningOnDevice())
			{
				Thread.Sleep(100);
			}
			// run StopProcess for consistency and for setting bStopped
			StopProcess();
		}

		public FileReference WriteOutputToFile(string FileName)
		{
			return HostRunProcess.WriteOutputToFile(FileName);
		}

		private bool IsProcessRunningOnDevice()
		{
			if (!Platform.IsDeviceRunningAnyApplication(DeviceName))
			{
				return false;
			}

			string ApplicationName = Platform.GetApplicationName(DeviceName);
			bool bIsProcessRunning = !String.IsNullOrWhiteSpace(ApplicationName) && ProcessName.Contains(ApplicationName);

			return bIsProcessRunning;
		}
	}




	/*

	As of v18.2.0, we cannot use the normal ProcessUtils.Run() due to the way Nintendo's AuthoringTool uses std and stdout.

	This is only apparent when launching Unreal AutomationTool from the editor. The editor uses FWindowsPlatformProcess::CreatePipe which disables HANDLE_FLAG_INHERIT on the pipes used to capture STDOUT, 
	as-per the Windows SDK documentation (https://learn.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output) ... 
	
	However, the current engine implementation is incorrect. Instead of creating a separate STDIN pipe, it is passing 'read' end of the STDOUT pipe to the child process as if it was a STDIN pipe, meaning any
	process that tries to read or peek STDIN will be reading its own output (#jira UE-208628)

	This workaround spawns AuthoringTool via cmd and captures stdout and stderr to temporary files, then logs the final result

	*/
	private static bool TEMP_RunViaCmdAndCaptureOutput( string Command, string CommandLine )
	{
		// spawn the command via the command prompt, allowing us to capture the stdout and stderrr to temporary files
		string StdOut = Path.GetTempFileName();
		string StdErr = Path.GetTempFileName();
		bool bResult = Run("cmd.exe", $"/C {Command} {CommandLine} > {StdOut} 2> {StdErr}", Options:ERunOptions.NoStdOutRedirect).ExitCode == 0;
		
		// dump all output from the command
		foreach (string Line in File.ReadAllLines(StdOut))
		{
			Logger.LogInformation(Line);
		}
		foreach (string Line in File.ReadAllLines(StdErr))
		{
			Logger.LogError(Line);
		}
		File.Delete(StdOut);
		File.Delete(StdErr);

		return bResult;
	}
}