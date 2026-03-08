// Copyright Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;
using System;
using System.IO;
using System.Collections.Generic;

namespace UnrealBuildTool.Rules
{
    public class FMODStudio_PS5 : FMODStudio
    {
        protected override string FMODLibDir
        {
            get
            {
                string ThisModuleDir = GetModuleDirectoryForSubClass(GetType()).FullName;
                return Path.GetFullPath(Path.Combine(ThisModuleDir, @"..\..\Binaries"));
            }
        }
        protected override string ConsoleLinkExt 	{ get { return "_stub.a"; } }
        protected override string ConsoleDllExt 	{ get { return ".prx"; } }
        protected override string ConsoleLibPrefix 	{ get { return "lib"; } }
        protected override bool   ConsoleDelayLoad 	{ get { return true; } }

        public FMODStudio_PS5(ReadOnlyTargetRules Target) : base(Target)
        {
            PublicIncludePaths.Add(Path.Combine(GetModuleDirectoryForSubClass(GetType()).FullName, "Public/FMOD"));
            PublicDefinitions.Add("FMOD_PLATFORM_HEADER=1");
        }
    }
}