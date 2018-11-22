#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "TextureResource.h"

class SHADERPLUS_API FShaderBase
    : public FGlobalShader
{
public:
    explicit FShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

    template <typename TParameter>
    virtual void BindParameter(FRHICommandList& RHICmdList, TParameter Parameter);

    virtual void Unbind(FRHICommandList& RHICmdList) { }
};