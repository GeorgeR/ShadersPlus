#pragma once

#include "ShadersPlusMacros.h"
#include <GlobalShader.h>

struct FTextureVertex
{
public:
    FVector4 Position;
    FVector2D UV;
};

class FTextureVertexDeclaration
    : public FRenderResource
{
public:
    FVertexDeclarationRHIRef VertexDeclarationRHI;

    void InitRHI() override
    {
        FVertexDeclarationElementList Elements;
        const auto Stride = sizeof(FTextureVertex);
        Elements.Add(FVertexElement(0, STRUCT_OFFSET(FTextureVertex, Position), VET_Float4, 0, Stride));
        Elements.Add(FVertexElement(0, STRUCT_OFFSET(FTextureVertex, UV), VET_Float2, 1, Stride));
        VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
    }

    void ReleaseRHI() override
    {
        VertexDeclarationRHI.SafeRelease();
    }
};

extern SHADERSPLUS_API TGlobalResource<FTextureVertexDeclaration> GTextureVertexDeclaration;

class SHADERSPLUS_API FQuadUVVS
    : public FGlobalShader
{
    DECLARE_SHADER_TYPE(FQuadUVVS, Global);

public:
    SHOULD_CACHE(true)
    SHOULD_COMPILE(true)

    FQuadUVVS() = default;
    explicit FQuadUVVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
};