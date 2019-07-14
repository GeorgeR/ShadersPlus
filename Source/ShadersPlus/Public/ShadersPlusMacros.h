#pragma once

#define SHOULD_CACHE(Condition) static bool ShouldCache(EShaderPlatform Platform) { return Condition; }
#define SHOULD_CACHE_WITH_FEATURE_LEVEL(FeatureLevel) SHOULD_CACHE(IsFeatureLevelSupported(Platform, FeatureLevel))

#define SHOULD_COMPILE(Condition)	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) \
    {   \
		return Condition; \
	}
#define SHOULD_COMPILE_WITH_FEATURE_LEVEL(FeatureLevel) SHOULD_COMPILE(IsFeatureLevelSupported(Parameters.Platform, FeatureLevel))

#define DECLARE_SET_PARAMETER(ParameterType, ParameterName) void Set##ParameterName(FRHICommandList& RHICmdList, ParameterType Value)

#define _IMPLEMENT_SET_PARAMETER(DeclarationType, ParameterType, ParameterName, ShaderType, Setter) void DeclarationType::Set##ParameterName(FRHICommandList& RHICmdList, ParameterType Value)    \
{   \
    const auto ShaderType##ShaderRHI = Get##ShaderType##Shader();    \
    if (this->##ParameterName.IsBound())    \
        RHICmdList.##Setter(ShaderType##ShaderRHI, this->##ParameterName.GetBaseIndex(), Value);     \
}

#define IMPLEMENT_SET_PARAMETER_SRV(DeclarationType, ParameterName, ShaderType) _IMPLEMENT_SET_PARAMETER(DeclarationType, FShaderResourceViewRHIRef, ParameterName, ShaderType, SetShaderResourceViewParameter)
#define IMPLEMENT_SET_PARAMETER_UAV(DeclarationType, ParameterName, ShaderType) _IMPLEMENT_SET_PARAMETER(DeclarationType, FUnorderedAccessViewRHIRef, ParameterName, ShaderType, SetUAVParameter)

#if ENGINE_MINOR_VERSION >= 23
#define IMPLEMENT_SET_PARAMETER_SAMPLER(DeclarationType, ParameterName, ShaderType) _IMPLEMENT_SET_PARAMETER(DeclarationType, FRHISamplerState*, ParameterName, ShaderType, SetShaderSampler)
#else
#define IMPLEMENT_SET_PARAMETER_SAMPLER(DeclarationType, ParameterName, ShaderType) _IMPLEMENT_SET_PARAMETER(DeclarationType, FSamplerStateRHIParamRef, ParameterName, ShaderType, SetShaderSampler)
#endif

#define BIND(ParameterName) ParameterName.Bind(Initializer.ParameterMap, TEXT(#ParameterName))

#define _UNBIND(ParameterType, ParameterName, ShaderType, Setter) if(this->##ParameterName##.IsBound())  \
    RHICmdList.##Setter(ShaderType##ShaderRHI, this->##ParameterName##.GetBaseIndex(), ##ParameterType())  \

#define UNBIND_SRV(ParameterName, ShaderType) _UNBIND(FShaderResourceViewRHIParamRef, ParameterName, ShaderType, SetShaderResourceViewParameter)
#define UNBIND_UAV(ParameterName, ShaderType) _UNBIND(FUnorderedAccessViewRHIRef, ParameterName, ShaderType, SetUAVParameter)
