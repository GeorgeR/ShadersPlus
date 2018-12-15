#pragma once

#include "ShadersPlusMacros.h"
#include <GlobalShader.h>

template <int32 InputComponents>
class TCombineCS
    : public FGlobalShader
{
    DECLARE_EXPORTED_SHADER_TYPE(TCombineCS, Global, SHADERSPLUS_API);

public:
    SHOULD_CACHE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)
    SHOULD_COMPILE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)
    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

    TCombineCS() = default;

    explicit TCombineCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

    DECLARE_SET_PARAMETER(FShaderResourceViewRHIRef, InputX);
    DECLARE_SET_PARAMETER(FShaderResourceViewRHIRef, InputY);
    DECLARE_SET_PARAMETER(FShaderResourceViewRHIRef, InputZ);
    DECLARE_SET_PARAMETER(FShaderResourceViewRHIRef, InputW);
    DECLARE_SET_PARAMETER(FUnorderedAccessViewRHIRef, Output);

    void Unbind(FRHICommandList& RHICmdList);

    virtual bool Serialize(FArchive& Ar) override;

private:
    FShaderResourceParameter InputX;
    FShaderResourceParameter InputY;
    FShaderResourceParameter InputZ;
    FShaderResourceParameter InputW;

    FShaderResourceParameter Output;
};

template <int32 InputComponents>
void TCombineCS<InputComponents>::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
    FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
    OutEnvironment.SetDefine(TEXT("INPUT_COMPONENTS"), InputComponents);
}

template <int32 InputComponents>
TCombineCS<InputComponents>::TCombineCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
    : FGlobalShader(Initializer)
{
    BIND(InputX);
    BIND(InputY);
    BIND(InputZ);
    BIND(InputW);
    BIND(Output);
}

template <int32 InputComponents>
IMPLEMENT_SET_PARAMETER_SRV(TCombineCS<InputComponents>, InputX, Compute)

template <int32 InputComponents>
IMPLEMENT_SET_PARAMETER_SRV(TCombineCS<InputComponents>, InputY, Compute)

template <int32 InputComponents>
IMPLEMENT_SET_PARAMETER_SRV(TCombineCS<InputComponents>, InputZ, Compute)

template <int32 InputComponents>
IMPLEMENT_SET_PARAMETER_SRV(TCombineCS<InputComponents>, InputW, Compute)

template <int32 InputComponents>
IMPLEMENT_SET_PARAMETER_UAV(TCombineCS<InputComponents>, Output, Compute)

template <int32 InputComponents>
void TCombineCS<InputComponents>::Unbind(FRHICommandList& RHICmdList)
{
    const auto ComputeShaderRHI = GetComputeShader();

    UNBIND_SRV(InputX, Compute);
    UNBIND_SRV(InputY, Compute);
    UNBIND_SRV(InputZ, Compute);
    UNBIND_SRV(InputW, Compute);
    UNBIND_UAV(Output, Compute);
}

template <int32 InputComponents>
bool TCombineCS<InputComponents>::Serialize(FArchive& Ar)
{
    const auto bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);

    Ar << InputX << InputY << InputZ << InputW << Output;

    return bShaderHasOutdatedParameters;
}