#include "Definitions/QuadUVVS.h"

TGlobalResource<FTextureVertexDeclaration> GTextureVertexDeclaration;

FQuadUVVS::FQuadUVVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
    : FGlobalShader(Initializer) { }

IMPLEMENT_SHADER_TYPE(, FQuadUVVS, TEXT("/Plugin/ShadersPlus/Private/QuadUVVS.usf"), TEXT("MainVS"), SF_Vertex);
