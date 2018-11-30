#pragma once

// WIP/Redudant
template <typename TResource>
struct FShaderResourceTuple
{
public:
    inline TRefCountPtr<TResource> GetResource() { return Resource; }
    inline FShaderResourceViewRHIRef GetSRV() { return SRV; }
    inline FUnorderedAccessViewRHIRef GetUAV() { return UAV; }

private:
    TRefCountPtr<TResource> Resource;
    FShaderResourceViewRHIRef SRV;
    FUnorderedAccessViewRHIRef UAV;
};