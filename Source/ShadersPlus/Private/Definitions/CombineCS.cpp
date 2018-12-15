#include "CombineCS.h"

#define IMPLEMENT_PERMUTATION(InputComponents) typedef TCombineCS<InputComponents> TCombineCS##InputComponents##;   \
    IMPLEMENT_SHADER_TYPE(template<> SHADERSPLUS_API, TCombineCS##InputComponents##, TEXT("/Plugin/ShadersPlus/Private/CombineCS.usf"), TEXT("MainCS"), SF_Compute);

IMPLEMENT_PERMUTATION(1)
IMPLEMENT_PERMUTATION(2)
IMPLEMENT_PERMUTATION(3)
IMPLEMENT_PERMUTATION(4)

#undef IMPLEMENT_PERMUTATION