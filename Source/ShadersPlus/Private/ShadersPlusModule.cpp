#include "ShadersPlusModule.h"

#include "Interfaces/IPluginManager.h"
#include "Logging/LogMacros.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FShadersPlusModule"

void FShadersPlusModule::StartupModule()
{
    auto ShaderDirectory = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("ShadersPlus"))->GetBaseDir(), TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/Plugin/ShadersPlus"), ShaderDirectory);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FShadersPlusModule, ShadersPlus)