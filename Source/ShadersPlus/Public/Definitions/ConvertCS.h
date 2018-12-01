#pragma once

#include "ShadersPlusMacros.h"
#include <GlobalShader.h>

template <int32 InputComponents, int32 OutputComponents>
class TConvertCS
    : public FGlobalShader
{
    DECLARE_EXPORTED_SHADER_TYPE(TConvertCS, Global, SHADERSPLUS_API);

public:
    SHOULD_CACHE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)
    SHOULD_COMPILE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)
    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

    TConvertCS() = default;

    explicit TConvertCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

    DECLARE_SET_PARAMETER(FShaderResourceViewRHIRef, Input);
    DECLARE_SET_PARAMETER(FUnorderedAccessViewRHIRef, Output);

    void Unbind(FRHICommandList& RHICmdList);

    virtual bool Serialize(FArchive& Ar) override;

private:
    FShaderResourceParameter Input;    
    FShaderResourceParameter Output;
};

template <int32 InputComponents, int32 OutputComponents>
void TConvertCS<InputComponents, OutputComponents>::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
    FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
    OutEnvironment.SetDefine(TEXT("INPUT_COMPONENTS"), InputComponents);
    OutEnvironment.SetDefine(TEXT("OUTPUT_COMPONENTS"), OutputComponents);
}

template <int32 InputComponents, int32 OutputComponents>
TConvertCS<InputComponents, OutputComponents>::TConvertCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
    : FGlobalShader(Initializer)
{
    Input.Bind(Initializer.ParameterMap, TEXT("Input"));
    Output.Bind(Initializer.ParameterMap, TEXT("Output"));
}

template <int32 InputComponents, int32 OutputComponents>
void TConvertCS<InputComponents, OutputComponents>::SetInput(FRHICommandList& RHICmdList, FShaderResourceViewRHIRef Input)
{
    const auto ComputeShaderRHI = GetComputeShader();
    if (this->Input.IsBound())
        RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, this->Input.GetBaseIndex(), Input);
}

template <int32 InputComponents, int32 OutputComponents>
void TConvertCS<InputComponents, OutputComponents>::SetOutput(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef Output)
{
    const auto ComputeShaderRHI = GetComputeShader();
    if (this->Output.IsBound())
        RHICmdList.SetUAVParameter(ComputeShaderRHI, this->Output.GetBaseIndex(), Output);
}

template <int32 InputComponents, int32 OutputComponents>
void TConvertCS<InputComponents, OutputComponents>::Unbind(FRHICommandList& RHICmdList)
{
    const auto ComputeShaderRHI = GetComputeShader();

    if (this->Input.IsBound())
        RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, this->Input.GetBaseIndex(), FShaderResourceViewRHIRef());

    if (this->Output.IsBound())
        RHICmdList.SetUAVParameter(ComputeShaderRHI, this->Output.GetBaseIndex(), FUnorderedAccessViewRHIRef());
}

template <int32 InputComponents, int32 OutputComponents>
bool TConvertCS<InputComponents, OutputComponents>::Serialize(FArchive& Ar)
{
    const auto bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);

    Ar << Input << Output;

    return bShaderHasOutdatedParameters;
}