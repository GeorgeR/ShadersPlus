#pragma once

#include "CoreMinimal.h"

#include "ImagePixelData.h"
#include "HAL/CriticalSection.h"
#include "Containers/Queue.h"
#include "Containers/Array.h"

struct FImageSaveTask
{
public:
    TUniquePtr<FImagePixelData> Data;
    FString FilePath;

    FImageSaveTask() = default;
    FImageSaveTask(const FImageSaveTask& Other)
    {
        Data = Other.Data->MoveImageDataToNew();
        FilePath = Other.FilePath;
    }

    FImageSaveTask& operator=(const FImageSaveTask& Other)
    {
        if(Other.Data.IsValid())
            Data = Other.Data->MoveImageDataToNew();

        FilePath = Other.FilePath;

        return *this;
    }

    bool IsValid() const { return Data.IsValid(); }
};

class FImageWriter
{
public:
    void Enqueue(FImageSaveTask&& Task);
    void TryWrite();

private:
    FCriticalSection WriteLock;
    FCriticalSection QueueLock;

    TArray<FImageSaveTask> Test;
    TQueue<FImageSaveTask, EQueueMode::Mpsc> Tasks;

    static TSharedPtr<IImageWrapper> GetImageWrapperForFormat(EImageFormat Format);
};
