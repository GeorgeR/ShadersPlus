#include "ConvertCS.h"

FConvertCS::FConvertCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
    : FGlobalShader(Initializer)
{
    BIND(Input);
    BIND(Output);
}

IMPLEMENT_SET_PARAMETER_SRV(FConvertCS, Input, Compute)
IMPLEMENT_SET_PARAMETER_UAV(FConvertCS, Output, Compute)

void FConvertCS::Unbind(FRHICommandList& RHICmdList)
{
    const auto ComputeShaderRHI = GetComputeShader();

    UNBIND_SRV(Input, Compute);
    UNBIND_UAV(Output, Compute);
}

bool FConvertCS::Serialize(FArchive& Ar)
{
    const auto bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);

    Ar << Input << Output;

    return bShaderHasOutdatedParameters;
}

IMPLEMENT_SHADER_TYPE(, FConvertCS, TEXT("/Plugin/ShadersPlus/Private/ConvertCS.usf"), TEXT("MainCS"), SF_Compute);

#ifndef IMPLEMENT_PERMUTATION
#define IMPLEMENT_PERMUTATION(InputComponents, OutputComponents) typedef TConvertCS<InputComponents, OutputComponents> TConvertCS##InputComponents##OutputComponents##;   \
    IMPLEMENT_SHADER_TYPE(template<> SHADERSPLUS_API, TConvertCS##InputComponents##OutputComponents##, TEXT("/Plugin/ShadersPlus/Private/ConvertCS.usf"), TEXT("MainCS"), SF_Compute);

IMPLEMENT_PERMUTATION(1, 1)
IMPLEMENT_PERMUTATION(1, 2)
IMPLEMENT_PERMUTATION(1, 3)
IMPLEMENT_PERMUTATION(1, 4)

IMPLEMENT_PERMUTATION(2, 1)
IMPLEMENT_PERMUTATION(2, 2)
IMPLEMENT_PERMUTATION(2, 3)
IMPLEMENT_PERMUTATION(2, 4)

IMPLEMENT_PERMUTATION(3, 1)
IMPLEMENT_PERMUTATION(3, 2)
IMPLEMENT_PERMUTATION(3, 3)
IMPLEMENT_PERMUTATION(3, 4)

IMPLEMENT_PERMUTATION(4, 1)
IMPLEMENT_PERMUTATION(4, 2)
IMPLEMENT_PERMUTATION(4, 3)
IMPLEMENT_PERMUTATION(4, 4)

#undef IMPLEMENT_PERMUTATION
#endif