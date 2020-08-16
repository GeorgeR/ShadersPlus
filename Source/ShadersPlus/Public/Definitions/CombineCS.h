#pragma once

#include "ShadersPlusMacros.h"
#include <GlobalShader.h>

class FCombineCS
    : public FGlobalShader
{
    DECLARE_EXPORTED_SHADER_TYPE(FCombineCS, Global, SHADERSPLUS_API);

public:
    SHOULD_CACHE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)
    SHOULD_COMPILE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)

    FCombineCS() = default;
    explicit FCombineCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

    DECLARE_SET_PARAMETER(FShaderResourceViewRHIRef, InputX);
    DECLARE_SET_PARAMETER(FShaderResourceViewRHIRef, InputY);
    DECLARE_SET_PARAMETER(FShaderResourceViewRHIRef, InputZ);
    DECLARE_SET_PARAMETER(FShaderResourceViewRHIRef, InputW);
    DECLARE_SET_PARAMETER(FUnorderedAccessViewRHIRef, Output);

    void Unbind(FRHICommandList& RHICmdList);

    //virtual bool Serialize(FArchive& Ar) override;

private:
	LAYOUT_FIELD(FShaderResourceParameter, InputX)
	LAYOUT_FIELD(FShaderResourceParameter, InputY)
	LAYOUT_FIELD(FShaderResourceParameter, InputZ)
	LAYOUT_FIELD(FShaderResourceParameter, InputW)

	LAYOUT_FIELD(FShaderResourceParameter, Output)
};

template <int32 InputComponents>
class TCombineCS
    : public FCombineCS
{
    DECLARE_EXPORTED_SHADER_TYPE(TCombineCS, Global, SHADERSPLUS_API);

public:
    SHOULD_CACHE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)
    SHOULD_COMPILE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)
    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

    TCombineCS() = default;
    explicit TCombineCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FCombineCS(Initializer) { }
};

template <int32 InputComponents>
void TCombineCS<InputComponents>::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
    FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
    OutEnvironment.SetDefine(TEXT("INPUT_COMPONENTS"), InputComponents);
}
