#pragma once

#include "GlobalShader.h"
#include "RHICommandList.h"

// Probably depreciated
class SHADERSPLUS_API FShaderBase
    : public FGlobalShader
{
public:
    FShaderBase();
    explicit FShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

    //template <typename TParameter>
    //void SetParameter(FRHICommandList& RHICmdList, TParameter Parameter) { }

    virtual void Unbind(FRHICommandList& RHICmdList);
};