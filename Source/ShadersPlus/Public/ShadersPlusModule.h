#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FShadersPlusModule 
	: public IModuleInterface
{
public:
    virtual void StartupModule() override;
};