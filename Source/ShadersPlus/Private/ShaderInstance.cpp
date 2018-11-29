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

bool FShaderInstance::CanExecute()
{
    check(IsInGameThread());

    return !bIsUnloading && !bIsExecuting;
}

FComputeShaderInstance::FComputeShaderInstance(const ERHIFeatureLevel::Type FeatureLevel) 
    : FShaderInstance(FeatureLevel)
{
    bHasBeenRun = false;
}

void FComputeShaderInstance::DispatchOnce()
{
    if (!CanExecute() || bHasBeenRun)
        return;

    bIsExecuting = true;
    OnDispatchOnce();
    bIsExecuting = false;

    bHasBeenRun = true;
}