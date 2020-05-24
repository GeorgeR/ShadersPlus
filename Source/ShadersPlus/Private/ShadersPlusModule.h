#pragma once

#include "IShadersPlusModule.h"

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FImageWriteQueue;

class FShadersPlusModule 
	: public IShadersPlusModule
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
