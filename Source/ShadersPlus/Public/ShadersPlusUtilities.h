#pragma once

#include <RHIResources.h>

class UTexture;

#define SHOULD_CACHE(Condition) static bool ShouldCache(EShaderPlatform Platform) { return Condition; }
#define SHOULD_CACHE_WITH_FEATURE_LEVEL(FeatureLevel) SHOULD_CACHE(IsFeatureLevelSupported(Platform, FeatureLevel))

#define SHOULD_COMPILE(Condition)	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) \
    {   \
		return Condition; \
	}
#define SHOULD_COMPILE_WITH_FEATURE_LEVEL(FeatureLevel) SHOULD_COMPILE(IsFeatureLevelSupported(Parameters.Platform, FeatureLevel))

#define DECLARE_SET_PARAMETER(ParameterType, ParameterName) void Set##ParameterName(FRHICommandList& RHICmdList, ParameterType ParameterName)

#define _IMPLEMENT_SET_PARAMETER(DeclarationType, ParameterType, ParameterName, ShaderType, Setter) void DeclarationType::Set##ParameterName(FRHICommandList& RHICmdList, ParameterType ParameterName)    \
{   \
    const auto ShaderType##ShaderRHI = Get##ShaderType##Shader();    \
    if (this->##ParameterName.IsBound())    \
        RHICmdList.##Setter(ShaderType##ShaderRHI, this->##ParameterName.GetBaseIndex(), ##ParameterName);     \
}

#define IMPLEMENT_SET_PARAMETER_SRV(DeclarationType, ParameterName, ShaderType) _IMPLEMENT_SET_PARAMETER(DeclarationType, FShaderResourceViewRHIRef, ParameterName, ShaderType, SetShaderResourceViewParameter)
#define IMPLEMENT_SET_PARAMETER_UAV(DeclarationType, ParameterName, ShaderType) _IMPLEMENT_SET_PARAMETER(DeclarationType, FUnorderedAccessViewRHIRef, ParameterName, ShaderType, SetUAVParameter)

class SHADERSPLUS_API FShadersPlusUtilities 
{
public:
    static bool CreateSRV(UTexture2D* Texture, FShaderResourceViewRHIRef& OutSRV);
};