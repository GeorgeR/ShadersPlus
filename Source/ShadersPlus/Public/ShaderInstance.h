#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
#include "ThreadSafeBool.h"

class SHADERSPLUS_API FShaderInstance
{
public:
    FShaderInstance(const ERHIFeatureLevel::Type FeatureLevel);
    virtual ~FShaderInstance();

protected:
    FThreadSafeBool bIsExecuting;
    FThreadSafeBool bIsUnloading;
    ERHIFeatureLevel::Type FeatureLevel;
};

class SHADERSPLUS_API FComputeShaderInstance
    : public FShaderInstance
{
public:
    FComputeShaderInstance(const ERHIFeatureLevel::Type FeatureLevel)
        : FShaderInstance(FeatureLevel) { }

    virtual ~FComputeShaderInstance() { }

    virtual void Dispatch() { }
};

template <typename TParameters>
class SHADERSPLUS_API TComputeShaderInstance
    : public FShaderInstance
{
public:
    TComputeShaderInstance(const ERHIFeatureLevel::Type FeatureLevel)
        : FShaderInstance(FeatureLevel) { }

    virtual ~TComputeShaderInstance() { }

    virtual void Dispatch(TParameters& Parameters) { }
};