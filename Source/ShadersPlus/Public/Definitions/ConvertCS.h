#pragma once

#include "ShadersPlusMacros.h"
#include <GlobalShader.h>

class FConvertCS
    : public FGlobalShader
{
    DECLARE_EXPORTED_SHADER_TYPE(FConvertCS, Global, SHADERSPLUS_API);

public:
    SHOULD_CACHE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)
    SHOULD_COMPILE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)

    FConvertCS() = default;
    explicit FConvertCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

#if ENGINE_MINOR_VERSION >= 23
    DECLARE_SET_PARAMETER(FShaderResourceViewRHIRef, Input);
#else
	DECLARE_SET_PARAMETER(FShaderResourceViewRHIRef, Input);
#endif
    DECLARE_SET_PARAMETER(FUnorderedAccessViewRHIRef, Output);

    void Unbind(FRHICommandList& RHICmdList);

    virtual bool Serialize(FArchive& Ar) override;

private:
    FShaderResourceParameter Input;
    FShaderResourceParameter Output;
};

template <int32 InputComponents, int32 OutputComponents>
class TConvertCS
    : public FConvertCS
{
    DECLARE_EXPORTED_SHADER_TYPE(TConvertCS, Global, SHADERSPLUS_API);

public:
    SHOULD_CACHE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)
    SHOULD_COMPILE_WITH_FEATURE_LEVEL(ERHIFeatureLevel::SM5)
    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

    TConvertCS() = default;
    explicit TConvertCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FConvertCS(Initializer) { }
};

template <int32 InputComponents, int32 OutputComponents>
void TConvertCS<InputComponents, OutputComponents>::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
    FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
    OutEnvironment.SetDefine(TEXT("INPUT_COMPONENTS"), InputComponents);
    OutEnvironment.SetDefine(TEXT("OUTPUT_COMPONENTS"), OutputComponents);
}
