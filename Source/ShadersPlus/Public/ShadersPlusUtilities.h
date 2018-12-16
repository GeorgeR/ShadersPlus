#pragma once

#include <RHIResources.h>
#include "RHICommandList.h"

#include "ImageWriter.h"

class UTexture2D;
class UTextureRenderTarget2D;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnImageSaveRequest, FImageSaveTask&&);

class SHADERSPLUS_API FShadersPlusUtilities 
{
public:
    static FVertexBufferRHIRef CreateQuadVertexBuffer();

    static bool CreateSRV(UTexture2D* Texture, FShaderResourceViewRHIRef& OutSRV);
    static bool CreateUAV(UTexture2D* Texture, FUnorderedAccessViewRHIRef& OutUAV);
    static bool CreateUAV(UTextureRenderTarget2D* Texture, FUnorderedAccessViewRHIRef& OutUAV);
    
    static void SaveScreenshot(UTexture2D* Texture, const FString& FilePath);
    static void SaveScreenshot(UTextureRenderTarget2D* Texture, const FString& FilePath);
    static void SaveScreenshot(FTexture2DRHIRef Texture, const FString& FilePath);
    static void SaveScreenshot(FShaderResourceViewRHIRef SRV, const FString& FilePath);

    static void SaveScreenshot_RenderThread(FRHICommandListImmediate& RHICmdList, FTexture2DRHIRef Texture, const FString& FilePath);

private:
    static TUniquePtr<FImageWriter> ImageWriter;
    static FOnImageSaveRequest OnImageSaveRequest;
};