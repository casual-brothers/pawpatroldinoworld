#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FMODSTUDIO_API IFMODStudioModule : public IModuleInterface
{
public:
    static inline IFMODStudioModule& Get()
    {
        return FModuleManager::LoadModuleChecked<IFMODStudioModule>("FMODStudio");
    }

    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("FMODStudio");
    }
};
