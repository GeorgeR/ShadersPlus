#include "ShadersPlusUtilities.h"

#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"

#include <RenderingThread.h>
#include "FileManager.h"
#include "FileHelper.h"

#include "IImageWrapperModule.h"
#include "ModuleManager.h"
#include "ImageWriteQueue/Public/ImagePixelData.h"
#include "ConvertCS.h"

#define NUM_THREADS_PER_GROUP_DIMENSION 32

// TODO: Return false if Texture has no resource
bool FShadersPlusUtilities::CreateSRV(UTexture2D* Texture, FShaderResourceViewRHIRef& OutSRV)
{
    check(IsInGameThread());
    check(Texture);

    ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
        CreateSRV,
        UTexture2D*, Texture, Texture,
        FShaderResourceViewRHIRef&, OutSRV, OutSRV,
        {
            auto RHIRef = StaticCast<FTexture2DResource*>(Texture->Resource)->GetTexture2DRHI();
            OutSRV = RHICreateShaderResourceView(RHIRef, 0);
        });

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

    ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
        SaveScreenshot,
        FTexture2DRHIRef, Texture, Texture,
        const FString&, FilePath, FilePath,
        {
            FShadersPlusUtilities::SaveScreenshot_RenderThread(RHICmdList, Texture, FilePath);
        }
    );

    FlushRenderingCommands();
}

void FShadersPlusUtilities::SaveScreenshot_RenderThread(FRHICommandListImmediate& RHICmdList, FTexture2DRHIRef Texture, const FString& FilePath)
{
    check(IsInRenderingThread());

    auto SourceRect = FIntRect(0, 0, Texture->GetSizeX(), Texture->GetSizeY());

    FReadSurfaceDataFlags ReadDataFlags(RCM_MinMax);
    ReadDataFlags.SetLinearToGamma(false);
    //ReadDataFlags.SetOutputStencil(false);
    //ReadDataFlags.SetMip(0);

    EImageFormat ImageFormat = EImageFormat::PNG;
    auto ImageWrapper = GetImageWrapperForFormat(EImageFormat::EXR);

    TUniquePtr<FImagePixelData> PixelData;

    auto Format = Texture->GetFormat();
    switch (Format)
    {
        case PF_G32R32F:
        {
            FRHIResourceCreateInfo CreateInfo;
            auto ConvertedOutput = RHICreateTexture2D(SourceRect.Width(), SourceRect.Height(), PF_A32B32G32R32F, 1, 1, TexCreate_UAV, CreateInfo);
            auto ConvertedOutput_UAV = RHICreateUnorderedAccessView(ConvertedOutput);

            auto Input_SRV = RHICreateShaderResourceView(Texture, 0);

            TShaderMapRef<TConvertCS<2,4>> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
            RHICmdList.SetComputeShader(ComputeShader->GetComputeShader());

            ComputeShader->SetInput(RHICmdList, Input_SRV);
            ComputeShader->SetOutput(RHICmdList, ConvertedOutput_UAV);

            DispatchComputeShader(RHICmdList, *ComputeShader, SourceRect.Width() / NUM_THREADS_PER_GROUP_DIMENSION, SourceRect.Height() / NUM_THREADS_PER_GROUP_DIMENSION, 1);
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
            TUniquePtr<TImagePixelData<FFloat16Color>> BitmapData = MakeUnique<TImagePixelData<FFloat16Color>>(SourceRect.Size());
            RHICmdList.ReadSurfaceFloatData(Texture, SourceRect, BitmapData->Pixels, ECubeFace::CubeFace_PosX, 0, 0);

            PixelData = MoveTemp(BitmapData);

            break;
        }

        case PF_A32B32G32R32F:
        {
            TUniquePtr<TImagePixelData<FLinearColor>> BitmapData = MakeUnique<TImagePixelData<FLinearColor>>(SourceRect.Size());
            RHICmdList.ReadSurfaceData(Texture, SourceRect, BitmapData->Pixels, ReadDataFlags);

            PixelData = MoveTemp(BitmapData);

            break;
        }

        case PF_R8G8B8A8:
        case PF_B8G8R8A8:
        {
            TUniquePtr<TImagePixelData<FColor>> BitmapData = MakeUnique<TImagePixelData<FColor>>(SourceRect.Size());
            RHICmdList.ReadSurfaceData(Texture, SourceRect, BitmapData->Pixels, ReadDataFlags);

            PixelData = MoveTemp(BitmapData);

            break;
        }
    }

    if (PixelData.IsValid() && PixelData->IsDataWellFormed())
    {
        const void* RawPtr = nullptr;
        int32 SizeInBytes = 0;

        if (PixelData->GetRawData(RawPtr, SizeInBytes))
        {
            uint8 BitDepth = PixelData->GetBitDepth();
            FIntPoint Size = PixelData->GetSize();
            ERGBFormat PixelLayout = PixelData->GetPixelLayout();

            ImageWrapper->SetRaw(RawPtr, SizeInBytes, Size.X, Size.Y, PixelLayout, BitDepth);
        }

        FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(), *FilePath);
    }
}

TSharedPtr<IImageWrapper> FShadersPlusUtilities::GetImageWrapperForFormat(EImageFormat Format)
{
    auto ImageWrapperModule = FModuleManager::GetModulePtr<IImageWrapperModule>(TEXT("ImageWrapper"));
    return ImageWrapperModule->CreateImageWrapper(Format);
}