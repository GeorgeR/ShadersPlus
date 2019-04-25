#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
#include "ThreadSafeBool.h"
#include "RHIStaticStates.h"
#include "PipelineStateCache.h"

#include "QuadUVVS.h"

class UTextureRenderTarget2D;

class SHADERSPLUS_API FShaderInstance
{
public:
    FShaderInstance(const ERHIFeatureLevel::Type FeatureLevel);
    virtual ~FShaderInstance();

    virtual bool CanExecute();

    FORCEINLINE void SetDebugLabel(const FName Label) { DebugLabel = Label; }

protected:
    TOptional<FName> DebugLabel;

    FThreadSafeBool bIsExecuting;
    FThreadSafeBool bIsUnloading;
    ERHIFeatureLevel::Type FeatureLevel;
};

class SHADERSPLUS_API FComputeShaderInstance
    : public FShaderInstance
{
public:
    FComputeShaderInstance(const ERHIFeatureLevel::Type FeatureLevel);

    virtual ~FComputeShaderInstance() { }

    void Dispatch()
    {
        if (!CanExecute())
            return;

        bIsExecuting = true;
        OnDispatch();
        bIsExecuting = false;
    }

    virtual void OnDispatch() { }

    void DispatchOnce();
    virtual void OnDispatchOnce() { }

protected:
    FThreadSafeBool bHasBeenRun;
};

template <typename TParameters>
class TComputeShaderInstance
    : public FShaderInstance
{
public:
    TComputeShaderInstance(const ERHIFeatureLevel::Type FeatureLevel)
        : FShaderInstance(FeatureLevel) { }

    virtual ~TComputeShaderInstance() { }

    void Dispatch(TParameters& Parameters)
    {
        if (!CanExecute())
            return;

        bIsExecuting = true;
        OnDispatch(Parameters);
        bIsExecuting = false;
    }

    virtual void OnDispatch(TParameters& Parameters) { }

    void DispatchOnce(TParameters& Parameters)
    {
        if (!CanExecute() || bHasBeenRun)
            return;

        bIsExecuting = true;
        OnDispatchOnce(Parameters);
        bIsExecuting = false;

        bHasBeenRun = true;
    }

    virtual void OnDispatchOnce(TParameters& Parameters) { }

protected:
    FThreadSafeBool bHasBeenRun;
};

template <typename TVertexShader, typename TPixelShader>
class FPixelShaderInstanceBase
    : public FShaderInstance
{
public:
    FPixelShaderInstanceBase(const ERHIFeatureLevel::Type FeatureLevel)
        : FShaderInstance(FeatureLevel)
    {
        TargetResource = nullptr;
        TargetTexture = nullptr;
    }

    virtual ~FPixelShaderInstanceBase() { }

protected:
    FThreadSafeBool bHasBeenRun;

    FTexture2DRHIRef TargetResource;
    UTextureRenderTarget2D* TargetTexture;
};


struct FDrawToQuadParameters
{
public:
    bool bGenerateMips;

    FDrawToQuadParameters()
        : bGenerateMips(true)
    {

    }
};

template <typename TParameters, typename TPixelShader>
class TDrawToQuadInstance
    : public FPixelShaderInstanceBase<FQuadUVVS, TPixelShader>
{
public:
    TDrawToQuadInstance(const ERHIFeatureLevel::Type FeatureLevel)
        : FPixelShaderInstanceBase(FeatureLevel) { }

    void DrawToQuad(TParameters& Parameters, UTextureRenderTarget2D* RenderTarget)
    {
        FDrawToQuadParameters DrawToQuadParameters;
        DrawToQuad(Parameters, RenderTarget, DrawToQuadParameters);
    }

    void DrawToQuad(TParameters& Parameters, UTextureRenderTarget2D* RenderTarget, FDrawToQuadParameters& DrawToQuadParameters)
    {
        if (!CanExecute() || RenderTarget == nullptr)
            return;

        bIsExecuting = true;

#if (ENGINE_MINOR_VERSION < 22)
		ENQUEUE_UNIQUE_RENDER_COMMAND_FOURPARAMETER(
			DrawToQuad,
			TDrawToQuadInstance*, DrawToQuadInstance, this,
			FDrawToQuadParameters, DrawToQuadParameters, DrawToQuadParameters,
			TParameters, Parameters, Parameters,
			FTextureRenderTargetResource*, RenderTarget, RenderTarget->GameThread_GetRenderTargetResource(),
			{
				FRHICommandListImmediate& RHICmdListImmediate = GRHICommandList.GetImmediateCommandList();
				DrawToQuadInstance->DrawToQuad_RenderThread_Pre_422(RHICmdListImmediate, Parameters, RenderTarget, DrawToQuadParameters);
			}
		);
#else
		ENQUEUE_RENDER_COMMAND(FDrawToQuad)(
			[this, &DrawToQuadParameters, &Parameters, &RenderTarget](FRHICommandListImmediate& RHICmdList)
			{
				FRHICommandListImmediate& RHICmdListImmediate = GRHICommandList.GetImmediateCommandList();
				this->DrawToQuad_RenderThread(RHICmdListImmediate, Parameters, RenderTarget->GetRenderTargetResource(), DrawToQuadParameters);
			});
#endif
    }

protected:
	void DrawToQuad_RenderThread(FRHICommandListImmediate& RHICmdList, TParameters& Parameters, FTextureRenderTargetResource* RenderTarget, FDrawToQuadParameters& DrawToQuadParameters)
	{
		check(IsInRenderingThread());

		SCOPED_DRAW_EVENT(RHICmdList, DrawToQuad);

		FRHIRenderPassInfo RenderPassInfo(RenderTarget->GetRenderTargetTexture(), ERenderTargetActions::Clear_Store);
		RHICmdList.BeginRenderPass(RenderPassInfo, TEXT("FDrawToQuad"));

		TShaderMapRef<FQuadUVVS> VertexShader(GetGlobalShaderMap(FeatureLevel));
		TShaderMapRef<TPixelShader> PixelShader(GetGlobalShaderMap(FeatureLevel));

		FGraphicsPipelineStateInitializer GraphicsPSOInitializer;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInitializer);
		GraphicsPSOInitializer.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInitializer.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInitializer.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInitializer.PrimitiveType = PT_TriangleStrip;
		GraphicsPSOInitializer.BoundShaderState.VertexDeclarationRHI = GTextureVertexDeclaration.VertexDeclarationRHI;
		GraphicsPSOInitializer.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
		GraphicsPSOInitializer.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInitializer);

		OnSetupPixelShader_RenderThread(RHICmdList, PixelShader, Parameters);

		RHICmdList.SetStreamSource(0, FShadersPlusUtilities::CreateQuadVertexBuffer(), 0);
		RHICmdList.DrawPrimitive(0, 2, 1);

		if (DrawToQuadParameters.bGenerateMips)
			RHICmdList.GenerateMips(RenderTarget->TextureRHI);

		OnTeardownPixelShader_RenderThread(RHICmdList, PixelShader);

		RHICmdList.EndRenderPass();

		bIsExecuting = false;
	}

    void DrawToQuad_RenderThread_Pre_422(FRHICommandListImmediate& RHICmdList, TParameters& Parameters, FTextureRenderTargetResource* RenderTarget, FDrawToQuadParameters& DrawToQuadParameters)
    {
        check(IsInRenderingThread());

        SCOPED_DRAW_EVENT(RHICmdList, DrawToQuad);

        SetRenderTarget(RHICmdList, RenderTarget->GetRenderTargetTexture(), nullptr);

        TShaderMapRef<FQuadUVVS> VertexShader(GetGlobalShaderMap(FeatureLevel));
        TShaderMapRef<TPixelShader> PixelShader(GetGlobalShaderMap(FeatureLevel));

        FGraphicsPipelineStateInitializer GraphicsPSOInitializer;
        RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInitializer);
        GraphicsPSOInitializer.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
        GraphicsPSOInitializer.BlendState = TStaticBlendState<>::GetRHI();
        GraphicsPSOInitializer.RasterizerState = TStaticRasterizerState<>::GetRHI();
        GraphicsPSOInitializer.PrimitiveType = PT_TriangleStrip;
        GraphicsPSOInitializer.BoundShaderState.VertexDeclarationRHI = GTextureVertexDeclaration.VertexDeclarationRHI;
        GraphicsPSOInitializer.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
        GraphicsPSOInitializer.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
        SetGraphicsPipelineState(RHICmdList, GraphicsPSOInitializer);

        OnSetupPixelShader_RenderThread(RHICmdList, PixelShader, Parameters);

#if (ENGINE_MINOR_VERSION >= 21)
        RHICmdList.SetStreamSource(0, FShadersPlusUtilities::CreateQuadVertexBuffer(), 0);
        RHICmdList.DrawPrimitive(PT_TriangleStrip, 0, 2, 1);

        //RHICmdList.CopyToResolveTarget(
        //    RenderTarget->GetRenderTargetTexture(),
        //    RenderTarget->TextureRHI,
        //    FResolveParams());
#else
        FTextureVertex Vertices[4];
        Vertices[0].Position = FVector4(-1.0f, 1.0f, 0, 1.0f);
        Vertices[1].Position = FVector4(1.0f, 1.0f, 0, 1.0f);
        Vertices[2].Position = FVector4(-1.0f, -1.0f, 0, 1.0f);
        Vertices[3].Position = FVector4(1.0f, -1.0f, 0, 1.0f);
        Vertices[0].UV = FVector2D(0, 0);
        Vertices[1].UV = FVector2D(1, 0);
        Vertices[2].UV = FVector2D(0, 1);
        Vertices[3].UV = FVector2D(1, 1);

        // Deprecated in 4.21
        DrawPrimitiveUP(RHICmdList, PT_TriangleStrip, 2, Vertices, sizeof(Vertices[0]));

        //RHICmdList.CopyToResolveTarget(
        //    RenderTarget->GetRenderTargetTexture(),
        //    RenderTarget->TextureRHI,
        //    false,
        //    FResolveParams());
#endif
        if(DrawToQuadParameters.bGenerateMips)
            RHICmdList.GenerateMips(RenderTarget->TextureRHI);

        //RHICmdList.GenerateMips(RenderTarget->GetRenderTargetTexture());

        OnTeardownPixelShader_RenderThread(RHICmdList, PixelShader);

        bIsExecuting = false;
    }

    virtual void OnSetupVertexShader_RenderThread(FRHICommandList& RHICmdList, TShaderMapRef<FQuadUVVS> VertexShader, TParameters& Parameters) { }
    virtual void OnSetupPixelShader_RenderThread(FRHICommandList& RHICmdList, TShaderMapRef<TPixelShader> PixelShader, TParameters& Parameters) { }
    virtual void OnTeardownPixelShader_RenderThread(FRHICommandList& RHICmdList, TShaderMapRef<TPixelShader> PixelShader) { }
};
