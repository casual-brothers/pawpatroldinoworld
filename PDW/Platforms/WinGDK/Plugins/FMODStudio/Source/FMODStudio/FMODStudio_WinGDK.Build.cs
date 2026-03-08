// Copyright Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;
using System;
using System.IO;
using System.Collections.Generic;

namespace UnrealBuildTool.Rules
{
    public class FMODStudio_WinGDK : FMODStudio
    {
        protected override string FMODLibDir
        {
            get
            {
                string ThisModuleDir = GetModuleDirectoryForSubClass(GetType()).FullName;
                return Path.GetFullPath(Path.Combine(ThisModuleDir, @"..\..\Binaries"));
            }
        }
        protected override string ConsoleLinkExt 	{ get { return ".lib"; } }
        protected override string ConsoleDllExt 	{ get { return ".dll"; } }
        protected override bool   ConsoleDelayLoad 	{ get { return true; } }

        public FMODStudio_WinGDK(ReadOnlyTargetRules Target) : base(Target)
        {
            PublicIncludePaths.Add(Path.Combine(GetModuleDirectoryForSubClass(GetType()).FullName, "Public/FMOD"));
            PublicDefinitions.Add("FMOD_PLATFORM_HEADER=1");
        }
    }
}
