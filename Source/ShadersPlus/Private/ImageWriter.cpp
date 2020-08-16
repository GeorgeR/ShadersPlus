#include "ImageWriter.h"

#include "Misc/ScopeLock.h"
#include "Async/Async.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapperModule.h"

void FImageWriter::Enqueue(FImageSaveTask&& Task)
{
    //FScopeLock WriteLock(&QueueLock);

    //Test.Add(Task);
    Tasks.Enqueue(Task);
    TryWrite();
}

void FImageWriter::TryWrite()
{
    FScopeLock Lock(&WriteLock);

    if (Tasks.IsEmpty())
        return;

    while (!Tasks.IsEmpty())
    {
        FImageSaveTask Task;
        Tasks.Dequeue(Task);
        if (!Task.IsValid())
            continue;

        AsyncTask(ENamedThreads::GameThread, [Task = MoveTemp(Task)]{
            const void* RawPtr = nullptr;
            int64 SizeInBytes = 0;

            if (Task.Data->GetRawData(RawPtr, SizeInBytes))
            {
                uint8 BitDepth = Task.Data->GetBitDepth();
                FIntPoint Size = Task.Data->GetSize();
                ERGBFormat PixelLayout = Task.Data->GetPixelLayout();

                auto ImageWrapper = FImageWriter::GetImageWrapperForFormat(EImageFormat::EXR);
                ImageWrapper->SetRaw(RawPtr, SizeInBytes, Size.X, Size.Y, PixelLayout, BitDepth);
                FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(), *Task.FilePath);
            }
        });
    }
}

TSharedPtr<IImageWrapper> FImageWriter::GetImageWrapperForFormat(EImageFormat Format)
{
    auto ImageWrapperModule = FModuleManager::GetModulePtr<IImageWrapperModule>(TEXT("ImageWrapper"));
    return ImageWrapperModule->CreateImageWrapper(Format);
}
