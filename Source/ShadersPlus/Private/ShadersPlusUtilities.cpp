#include "ShadersPlusUtilities.h"

#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include <RenderingThread.h>
#include "FileManager.h"
#include "FileHelper.h"

// TODO: Return false if Texture has no resource
bool FShadersPlusUtilities::CreateSRV(UTexture2D* Texture, FShaderResourceViewRHIRef& OutSRV)
{
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

    auto TextureResource = (FTexture2DResource*)(Texture->Resource);
    auto TextureReference = TextureResource->GetTexture2DRHI();

    SaveScreenshot(TextureReference, FilePath);
}

void FShadersPlusUtilities::SaveScreenshot(FTexture2DRHIRef Texture, const FString& FilePath)
{
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

    TArray<FColor> Bitmap;
    FReadSurfaceDataFlags ReadDataFlags;
    ReadDataFlags.SetLinearToGamma(false);
    ReadDataFlags.SetOutputStencil(false);
    ReadDataFlags.SetMip(0);

    RHICmdList.ReadSurfaceData(Texture, FIntRect(0, 0, Texture->GetSizeX(), Texture->GetSizeY()), Bitmap, ReadDataFlags);

    if (Bitmap.Num())
    {
        uint32 ExtendXWithMSAA = Bitmap.Num() / Texture->GetSizeY();
        FFileHelper::CreateBitmap(*FilePath, ExtendXWithMSAA, Texture->GetSizeY(), Bitmap.GetData());
    }
}