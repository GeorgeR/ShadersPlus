#include "ShadersPlusUtilities.h"

#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include <RenderingThread.h>

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