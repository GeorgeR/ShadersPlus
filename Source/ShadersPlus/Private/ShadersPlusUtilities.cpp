#include "ShadersPlusUtilities.h"

#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"

#include "RenderingThread.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"

#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "ImageWriteQueue/Public/ImagePixelData.h"
#include "Definitions/ConvertCS.h"
#include "Async/Async.h"
#include "TextureResource.h"
#include "Definitions/QuadUVVS.h"

#define NUM_THREADS_PER_GROUP_DIMENSION 32

TUniquePtr<FImageWriter> FShadersPlusUtilities::ImageWriter = MakeUnique<FImageWriter>();

FVertexBufferRHIRef FShadersPlusUtilities::CreateQuadVertexBuffer()
{
    FRHIResourceCreateInfo CreateInfo;
    FVertexBufferRHIRef VertexBufferRHI = RHICreateVertexBuffer(sizeof(FTextureVertex) * 4, BUF_Volatile, CreateInfo);
    void* VoidPtr = RHILockVertexBuffer(VertexBufferRHI, 0, sizeof(FTextureVertex) * 4, RLM_WriteOnly);

    FTextureVertex* Vertices = (FTextureVertex*)VoidPtr;
    Vertices[0].Position = FVector4(-1.0f, 1.0f, 0, 1.0f);
    Vertices[1].Position = FVector4(1.0f, 1.0f, 0, 1.0f);
    Vertices[2].Position = FVector4(-1.0f, -1.0f, 0, 1.0f);
    Vertices[3].Position = FVector4(1.0f, -1.0f, 0, 1.0f);
    Vertices[0].UV = FVector2D(0, 0);
    Vertices[1].UV = FVector2D(1, 0);
    Vertices[2].UV = FVector2D(0, 1);
    Vertices[3].UV = FVector2D(1, 1);
    RHIUnlockVertexBuffer(VertexBufferRHI);

    return VertexBufferRHI;
}

// #todo Return false if Texture has no resource
bool FShadersPlusUtilities::CreateSRV(UTexture2D* Texture, FShaderResourceViewRHIRef& OutSRV)
{
    check(IsInGameThread());
    check(Texture);

#if ENGINE_MINOR_VERSION >= 22
	ENQUEUE_RENDER_COMMAND(CreateSRV)(
		[Texture, &OutSRV](FRHICommandListImmediate& RHICmdList)
	{
		const auto RHIRef = StaticCast<FTexture2DResource*>(Texture->Resource)->GetTexture2DRHI();
		OutSRV = RHICreateShaderResourceView(RHIRef, 0);
	});
#else
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		CreateSRV,
		UTexture2D*, Texture, Texture,
		FShaderResourceViewRHIRef&, OutSRV, OutSRV,
		{
			const auto RHIRef = StaticCast<FTexture2DResource*>(Texture->Resource)->GetTexture2DRHI();
			OutSRV = RHICreateShaderResourceView(RHIRef, 0);
		});
#endif

    FlushRenderingCommands();

    return true;
}

bool FShadersPlusUtilities::CreateUAV(UTexture2D* Texture, FUnorderedAccessViewRHIRef& OutUAV)
{
    check(IsInGameThread());
    check(Texture);

#if ENGINE_MINOR_VERSION >= 22
	ENQUEUE_RENDER_COMMAND(CreateUAV)(
		[Texture, &OutUAV](FRHICommandListImmediate& RHICmdList)
	{
		const auto RHIRef = StaticCast<FTexture2DResource*>(Texture->Resource)->GetTexture2DRHI();
		OutUAV = RHICreateUnorderedAccessView(RHIRef);
	});
#else
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		CreateUAV,
		UTexture2D*, Texture, Texture,
		FUnorderedAccessViewRHIRef&, OutUAV, OutUAV,
		{
			const auto RHIRef = StaticCast<FTexture2DResource*>(Texture->Resource)->GetTexture2DRHI();
			OutUAV = RHICreateUnorderedAccessView(RHIRef);
		});
#endif

    FlushRenderingCommands();

    return true;
}

bool FShadersPlusUtilities::CreateUAV(UTextureRenderTarget2D* Texture, FUnorderedAccessViewRHIRef& OutUAV)
{
    check(IsInGameThread());
    check(Texture);

#if ENGINE_MINOR_VERSION >= 22
	auto Resource = Texture->GameThread_GetRenderTargetResource();
	ENQUEUE_RENDER_COMMAND(CreateUAV)(
		[Resource, &OutUAV](FRHICommandListImmediate& RHICmdList)
	{
		const auto RHIRef = Resource->GetRenderTargetTexture();
		OutUAV = RHICreateUnorderedAccessView(RHIRef);
	});
#else
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		CreateUAV,
		FTextureRenderTargetResource*, Resource, Texture->GameThread_GetRenderTargetResource(),
		FUnorderedAccessViewRHIRef&, OutUAV, OutUAV,
		{
			const auto RHIRef = Resource->GetRenderTargetTexture();
			OutUAV = RHICreateUnorderedAccessView(RHIRef);
		});
#endif

    FlushRenderingCommands();

    return true;
}

void FShadersPlusUtilities::SaveScreenshot(UTexture2D* Texture, const FString& FilePath)
{
    check(IsInGameThread());
    check(Texture);

    auto TextureResource = (FTexture2DResource*)(Texture->Resource);
    auto TextureReference = TextureResource->GetTexture2DRHI();

    SaveScreenshot(TextureReference, FilePath);
}

void FShadersPlusUtilities::SaveScreenshot(UTextureRenderTarget2D* Texture, const FString& FilePath)
{
    check(IsInGameThread());
    check(Texture);

    auto TextureResource = Texture->GameThread_GetRenderTargetResource();
    auto TextureReference = TextureResource->GetRenderTargetTexture();

    SaveScreenshot(TextureReference, FilePath);
}

// This doesn'tw ork, dont use it
void FShadersPlusUtilities::SaveScreenshot(FShaderResourceViewRHIRef SRV, const FString& FilePath)
{
    check(IsInGameThread());
    check(SRV);

    auto Texture = (FRHITexture2D*)(SRV.GetReference());
    
    SaveScreenshot(Texture, FilePath);
}

void FShadersPlusUtilities::SaveScreenshot(FTexture2DRHIRef Texture, const FString& FilePath)
{
    check(IsInGameThread());
    check(Texture);

#if ENGINE_MINOR_VERSION >= 22
	ENQUEUE_RENDER_COMMAND(SaveScreenshot)(
		[Texture, FilePath](FRHICommandListImmediate& RHICmdList)
	{
		FShadersPlusUtilities::SaveScreenshot_RenderThread(RHICmdList, Texture, FilePath);
	});
#else
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		SaveScreenshot,
		FTexture2DRHIRef, Texture, Texture,
		const FString&, FilePath, FilePath,
		{
			FShadersPlusUtilities::SaveScreenshot_RenderThread(RHICmdList, Texture, FilePath);
		}
	);
#endif


    FlushRenderingCommands();
}

// @note: this is lifted completely from Engine/Source/Runtime/ImageWriteQueue/Private/ImageWriteBlueprintLibrary.cpp
// need to check if it's actually needed here
void FShadersPlusUtilities::SaveScreenshot_RenderThread(FRHICommandListImmediate& RHICmdList, FTexture2DRHIRef Texture, const FString& FilePath)
{
    check(IsInRenderingThread());

    auto SourceRect = FIntRect(0, 0, Texture->GetSizeX(), Texture->GetSizeY());

    FReadSurfaceDataFlags ReadDataFlags(RCM_MinMax);
    ReadDataFlags.SetLinearToGamma(false);
    ReadDataFlags.SetOutputStencil(false);
    ReadDataFlags.SetMip(0);

    TUniquePtr<FImagePixelData> PixelData;

    auto Format = Texture->GetFormat();
    switch (Format)
    {
        case PF_G32R32F:
        {
            FRHIResourceCreateInfo CreateInfo;
            const auto ConvertedOutput = RHICreateTexture2D(SourceRect.Width(), SourceRect.Height(), PF_A32B32G32R32F, 1, 1, TexCreate_UAV, CreateInfo);
            const auto ConvertedOutput_UAV = RHICreateUnorderedAccessView(ConvertedOutput);

            const auto Input_SRV = RHICreateShaderResourceView(Texture, 0);

            const TShaderMapRef<TConvertCS<2,4>> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
            RHICmdList.SetComputeShader(ComputeShader.GetComputeShader());

            ComputeShader->SetInput(RHICmdList, Input_SRV);
            ComputeShader->SetOutput(RHICmdList, ConvertedOutput_UAV);

            DispatchComputeShader(RHICmdList, ComputeShader, SourceRect.Width() / NUM_THREADS_PER_GROUP_DIMENSION, SourceRect.Height() / NUM_THREADS_PER_GROUP_DIMENSION, 1);
            ComputeShader->Unbind(RHICmdList);

            Texture = ConvertedOutput;

            break;
        }
    }
    
    Format = Texture->GetFormat();
    switch (Format)
    {
        case PF_FloatRGBA:
        {
			TArray<FFloat16Color> RawPixels;
			RawPixels.SetNum(SourceRect.Width() * SourceRect.Height());
			RHICmdList.ReadSurfaceFloatData(Texture, SourceRect, RawPixels, ECubeFace::CubeFace_PosX, 0, 0);

            PixelData = MakeUnique<TImagePixelData<FFloat16Color>>(SourceRect.Size(), TArray64<FFloat16Color>(MoveTemp(RawPixels)));

            break;
        }

        case PF_A32B32G32R32F:
        {
			TArray<FLinearColor> RawPixels;
			RawPixels.SetNum(SourceRect.Width() * SourceRect.Height());
			RHICmdList.ReadSurfaceData(Texture, SourceRect, RawPixels, ReadDataFlags);

            PixelData = MakeUnique<TImagePixelData<FLinearColor>>(SourceRect.Size(), TArray64<FLinearColor>(MoveTemp(RawPixels)));

            break;
        }

        case PF_R8G8B8A8:
        case PF_B8G8R8A8:
        {
			TArray<FColor> RawPixels;
			RawPixels.SetNum(SourceRect.Width() * SourceRect.Height());
			RHICmdList.ReadSurfaceData(Texture, SourceRect, RawPixels, ReadDataFlags);

			PixelData = MakeUnique<TImagePixelData<FColor>>(SourceRect.Size(), TArray64<FColor>(MoveTemp(RawPixels)));

            break;
        }
    }

    if (PixelData.IsValid() && PixelData->IsDataWellFormed())
    {
        FImageSaveTask Task;
        Task.Data = MoveTemp(PixelData);
        Task.FilePath = FilePath;

        ImageWriter->Enqueue(MoveTemp(Task));

        //AsyncTask(ENamedThreads::GameThread, [Task]{ 
        //    auto T = Task;
        //    ImageWriter->Enqueue(MoveTemp(T));
        //});
        //OnImageSaveRequest.Broadcast(MoveTemp(Task));
    }
}
