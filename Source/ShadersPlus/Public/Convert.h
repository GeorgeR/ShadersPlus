#pragma once

#include "ShaderInstance.h"
#include "RenderingThread.h"

struct FConvertParameters
{
public:
    FIntPoint Size;

    /* Inputs */
    FShaderResourceViewRHIRef Input_SRV;

    /* Outputs */
    FUnorderedAccessViewRHIRef Output_UAV;
};

template <EPixelFormat InputFormat, EPixelFormat OutputFormat>
class TConvertInstance
    : public TComputeShaderInstance<FConvertParameters>
{
public:
    TConvertInstance(const int32 SizeX, const int32 SizeY, const ERHIFeatureLevel::Type FeatureLevel);
    virtual ~TConvertInstance() { }

    void OnDispatch(FConvertParameters& Parameters) override;

    FTexture2DRHIRef GetOutput() const { return Output; }
    FUnorderedAccessViewRHIRef GetOutput_UAV() const { return Output_UAV; }
    FShaderResourceViewRHIRef GetOutput_SRV() const { return Output_SRV; }

private:
    FTexture2DRHIRef Output;
    FUnorderedAccessViewRHIRef Output_UAV;
    FShaderResourceViewRHIRef Output_SRV;

    constexpr int32 GetComponentCount(EPixelFormat Format);
};

template <EPixelFormat InputFormat, EPixelFormat OutputFormat>
TConvertInstance<InputFormat, OutputFormat>::TConvertInstance(const int32 SizeX, const int32 SizeY, const ERHIFeatureLevel::Type FeatureLevel)
    : TComputeShaderInstance<FConvertParameters>(FeatureLevel)
{
    FRHIResourceCreateInfo CreateInfo;
    Output = RHICreateTexture2D(SizeX, SizeY, OutputFormat, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
    Output_UAV = RHICreateUnorderedAccessView(Output);
    Output_SRV = RHICreateShaderResourceView(Output, 0);
}

template <EPixelFormat InputFormat, EPixelFormat OutputFormat>
void TConvertInstance<InputFormat, OutputFormat>::OnDispatch(FConvertParameters& Parameters)
{
    check(Parameters.Input_SRV.IsValid());

    Parameters.Size = Output->GetSizeXY();
    Parameters.Output_UAV = Output_UAV;

    typedef TConvertCS<GetComponentCount(InputFormat), GetComponentCount(OutputFormat)> FConvertCS;

    ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
        Convert,
        FConvertParameters, Parameters, Parameters,
        {
            SCOPED_DRAW_EVENT(RHICmdList, Convert);

            TShaderMapRef<FConvertCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
            RHICmdList.SetComputeShader(ComputeShader->GetComputeShader());

            ComputeShader->SetInput(RHICmdList, Parameters.Input_SRV);
            ComputeShader->SetOutput(RHICmdList, Parameters.Output_UAV);

            DispatchComputeShader(RHICmdList, *ComputeShader, Parameters.Size / NUM_THREADS_PER_GROUP_DIMENSION, Parameters.Size / NUM_THREADS_PER_GROUP_DIMENSION, 1);
            ComputeShader->Unbind(RHICmdList);
        }
    )
}

template <EPixelFormat InputFormat, EPixelFormat OutputFormat>
constexpr int32 TConvertInstance<InputFormat, OutputFormat>::GetComponentCount(EPixelFormat Format)
{
    switch (Format)
    {
    case PF_Unknown:
    case PF_DepthStencil:
    case PF_ShadowDepth:
    case PF_V8U8:
    case PF_X24_G8:
    case PF_ETC1:
    case PF_ETC2_RGB:
    case PF_ASTC_4x4:
    case PF_ASTC_6x6:
    case PF_ASTC_8x8:
    case PF_ASTC_10x10:
    case PF_ASTC_12x12:
    case PF_BC6H:
    case PF_XGXR8:
    case PF_PLATFORM_HDR_0:
    case PF_PLATFORM_HDR_1:
    case PF_PLATFORM_HDR_2:
    case PF_MAX:
    default:
        return -1;        

    case PF_G8:
    case PF_G16:
    case PF_R32_FLOAT:
    case PF_D24:
    case PF_R16F:
    case PF_R16F_FILTER:
    case PF_A1:
    case PF_A8:
    case PF_R32_UINT:
    case PF_R32_SINT:
    case PF_R16_UINT:
    case PF_R16_SINT:
    case PF_R8_UINT:
    case PF_L8:
        return 1;        

    case PF_G16R16:
    case PF_G16R16F:
    case PF_G16R16F_FILTER:
    case PF_G32R32F:
    case PF_R8G8:
    case PF_R16G16_UINT:
        return 2;        

    case PF_DXT1:
    case PF_UYVY:
    case PF_B8G8R8A8:
    case PF_FloatRGB:
    case PF_FloatR11G11B10:
    case PF_R5G6B5_UNORM:
    case PF_R8G8B8A8_UINT:
    case PF_R8G8B8A8_SNORM:
    case PF_ATC_RGB:
        return 3;        

    case PF_DXT3:
    case PF_DXT5:
    case PF_A32B32G32R32F:
    case PF_FloatRGBA:
    case PF_A2B10G10R10:
    case PF_A16B16G16R16:
    case PF_R16G16B16A16_UINT:
    case PF_R16G16B16A16_SINT:
    case PF_R8G8B8A8:
    case PF_A8R8G8B8:
    case PF_ATC_RGBA_E:
    case PF_ATC_RGBA_I:
    case PF_ETC2_RGBA:
    case PF_R32G32B32A32_UINT:
    case PF_R16G16B16A16_UNORM:
    case PF_R16G16B16A16_SNORM:
    case PF_BC4:
    case PF_BC5:
    case PF_BC7:
    case PF_PVRTC2:
    case PF_PVRTC4:
        return 4;
    }
}