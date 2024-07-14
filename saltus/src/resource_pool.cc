#include "saltus/resource_pool.hh"

namespace saltus
{
    ResourcePoolInUseError::ResourcePoolInUseError():
        std::runtime_error("Resource pool has handle still in use")
    {
        
    }
} // namespace saltus
