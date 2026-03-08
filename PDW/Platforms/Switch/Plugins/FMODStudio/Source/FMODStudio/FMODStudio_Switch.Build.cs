// Copyright Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;
using System;
using System.IO;
using System.Collections.Generic;

namespace UnrealBuildTool.Rules
{
    public class FMODStudio_Switch : FMODStudio
    {
        protected override string FMODLibDir
        {
            get
            {
                string ThisModuleDir = GetModuleDirectoryForSubClass(GetType()).FullName;
                return Path.GetFullPath(Path.Combine(ThisModuleDir, @"..\..\Binaries"));
            }
        }
        protected override string ConsoleLinkExt			{ get { return ".a"; } }
        protected override string ConsoleDllExt				{ get { return ".a"; } }
        protected override string ConsoleLibPrefix			{ get { return "lib"; } }
        protected override bool LinkDebugFiles				{ get { return true; } }
        protected override bool ConsoleRuntimeDependencies	{ get { return false; } }
        protected override bool ImportLibraries             { get { return true; } }



		public FMODStudio_Switch(ReadOnlyTargetRules Target) : base(Target)
        {
            PublicIncludePaths.Add(Path.Combine(GetModuleDirectoryForSubClass(GetType()).FullName, "Public/FMOD"));
            PublicDefinitions.Add("FMOD_DONT_LOAD_LIBRARIES=1");
            PublicDefinitions.Add("FMOD_PLATFORM_HEADER=1");
        }
    }
}
