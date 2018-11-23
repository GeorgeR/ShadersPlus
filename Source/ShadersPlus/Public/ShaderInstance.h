#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
#include "ThreadSafeBool.h"

class SHADERSPLUS_API FComputeShaderInstance
{
public:
    FComputeShaderInstance(const ERHIFeatureLevel::Type FeatureLevel);
    virtual ~FComputeShaderInstance();

    virtual void Dispatch(void* Data);

protected:
    FThreadSafeBool bIsExecuting;
    FThreadSafeBool bIsUnloading;
    ERHIFeatureLevel::Type FeatureLevel;
};