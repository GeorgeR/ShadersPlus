#pragma once

#include <RHIResources.h>
#include "RHICommandList.h"

class UTexture2D;
class UTextureRenderTarget2D;

class SHADERSPLUS_API FShadersPlusUtilities 
{
public:
    static bool CreateSRV(UTexture2D* Texture, FShaderResourceViewRHIRef& OutSRV);

    static void SaveScreenshot(UTexture2D* Texture, const FString& FilePath);
    static void SaveScreenshot(UTextureRenderTarget2D* Texture, const FString& FilePath);
    static void SaveScreenshot(FTexture2DRHIRef Texture, const FString& FilePath);
  
    static void SaveScreenshot_RenderThread(FRHICommandListImmediate& RHICmdList, FTexture2DRHIRef Texture, const FString& FilePath);
};