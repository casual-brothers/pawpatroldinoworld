// Copyright Epic Games, Inc. All Rights Reserved.

using EpicGames.Core;
using Ionic.Zip;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using System.Xml.XPath;
using UnrealBuildBase;

namespace UnrealBuildTool
{
	abstract class NintendoToolChain : ClangToolChain
	{
		protected class NintendoToolChainInfo : ClangToolChainInfo
		{
			public FileReference MakeNso { get; init; }
			public FileReference MakeNro { get; init; }
			public FileReference MakeNrr { get; init; }
			public FileReference MakeMeta { get; init; }

			public Version NintendoClangVersion => LazyNintendoClangVersion.Value;
			private readonly Lazy<Version> LazyNintendoClangVersion;

			public NintendoToolChainInfo(DirectoryReference SdkInstallDir, FileReference Clang, FileReference Archiver, FileReference MakeNso, FileReference MakeNro, FileReference MakeNrr, FileReference MakeMeta, ILogger Logger)
				: base(SdkInstallDir, Clang, Archiver, Logger)
			{
				this.MakeNso = MakeNso;
				this.MakeNro = MakeNro;
				this.MakeNrr = MakeNrr;
				this.MakeMeta = MakeMeta;

				LazyNintendoClangVersion = new Lazy<Version>(() => QueryNintendoClangVersion());
			}

			/// <inheritdoc/>
			protected override Version QueryClangVersion()
			{
				Match MatchClangVersion = Regex.Match(ClangVersionString, @"(based on LLVM|clang version) (?<full>(?<major>\d+)\.(?<minor>\d+)\.(?<patch>\d+))");
				if (MatchClangVersion.Success && Version.TryParse(MatchClangVersion.Groups["full"].Value, out Version? ClangVersion))
				{
					return ClangVersion;
				}
				throw new BuildException("Failed to query the Clang version number!");
			}

			private Version QueryNintendoClangVersion()
			{
				Match MatchNintendoClangVersion = Regex.Match(ClangVersionString, @"Nintendo version (?<full>(?<major>\d+)\.(?<minor>\d+)\.(?<patch>\d+))");
				if (MatchNintendoClangVersion.Success && Version.TryParse(MatchNintendoClangVersion.Groups["full"].Value, out Version? NintendoClangVersion))
				{
					return NintendoClangVersion;
				}
				throw new BuildException("Failed to query the Nintendo Clang version number!");
			}
		}

		protected NintendoToolChainInfo NintendoInfo => (Info as NintendoToolChainInfo)!;
		protected UnrealTargetConfiguration TargetConfiguration;
		
		private List<string> BundledNroFiles = new List<string>();
		public NintendoToolChain(FileReference? InProjectFile, ILogger InLogger, ClangToolChainOptions InOptions) : base(InOptions, InLogger)
		{
			ProjectFile = InProjectFile;
		}

		public override void SetUpGlobalEnvironment(ReadOnlyTargetRules Target, CppCompileEnvironment GlobalCompileEnvironment, LinkEnvironment GlobalLinkEnvironment)
		{
			base.SetUpGlobalEnvironment(Target, GlobalCompileEnvironment, GlobalLinkEnvironment);

			TargetConfiguration = Target.Configuration;
		}

		public abstract ReadOnlyNintendoTargetRules GetNintendoTargetRules(ReadOnlyTargetRules Target);
		protected abstract UnrealTargetPlatform Platform { get; }
		protected abstract string ToolPlatformArg { get; }
		protected abstract string TargetTriple { get; }
		protected abstract string TargetMpu { get; }
		protected static string GetConfigSectionNameForPlatform(UnrealTargetPlatform Platform)
		{
			return $"/Script/{Platform}RuntimeSettings.{Platform}RuntimeSettings";
		}
		protected string GetConfigSectionName()
		{
			return GetConfigSectionNameForPlatform(Platform);
		}

		protected override ClangToolChainInfo GetToolChainInfo()
		{
			// location inside SDK for tools
			DirectoryReference SdkInstallDir = DirectoryReference.FromString(NintendoPlatformSDK.GetSDKInstallLocation())!;
			DirectoryReference BinDir = DirectoryReference.Combine(SdkInstallDir, "Compilers/NintendoClang/bin");
			if (!DirectoryReference.Exists(BinDir))
			{
				BinDir = DirectoryReference.Combine(SdkInstallDir, "Compilers/NX/nx/aarch64/bin");
			}
				
			DirectoryReference CommandLineToolsDir = DirectoryReference.Combine(SdkInstallDir, "Tools/CommandLineTools");
			FileReference CompilerPath = FileReference.Combine(BinDir, "clang++.exe");
			FileReference ArchiverPath = FileReference.Combine(BinDir, "aarch64-nintendo-nn-elf-ar.exe");
			FileReference MakeNsoPath = FileReference.Combine(CommandLineToolsDir, "MakeNso/MakeNso.exe");
			FileReference MakeNroPath = FileReference.Combine(CommandLineToolsDir, "MakeNro/MakeNro.exe");
			FileReference MakeNrrPath = FileReference.Combine(CommandLineToolsDir, "MakeNrr/MakeNrr.exe");
			FileReference MakeMetaPath = FileReference.Combine(CommandLineToolsDir, "MakeMeta/MakeMeta.exe");
			return new NintendoToolChainInfo(SdkInstallDir, CompilerPath, ArchiverPath, MakeNsoPath, MakeNroPath, MakeNrrPath, MakeMetaPath, Logger);
		}

		/// <inheritdoc/>
		protected override void GetCompileArguments_WarningsAndErrors(CppCompileEnvironment CompileEnvironment, List<string> Arguments)
		{
			base.GetCompileArguments_WarningsAndErrors(CompileEnvironment, Arguments);

			Arguments.Add("-Wno-nonportable-include-path");

			// this hides the "use of logical '||' with constant operand" which we use with, ie GIsEditor || IsRunningCommandlet(), which we want to be valid.
			Arguments.Add("-Wno-constant-logical-operand");

			if (NintendoInfo.ClangVersion.Major >= 17)
			{
				// Needed as of 18.1.X to skip errors when a lambda captures [myVar = myVar]
				Arguments.Add("-Wno-shadow");
				// Needed as of 18.1.X to skip error on static_asserts where the string includes the TEXT() macro
				Arguments.Add("-Wno-invalid-unevaluated-string");
			}
		}

		/// <inheritdoc/>
		protected override void GetCompileArguments_Optimizations(CppCompileEnvironment CompileEnvironment, List<string> Arguments)
		{
			base.GetCompileArguments_Optimizations(CompileEnvironment, Arguments);

			// Profile Guided Optimization (PGO) and Link Time Optimization (LTO)
			if (CompileEnvironment.bPGOOptimize)
			{
				// Always enable LTO when using PGO.
				Arguments.Add("-flto=thin");
			}
			else if (CompileEnvironment.bPGOProfile)
			{
				// Always enable LTO when generating PGO profile data.
				Arguments.Add("-flto=thin");
			}
			else if (CompileEnvironment.bAllowLTCG)
			{
				// When not using PGO, enable LTO in shipping builds if opted-in by the build configuration.
				Arguments.Add("-flto=thin");
			}

			if (CompileEnvironment.bOptimizeCode)
			{
				if (CompileEnvironment.OptimizationLevel == OptimizationMode.Size)
				{
					Arguments.Add("-Oz");
				}
				else if (CompileEnvironment.OptimizationLevel == OptimizationMode.SizeAndSpeed)
				{
					Arguments.Add("-Os");
				}
				else
				{
					if (CompileEnvironment.bPGOOptimize || CompileEnvironment.bPGOProfile)
					{
						// -Os tends to be both faster and smaller than -O3 when PGO is enabled
						Arguments.Add("-Os");
					}
					else
					{
					Arguments.Add("-O3");
				}
			}
			}
			else
			{
				Arguments.Add("-O0");
			}

			if (CompileEnvironment.bRetainFramePointers)
			{
				Arguments.Add("-fno-omit-frame-pointer");
			}
			else
			{
				Arguments.Add("-fomit-frame-pointer");
			}
		}

		/// <inheritdoc/>
		protected override void GetCompileArguments_Debugging(CppCompileEnvironment CompileEnvironment, List<string> Arguments)
		{
			base.GetCompileArguments_Debugging(CompileEnvironment, Arguments);

			// Create DWARF format debug info if wanted, or if using the malloc profiler, for better callstacks
			if (CompileEnvironment.bCreateDebugInfo)
			{
				Arguments.Add("-g");
				Arguments.Add("-gstrict-dwarf");
				Arguments.Add("-gdwarf-4");

				if (CompileEnvironment.bDebugLineTablesOnly)
				{
					Arguments.Add("-gline-tables-only");
				}
			}

			if (!bMergeModules) // With merge modules we use version-script to control visibility instead.
			{
				// hides the linker warnings with PhysX.
				// allows proper usage of NRO.
				Arguments.Add("-fvisibility=hidden");
			}

			Arguments.Add("-fvisibility-inlines-hidden");
		}

		/// <inheritdoc/>
		protected override void GetCompileArguments_Global(CppCompileEnvironment CompileEnvironment, List<string> Arguments)
		{
			base.GetCompileArguments_Global(CompileEnvironment, Arguments);

			Arguments.Add("-fPIC");

			if (NintendoPlatformSDK.bVerboseCompiler)
			{
				Arguments.Add("-v");
			}

			if (CompileEnvironment.RootPaths.bUseVfs)
			{
				// These are needed to make .gch/.o portable
				Arguments.Add(GetSystemIncludePathArgument(DirectoryReference.Combine(Info.BasePath!, "Compilers", "NintendoClang", "include"), CompileEnvironment));
				Arguments.Add(GetSystemIncludePathArgument(DirectoryReference.Combine(Info.BasePath!, "Compilers", "NintendoClang", "include", "c++", "v1"), CompileEnvironment));
				Arguments.Add(GetSystemIncludePathArgument(DirectoryReference.Combine(Info.BasePath!, "Compilers", "NintendoClang", "include", "aarch64-nintendo-nn-elf"), CompileEnvironment));
				Arguments.Add(GetSystemIncludePathArgument(DirectoryReference.Combine(Info.BasePath!, "Compilers", "NintendoClang", "include", "aarch64-nintendo-nn-elf", "c++", "v1"), CompileEnvironment));
			}

			Arguments.Add("-fno-common");
			Arguments.Add("-fno-strict-aliasing");
			Arguments.Add("-fno-rtti");

			// Some flags found to reduce executable size
			if (CompileEnvironment.Configuration == CppConfiguration.Shipping)
			{
				Arguments.Add("-ffunction-sections");
				Arguments.Add("-fdata-sections");
				Arguments.Add("-fno-unwind-tables");
				Arguments.Add("-fno-asynchronous-unwind-tables");
			}

			if (CompileEnvironment.Configuration == CppConfiguration.Shipping && !CompileEnvironment.bPGOProfile)
			{
				Arguments.Add("-fno-use-cxa-atexit");
			}

			Arguments.Add($"-mcpu={TargetMpu}");

			if (Info.ClangVersion >= new Version(16,0,6))
			{
				Arguments.Add($"--target={TargetTriple}");
			}
		}

		protected virtual void GetLinkArguments_Global(LinkEnvironment LinkEnvironment, List<string> Arguments)
		{
			CppRootPaths RootPaths = LinkEnvironment.RootPaths;
			ulong SDKVersion = NintendoPlatformSDK.GetSDKVersionInt();

			Arguments.Add("-nostartfiles");
			Arguments.Add("-Wl,--build-id=md5");
			Arguments.Add("-Wl,--gc-sections");

			Arguments.Add("-Wl,--no-allow-shlib-undefined");
			Arguments.Add("-Wl,--no-undefined");

			Arguments.Add("-fuse-ld=lld.exe");

			// Profile Guided Optimization (PGO) and Link Time Optimization (LTO)
			if (LinkEnvironment.bPGOOptimize)
			{
				// Always enable LTO when using PGO.
				Log.TraceInformationOnce("Enabling Profile Guided Optimization (PGO). Linking will take a while.");
				DirectoryReference? PGODir = DirectoryReference.FromString(LinkEnvironment.PGODirectory!);
				Arguments.Add($"-fprofile-instr-use=\"{NormalizeCommandLinePath(DirectoryReference.Combine(PGODir!, LinkEnvironment.PGOFilenamePrefix!), RootPaths)}\"");
				Arguments.Add("-flto=thin");
				// lld should consider logical cores when determining how many threads to use
				Arguments.Add("-Wl,--thinlto-jobs=all");
				Arguments.Add("-Wl,-mllvm,-enable-machine-outliner=never");
			}
			else if (LinkEnvironment.bPGOProfile)
			{
				// Always enable LTO when generating PGO profile data.
				Log.TraceInformationOnce("Enabling Profile Guided Instrumentation (PGI). Linking will take a while.");
				Arguments.Add("-fprofile-instr-generate");
				Arguments.Add("-flto=thin");
				// lld should consider logical cores when determining how many threads to use
				Arguments.Add("-Wl,--thinlto-jobs=all");
				Arguments.Add("-Wl,-mllvm,-enable-machine-outliner=never");
			}
			else if (LinkEnvironment.bAllowLTCG)
			{
				// When not using PGO, enable LTO in shipping builds if opted-in by the build configuration.
				Log.TraceInformationOnce("Enabling Link Time Optimization (LTO). Linking may take a while.");
				Arguments.Add("-flto=thin");
				// lld should consider logical cores when determining how many threads to use
				Arguments.Add("-Wl,--thinlto-jobs=all");
				Arguments.Add("-Wl,-mllvm,-enable-machine-outliner=never");

				DirectoryReference? ThinLTOCacheDir = DirectoryReference.FromString(LinkEnvironment.ThinLTOCacheDirectory);
				if (ThinLTOCacheDir != null)
				{
					Arguments.Add($"-Wl,--thinlto-cache-dir=\"{NormalizeCommandLinePath(ThinLTOCacheDir, RootPaths)}\"");
					string? ThinLTOCachePruningArgs = LinkEnvironment.ThinLTOCachePruningArguments;
					if (ThinLTOCachePruningArgs != null)
					{
						Arguments.Add($"-Wl,--thinlto-cache-policy,{ThinLTOCachePruningArgs}");
					}
				}
			}

			if (NintendoPlatformSDK.bVerboseLinker)
			{
				Arguments.Add("-v");
			}

			Arguments.Add("-Wl,-init=_init,-fini=_fini");
			Arguments.Add("-Wl,-z,combreloc,-z,relro,--enable-new-dtags");

			if (LinkEnvironment.bIsBuildingDLL || bMergeModules)
			{
				Arguments.Add("-shared");
			}
			else
			{
				if (SDKVersion >= NintendoPlatformSDK.VersionXYZToInt("17", "2", "0"))
				{
					Arguments.Add("-pie");
				}
				else
				{
					Arguments.Add("-Wl,-pie");
				}
			}

			if (bMergeModules)
			{
				Arguments.Add("-Wl,-Bsymbolic");
				Arguments.Add("-Wl,--hash-style=gnu");
			}

			if (LinkEnvironment.Configuration == CppConfiguration.Shipping)
			{
				Arguments.Add("-Wl,--icf=all"); // Enables ICF (Identical Code Folding). [all, safe] safe == fold functions that can be proven not to have their address taken.
			}

			if (LinkEnvironment.PackagePath != null)
			{
				string? PackageDirectory = Path.GetDirectoryName(LinkEnvironment.PackagePath);

				if (PackageDirectory != null && !Directory.Exists(PackageDirectory))
				{
					Logger.LogWarning("PackagePath argument has been specified but directory \"{PackageDirectory}\" does not exist. Linker argument \"--package\" has been discarded.", PackageDirectory);
				}
				else if (PackageDirectory != null)
				{
					string? PackageFilename = Path.GetFileName(LinkEnvironment.PackagePath);

					if (PackageFilename != null && PackageFilename.Length < 4)
					{
						Logger.LogWarning("PackagePath argument has been specified but filename seems invalid or missing \"{PackageDirectory}\". Linker argument \"--package\" has been discarded.", PackageDirectory);
					}
					else if (PackageFilename != null)
					{
						FileReference PackagePath = new(LinkEnvironment.PackagePath!);
						Logger.LogInformation("--package has been specified. Linker input files will be zipped and written to \"{PackagePath}\"", LinkEnvironment.PackagePath);

						Arguments.Add($"-Wl,--reproduce=\"{NormalizeCommandLinePath(PackagePath, RootPaths)}\"");
					}
				}
			}

			// Set the output directory for crashes
			DirectoryReference? CrashDiagnosticDirectory = DirectoryReference.FromString(LinkEnvironment.CrashDiagnosticDirectory);
			if (CrashDiagnosticDirectory != null)
			{
				if (DirectoryReference.Exists(CrashDiagnosticDirectory))
				{
					Arguments.Add($"-fcrash-diagnostics-dir=\"{NormalizeCommandLinePath(CrashDiagnosticDirectory, RootPaths)}\"");
				}
				else
				{
					Log.TraceWarningOnce("CrashDiagnosticDirectory has been specified but directory \"{CrashDiagnosticDirectory}\" does not exist. Linker argument \"-fcrash-diagnostics-dir\" has been discarded.", CrashDiagnosticDirectory);
				}
			}

			if (LinkEnvironment.bCreateMapFile)
			{
				string MapFile = Utils.MakePathSafeToUseWithCommandLine(LinkEnvironment.OutputFilePath.ChangeExtension("map").ToString());
				Log.TraceInformationOnce("Creating map file {0}", MapFile);
				Arguments.Add($"-Wl,-Map,{MapFile}");
			}

			Arguments.Add("-Wl,-O3");

			if (SDKVersion >= NintendoPlatformSDK.VersionXYZToInt("16", "2", "3"))
			{
				Arguments.Add("-Wl,--pack-dyn-relocs=relr");
			}

			if (Info.ClangVersion >= new Version(16, 0, 6))
			{
				Arguments.Add($"--target={TargetTriple}");
			}
		}

		static void GetArchiveArguments_Global(LinkEnvironment LinkEnvironment, List<string> Arguments)
		{
			Arguments.Add("rc");
		}

		private static XElement AddElementTo(string ElementName, XElement Parent, bool bForceAddNewElement)
		{
			XElement? Elem = Parent.XPathSelectElement(ElementName);
			// make a new one even if there was already one there
			if (Elem == null || bForceAddNewElement)
			{
				Elem = new XElement(ElementName);
				Parent.Add(Elem);
			}
			return Elem;
		}
		private static void RemoveElement(string ElementName, XElement Parent)
		{
			XElement? Elem = Parent.XPathSelectElement(ElementName);
			Elem?.Remove();
		}

		private static int AlignUpArbitrary(int Value, int Align)
		{
			long Aligned = (((long)Value + Align - 1) / Align) * Align;
			return checked((int)Aligned);
		}

		private static void DirectoryCopy(string SourceDir, string DestDir)
		{
			DirectoryInfo DirInfo = new(SourceDir);

			// make sure destination exists
			if (!Directory.Exists(DestDir))
			{
				Directory.CreateDirectory(DestDir);
			}

			// get files in source directory
			foreach (FileInfo File in DirInfo.GetFiles())
			{
				string TargetFile = Path.Combine(DestDir, File.Name);
				FileInfo TargetFileInfo = new(TargetFile);
				if (TargetFileInfo.Exists)
				{
					TargetFileInfo.Attributes &= ~FileAttributes.ReadOnly;
				}
				File.CopyTo(TargetFile, true);
			}

			// now recurse
			foreach (DirectoryInfo Subdir in DirInfo.GetDirectories())
			{
				DirectoryCopy(Subdir.FullName, Path.Combine(DestDir, Subdir.Name));
			}
		}
		private static void DirectoryDelete(string Dir)
		{
			DirectoryInfo DirInfo = new(Dir);

			// get files in directory
			foreach (FileInfo File in DirInfo.GetFiles())
			{
				File.Attributes &= ~FileAttributes.ReadOnly;
				File.Delete();
			}

			// recurse
			foreach (DirectoryInfo Subdir in DirInfo.GetDirectories())
			{
				DirectoryDelete(Subdir.FullName);
			}

			DirInfo.Delete();
		}

		private static bool SDKHasSettingHasInGameVoiceChat()
		{
			bool bHasSetting = false;
			ulong SDKVersionInt = NintendoPlatformSDK.GetSDKVersionInt();
			(int Major, int Minor, int Micro) = NintendoPlatformSDK.VersionXYZFromInt(SDKVersionInt);
			bHasSetting = (Major == 19) && (SDKVersionInt >= NintendoPlatformSDK.VersionXYZToInt("19", "3", "4"));
			bHasSetting |= SDKVersionInt >= NintendoPlatformSDK.VersionXYZToInt("20", "2", "0");
			return bHasSetting;
		}

		private static string ModifyMetaFile(UnrealTargetPlatform Platform, string SourceMetaPath, string StrippedProjectName, UnrealTargetConfiguration TargetConfig, FileReference? ProjectFile, ILogger Logger, bool bZeroApplicationVersion = false, int SuggestedApplicationVersion = -1)
		{
			NintendoPlatform BuildPlatform = (NintendoPlatform)UEBuildPlatform.GetBuildPlatform(Platform);

			// read settings from the config
			string? IniPath = ProjectFile?.Directory.FullName;
			if (string.IsNullOrEmpty(IniPath))
			{
				// If the project file hasn't been specified, try to get the path from -remoteini command line param
				IniPath = UnrealBuildTool.GetRemoteIniPath();
			}

			string ConfigSectionName = GetConfigSectionNameForPlatform(Platform);

			string ProjectDir = GetProjectDir(ProjectFile);

			DirectoryReference? IniDir = !string.IsNullOrEmpty(IniPath) ? new DirectoryReference(IniPath) : null;

			ConfigHierarchy Ini = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, IniDir, Platform);

			string ProgramId;
			string StartupUserAccount;
			string TouchScreenUsage;
			string PresenceGroupId;
			string ApplicationNameString;
			string ApplicationVersionString;
			string ApplicationErrorCodeCategory;
			string SaveGameOwnerId;
			string LogoType;
			string LocalCommunicationId;
			string RuntimeAddOnContentInstall;
			List<string> SupportedLanguages;
			List<string> LocalizedTitles;
			List<string> AgeRatings;
			List<string> ParentalControls;

			Ini.GetString(ConfigSectionName, "ProgramId", out ProgramId!);
			Ini.GetString(ConfigSectionName, "StartupAccountMode", out StartupUserAccount!);
			Ini.GetString(ConfigSectionName, "TouchScreenUsage", out TouchScreenUsage!);
			Ini.GetString(ConfigSectionName, "FriendPresenceGroupId", out PresenceGroupId!);
			Ini.GetString(ConfigSectionName, "ApplicationVersionString", out ApplicationVersionString!);
			Ini.GetString(ConfigSectionName, "ApplicationErrorCode", out ApplicationErrorCodeCategory!);
			Ini.GetString(ConfigSectionName, "SaveGameOwnerId", out SaveGameOwnerId!);
			Ini.GetString(ConfigSectionName, "LogoType", out LogoType!);
			Ini.GetString(ConfigSectionName, "LdnLocalCommId", out LocalCommunicationId!);
			Ini.GetString(ConfigSectionName, "RuntimeAddOnContentInstall", out RuntimeAddOnContentInstall!);
			Ini.GetArray(ConfigSectionName, "SupportedLanguages", out SupportedLanguages!);
			Ini.GetArray(ConfigSectionName, "LocalizedTitles", out LocalizedTitles!);
			Ini.GetArray(ConfigSectionName, "ParentalControls", out ParentalControls!);
			Ini.GetArray(ConfigSectionName, "AgeRatings", out AgeRatings!);
			Ini.GetInt32(ConfigSectionName, "SaveGameSizeKB", out int SaveGameSizeKB);
			Ini.GetInt32(ConfigSectionName, "SaveGameJournalSizeKB", out int SaveGameJournalSizeKB);
			Ini.GetInt32(ConfigSectionName, "SaveGameSizeMaxMB", out int SaveGameSizeMaxMB);
			Ini.GetInt32(ConfigSectionName, "SaveGameJournalSizeMaxMB", out int SaveGameJournalSizeMaxMB);
			Ini.GetInt32(ConfigSectionName, "TempDataSizeKB", out int TempDataSizeKB);
			Ini.GetInt32(ConfigSectionName, "CacheDataSizeKB", out int CacheDataSizeKB);
			Ini.GetInt32(ConfigSectionName, "CacheDataJournalSizeKB", out int CacheDataJournalSizeKB);
			Ini.GetInt32(ConfigSectionName, "CacheStorageDataAndJournalSizeMaxKB", out int CacheStorageDataAndJournalSizeMaxKB);
			Ini.GetInt32(ConfigSectionName, "MainThreadStackSizeKB", out int MainThreadStackSizeKB);
			Ini.GetInt32(ConfigSectionName, "DebugMainThreadStackSizeKB", out int DebugMainThreadStackSizeKB);
			Ini.GetInt32(ConfigSectionName, "ApplicationVersion", out int ApplicationVersion);
			Ini.GetInt32(ConfigSectionName, "SystemResourceSizeMB", out int SystemResourceSizeMB);
			Ini.GetInt32(ConfigSectionName, "GameCardSizeGB", out int GameCardSizeGB);
			Ini.GetInt32(ConfigSectionName, "GameCardClockRate", out int GameCardClockRate);
			Ini.GetBool(ConfigSectionName, "bAllowScreenshots", out bool bAllowScreenshot);
			Ini.GetBool(ConfigSectionName, "bShowDataLossConfirmationDialog", out bool bShowDataLossConfirmationDialog);
			Ini.GetBool(ConfigSectionName, "bAllowCrashReporting", out bool bAllowCrashReporting);
			Ini.GetBool(ConfigSectionName, "bAllowScreenshotsWithCrashReports", out bool bAllowScreenshotsWithCrashReports);
			Ini.GetBool(ConfigSectionName, "bAllowScreenshotsWithCrashReportsInDevelopmment", out bool bAllowScreenshotsWithCrashReportsInDevelopment);
			Ini.GetBool(ConfigSectionName, "bAllowVideoCapture", out bool bAllowVideoCapture);
			Ini.GetBool(ConfigSectionName, "bUseManualLogoHide", out bool bUseManualLogoHide);
			Ini.GetBool(ConfigSectionName, "bDemoBuild", out bool bDemoBuild);
			Ini.GetBool(ConfigSectionName, "bUserAccountSwitchLock", out bool bUserAccountSwitchLock);
			Ini.GetBool(ConfigSectionName, "bStartupUserAccountIsOptional", out bool bStartupUserAccountIsOptional);
			Ini.GetBool(ConfigSectionName, "bAllowAOCInstallWhileRunning", out bool bAllowAOCInstallWhileRunning);
			Ini.GetBool(ConfigSectionName, "bAllowEngineHTMLDocsInShipping", out bool bAllowEngineHTMLDocsInShipping);
			Ini.GetBool(ConfigSectionName, "bUseBuildLabelAsGameNameInNonShippingBuilds", out bool bUseBuildLabelAsGameName);
			Ini.GetBool(ConfigSectionName, "bHasInGameVoiceChat", out bool bHasInGameVoiceChat);

			bool bReadIndexedCacheStorage = Ini.GetInt32(ConfigSectionName, "CacheStorageIndexMaxHexValueCorrected", out int CacheStorageIndexMax);
			bool bUseIndexedCacheStorage = bReadIndexedCacheStorage && CacheStorageIndexMax > 0;

			if (!bReadIndexedCacheStorage)
			{
				// detect the presence of deprecated CacheStorageIndexMax and convert it if present
				// CacheStorageIndexMax was deprecated in 5.1
				bool bReadIndexedCacheStorageError = Ini.GetInt32(ConfigSectionName, "CacheStorageIndexMax", out int CacheStorageIndexMaxError);

				if (bReadIndexedCacheStorageError && CacheStorageIndexMaxError > 0)
				{
					Logger.LogWarning("CacheStorageIndexMax Ini property is deprecated, please open the project in the Editor to automatically convert to CacheStorageIndexMaxHexValueCorrected");

					// reinterpret value as hexadecimal to preserve original behavior
					CacheStorageIndexMax = Convert.ToInt32(CacheStorageIndexMaxError.ToString(), 16);
					bUseIndexedCacheStorage = true;
				}
			}

			Ini.GetString(ConfigSectionName, "ApplicationNameString", out ApplicationNameString!);
			if (string.IsNullOrWhiteSpace(ApplicationNameString))
			{
				ApplicationNameString = StrippedProjectName;
			}


			// potentially override video capture during development
			if (TargetConfig == UnrealTargetConfiguration.Debug || TargetConfig == UnrealTargetConfiguration.DebugGame || TargetConfig == UnrealTargetConfiguration.Development)
			{
				if (Ini.GetBool(ConfigSectionName, "bAllowVideoCaptureInDevelopment", out bool bAllowVideoCaptureInDevelopment))
				{
					bAllowVideoCapture = bAllowVideoCaptureInDevelopment;
				}
			}

			if (bZeroApplicationVersion)
			{
				if (SuggestedApplicationVersion >= 0)
				{
					Logger.LogWarning("SuggestedApplicationVersion ignored when bZeroApplicationVersion is true. Only one should be provided. SuggestedApplicationVersion will be ignored.");
				}
				ApplicationVersion = 0;
			}
			else if (SuggestedApplicationVersion > 0)
			{
				if (ApplicationVersion < SuggestedApplicationVersion)
				{
					Logger.LogWarning("ApplicationVersion {IniApplicationVersion} is less than SuggestedApplicationVersion {SuggestedApplicationVersion}. Using SuggestedApplicationVersion for this build. ApplicationVersion should be updated in {Platform}Engine.ini or NintendoEngine.ini.", ApplicationVersion, SuggestedApplicationVersion, Platform);
					ApplicationVersion = SuggestedApplicationVersion;
				}
			}

			// load the source meta file from the SDK
			XDocument MetaDoc = XDocument.Load(SourceMetaPath);

			// strip out the comments for more readable output file
			List<XNode> Comments = new();

			IEnumerable<XNode> Nodes = MetaDoc.DescendantNodes();
			foreach (XNode node in Nodes)
			{
				if (node.NodeType == System.Xml.XmlNodeType.Comment)
				{
					Comments.Add(node);
				}
			}
			foreach (XNode node in Comments)
			{
				node.Remove();
			}

			// find the appropriate root Xml element
			XElement? MetaElement = MetaDoc.XPathSelectElement("/NintendoSdkMeta");
			MetaElement ??= MetaDoc.XPathSelectElement("/Meta");

			BuildPlatform.ModifyMeta(MetaElement, Ini, ConfigSectionName);

			string? BuildLabel = null;

			// generate the BuildLabel and replace the displayed game name with it if enabled, it's not a Shipping build, the Build\Build.version files exists and formed correctly, and it has a branch name
			// 0 is a valid CL in some cases
			BuildVersion? Version;
			if (bUseBuildLabelAsGameName && TargetConfig != UnrealTargetConfiguration.Shipping && BuildVersion.TryRead(BuildVersion.GetDefaultFileName(), out Version) && !string.IsNullOrEmpty(Version.BranchName))
			{
				BuildLabel = string.Format("{0}-CL-{1}", Version.BranchName, Version.Changelist);

				// the maximum allowed game length by Nintendo is 127 chars so truncate if necessary
				const int MaxAllowedLength = 127;
				if (BuildLabel.Length > MaxAllowedLength)
				{
					BuildLabel = BuildLabel.Substring(0, MaxAllowedLength);
				}
			}          
			
			// add some stuff to Core tree
			{
				XElement CoreElement = AddElementTo("Core", MetaElement!, false);
				CoreElement.SetElementValue("Name", ApplicationNameString);
				CoreElement.SetElementValue("MainThreadStackSize", string.Format("0x{0:X}", 1024 * ((TargetConfig != UnrealTargetConfiguration.Shipping && TargetConfig != UnrealTargetConfiguration.Test) ? DebugMainThreadStackSizeKB : MainThreadStackSizeKB)));
				CoreElement.SetElementValue("ApplicationId", ProgramId);
				// force 64-bit
				CoreElement.SetElementValue("ProcessAddressSpace", "AddressSpace64Bit");

				int SystemResourceSizeBytes = SystemResourceSizeMB * 1024 * 1024;
				const int SystemResourceAlignment = 0x200000; // From Nintendo SDK documentation, "Writing the NMETA file"
				if (SystemResourceSizeBytes % SystemResourceAlignment != 0)
				{
					SystemResourceSizeBytes = AlignUpArbitrary(SystemResourceSizeBytes, SystemResourceAlignment);
					Logger.LogWarning("SystemResourceSizeMB ({SystemResourceSizeMB}) was not a multiple of 2MB, increasing to {SystemResourceSizeMBCorrected}",
						SystemResourceSizeMB, SystemResourceSizeBytes / 1024 / 1024);
				}
				CoreElement.SetElementValue("SystemResourceSize", string.Format("0x{0:X}", SystemResourceSizeBytes));
			}

			// ensure we always support one language.
			SupportedLanguages ??= new List<string>();

			if (SupportedLanguages.Count == 0)
			{
				SupportedLanguages.Add("AmericanEnglish");
			}

			// add stuff to Application tree
			{
				XElement ApplicationElement = AddElementTo("Application", MetaElement!, false);
				// remove some of the dummy entries that we don't replace but add to
				RemoveElement("Title", ApplicationElement);
				RemoveElement("Icon", ApplicationElement);

				if (SupportedLanguages != null)
				{
					RemoveElement("SupportedLanguage", ApplicationElement);

					// using ToHashSet() for easy way to remove duplicates
					foreach (string Language in SupportedLanguages.Distinct())
					{
						XElement LangElement = AddElementTo("SupportedLanguage", ApplicationElement, true);
						LangElement.SetValue(Language);
					}
				}

				if (ParentalControls != null)
				{
					// using ToHashSet() for easy way to remove duplicates
					foreach (string Control in ParentalControls.Distinct())
					{
						XElement LangElement = AddElementTo("ParentalControl", ApplicationElement, true);
						LangElement.SetValue(Control);
					}
				}

				// age rating settings
				if (AgeRatings != null)
				{
					RemoveElement("Rating", ApplicationElement);

					// add each localized title
					foreach (string RatingStruct in AgeRatings.Distinct())
					{
						// parse the string from a string like (Organization=ESRB,Age=16) format
						Dictionary<string, string> Props = ConfigHierarchy.GetStructKeyValuePairs(RatingStruct)!;
						// make sure we have all required settings
						string Organization, Age;
						try
						{
							Organization = Props["Organization"];
							Age = Props["Age"];

							// if we got all three, then add it
							XElement TitleElement = AddElementTo("Rating", ApplicationElement, true);
							TitleElement.SetElementValue("Organization", Organization);
							TitleElement.SetElementValue("Age", Age);
						}
						catch (Exception)
						{

						}
					}
				}

				// if no localized titles, we put in a default
				if (LocalizedTitles == null)
				{
					ConfigHierarchy GameIni = ConfigCache.ReadHierarchy(ConfigHierarchyType.Game, DirectoryReference.FromFile(ProjectFile), Platform);
					GameIni.GetString("/Script/EngineSettings.GeneralProjectSettings", "CompanyName", out string? CompanyName);

					// default Title element
					XElement TitleElement = AddElementTo("Title", ApplicationElement, true);
					TitleElement.SetElementValue("Language", (SupportedLanguages != null && SupportedLanguages.Count > 0) ? SupportedLanguages[0] : "AmericanEnglish");
					TitleElement.SetElementValue("Name", BuildLabel != null ? BuildLabel : ApplicationNameString);
					TitleElement.SetElementValue("Publisher", CompanyName);
				}
				else
				{
					// add each localized title
					foreach (string TitleStruct in LocalizedTitles.Distinct())
					{
						// parse the string from a string like (Language=AmericanEnglish,Title="FunkyTitle",Publisher="FunCo") format
						Dictionary<string, string> Props = ConfigHierarchy.GetStructKeyValuePairs(TitleStruct)!;
						// make sure we have all required settings
						string Language, Title, Publisher;
						try
						{
							Language = Props["Language"];
							Title = Props["Title"];
							Publisher = Props["Publisher"];

							// if we got all three, then add it
							XElement TitleElement = AddElementTo("Title", ApplicationElement, true);
							TitleElement.SetElementValue("Language", Language);
							TitleElement.SetElementValue("Name", BuildLabel != null ? BuildLabel : Title);
							TitleElement.SetElementValue("Publisher", Publisher);
						}
						catch (Exception)
						{

						}
					}
				}

				// use the engine's icons as defaults
				string NewIconPath = @$"Platforms\{Platform}\Build\Resources\Icons";
				string OldIconPath = @$"Build\{Platform}\Resources\Icons";
				string DefaultIconName = "AmericanEnglish.bmp";
				string EngineIconsDir = Unreal.EngineDirectory.FullName + @"\" + NewIconPath;
				string DefaultIconPath = Path.Combine(EngineIconsDir, DefaultIconName);

				// add icons paths (the --icon setting to AuthoringTool is deprecated)

				string ProjectIconsDir;
				if (DirectoryReference.Exists(DirectoryReference.Combine(new DirectoryReference(ProjectDir), NewIconPath)))
				{
					ProjectIconsDir = Path.Combine(ProjectDir, NewIconPath);
					Logger.LogInformation("Using icons from {IconsDir}", ProjectIconsDir);
				}
				else
				{
					ProjectIconsDir = Path.Combine(ProjectDir, OldIconPath);

					if (DirectoryReference.Exists(new DirectoryReference(ProjectIconsDir)))
					{
						Logger.LogWarning("Using icons from outside Platform Extension path ({IconsDir}).  Please use: {NewIconsDir}",
							ProjectIconsDir, Path.Combine(ProjectDir, NewIconPath));
					}
				}
				string ProjectDefaultIconPath = Path.Combine(ProjectIconsDir, DefaultIconName);

				// create the full path to this project's icon destination directory
				string IconAbsStagingDirectory = Path.GetFullPath(Path.Combine(ProjectDir, @$"Intermediate\Build\{Platform}\Icon_Staging", TargetConfig.ToString()));
				if (Directory.Exists(IconAbsStagingDirectory))
				{
					// clear the directory to prevent stale files left over from previous attempts
					DirectoryDelete(IconAbsStagingDirectory);
				}
				Directory.CreateDirectory(IconAbsStagingDirectory);

				if (SupportedLanguages != null)
				{
					// using Distinct() for easy way to remove duplicates
					foreach (string Language in SupportedLanguages.Distinct())
					{
						string IconFileName = Language + ".bmp";

						// project-specific, language-specific source path
						string ProjectLanguageIconPath = Path.Combine(ProjectIconsDir, IconFileName);

						// engine level, language-specific source path
						string EngineLanguageIconPath = Path.Combine(EngineIconsDir, IconFileName);

						string IconSourcePath;

						if (File.Exists(ProjectLanguageIconPath))
						{
							// if the project-specific, language-specific icon exists, use it
							IconSourcePath = ProjectLanguageIconPath;
						}
						else if (File.Exists(ProjectDefaultIconPath))
						{
							// if the project has a default language (AmericanEnglish) icon, use it
							IconSourcePath = ProjectDefaultIconPath;
						}
						else if (File.Exists(EngineLanguageIconPath))
						{
							// if the engine-level, language-specific icon exists, use that.
							IconSourcePath = EngineLanguageIconPath;
						}
						else
						{
							// if none of the above exist, use the engine-level, default language (AmericanEnglish) icon
							IconSourcePath = DefaultIconPath;
						}

						string IconAbsStagingPath = Path.Combine(IconAbsStagingDirectory, IconFileName);
						File.Copy(IconSourcePath, IconAbsStagingPath, true);
						// File.Copy also copies the attributes, so make sure the new file isn't read only
						FileAttributes Attrs = File.GetAttributes(IconAbsStagingPath);
						if (Attrs.HasFlag(FileAttributes.ReadOnly))
						{
							File.SetAttributes(IconAbsStagingPath, Attrs & ~FileAttributes.ReadOnly);
						}

						XElement IconElement = AddElementTo("Icon", ApplicationElement, true);
						IconElement.SetElementValue("Language", Language);
						IconElement.SetElementValue("IconPath", IconAbsStagingPath);

						{
							// path to optional small icon, assume the same location as the full icon
							string IconSourceDirectory = Path.GetDirectoryName(IconSourcePath)!;
							string IconSourceFilenameNoExtension = Path.GetFileNameWithoutExtension(IconSourcePath);
							string SmallIconSourcePath = Path.Combine(IconSourceDirectory, IconSourceFilenameNoExtension + "_small.jpg");

							if (File.Exists(SmallIconSourcePath))
							{
								string SmallIconAbsStagingPath = Path.Combine(IconAbsStagingDirectory, Language + "_small.jpg");

								File.Copy(SmallIconSourcePath, SmallIconAbsStagingPath, true);
								// File.Copy also copies the attributes, so make sure the new file isn't read only
								Attrs = File.GetAttributes(SmallIconAbsStagingPath);
								if (Attrs.HasFlag(FileAttributes.ReadOnly))
								{
									File.SetAttributes(SmallIconAbsStagingPath, Attrs & ~FileAttributes.ReadOnly);
								}

								IconElement.SetElementValue("NxIconPath", SmallIconAbsStagingPath);
							}
						}
					}
				}

				ApplicationElement.SetElementValue("ReleaseVersion", ApplicationVersion.ToString());
				ApplicationElement.SetElementValue("DisplayVersion", ApplicationVersionString);
				ApplicationElement.SetElementValue("LogoType", LogoType);
				ApplicationElement.SetElementValue("LogoHandling", bUseManualLogoHide ? "Manual" : "Auto");
				ApplicationElement.SetElementValue("TouchScreenUsage", TouchScreenUsage);
				ApplicationElement.SetElementValue("Screenshot", bAllowScreenshot ? "Allow" : "Deny");
				ApplicationElement.SetElementValue("StartupUserAccount", StartupUserAccount);
				ApplicationElement.SetElementValue("SaveDataOwnerId", string.IsNullOrEmpty(SaveGameOwnerId) ? ProgramId : SaveGameOwnerId);
				ApplicationElement.SetElementValue("DataLossConfirmation", bShowDataLossConfirmationDialog ? "Required" : "None");

				ApplicationElement.SetElementValue("CrashReport", bAllowCrashReporting ? "Allow" : "Deny");
				ApplicationElement.SetElementValue("CrashScreenshotForProd", bAllowScreenshotsWithCrashReports ? "Allow" : "Deny");
				ApplicationElement.SetElementValue("CrashScreenshotForDev", bAllowScreenshotsWithCrashReportsInDevelopment ? "Allow" : "Deny");

				ApplicationElement.SetElementValue("VideoCapture", bAllowVideoCapture ? "Enable" : "Disable");
				ApplicationElement.SetElementValue("RuntimeAddOnContentInstall", bAllowAOCInstallWhileRunning ? "AllowAppend" : "Deny");

				if (bStartupUserAccountIsOptional)
				{
					ApplicationElement.SetElementValue("StartupUserAccountOption", "IsOptional");
				}

				if (bDemoBuild)
				{
					ApplicationElement.SetElementValue("Attribute", "Demo");
				}

				// if no save game data size write 0 for both save size and journal size
				if (SaveGameSizeKB <= 0)
				{
					ApplicationElement.SetElementValue("UserAccountSaveDataSize", string.Format("0x{0:X}", 0));
					ApplicationElement.SetElementValue("UserAccountSaveDataJournalSize", string.Format("0x{0:X}", 0));

				}
				else
				{
					// ensure save data size is at least 64k
					if (SaveGameSizeKB < 64)
					{
						SaveGameSizeKB = 64;
					}

					// ensure save data size is a multiple of 16k
					ApplicationElement.SetElementValue("UserAccountSaveDataSize", string.Format("0x{0:X}", AlignUpArbitrary(SaveGameSizeKB, 16) * 1024));

					// ensure journal size is not 0 and is also a multiple of 16k
					if (SaveGameJournalSizeKB == 0)
					{
						SaveGameJournalSizeKB = 16;
					}
					ApplicationElement.SetElementValue("UserAccountSaveDataJournalSize", string.Format("0x{0:X}", AlignUpArbitrary(SaveGameJournalSizeKB, 16) * 1024));

					if (SaveGameSizeMaxMB > 0)
					{
						// if it's less than the base save size increase it
						if ((SaveGameSizeMaxMB * 1024) < SaveGameSizeKB)
						{
							SaveGameSizeMaxMB = (SaveGameSizeKB + 1023) / 1024;
						}

						SaveGameJournalSizeMaxMB = SaveGameJournalSizeMaxMB > 0 ? SaveGameJournalSizeMaxMB : 1;

						ApplicationElement.SetElementValue("UserAccountSaveDataSizeMax", string.Format("0x{0:X}", SaveGameSizeMaxMB * 1024 * 1024));
						ApplicationElement.SetElementValue("UserAccountSaveDataJournalSizeMax", string.Format("0x{0:X}", SaveGameJournalSizeMaxMB * 1024 * 1024));
					}
				}

				ApplicationElement.SetElementValue("UserAccountSwitchLock", bUserAccountSwitchLock ? "Enable" : "Disable");

				if (CacheDataSizeKB > 0)
				{
					// use the cache data size for journaled amount if nothing is specified?
					if (CacheDataJournalSizeKB == 0)
					{
						CacheDataJournalSizeKB = CacheDataSizeKB;
					}

					if (bUseIndexedCacheStorage)
					{
						// Use indexed cache storage

						// ensure it is large enough
						CacheStorageDataAndJournalSizeMaxKB = Math.Max(CacheStorageDataAndJournalSizeMaxKB, CacheDataSizeKB + CacheDataJournalSizeKB);

						ApplicationElement.SetElementValue("CacheStorageIndexMax", string.Format("0x{0:X}", CacheStorageIndexMax));
						ApplicationElement.SetElementValue("CacheStorageDataAndJournalSizeMax", string.Format("0x{0:X}", AlignUpArbitrary(CacheStorageDataAndJournalSizeMaxKB, 16) * 1024));
					}
					else
					{
						ApplicationElement.SetElementValue("CacheStorageSize", string.Format("0x{0:X}", AlignUpArbitrary(CacheDataSizeKB, 16) * 1024));
						ApplicationElement.SetElementValue("CacheStorageJournalSize", string.Format("0x{0:X}", AlignUpArbitrary(CacheDataJournalSizeKB, 16) * 1024));
					}
				}

				if (TempDataSizeKB > 0)
				{
					ApplicationElement.SetElementValue("TemporaryStorageSize", string.Format("0x{0:X}", AlignUpArbitrary(TempDataSizeKB, 16) * 1024));
				}

				// ensure we don't write out empty fields
				if (!string.IsNullOrEmpty(PresenceGroupId))
				{
					ApplicationElement.SetElementValue("PresenceGroupId", PresenceGroupId);
				}
				if (!string.IsNullOrEmpty(ApplicationErrorCodeCategory))
				{
					ApplicationElement.SetElementValue("ApplicationErrorCodeCategory", ApplicationErrorCodeCategory);
				}
				if (!string.IsNullOrEmpty(RuntimeAddOnContentInstall))
				{
					ApplicationElement.SetElementValue("RuntimeAddOnContentInstall", RuntimeAddOnContentInstall);
				}

				// add some directories and files if they exist
				// html files dir
				string ProjectHtmlFilesDir = Path.Combine(ProjectDir, @$"Platforms\{Platform}\Build\Resources\HtmlDocument");
				string EngineHtmlFilesDir = Path.Combine(Unreal.EngineDirectory.FullName, @$"Platforms\{Platform}\Build\Resources\HtmlDocument");

				// check original, non-platform extension path
				if (!Directory.Exists(ProjectHtmlFilesDir))
				{
					ProjectHtmlFilesDir = Path.Combine(ProjectDir, @$"Build\{Platform}\Resources\HtmlDocument");
					if (Directory.Exists(ProjectHtmlFilesDir))
					{
						Logger.LogWarning("Using HTML documents from outside Platform Extension path ({HtmlPath}), please use {RecommendedPath}",
							ProjectHtmlFilesDir, Path.Combine(ProjectDir, @$"Platforms\{Platform}\Build\Resources\HtmlDocument"));
					}
				}
				else
				{
					Logger.LogInformation("Using HTML documents from {HtmlDir}", ProjectHtmlFilesDir);
				}

				// copy any relevant HTML files to staging directory.
				string HtmlStagingDir = Path.Combine(ProjectDir, @$"Intermediate\Build\{Platform}\HtmlDocument_Staging", TargetConfig.ToString());

				// delete the target dir
				if (Directory.Exists(HtmlStagingDir))
				{
					DirectoryDelete(HtmlStagingDir);
				}

				// copy the game's documents
				if (ProjectHtmlFilesDir.ToLower() != EngineHtmlFilesDir.ToLower() && Directory.Exists(ProjectHtmlFilesDir))
				{
					DirectoryCopy(ProjectHtmlFilesDir, HtmlStagingDir);
				}


				// copy engine documents
				// NOTE: Historically this has copied the engine HTML files regardless of the build configuration, the new behavior is
				// to only copy them in non-shipping builds. If this behavior causes problems the ini variable bAllowEngineHTMLDocsInShipping can
				// be set to true to restore the old behavior.
				bool bShouldCopyEngineDocuments = bAllowEngineHTMLDocsInShipping || TargetConfig != UnrealTargetConfiguration.Shipping;
				if (bShouldCopyEngineDocuments && Directory.Exists(EngineHtmlFilesDir))
				{
					DirectoryCopy(EngineHtmlFilesDir, HtmlStagingDir);
				}

				// If the directory doesn't exists DirectoryCopy was not called due to the logic
				// above and we have no HTML files.
				if (Directory.Exists(HtmlStagingDir))
				{
					ApplicationElement.SetElementValue("HtmlDocumentPath", Path.GetFullPath(HtmlStagingDir));
				}

				// accessible URLs text file
				string AccessibleUrlsFile = Path.Combine(ProjectDir, @$"Platforms\{Platform}\Build\Resources\AccessibleUrls.txt");
				if (!File.Exists(AccessibleUrlsFile))
				{
					// check original, non-platform extension path
					AccessibleUrlsFile = Path.Combine(ProjectDir, @$"Build\{Platform}\Resources\AccessibleUrls.txt");
					if (File.Exists(AccessibleUrlsFile))
					{
						Logger.LogWarning("Using AccessibleUrls.txt from outside Platform Extension path ({Path}).  Please use {RecommendedPath}",
							AccessibleUrlsFile, Path.Combine(ProjectDir, @$"Platforms\{Platform}\Build\Resources\AccessibleUrls.txt"));
					}
				}
				else
				{
					Logger.LogInformation("Using AccessibleUrls.txt from path {AccessibleUrlsFile}", AccessibleUrlsFile);
				}

				if (File.Exists(AccessibleUrlsFile))
				{
					ApplicationElement.SetElementValue("AccessibleUrlsFilePath", Path.GetFullPath(AccessibleUrlsFile));
				}

				// legal info zip file
				string LegalInformationFile = Path.Combine(ProjectDir, @$"Platforms\{Platform}\Build\Resources\LegalInformation.zip");
				if (!File.Exists(LegalInformationFile))
				{
					// check original, non-platform extension path
					LegalInformationFile = Path.Combine(ProjectDir, @$"Build\{Platform}\Resources\LegalInformation.zip");

					if (File.Exists(LegalInformationFile))
					{
						Logger.LogWarning("Using LegalInformation.zip from outside Platform Extension path ({Path}).  Please use {RecommendedPath}",
							LegalInformationFile, Path.Combine(ProjectDir, @$"Platforms\{Platform}\Build\Resources\LegalInformation.zip"));
					}
				}
				else
				{
					Logger.LogInformation("Using LegalInformation.zip from path {LegalInformationFile}", LegalInformationFile);
				}

				if (File.Exists(LegalInformationFile))
				{
					ApplicationElement.SetElementValue("LegalInformationFilePath", Path.GetFullPath(LegalInformationFile));
				}

				// validate CardSpec parameters
				if (!IsZeroOrPowerOfTwo(GameCardSizeGB))
				{
					GameCardSizeGB = 0;
				}
				else if (GameCardSizeGB > 32)
				{
					GameCardSizeGB = 32;
				}

				if (GameCardClockRate != 25 && GameCardClockRate != 50)
				{
					GameCardClockRate = 0;
				}

				// write CardSpec section if one of the values is non-zero.
				if (GameCardSizeGB != 0 || GameCardClockRate != 0)
				{
					XElement GameCardElement = AddElementTo("CardSpec", MetaElement!, false);

					if (GameCardSizeGB != 0)
					{
						GameCardElement.SetElementValue("Size", GameCardSizeGB.ToString());
					}
					if (GameCardClockRate != 0)
					{
						GameCardElement.SetElementValue("ClockRate", GameCardClockRate.ToString());
					}

				}

				if (!string.IsNullOrEmpty(LocalCommunicationId))
				{
					ApplicationElement.SetElementValue("LocalCommunicationId", LocalCommunicationId);
				}

				if (SDKHasSettingHasInGameVoiceChat())
				{
					ApplicationElement.SetElementValue("HasInGameVoiceChat", bHasInGameVoiceChat ? "True" : "False");
				}
			}

			return MetaDoc.ToString();
		}

		private static bool IsZeroOrPowerOfTwo(int val)
		{
			return (val & (val - 1)) == 0;
		}

		private static string GetStrippedProjectName(string PathWithProjectName)
		{
			string StrippedProjectName = Path.GetFileNameWithoutExtension(PathWithProjectName);
			if (StrippedProjectName.Length > 11)
			{
				StrippedProjectName = StrippedProjectName[..11];
			}

			return StrippedProjectName;
		}

		protected static string GetProjectDir(FileReference? ProjectFile)
		{
			// get location of the project
			string ProjectDir = Unreal.EngineDirectory.FullName;
			if (ProjectFile != null)
			{
				ProjectDir = Path.GetDirectoryName(ProjectFile.FullName)!;
			}

			return ProjectDir;
		}

		public static string GenerateMetaFile(UnrealTargetPlatform Platform, FileReference? ProjectFile, UnrealTargetConfiguration TargetConfig, ILogger Logger, bool bZeroApplicationVersion = false, int SuggestedApplicationVersion = -1)
		{
			string ProjectDir = GetProjectDir(ProjectFile);
			NintendoPlatformSDK? PlatformSDK = UEBuildPlatformSDK.GetSDKForPlatform(Platform.ToString()) as NintendoPlatformSDK;

			string? MetaFilePath = null;
			// look for an override meta file, instead of making one from the SDK
			string OverrideMetaFile = Path.Combine(ProjectDir, @$"Platforms\{Platform}\Build\Resources\Override.meta");

			if (!File.Exists(OverrideMetaFile))
			{
				// check original, non-platform extension path
				OverrideMetaFile = Path.Combine(ProjectDir, @$"Build\{Platform}\Resources\Override.meta");

				if (File.Exists(OverrideMetaFile))
				{
					Logger.LogWarning(@$"Using Override.meta from outside Platform Extension path ({OverrideMetaFile}).  Please use: {ProjectDir}\Platforms\{Platform}\Build\Resources\Override.meta");
				}
			}

			if (File.Exists(OverrideMetaFile))
			{
				MetaFilePath = OverrideMetaFile;
				Logger.LogInformation("Using override NMeta file at: {OverrideMetaFile}", OverrideMetaFile);
			}
			else
			{
				string SourceMetaPath = PlatformSDK!.GetMetaFile();

				string ProjectName = GetStrippedProjectName(ProjectFile != null ? ProjectFile.FullName : "UnrealGame");
				// update the meta data with our project settings
				string MetaDocContents = ModifyMetaFile(Platform, SourceMetaPath, ProjectName, TargetConfig, ProjectFile, Logger, bZeroApplicationVersion, SuggestedApplicationVersion);

				// write it out if it's different (doing this check avoids the file always being out of date)
				string Extension = (TargetConfig == UnrealTargetConfiguration.Development) ? "" : "-" + TargetConfig.ToString();

				MetaFilePath = Path.Combine(ProjectDir, @$"Intermediate\Build\{Platform}\{ProjectName}{Extension}.meta");
				if (!File.Exists(MetaFilePath) || File.ReadAllText(MetaFilePath) != MetaDocContents)
				{
					Directory.CreateDirectory(Path.GetDirectoryName(MetaFilePath)!);
					File.WriteAllText(MetaFilePath, MetaDocContents);
				}
				Logger.LogInformation("Using generated NMeta file at: {Path}", MetaFilePath);
				Logger.LogDebug("{Contents}", MetaDocContents);
			}

			return Path.GetFullPath(MetaFilePath.Replace("/", "\\"));
		}

		private static string ModifyAddOnContentMetaFile(UnrealTargetPlatform Platform, string? SourceMetaPath, string ProjectName, Dictionary<int, string> AddOnContentList, FileReference? ProjectFile, string AddOnContentVersion, DirectoryReference StageRoot, FileReference BaseFilterRulesFile)
		{
			// read settings from the config
			string? IniPath = ProjectFile?.Directory.FullName;
			if (string.IsNullOrEmpty(IniPath))
			{
				// If the project file hasn't been specified, try to get the path from -remoteini command line param
				IniPath = UnrealBuildTool.GetRemoteIniPath();
			}

			DirectoryReference? IniDir = !string.IsNullOrEmpty(IniPath) ? new DirectoryReference(IniPath) : null;

			ConfigHierarchy Ini = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, IniDir, Platform);
			string ProgramId;

			string ConfigSectionName = GetConfigSectionNameForPlatform(Platform);


			Ini.GetString(ConfigSectionName, "ProgramId", out ProgramId!);
			Ini.GetInt32(ConfigSectionName, "ApplicationVersion", out int Version);

			// load the source meta file from the SDK
			XDocument MetaDoc;
			if (string.IsNullOrEmpty(SourceMetaPath))
			{
				MetaDoc = new XDocument();
			}
			else
			{
				MetaDoc = XDocument.Load(SourceMetaPath);
			}

			// strip out the comments for more readable output file
			List<XNode> Comments = new();

			IEnumerable<XNode> Nodes = MetaDoc.DescendantNodes();
			foreach (XNode node in Nodes)
			{
				if (node.NodeType == System.Xml.XmlNodeType.Comment)
				{
					Comments.Add(node);
				}
			}
			foreach (XNode node in Comments)
			{
				node.Remove();
			}

			// Create the root element if it does not exist
			if (MetaDoc.XPathSelectElement("/NintendoSdkMeta") == null)
			{
				MetaDoc.Add(new XElement("NintendoSdkMeta"));
			}

			// AddOnContent
			{
				List<XElement> AddOnContentElements = MetaDoc.XPathSelectElements("/NintendoSdkMeta/AddOnContent").ToList();

				// Clean out whatever is there.
				foreach (XElement Element in AddOnContentElements)
				{
					Element.Remove();
				}

				foreach (KeyValuePair<int, string> AddOnContent in AddOnContentList)
				{
					XElement Element = new("AddOnContent");
					Element.SetElementValue("Index", AddOnContent.Key.ToString());
					Element.SetElementValue("ApplicationId", ProgramId);
					Element.SetElementValue("Tag", AddOnContent.Value);

					if (StageRoot != null)
					{
						Element.SetElementValue("DataPath", StageRoot.FullName);
					}
					else
					{
						Element.SetElementValue("DataPath", GetProjectDir(ProjectFile) + @"\Plugins\" + AddOnContent.Value + @$"\Saved\StagedBuilds\{Platform}\" + ProjectName + @"\Plugins\" + AddOnContent.Value);
					}

					if (BaseFilterRulesFile != null)
					{
						string FilterRulesFile = BaseFilterRulesFile.ToString() + "_" + AddOnContent.Key.ToString();
						Element.SetElementValue("FilterDescriptionFilePath", FilterRulesFile);
					}

					Element.SetElementValue("RequiredApplicationReleaseVersion", Version.ToString());
					Element.SetElementValue("ReleaseVersion", AddOnContentVersion);
					MetaDoc.XPathSelectElement("/NintendoSdkMeta")!.Add(Element);
				}
			}

			return "<?xml version=\"1.0\"?>" + Environment.NewLine + MetaDoc.ToString() + Environment.NewLine;
		}

		public static string GenerateAddOnContentMetaFile(UnrealTargetPlatform Platform, FileReference ProjectFile, Dictionary<int, string> AddOnContentList, FileReference OverrideMetaFile, 
			string AddOnContentVersion, string AddOnContentNSPName, DirectoryReference StagedRoot, FileReference BaseFilterRulesFile)
		{
			string ProjectDir = GetProjectDir(ProjectFile);

			string? MetaFilePath = null;
			// look for an override meta file, instead of making one from the SDK
			if (OverrideMetaFile != null && File.Exists(OverrideMetaFile.FullName))
			{
				MetaFilePath = OverrideMetaFile.FullName;
			}
			else
			{
				string ProjectName = Path.GetFileNameWithoutExtension(ProjectFile != null ? ProjectFile.FullName : "UnrealGame");
				// update the meta data with our project settings
				string MetaDocContents = ModifyAddOnContentMetaFile(Platform, null, ProjectName, AddOnContentList, ProjectFile, AddOnContentVersion, StagedRoot, BaseFilterRulesFile);

				// write it out if it's different (doing this check avoids the file always being out of date)
				MetaFilePath = Path.Combine(ProjectDir, @$"Intermediate\Build\{Platform}\" + AddOnContentNSPName + ".nmeta");
				if (!File.Exists(MetaFilePath) || File.ReadAllText(MetaFilePath) != MetaDocContents)
				{
					Directory.CreateDirectory(Path.GetDirectoryName(MetaFilePath)!);
					File.WriteAllText(MetaFilePath, MetaDocContents);
				}

				if (OverrideMetaFile != null)
				{
					// This now becomes the override file (and developer can change DLC index in this file for the case of multiple DLCs)
					Directory.CreateDirectory(Path.GetDirectoryName(OverrideMetaFile.FullName)!);
					File.WriteAllText(OverrideMetaFile.FullName, MetaDocContents);
				}
			}

			return Path.GetFullPath(MetaFilePath.Replace("/", "\\"));
		}

		protected override List<string> ExpandResponseFileContents(List<string> ResponseFileContents)
		{
			return ResponseFileContents.Select(x => Utils.ExpandVariables(x)).ToList();
		}

		public static string LocateDescFile(UnrealTargetPlatform Platform, string ProjectRoot, ILogger Logger)
		{
			string DescFilePath;
			// look for an override desc file, instead of getting one from the SDK
			DescFilePath = Path.Combine(ProjectRoot, @$"Platforms\{Platform}\Build\Resources\Override.desc");

			if (!File.Exists(DescFilePath))
			{
				// check non-platform extension path
				DescFilePath = Path.Combine(ProjectRoot, @$"Build\{Platform}\Resources\Override.desc");

				if (File.Exists(DescFilePath))
				{
					Logger.LogWarning("Using Override.desc from outside {Platform} Platform Extension path ({Path}).  Please use: {RecommendedPath}",
						Platform, DescFilePath, Path.Combine(ProjectRoot, @$"Platforms\{Platform}\Build\Resources\Override.desc"));
				}
			}

			if (!File.Exists(DescFilePath))
			{
				NintendoPlatformSDK? PlatformSDK = UEBuildPlatformSDK.GetSDKForPlatform(Platform.ToString()) as NintendoPlatformSDK;

				string TargetName = PlatformSDK!.GetTargetName();
				DescFilePath = Path.Combine(NintendoPlatformSDK.GetSDKInstallLocation()!, $@"Resources\SpecFiles\Desc\{TargetName}\Application.autogen.desc");
				if (!File.Exists(DescFilePath))
				{
					DescFilePath = Path.Combine(NintendoPlatformSDK.GetSDKInstallLocation()!, @"Resources\SpecFiles\Application.desc");
					Logger.LogInformation("Using Application.desc from {Platform} SDK: {DescFilePath}", Platform, DescFilePath);
				}
				else
				{
					Logger.LogInformation("Using Application.autogen.desc from {Platform} SDK: {DescFilePath}", Platform, DescFilePath);
				}
			}
			else
			{
				Logger.LogInformation("Using Override.desc from {DescFilePath}", DescFilePath);
			}

			return DescFilePath;
		}

		public static string LocateDescFile(UnrealTargetPlatform Platform, FileReference? ProjectFile, ILogger Logger)
		{
			return LocateDescFile(Platform, GetProjectDir(ProjectFile), Logger);
		}

		public static string GetMakeMetaCommandline(string MetaFile, string DescFile, string NpdmFile, string AuthoringToolPlatformArg)
		{
			return $"--desc \"{DescFile}\" --meta \"{MetaFile}\" -o \"{NpdmFile}\"" +
				   $" -d DefaultIs64BitInstruction=True -d DefaultProcessAddressSpace=AddressSpace64Bit {AuthoringToolPlatformArg}";
		}

		public static FileItem MakeFileCopyAction(string SourceFilePath, string DestFilePath, IActionGraphBuilder Graph)
		{
			FileItem SourceFileItem = FileItem.GetItemByPath(SourceFilePath);
			FileItem DestFileItem = FileItem.GetItemByPath(DestFilePath);

			Graph.CreateCopyAction(SourceFileItem, DestFileItem);
			return DestFileItem;
		}
		public override void SetupBundleDependencies(ReadOnlyTargetRules Target, IEnumerable<UEBuildBinary> Binaries, string GameName)
		{
			base.SetupBundleDependencies(Target, Binaries, GameName);

			BundledNroFiles.Clear();

			foreach (UEBuildBinary Binary in Binaries)
			{
				if (Path.GetExtension(Binary.OutputFilePath.FullName).Equals(".nro", StringComparison.InvariantCultureIgnoreCase))
				{
					BundledNroFiles.Add(Binary.OutputFilePath.FullName);
				}
			}
		}
		private FileReference GetNrrFile(DirectoryReference? OutputDirectory,  UnrealTargetConfiguration Config)
		{
			if (OutputDirectory == null)
			{
				throw new BuildException("OutputDirectory is not set.");
			}

			return new FileReference(Path.Combine(OutputDirectory.ToString(), "ModuleInfo-" + Config + ".nrr"));
		}

		public override void ModifyBuildProducts(ReadOnlyTargetRules Target, UEBuildBinary Binary, IEnumerable<string> Libraries, IEnumerable<UEBuildBundleResource> BundleResources, Dictionary<FileReference, BuildProductType> BuildProducts)
		{
			if (Target.Platform != Platform)
			{
				throw new BuildException("ModifyBuildProducts's Target's Platform didn't match the Toolchain's platform. Unexpected");
			}

			if (Binary.Type == UEBuildBinaryType.Executable)
			{
				HashSet<string> Nsos = new();

				int NextSubSdkIndex = 0;

				DirectoryReference ProjectDirRef = new(GetProjectDir(Target.ProjectFile));

				// determine where to get our .ini settings
				DirectoryReference IniDirRef = Unreal.EngineDirectory;

				if (Target.ProjectFile != null)
				{
					IniDirRef = ProjectDirRef;
				}
				else
				{
					string? RemoteIniPath = UnrealBuildTool.GetRemoteIniPath();
					if (RemoteIniPath != null)
					{
						IniDirRef = new DirectoryReference(RemoteIniPath);
					}
				}

				ConfigHierarchy Ini = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, IniDirRef, Target.Platform);
				string ConfigSectionName = GetConfigSectionName();


				DirectoryReference ResourcesDirRef = DirectoryReference.Combine(ProjectDirRef, @$"Platforms\{Target.Platform}\Build\Resources");
				DirectoryReference ResourcesDirRefDeprecated = DirectoryReference.Combine(ProjectDirRef, @$"Build\{Target.Platform}\Resources");

				DirectoryReference OutputDirRef = new(Path.ChangeExtension(Binary.OutputFilePath.FullName, ".nspd"));

				// add symbol file
				BuildProducts.Add(new FileReference(Path.ChangeExtension(Binary.OutputFilePath.FullName, ".nss")), BuildProductType.SymbolFile);

				string ProgramDataDir = DirectoryReference.Combine(OutputDirRef, @"program0.ncd\data").ToString();
				string ProgramDataNroDir = Path.Combine(ProgramDataDir, "nro");
				string ProgramDataNrrDir = Path.Combine(ProgramDataDir, ".nrr");
				bool bHasNro = false;
				foreach (ModuleRules.RuntimeDependency Dep in Binary.RuntimeDependencies)
				{
					if (Path.GetExtension(Dep.Path).Equals(".nro", StringComparison.InvariantCultureIgnoreCase))
					{
						bHasNro = true;
						BuildProducts.Add(new FileReference(Path.Combine(ProgramDataNroDir, Path.GetFileName(Dep.Path))), BuildProductType.DynamicLibrary);
					}
					else if (Path.GetExtension(Dep.Path).Equals(".nrr", StringComparison.InvariantCultureIgnoreCase))
					{
						BuildProducts.Add(new FileReference(Path.Combine(ProgramDataNrrDir, Path.GetFileName(Dep.Path))), BuildProductType.RequiredResource);
					}
				}

				if (bHasNro)
				{
					FileReference NrrFile = GetNrrFile(Binary.OutputDir, Target.Configuration);
					BuildProducts.Add(NrrFile, BuildProductType.RequiredResource);

					// Add the output copy of the NRR in the NSPD to BuildProducts to ensure it is staged along with the rest of the NSPD
					// This allows us to keep build configuration specific copies of the NRR
					// - build the NRR file path located in the NSPD directory (the NRR is copied to this location by AuthoringTool.exe)
					string ProgramDataNrrPath = Path.Combine(ProgramDataNrrDir, NrrFile.GetFileName());

					// - ensure that NRR file is staged by adding it to BuildProducts
					Logger.LogInformation("Adding AuthoringTool.exe destination path {ProgramDataNrrPath} to BuildProducts", ProgramDataNrrPath);
					BuildProducts.Add(new FileReference(ProgramDataNrrPath), BuildProductType.RequiredResource);
				}

				// Add the nrs files - required in SDK 18.2.0 onwards
				if (NintendoPlatformSDK.GetSDKVersionInt() >= NintendoPlatformSDK.VersionXYZToInt("18","2","0"))
				{
					foreach (string Library in Libraries.Where( X => Path.GetExtension(X).Equals(".nrs", StringComparison.InvariantCultureIgnoreCase)))
					{
						BuildProducts.Add(new FileReference(Library), BuildProductType.RequiredResource);
					}
				}

				// new verificationData file
				if (NintendoPlatformSDK.GetSDKVersionInt() >= NintendoPlatformSDK.VersionXYZToInt("20", "0", "0"))
				{
					DirectoryReference ProgramDirRef = DirectoryReference.Combine(OutputDirRef, @"program0.ncd");
					BuildProducts.Add(FileReference.Combine(ProgramDirRef, "verificationData"), BuildProductType.RequiredResource);
				}

				// program0.ndc directory
				DirectoryReference ProgramCodeDirRef = DirectoryReference.Combine(OutputDirRef, @"program0.ncd\code");

				// this is a hack since we don't have access to the GlobalLinkEnvironment in ModifyBuildProducts
				BuildProducts.Add(FileReference.Combine(ProgramCodeDirRef, "main"), BuildProductType.RequiredResource);
				BuildProducts.Add(FileReference.Combine(ProgramCodeDirRef, "main.npdm"), BuildProductType.RequiredResource);
				BuildProducts.Add(FileReference.Combine(ProgramCodeDirRef, "sdk"), BuildProductType.RequiredResource);
				BuildProducts.Add(FileReference.Combine(ProgramCodeDirRef, "rtld"), BuildProductType.RequiredResource);

				if (!Directory.Exists(Binary.OutputDir.FullName))
				{
					Directory.CreateDirectory(Binary.OutputDir.FullName);
				}

				NintendoPlatformSDK? PlatformSDK = UEBuildPlatformSDK.GetSDKForPlatform(Target.Platform.ToString()) as NintendoPlatformSDK;
				string ConfigLibDir = PlatformSDK!.GetConfigLibDir(Target);

 				foreach (string Framework in BaseNspFrameworks)
				{
					string SrcFrameworkSymbolName = Path.Combine(ConfigLibDir, Path.ChangeExtension(Framework, ".nss"));
					if (File.Exists(SrcFrameworkSymbolName))
					{
						string DstFrameworkSymbolName = Path.Combine(Binary.OutputDir.FullName, Path.GetFileName(SrcFrameworkSymbolName));
						if (!File.Exists(DstFrameworkSymbolName))
						{
							File.Copy(SrcFrameworkSymbolName, DstFrameworkSymbolName);
						}
						if (!BuildProducts.ContainsKey(new FileReference(DstFrameworkSymbolName)))
						{
							BuildProducts.Add(new FileReference(DstFrameworkSymbolName), BuildProductType.SymbolFile);
						}
					}
				}

				foreach (UEBuildModule Module in Binary.Modules)
				{
					List<string> Frameworks = Module.GetPublicFrameworks();

					foreach (string Framework in Frameworks)
					{
						if (!Nsos.Contains(Framework))
						{
							Nsos.Add(Framework);
							BuildProducts.Add(FileReference.Combine(ProgramCodeDirRef, string.Format("subsdk{0}", NextSubSdkIndex++)), BuildProductType.RequiredResource);

							string SrcFrameworkSymbolFile = Path.ChangeExtension(Framework, ".nss");
							if (File.Exists(SrcFrameworkSymbolFile))
							{
								string DstFrameworkSymbolFile = Path.Combine(Path.GetDirectoryName(Binary.OutputFilePath.FullName)!, Path.GetFileName(SrcFrameworkSymbolFile));

								if (!File.Exists(DstFrameworkSymbolFile))
								{
									File.Copy(SrcFrameworkSymbolFile, DstFrameworkSymbolFile);
								}
								if (!BuildProducts.ContainsKey(new FileReference(DstFrameworkSymbolFile)))
								{
									BuildProducts.Add(new FileReference(DstFrameworkSymbolFile), BuildProductType.SymbolFile);
								}
							}
						}
					}
				}

				// meta0.ncd directory
				string ProgramId;

				Ini.GetString(ConfigSectionName, "ProgramId", out ProgramId!);
				if (ProgramId.StartsWith("0x"))
				{
					ProgramId = ProgramId.Remove(0, 2);
				}

				BuildProducts.Add(FileReference.Combine(OutputDirRef, @"meta0.ncd\data\Application_" + ProgramId + ".cnmt"), BuildProductType.RequiredResource);

				// control0.ncd directory
				DirectoryReference ControlDataDirRef = DirectoryReference.Combine(OutputDirRef, @"control0.ncd\data");

				BuildProducts.Add(FileReference.Combine(ControlDataDirRef, "control.nacp"), BuildProductType.RequiredResource);

				List<string> SupportedLanguages;

				Ini.GetArray(ConfigSectionName, "SupportedLanguages", out SupportedLanguages!);

				SupportedLanguages ??= new List<string>();

				if (SupportedLanguages.Count == 0)
				{
					SupportedLanguages.Add("AmericanEnglish");
				}

				foreach (string Language in SupportedLanguages.Distinct())
				{
					BuildProducts.Add(FileReference.Combine(ControlDataDirRef, "icon_" + Language + ".dat"), BuildProductType.RequiredResource);
				}

				// htmlDocument0.ncd directory
				DirectoryReference HTMLDocsOutputDir = DirectoryReference.Combine(OutputDirRef, @"htmlDocument0.ncd\data\html-document");
				Action<string> CollectHtmlFiles = HTMLDocsDir =>
				{
					if (Directory.Exists(HTMLDocsDir))
					{
						string[] HTMLFiles = Directory.GetFiles(HTMLDocsDir, "*", SearchOption.AllDirectories);

						Logger.LogDebug("Searched {HTMLDocsDir} for HTML files, found {Num} entries.", HTMLDocsDir, HTMLFiles.Length);

						foreach (string HTMLFile in HTMLFiles)
						{
							string StrippedHTMLFile = HTMLFile.Remove(0, HTMLDocsDir.Length + 1);
							BuildProducts.Add(FileReference.Combine(HTMLDocsOutputDir, StrippedHTMLFile), BuildProductType.RequiredResource);
							Logger.LogDebug("Added HTMLFile {HTMLFile} in {HTMLDocsDir} to BuildProducts", StrippedHTMLFile, HTMLDocsOutputDir);
						}
					}
				};

				string EngineHTMLDocsDir = Path.Combine(Unreal.EngineDirectory.FullName, @$"Platforms\{Target.Platform}\Build\Resources\HtmlDocument");

				string HTMLDocsDir = DirectoryReference.Combine(ResourcesDirRef, @"HtmlDocument").ToString();
				if (!Directory.Exists(HTMLDocsDir))
				{
					// check original, non-platform extension path
					HTMLDocsDir = DirectoryReference.Combine(ResourcesDirRefDeprecated, @"HtmlDocument").ToString();
				}
				if (HTMLDocsDir.ToLower() != EngineHTMLDocsDir.ToLower())
				{
					CollectHtmlFiles(HTMLDocsDir);
				}

				// Collect HTML files in engine platform folder.
				// NOTE: Historically this has copied the engine HTML files regardless of the build configuration, the new behavior is
				// to only copy them in non-shipping builds. If this behavior causes problems the ini variable bAllowEngineHTMLDocsInShipping can
				// be set to true to restore the old behavior.
				Ini.GetBool(GetConfigSectionName(), "bAllowEngineHTMLDocsInShipping", out bool bAllowEngineHTMLDocsInShipping);
				if (bAllowEngineHTMLDocsInShipping || Target.Configuration != UnrealTargetConfiguration.Shipping)
				{
					CollectHtmlFiles(EngineHTMLDocsDir);
				}

				// accessible URLs text file
				string AccessibleUrlsInputFile = Path.Combine(ResourcesDirRef.ToString(), "AccessibleUrls.txt");
				string AccessibleUrlsInputFilePlatformsDir = AccessibleUrlsInputFile;
				if (!File.Exists(AccessibleUrlsInputFile))
				{
					// check original, non-platform extension path
					AccessibleUrlsInputFile = Path.Combine(ResourcesDirRefDeprecated.ToString(), "AccessibleUrls.txt");
				}

				// if the input file exists, it will be moved to the output path by the createNSPD process.  Add that
				// (known) output location to the BuildProducts list for staging
				if (File.Exists(AccessibleUrlsInputFile))
				{
					string AccessibleUrlsOutputFile = Path.Combine(OutputDirRef.ToString(), @"htmlDocument0.ncd\data\accessible-urls\accessible-urls.txt");

					Logger.LogDebug("Adding AccessibleUrlsFile {AccessibleUrlsOutputFile} to BuildProducts (source location: {AccessibleUrlsInputFile})", AccessibleUrlsOutputFile, AccessibleUrlsInputFile);
					BuildProducts.Add(new FileReference(Path.GetFullPath(AccessibleUrlsOutputFile)), BuildProductType.RequiredResource);
				}
				else
				{
					Logger.LogDebug("AccessibleUrlsFile was not found at {AccessibleUrlsInputFilePlatformsDir} or {AccessibleUrlsInputFile}", AccessibleUrlsInputFilePlatformsDir, AccessibleUrlsInputFile);
				}

				// LegalInformation0.ncd directory
				string LegalInfoZipFile = FileReference.Combine(ResourcesDirRef, "LegalInformation.zip").ToString();

				if (!File.Exists(LegalInfoZipFile))
				{
					// check original, non-platform extension path
					LegalInfoZipFile = FileReference.Combine(ResourcesDirRefDeprecated, "LegalInformation.zip").ToString();
				}

				if (File.Exists(LegalInfoZipFile))
				{
					DirectoryReference LegalInfoDataDirRef = DirectoryReference.Combine(OutputDirRef, @"legalInformation0.ncd\data");

					ZipFile LegalZipFile = new(LegalInfoZipFile);
					foreach (ZipEntry Entry in LegalZipFile.Entries)
					{
						// don't include directories in the build products manifest, only files
						if (Entry.Attributes != FileAttributes.Directory)
						{
							BuildProducts.Add(FileReference.Combine(LegalInfoDataDirRef, Entry.FileName), BuildProductType.RequiredResource);
						}
					}
				}

				// Add symbol files to build products
				if (Target.Configuration != UnrealTargetConfiguration.Shipping && GetNintendoTargetRules(Target).bGenerateSymbols)
				{
					DirectoryReference SymbolsDirRef = DirectoryReference.Combine(ProjectDirRef, @$"Build\{Target.Platform}\Symbols");
					BuildProducts.Add(FileReference.Combine(SymbolsDirRef, Target.Configuration.ToString() + "-Symbols.bin"), BuildProductType.SymbolFile);
				}
			}
		}

		/// <summary>
		/// base frameworks used for all Applications.
		/// stored here so we have access to them in ModifyBuildProducts()
		/// </summary>
		public string[] BaseNspFrameworks = new string[] { "rocrt.o", "nnApplication.o", "nnSdk.nso", "nnrtld.nso" };

		/// <summary>
		/// base frameworks used for all Applications.
		/// stored here so we have access to them in ModifyBuildProducts()
		/// </summary>
		public string[] BaseNroFrameworks { get; set; } = new string[] { "rocrt_nro.o", "nnSdk.nso", "nnrtld.nso" };
		public override FileItem LinkFiles(LinkEnvironment LinkEnvironment, bool bBuildImportLibraryOnly, IActionGraphBuilder Graph)
		{
			CppRootPaths RootPaths = LinkEnvironment.RootPaths;
			FileReference ToolPath = (LinkEnvironment.bIsBuildingLibrary) ? Info.Archiver : Info.Clang;
			string ToolVersion = (LinkEnvironment.bIsBuildingLibrary) ? Info.ArchiverVersionString : Info.ClangVersionString;

			string InitialOutputFilePath = Path.GetFullPath(LinkEnvironment.OutputFilePath.ToString());
			string FinalOutputFilePath = InitialOutputFilePath;
			FileItem FinalOutputFile = FileItem.GetItemByPath(FinalOutputFilePath);
			// for the final link, we output a .nss for an app or .nrs for dynamic module
			if (LinkEnvironment.bIsBuildingDLL)
			{
				InitialOutputFilePath = Path.ChangeExtension(InitialOutputFilePath, ".nrs");
			}
			else if (!LinkEnvironment.bIsBuildingLibrary)
			{
				InitialOutputFilePath = Path.ChangeExtension(InitialOutputFilePath, ".nss");
			}

			// Potential files when building a executable which links NROs.
			FileItem NrrFile = FileItem.GetItemByFileReference(GetNrrFile(LinkEnvironment.OutputDirectory, TargetConfiguration));

			// Create an action that invokes the linker.
			Action LinkAction = Graph.CreateAction(ActionType.Link);
			LinkAction.RootPaths = RootPaths;
			LinkAction.WorkingDirectory = Unreal.EngineSourceDirectory;
			LinkAction.CommandPath = ToolPath;
			LinkAction.CommandVersion = ToolVersion;

			// build this up over the rest of the function
			List<string> LinkArguments = new();
			if (LinkEnvironment.bIsBuildingLibrary)
			{
				GetArchiveArguments_Global(LinkEnvironment, LinkArguments);
			}
			else
			{
				GetLinkArguments_Global(LinkEnvironment, LinkArguments);
			}

			// Add the output file as a production of the link action.
			FileItem InitialOutputFile = FileItem.GetItemByPath(InitialOutputFilePath);
			LinkAction.ProducedItems.Add(InitialOutputFile);

			// Add the input files to a response file, and pass the response file on the command-line.
			List<string> InputFileNames = new();
			foreach (FileItem InputFile in LinkEnvironment.InputFiles)
			{
				if (InputFile.HasExtension(".ldscript"))
				{
					LinkArguments.Add($"-Wl,--version-script=\"{NormalizeCommandLinePath(InputFile, RootPaths)}\"");
				}
				else
				{
					LinkArguments.Add($"\"{NormalizeCommandLinePath(InputFile, RootPaths)}\"");
				}
				LinkAction.PrerequisiteItems.Add(InputFile);
			}

			// Add profdata as a prerequisite for pgo optimize
			if (LinkEnvironment.bPGOOptimize && LinkEnvironment.PGODirectory != null && LinkEnvironment.PGOFilenamePrefix != null)
			{
				DirectoryReference PGODir = new DirectoryReference(LinkEnvironment.PGODirectory);
				LinkAction.PrerequisiteItems.Add(FileItem.GetItemByFileReference(FileReference.Combine(PGODir, LinkEnvironment.PGOFilenamePrefix)));
			}

			// we can't have duplicate .nso's linked in, they will be duplicated on disk, and mess with the profiler
			// so make a list of the frameworks that are unique
			// Also locate the `crtend.o` so that it can be linked explicitly at the end.
			FileReference? FrameworkCrtend = null;
			HashSet<FileReference> FrameworkNsos = new();
			HashSet<FileReference> FrameworkObjects = new();

			string[] BaseFrameworks = LinkEnvironment.bIsBuildingDLL ? BaseNroFrameworks : BaseNspFrameworks;

			foreach (string BaseFramework in BaseFrameworks)
			{
				foreach (DirectoryReference SystemLibraryPath in LinkEnvironment.SystemLibraryPaths)
				{
					string Framework = Path.Combine(SystemLibraryPath.FullName, BaseFramework);
					if (File.Exists(Framework))
					{
						FileReference FrameworkFile = FileReference.FromString(Framework);
						if (Framework.EndsWith("crtend.o", StringComparison.InvariantCultureIgnoreCase))
						{
							FrameworkCrtend = FrameworkFile;
						}
						else if (FrameworkFile.HasExtension(".nso"))
						{
							FrameworkNsos.Add(FrameworkFile);
						}
						else if (FrameworkFile.HasExtension(".nss"))
						{
							FrameworkNsos.Add(FrameworkFile.ChangeExtension("nso"));
						}
						else
						{
							FrameworkObjects.Add(FrameworkFile);
						}
						break;
					}
				}
			}

			foreach (string Framework in LinkEnvironment.Frameworks)
			{
				FileReference FrameworkFile = FileReference.FromString(Framework);
				if (Framework.EndsWith("crtend.o", StringComparison.InvariantCultureIgnoreCase))
				{
					FrameworkCrtend = FrameworkFile;
				}
				else if (FrameworkFile.HasExtension(".nso"))
				{
					FrameworkNsos.Add(FrameworkFile);
				}
				else if (FrameworkFile.HasExtension(".nss"))
				{
					FrameworkNsos.Add(FrameworkFile.ChangeExtension("nso"));
				}
				else if (!Framework.EndsWith("nnApplication.o", StringComparison.InvariantCultureIgnoreCase) || LinkEnvironment.bIsBuildingDLL == false)
				{
					FrameworkObjects.Add(FrameworkFile);
				}
			}

			List<string> NroFiles = new List<string>(BundledNroFiles);

			if (LinkEnvironment.bIsBuildingLibrary)
			{
				LinkArguments.Add(NormalizeCommandLinePath(InitialOutputFile, RootPaths));
			}
			else
			{
				DirectoryReference SdkInstallDir = DirectoryReference.FromString(NintendoPlatformSDK.GetSDKInstallLocation())!;
				DirectoryReference SpecFilesDir = DirectoryReference.Combine(SdkInstallDir, "Resources", "SpecFiles");

				if (LinkEnvironment.bIsBuildingDLL)
				{
					if (!bMergeModules)
					{
						LinkArguments.Add("-Wl,--whole-archive"); // Honk: Not sure why this is here?
					}
					LinkArguments.Add($"-Wl,-T \"{NormalizeCommandLinePath(FileReference.Combine(SpecFilesDir, "RoModule.aarch64.lp64.ldscript"), RootPaths)}\"");
				}
				else
				{
					if (!bMergeModules) // With merge modules we use the script provided in input files instead
					{
						// If monolith we better strip everything.. but if it is a merged modules build the executable actually exports to the dlls
						FileReference VersionScriptFile = FileReference.Combine(LinkEnvironment.IntermediateDirectory!, LinkEnvironment.OutputFilePath.GetFileName() + ".ldscript");
						LinkAction.PrerequisiteItems.Add(Graph.CreateIntermediateTextFile(VersionScriptFile, $"VERSION {{ global: {string.Join(';', GetExtraLinkFileAdditionalSymbols(UEBuildBinaryType.Executable))}; local: *; }};"));
						LinkArguments.Add($"-Wl,--version-script=\"{NormalizeCommandLinePath(VersionScriptFile, RootPaths)}\"");
					}
					LinkArguments.Add($"-Wl,-T \"{NormalizeCommandLinePath(FileReference.Combine(SpecFilesDir, "Application.aarch64.lp64.ldscript"), RootPaths)}\"");
				}

				// Add the library paths to the argument list.ldscript ld
				foreach (DirectoryReference LibraryPath in LinkEnvironment.SystemLibraryPaths)
				{
					LinkArguments.Add($"-L\"{NormalizeCommandLinePath(LibraryPath, RootPaths)}\"");
				}

				// First link any framework .o files.
				LinkArguments.Add("-Wl,--start-group");
				foreach (FileReference Framework in FrameworkObjects)
				{
					LinkArguments.Add($"\"{NormalizeCommandLinePath(Framework, RootPaths)}\"");
				}
				LinkArguments.Add("-Wl,--end-group");

				List<FileReference> NrsLibraries = new();
				List<FileReference> OtherLibraries = new();
				HashSet<FileReference> Handled = new();

				foreach (FileReference Library in LinkEnvironment.Libraries)
				{
					if (!Handled.Add(Library))
					{
						continue;
					}
					string AdditionalLibrary = Library.FullName;
					if (Path.GetExtension(AdditionalLibrary).Equals(".nrs", StringComparison.InvariantCultureIgnoreCase))
					{
						NrsLibraries.Add(Library);
						LinkAction.PrerequisiteItems.Add(FileItem.GetItemByFileReference(Library));
						// NOTE: Some NROs are found in a different folder than their corresponding NRS file, with this in mind
						// we just make sure that a NRO with the same filename is present in the RuntimeDependencies list.
						// NRS libraries need a corresponding NRO file that must be loaded at runtime with the nn::ro interface.
						// It is safe to do this test at makefile generation instead of at makefile execution because the list of
						// Libraries and RuntimeDependencies only change when build files are changed and changing a build file will
						// dirty the makefile.
						string NrsFileNameWithoutExt = Library.GetFileNameWithoutExtension();
						int FoundIndex = LinkEnvironment.RuntimeDependencies.FindIndex(x => Path.GetFileNameWithoutExtension(x.Path).Equals(NrsFileNameWithoutExt, StringComparison.InvariantCultureIgnoreCase));
						if (FoundIndex == -1)
						{
							throw new BuildException($"{Path.GetFileName(AdditionalLibrary)} does not have a .nro in the runtime dependency list, please add the .nro as a runtime dependency.");
						}
					}
					else if (Path.GetExtension(AdditionalLibrary).Equals(".nro", StringComparison.InvariantCultureIgnoreCase))
					{
						FileReference NrsFile = Library.ChangeExtension(".nrs");
						NrsLibraries.Add(NrsFile);
						LinkAction.PrerequisiteItems.Add(FileItem.GetItemByFileReference(NrsFile));
					}
					else if (Path.GetExtension(AdditionalLibrary).Equals(".nspd_root", StringComparison.InvariantCultureIgnoreCase))
					{
						FileReference NrsFile = Library.ChangeExtension(".nss");
						NrsLibraries.Add(NrsFile);
						LinkAction.PrerequisiteItems.Add(FileItem.GetItemByFileReference(NrsFile));
					}
					else
					{
						OtherLibraries.Add(Library);
						LinkAction.PrerequisiteItems.Add(FileItem.GetItemByFileReference(Library));
					}
				}

				LinkArguments.Add("-Wl,--start-group");
				foreach (FileReference AdditionalLibrary in OtherLibraries)
				{
					// full pathed libs are compiled by us, so we depend on linking them
					LinkArguments.Add($"\"{NormalizeCommandLinePath(AdditionalLibrary, RootPaths)}\"");
				}
				LinkArguments.Add("-Wl,--end-group");

				// now add the system libs
				LinkArguments.Add("-Wl,--start-group");
				foreach (string AdditionalLibrary in LinkEnvironment.SystemLibraries)
				{
					LinkArguments.Add($"\"-l{AdditionalLibrary}\"");
				}

				if (!LinkEnvironment.bIsBuildingDLL)
				{
					LinkArguments.Add("-lnn_init_memory");
				}

				LinkArguments.Add("-Wl,--end-group");

				// then link any .nss versions of the .nso's
				LinkArguments.Add("-Wl,--start-group");
				foreach (FileReference Framework in FrameworkNsos)
				{
					LinkArguments.Add($"\"{NormalizeCommandLinePath(Framework.ChangeExtension(".nss"), RootPaths)}\"");
				}
				LinkArguments.Add("-Wl,--end-group");

				// Finally link the .nrs so that they have a lesser chance of replacing system symbols.
				// NOTE: It is important that these link just before the crtend.o
				LinkArguments.Add("-Wl,--start-group");
				foreach (FileReference AdditionalLibrary in NrsLibraries)
				{
					// full pathed libs are compiled by us, so we depend on linking them
					LinkArguments.Add($"\"{NormalizeCommandLinePath(AdditionalLibrary, RootPaths)}\"");
				}
				LinkArguments.Add("-Wl,--end-group");

				// NOTE: It is important that crtend.o links last.
				if (FrameworkCrtend != null)
				{
					LinkArguments.Add($"\"{NormalizeCommandLinePath(FrameworkCrtend, RootPaths)}\"");
				}

				// Add the output file to the command-line.
				LinkArguments.Add($"-o \"{NormalizeCommandLinePath(InitialOutputFile, RootPaths)}\"");
			}

			// Only execute linking on the local PC.
			LinkAction.bCanExecuteRemotely = false;

			if (LinkEnvironment.bPGOProfile || LinkEnvironment.bPGOOptimize || (LinkEnvironment.bAllowLTCG && LinkEnvironment.Configuration == CppConfiguration.Shipping))
			{
				// Set the weight to number of logical cores as lld can max out the available cores
				LinkAction.Weight = Utils.GetLogicalProcessorCount();

				// Disallow remote to prevent this long running action from running on an agent if remote linking is enabled
				LinkAction.bCanExecuteRemotely = false;
			}

			// Create response file
			FileReference ResponseFileName = GetResponseFileName(LinkEnvironment, InitialOutputFile);
			FileItem ResponseFileItem = Graph.CreateIntermediateTextFile(ResponseFileName, LinkArguments);
			string ResponsFileArgument = GetResponseFileArgument(ResponseFileItem, RootPaths);

			// Add the additional arguments specified by the environment.
			if (!string.IsNullOrWhiteSpace(LinkEnvironment.AdditionalArguments))
			{
				ResponsFileArgument += " " + LinkEnvironment.AdditionalArguments.Replace("\\", "/");
			}

			LinkAction.CommandArguments = ResponsFileArgument;
			LinkAction.PrerequisiteItems.Add(ResponseFileItem);

			LinkAction.CommandDescription = "Link";
			LinkAction.StatusDescription = InitialOutputFile.Name;


			// now do the post process on the .nss
			if (!LinkEnvironment.bIsBuildingLibrary)
			{
				// get the project name that fits in the allowed length
				string StrippedProjectName = GetStrippedProjectName(FinalOutputFile.AbsolutePath);

				// setup the Code dir that will collect all the intermediate code files
				string IntermediateDir = LinkEnvironment.IntermediateDirectory!.FullName;
				string CodeDir = Path.GetFullPath(Path.Combine(IntermediateDir, StrippedProjectName + "_code"));
				if (LinkEnvironment.bIsBuildingDLL)
				{
					// make an NRO from the nrs
					Action NroAction = Graph.CreateAction(ActionType.Link);
					NroAction.WorkingDirectory = Unreal.EngineSourceDirectory;
					NroAction.CommandPath = NintendoInfo.MakeNro;
					NroAction.CommandArguments = $"\"{InitialOutputFile.AbsolutePath}\" \"{FinalOutputFile.AbsolutePath}\"";
					NroAction.ProducedItems.Add(FinalOutputFile);
					// If the main link action needs to run then always remake the NRO.
					NroAction.DeleteItems.Add(FinalOutputFile);
					NroAction.PrerequisiteItems.Add(InitialOutputFile);
					NroAction.bCanExecuteRemotely = false;
					NroAction.CommandDescription = "MakeNro";
					NroAction.StatusDescription = FinalOutputFilePath;
					NroAction.bCanExecuteInUBA = false;

				}
				else
				{
					// make an NSO from the nss
					string NewNsoFilePath = Path.GetFullPath(Path.Combine(CodeDir, "main"));
					FileItem NsoFile = FileItem.GetItemByPath(NewNsoFilePath);

					Action NsoAction = Graph.CreateAction(ActionType.Link);
					NsoAction.WorkingDirectory = Unreal.EngineSourceDirectory;
					NsoAction.CommandPath = BuildHostPlatform.Current.Shell;
					NsoAction.CommandArguments = $"/c set \"DOTNET_ROOT=\" && set \"DOTNET_HOST_PATH=\" && set \"DOTNET_MULTILEVEL_LOOKUP=1\" && set \"DOTNET_ROLL_FORWARD=LatestMajor\" && \"{NintendoInfo.MakeNso}\" \"{InitialOutputFile.AbsolutePath}\" \"{NewNsoFilePath}\"";
					NsoAction.ProducedItems.Add(NsoFile);
					NsoAction.PrerequisiteItems.Add(InitialOutputFile);
					NsoAction.bCanExecuteRemotely = false;
					NsoAction.StatusDescription = NewNsoFilePath;
					NsoAction.CommandDescription = "MakeNso";
					NsoAction.bCanExecuteInUBA = false;

					// create/locate a metadata files (or use overrides). the end result are files usable in MakeNca
					string MetaFilePath = GenerateMetaFile(LinkEnvironment.Platform, ProjectFile, TargetConfiguration, Logger);
					string DescFilePath = LocateDescFile(LinkEnvironment.Platform, ProjectFile, Logger);

					// "compiled" metadata file
					string NpdmFilePath = Path.ChangeExtension(NewNsoFilePath, ".npdm");

					FileItem NpdmFile = FileItem.GetItemByPath(NpdmFilePath);
					Action NpdmAction = Graph.CreateAction(ActionType.Link);
					NpdmAction.WorkingDirectory = Unreal.EngineSourceDirectory;
					NpdmAction.CommandPath = BuildHostPlatform.Current.Shell;
					// Clear Unreal's bundled dotnet variables so SDK-hosted tools can use the system runtime they require.
					string MakeMetaCommandLine = GetMakeMetaCommandline(MetaFilePath, DescFilePath, NpdmFilePath, ToolPlatformArg);
					NpdmAction.CommandArguments = $"/c set \"DOTNET_ROOT=\" && set \"DOTNET_HOST_PATH=\" && set \"DOTNET_MULTILEVEL_LOOKUP=1\" && set \"DOTNET_ROLL_FORWARD=LatestMajor\" && \"{NintendoInfo.MakeMeta}\" {MakeMetaCommandLine}";
					NpdmAction.ProducedItems.Add(NpdmFile);
					NpdmAction.PrerequisiteItems.Add(FileItem.GetItemByPath(DescFilePath));
					NpdmAction.PrerequisiteItems.Add(FileItem.GetItemByPath(MetaFilePath));
					NpdmAction.bCanExecuteRemotely = false;
					NpdmAction.StatusDescription = NpdmFilePath;
					NsoAction.CommandDescription = "MakeMeta";
					NpdmAction.bCanExecuteInUBA = false;

					// output NspdDir
					string NspdDir = Path.ChangeExtension(FinalOutputFilePath, ".nspd");
					(Action NspdAction, FileItem NspdCodeFile) = CreateNspdAction(Graph, FinalOutputFile, NspdDir, MetaFilePath, CodeDir, LinkEnvironment, DescFilePath);

					// output Nspd_root file
					Action NspdRootAction = Graph.CreateAction(ActionType.Link);
					NspdRootAction.WorkingDirectory = FinalOutputFile.Location.Directory;
					NspdRootAction.CommandPath = BuildHostPlatform.Current.Shell;
					NspdRootAction.CommandArguments = $"/c echo 0 >> \"{FinalOutputFilePath}\"";
					NspdRootAction.ProducedItems.Add(FinalOutputFile);
					NspdRootAction.PrerequisiteItems.Add(NspdCodeFile);
					NspdRootAction.bCanExecuteRemotely = false;
					NspdRootAction.bCanExecuteInUBA = false;
					NsoAction.CommandDescription = "CreateRoot";
					NspdRootAction.StatusDescription = Path.GetFileName(FinalOutputFilePath);

					// copy all the .nso's into the code dir, with special names
					int NsoIndex = 0;
					foreach (FileReference Framework in FrameworkNsos)
					{
						if (Framework.HasExtension(".nso"))
						{
							string FinalName;
							if (Framework.GetFileNameWithoutExtension().Equals("nnSdk", StringComparison.InvariantCultureIgnoreCase))
							{
								FinalName = "sdk";
							}
							else if (Framework.GetFileNameWithoutExtension().Equals("nnrtld", StringComparison.InvariantCultureIgnoreCase))
							{
								FinalName = "rtld";
							}
							else
							{
								FinalName = string.Format("subsdk{0}", NsoIndex++);
							}

							// make an action to copy the file, and add that output as a prereq to nca
							NspdAction.PrerequisiteItems.Add(MakeFileCopyAction(Framework.FullName, Path.Combine(CodeDir, FinalName), Graph));
						}
					}

					// ExtraData destination.
					string ExtraDataDir = Path.Combine(IntermediateDir, $"extraData-{TargetConfiguration}");
					if (Directory.Exists(ExtraDataDir))
					{
						DirectoryDelete(ExtraDataDir);
					}
					string NroDataDir = Path.Combine(ExtraDataDir, "nro");
					string NrrDataDir = Path.Combine(ExtraDataDir, ".nrr");
					string NrrDataDestination = Path.Combine(NrrDataDir, Path.GetFileName(NrrFile.AbsolutePath));

					HashSet<string> NrrFiles = new();
					foreach (ModuleRules.RuntimeDependency Dep in LinkEnvironment.RuntimeDependencies)
					{
						if (Path.GetExtension(Dep.Path).Equals(".nro", StringComparison.InvariantCultureIgnoreCase))
						{
							NspdAction.PrerequisiteItems.Add(MakeFileCopyAction(Dep.Path, Path.Combine(NroDataDir, Path.GetFileName(Dep.Path)), Graph));
							NroFiles.Add(Dep.Path);
						}
						else if (Path.GetExtension(Dep.Path).Equals(".nrr", StringComparison.InvariantCultureIgnoreCase))
						{
							NspdAction.PrerequisiteItems.Add(MakeFileCopyAction(Dep.Path, Path.Combine(NrrDataDir, Path.GetFileName(Dep.Path)), Graph));
							NrrFiles.Add(Path.GetFileNameWithoutExtension(Dep.Path.ToLower()));
						}
					}

					Action? NrrAction = null;
					foreach (string NroFile in NroFiles)
					{
						if (NrrAction == null)
						{
							NrrAction = Graph.CreateAction(ActionType.Link);
							NrrAction.WorkingDirectory = Unreal.EngineSourceDirectory;
							NrrAction.CommandPath = NintendoInfo.MakeNrr;
							NrrAction.ProducedItems.Add(NrrFile);
							NrrAction.DeleteItems.Add(NrrFile);
							NrrAction.DeleteItems.Add(FileItem.GetItemByPath(NrrDataDestination));
							NrrAction.bCanExecuteRemotely = false;
							NrrAction.StatusDescription = string.Format("{0}", NrrFile.FullName);
							NrrAction.CommandArguments = string.Format("-o \"{0}\"", NrrFile.AbsolutePath);
							NrrAction.StatusDescription = $"{NrrFile.FullName}";
							NrrAction.CommandDescription = "MakeNrr";
							NrrAction.CommandArguments = $"-o \"{NrrFile.AbsolutePath}\"";
							if (NintendoPlatformSDK.GetSDKVersionInt() >= NintendoPlatformSDK.VersionXYZToInt("19", "1", "1"))
							{
								NrrAction.CommandArguments += $" {ToolPlatformArg}";
							}
							NspdAction.PrerequisiteItems.Add(NrrFile);
							// Make sure that a copy of the NRR to the precise .nrr data location occurs before making the NSPD.
							NspdAction.PrerequisiteItems.Add(MakeFileCopyAction(NrrFile.AbsolutePath, NrrDataDestination, Graph));
							NspdAction.bCanExecuteInUBA = false;
						}
						// Don't include any NRO files that have explicitly include NRR files.
						if (!NrrFiles.Contains(Path.GetFileNameWithoutExtension(NroFile.ToLower())))
						{
							NrrAction.PrerequisiteItems.Add(FileItem.GetItemByPath(NroFile));
							NrrAction.CommandArguments += $" \"{NroFile}\"";
							NrrAction.bCanExecuteInUBA = false;
						}
					}

					if (NrrAction == null)
					{
						// Make sure there is no NrrFile if its not used.
						NspdAction.DeleteItems.Add(NrrFile);
					}
					else
					{
						NspdAction.CommandArguments += string.Format(" --data \"{0}\"", ExtraDataDir);
					}

					NspdAction.PrerequisiteItems.Add(FileItem.GetItemByPath(MetaFilePath));
					NspdAction.PrerequisiteItems.Add(NpdmFile);
					NspdAction.PrerequisiteItems.Add(NsoFile);
					NspdAction.bCanExecuteRemotely = false;
					NspdAction.bCanExecuteInUBA = false;
					NspdAction.StatusDescription = Path.GetFileName(NspdDir);
				}
			}
			return FinalOutputFile;
		}

		protected abstract (Action NspdAction, FileItem NspdCodeFile) CreateNspdAction(IActionGraphBuilder Graph, FileItem FinalOutputFile, string NspdDir, string MetaFilePath, string CodeDir, LinkEnvironment LinkEnvironment, string DescFilePath);

		private static ICollection<FileItem> GenerateSymbols(FileItem Executable, LinkEnvironment LinkEnvironment, IActionGraphBuilder Graph)
		{
			string NssPath = Path.GetFullPath(LinkEnvironment.OutputFilePath.ToString());
			NssPath = Path.ChangeExtension(NssPath, ".nss");

			string ConfigName = LinkEnvironment.IntermediateDirectory!.GetDirectoryName();

			string OutputFilenameBase = System.IO.Path.Combine(LinkEnvironment.OutputFilePath.Directory.FullName, @$"..\..\Build\{LinkEnvironment.Platform}\Symbols", ConfigName + "-");

			FileItem OutputSymbols = FileItem.GetItemByPath(OutputFilenameBase + "Symbols.bin");

			Action PostBuildAction = Graph.CreateAction(ActionType.Link);
			PostBuildAction.WorkingDirectory = Unreal.EngineSourceDirectory;
			PostBuildAction.CommandPath = FileReference.Combine(Unreal.EngineDirectory, "Platforms/Nintendo/Binaries/Win64/NintendoSymbolTool.bat");
			
			PostBuildAction.CommandArguments = string.Format("\"{0}\" \"{1}\"", NssPath, OutputSymbols.AbsolutePath);

			// We depend on the .exe file generated by the linker
			PostBuildAction.PrerequisiteItems.Add(Executable);

			PostBuildAction.ProducedItems.Add(OutputSymbols);
			PostBuildAction.CommandDescription = $"{LinkEnvironment.Platform}SymbolTools";
			PostBuildAction.bCanExecuteRemotely = false;
			PostBuildAction.StatusDescription = Path.GetFileName(NssPath);
			PostBuildAction.bShouldOutputStatusDescription = true;

			return PostBuildAction.ProducedItems;
		}


		public override ICollection<FileItem> PostBuild(ReadOnlyTargetRules Target, FileItem Executable, LinkEnvironment BinaryLinkEnvironment, IActionGraphBuilder Graph)
		{
			List<FileItem> OutputFiles = new();

			if (BinaryLinkEnvironment.bIsBuildingLibrary == false && BinaryLinkEnvironment.bCreateDebugInfo)
			{
				OutputFiles.AddRange(GenerateSymbols(Executable, BinaryLinkEnvironment, Graph));
			}

			return OutputFiles;
		}

		public override List<string> GetISPCCompileTargets(UnrealTargetPlatform Platform, UnrealArch Arch)
		{
			List<string> ISPCTargets = new();

			if (Platform.IsInGroup(UnrealPlatformGroup.Nintendo))
			{
				ISPCTargets.Add("neon");
			}
			else
			{
				ISPCTargets = base.GetISPCCompileTargets(Platform, Arch);
			}

			return ISPCTargets;
		}

		public override string GetISPCOSTarget(UnrealTargetPlatform Platform)
		{
			if (Platform.IsInGroup(UnrealPlatformGroup.Nintendo))
			{
				return "custom_linux";
			}
			return base.GetISPCOSTarget(Platform);
		}

		public override string GetISPCArchTarget(UnrealTargetPlatform Platform, UnrealArch Arch)
		{
			if (Platform.IsInGroup(UnrealPlatformGroup.Nintendo))
			{
				return "aarch64";
			}
			return base.GetISPCArchTarget(Platform, Arch);
		}

		public override string GetISPCObjectFileFormat(UnrealTargetPlatform Platform)
		{
			if (Platform.IsInGroup(UnrealPlatformGroup.Nintendo))
			{
				return "obj";
			}
			return base.GetISPCObjectFileFormat(Platform);
		}

		public override string GetISPCObjectFileSuffix(UnrealTargetPlatform Platform)
		{
			if (Platform.IsInGroup(UnrealPlatformGroup.Nintendo))
			{
				return ".o";
			}
			return base.GetISPCObjectFileSuffix(Platform);
		}

		public string GetNintendoClangVersion()
		{
			return NintendoInfo.NintendoClangVersion.ToString();
		}

		public override string GetExtraLinkFileExtension()
		{
			return "ldscript";
		}

		public override IEnumerable<string> GetExtraLinkFileAdditionalSymbols(UEBuildBinaryType binaryType)
		{
			if (binaryType != UEBuildBinaryType.Executable)
			{
				return Enumerable.Empty<string>();
			}

			return [
				"__nnDetailNintendoSdkRuntimeObjectFile",
				"nninitStartup",
				"nnMain",
				"malloc",
				"aligned_alloc",
				"calloc",
				"realloc",
				"free",
				"pfnc_nvn*",
				"nvnLoadCProc*",
				];
		}

		protected override DirectoryReference GetResourceDir(CppCompileEnvironment? CompileEnvironment)
		{
			return DirectoryReference.Combine(Info.BasePath!, "Compilers", "NintendoClang", "lib", "clang", NintendoInfo.NintendoClangVersion.ToString());
		}
	};
}
