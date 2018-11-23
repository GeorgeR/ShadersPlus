#include "ShaderInstance.h"

FComputeShaderInstance::FComputeShaderInstance(const ERHIFeatureLevel::Type FeatureLevel)
{
    this->FeatureLevel = FeatureLevel;

    bIsExecuting = false;
    bIsUnloading = false;
}

FComputeShaderInstance::~FComputeShaderInstance()
{
    bIsUnloading = true;
}

void FComputeShaderInstance::Dispatch(void* Data) { }