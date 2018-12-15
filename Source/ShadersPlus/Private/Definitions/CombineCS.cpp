#include "CombineCS.h"

FCombineCS::FCombineCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
    : FGlobalShader(Initializer)
{
    BIND(InputX);
    BIND(InputY);
    BIND(InputZ);
    BIND(InputW);
    BIND(Output);
}

IMPLEMENT_SET_PARAMETER_SRV(FCombineCS, InputX, Compute)
IMPLEMENT_SET_PARAMETER_SRV(FCombineCS, InputY, Compute)
IMPLEMENT_SET_PARAMETER_SRV(FCombineCS, InputZ, Compute)
IMPLEMENT_SET_PARAMETER_SRV(FCombineCS, InputW, Compute)
IMPLEMENT_SET_PARAMETER_UAV(FCombineCS, Output, Compute)

void FCombineCS::Unbind(FRHICommandList& RHICmdList)
{
    const auto ComputeShaderRHI = GetComputeShader();

    UNBIND_SRV(InputX, Compute);
    UNBIND_SRV(InputY, Compute);
    UNBIND_SRV(InputZ, Compute);
    UNBIND_SRV(InputW, Compute);
    UNBIND_UAV(Output, Compute);
}

bool FCombineCS::Serialize(FArchive& Ar)
{
    const auto bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);

    Ar << InputX << InputY << InputZ << InputW << Output;

    return bShaderHasOutdatedParameters;
}

IMPLEMENT_SHADER_TYPE(, FCombineCS, TEXT("/Plugin/ShadersPlus/Private/CombineCS.usf"), TEXT("MainCS"), SF_Compute);

#ifndef IMPLEMENT_PERMUTATION
#define IMPLEMENT_PERMUTATION(InputComponents) typedef TCombineCS<InputComponents> TCombineCS##InputComponents##;   \
    IMPLEMENT_SHADER_TYPE(template<> SHADERSPLUS_API, TCombineCS##InputComponents##, TEXT("/Plugin/ShadersPlus/Private/CombineCS.usf"), TEXT("MainCS"), SF_Compute);

IMPLEMENT_PERMUTATION(1)
IMPLEMENT_PERMUTATION(2)
IMPLEMENT_PERMUTATION(3)
IMPLEMENT_PERMUTATION(4)

#undef IMPLEMENT_PERMUTATION
#endif