#include "context.hpp"
#include "ecs/dual.h"
#include "type_registry.hpp"

namespace dual
{
    type_registry_t& type_registry_t::get()
    {
        return dual_get_context()->typeRegistry;
    }
    pool_t& get_default_pool()
    {
        return dual_get_context()->normalPool;
    }
    pool_t& get_default_pool_small()
    {
        return dual_get_context()->smallPool;
    }
    pool_t& get_default_pool_large()
    {
        return dual_get_context()->largePool;
    }
    scheduler_t& scheduler_t::get()
    {
        return dual_get_context()->scheduler;
    }
    std::string& get_error()
    {
        return dual_get_context()->error;
    }
}

dual_context_t::dual_context_t()
    :normalPool(dual::kFastBinSize, dual::kFastBinCapacity),
    largePool(dual::kLargeBinSize, dual::kLargeBinCapacity),
    smallPool(dual::kSmallBinSize, dual::kSmallBinCapacity),
    typeRegistry(smallPool),
    scheduler()
{
}

extern "C"
{
    dual_context_t* dual_initialize()
    {
        return new dual_context_t;
    }

    void dual_shutdown()
    {
        delete dual_get_context();
    }
}