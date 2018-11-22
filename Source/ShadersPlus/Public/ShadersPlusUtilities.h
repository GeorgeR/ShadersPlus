#pragma once

#define SHOULD_CACHE(Condition) static bool ShouldCache(EShaderPlatform Platform) { return ConstantCondition; }
#define SHOULD_CACHE_WITH_FEATURE_LEVEL(FeatureLevel) SHOULD_CACHE(IsFeatureLevelSupported(Platform, FeatureLevel))

#define SHOULD_COMPILE(Condition)	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) \
    {   \
		return Condition; \
	}
#define SHOULD_COMPILE_WITH_FEATURE_LEVEL(FeatureLevel) SHOULD_COMPILE(IsFeatureLevelSupported(Parameters.Platform, FeatureLevel))	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)