#pragma once

#include "ShaderInstance.h"
#include "RenderingThread.h"

struct FCombineParameters
{
public:
    FIntPoint Size;

    /* Inputs */
    FShaderResourceViewRHIRef InputX_SRV;
    FShaderResourceViewRHIRef InputY_SRV;
    FShaderResourceViewRHIRef InputZ_SRV;
    FShaderResourceViewRHIRef InputW_SRV;

    /* Outputs */
    FUnorderedAccessViewRHIRef Output_UAV;
};

template <int32 InputComponents>
class TCombineInstance
    : public TComputeShaderInstance<FCombineParameters>
{
public:
    TCombineInstance(const int32 SizeX, const int32 SizeY, const ERHIFeatureLevel::Type FeatureLevel);
    virtual ~TCombineInstance() { }

    void OnDispatch(FCombineParameters& Parameters) override;

    FTexture2DRHIRef GetOutput() const { return Output; }
    FUnorderedAccessViewRHIRef GetOutput_UAV() const { return Output_UAV; }
    FShaderResourceViewRHIRef GetOutput_SRV() const { return Output_SRV; }

private:
    FTexture2DRHIRef Output;
    FUnorderedAccessViewRHIRef Output_UAV;
    FShaderResourceViewRHIRef Output_SRV;
};

template <int32 InputComponents>
TCombineInstance<InputComponents>::TCombineInstance(const int32 SizeX, const int32 SizeY, const ERHIFeatureLevel::Type FeatureLevel)
    : TComputeShaderInstance<FConvertParameters>(FeatureLevel)
{
    FRHIResourceCreateInfo CreateInfo;
    Output = RHICreateTexture2D(SizeX, SizeY, OutputFormat, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
    Output_UAV = RHICreateUnorderedAccessView(Output);
    Output_SRV = RHICreateShaderResourceView(Output, 0);
}

template <int32 InputComponents>
void TCombineInstance<InputComponents>::OnDispatch(FCombineParameters& Parameters)
{
    check(Parameters.InputX_SRV.IsValid());

    Parameters.Size = Output->GetSizeXY();
    Parameters.Output_UAV = Output_UAV;

    typedef TCombineCS<InputComponents> FCombineCS;

    ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
        Convert,
        FCombineParameters, Parameters, Parameters,
        {
            SCOPED_DRAW_EVENT(RHICmdList, Convert);

            TShaderMapRef<FCombineCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
            RHICmdList.SetComputeShader(ComputeShader->GetComputeShader());

            ComputeShader->SetInputX(RHICmdList, Parameters.InputX_SRV);
            ComputeShader->SetInputX(RHICmdList, Parameters.InputY_SRV);
            ComputeShader->SetInputX(RHICmdList, Parameters.InputZ_SRV);
            ComputeShader->SetInputX(RHICmdList, Parameters.InputW_SRV);
            ComputeShader->SetOutput(RHICmdList, Parameters.Output_UAV);

            DispatchComputeShader(RHICmdList, *ComputeShader, Parameters.Size / NUM_THREADS_PER_GROUP_DIMENSION, Parameters.Size / NUM_THREADS_PER_GROUP_DIMENSION, 1);
            ComputeShader->Unbind(RHICmdList);
        }
    )
}