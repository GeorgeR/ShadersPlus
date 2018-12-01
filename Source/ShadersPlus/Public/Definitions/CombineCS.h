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
    InputX.Bind(Initializer.ParameterMap, TEXT("InputX"));
    InputY.Bind(Initializer.ParameterMap, TEXT("InputY"));
    InputZ.Bind(Initializer.ParameterMap, TEXT("InputZ"));
    InputW.Bind(Initializer.ParameterMap, TEXT("InputW"));

    Output.Bind(Initializer.ParameterMap, TEXT("Output"));
}

template <int32 InputComponents>
void TCombineCS<InputComponents>::SetInputX(FRHICommandList& RHICmdList, FShaderResourceViewRHIRef InputX)
{
    const auto ComputeShaderRHI = GetComputeShader();
    if (this->InputX.IsBound())
        RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, this->InputX.GetBaseIndex(), InputX);
}

template <int32 InputComponents>
void TCombineCS<InputComponents>::SetInputY(FRHICommandList& RHICmdList, FShaderResourceViewRHIRef InputY)
{
    const auto ComputeShaderRHI = GetComputeShader();
    if (this->InputY.IsBound())
        RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, this->InputY.GetBaseIndex(), InputY);
}

template <int32 InputComponents>
void TCombineCS<InputComponents>::SetInputZ(FRHICommandList& RHICmdList, FShaderResourceViewRHIRef InputZ)
{
    const auto ComputeShaderRHI = GetComputeShader();
    if (this->InputZ.IsBound())
        RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, this->InputZ.GetBaseIndex(), InputZ);
}

template <int32 InputComponents>
void TCombineCS<InputComponents>::SetInputW(FRHICommandList& RHICmdList, FShaderResourceViewRHIRef InputW)
{
    const auto ComputeShaderRHI = GetComputeShader();
    if (this->InputW.IsBound())
        RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, this->InputWX.GetBaseIndex(), InputW);
}

template <int32 InputComponents>
void TCombineCS<InputComponents>::SetOutput(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef Output)
{
    const auto ComputeShaderRHI = GetComputeShader();
    if (this->Output.IsBound())
        RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, this->Output.GetBaseIndex(), Output);
}

template <int32 InputComponents>
void TCombineCS<InputComponents>::Unbind(FRHICommandList& RHICmdList)
{
    const auto ComputeShaderRHI = GetComputeShader();

    if (this->InputX.IsBound())
        RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, this->InputX.GetBaseIndex(), FShaderResourceViewRHIRef());

    if (this->InputY.IsBound())
        RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, this->InputY.GetBaseIndex(), FShaderResourceViewRHIRef());

    if (this->InputZ.IsBound())
        RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, this->InputZ.GetBaseIndex(), FShaderResourceViewRHIRef());

    if (this->InputW.IsBound())
        RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, this->InputW.GetBaseIndex(), FShaderResourceViewRHIRef());

    if (this->Output.IsBound())
        RHICmdList.SetUAVParameter(ComputeShaderRHI, this->Output.GetBaseIndex(), FUnorderedAccessViewRHIRef());
}

template <int32 InputComponents>
bool TCombineCS<InputComponents>::Serialize(FArchive& Ar)
{
    const auto bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);

    Ar << InputX << InputY << InputZ << InputW << Output;

    return bShaderHasOutdatedParameters;
}