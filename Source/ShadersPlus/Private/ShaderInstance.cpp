#include "ShaderInstance.h"

FShaderInstance::FShaderInstance(const ERHIFeatureLevel::Type FeatureLevel)
{
    this->FeatureLevel = FeatureLevel;

    bIsExecuting = false;
    bIsUnloading = false;
}

FShaderInstance::~FShaderInstance()
{
    bIsUnloading = true;
}