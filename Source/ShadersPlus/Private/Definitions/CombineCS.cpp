#include "CombineCS.h"

#define IMPLEMENT_COMBINECS(InputComponents) typedef TCombineCS<InputComponents> TCombineCS##InputComponents##;   \
    IMPLEMENT_SHADER_TYPE(template<> SHADERSPLUS_API, TCombineCS##InputComponents##, TEXT("/Plugin/ShadersPlus/Private/CombineCS.usf"), TEXT("MainCS"), SF_Compute);

IMPLEMENT_COMBINECS(1)
IMPLEMENT_COMBINECS(2)
IMPLEMENT_COMBINECS(3)
IMPLEMENT_COMBINECS(4)

#undef IMPLEMENT_COMBINECS