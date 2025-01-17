#pragma once
#include "SkrContainers/ring_buffer.hpp"
#include "SkrRT/misc/types.h"
#include "SkrCore/time.h"
#include "SkrRT/async/fib_task.hpp"
#include "SkrRT/ecs/sugoi.h"
#include "SkrContainers/span.hpp"
#include "SkrContainers/stl_function.hpp"
#include "SkrContainers/hashmap.hpp"
#include "SkrContainers/vector.hpp"
#include "SkrBase/types.h"
#include "SkrBase/types.h"
#include "SkrBase/config.h"

// override the default serialization of sugoi_entity_t to use a packed version
struct packed_entity_t {
    sugoi_entity_t entity;
    packed_entity_t() = default;
    packed_entity_t(sugoi_entity_t e)
        : entity(e)
    {
    }
    operator sugoi_entity_t() const { return entity; }
    packed_entity_t operator=(sugoi_entity_t e)
    {
        entity = e;
        return *this;
    }
    bool operator==(sugoi_entity_t e) const { return entity == e; }
};
namespace skr
{
namespace binary
{
template <>
struct WriteTrait<packed_entity_t> {
    static int Write(skr_binary_writer_t* writer, const packed_entity_t& value, sugoi_entity_t maxEntity);
};
template <>
struct ReadTrait<packed_entity_t> {
    static int Read(skr_binary_reader_t* reader, packed_entity_t& value, sugoi_entity_t maxEntity);
};
BLOB_POD(packed_entity_t)
} // namespace binary
} // namespace skr

namespace skr
{
namespace json
{
template <>
struct WriteTrait<packed_entity_t> {
    static void Write(skr_json_writer_t* writer, const packed_entity_t& value);
};
} // namespace json
} // namespace skr

#ifndef __meta__
    #include "MPShared/world_delta.generated.h" // IWYU pragma: export
#endif

using NetEntityId    = uint16_t;
using NetComponentId = uint8_t;

sreflect_struct("guid": "8F20F35F-38D6-4D2F-AB41-24E582F2378B") 
sattr("blob" : true)
sattr("debug" : true)
MPEntityCreationView {
    NetEntityId               entity;
    skr::span<NetComponentId> components;
    skr_guid_t                prefab;
};

GENERATED_BLOB_BUILDER(MPEntityCreationView)

sreflect_struct("guid": "6DEDEB26-1A03-4194-95EF-0E589366C985") 
sattr("blob" : true)
sattr("debug" : true)
MPEntityDeltaView {
    NetEntityId               entity;
    skr::span<NetComponentId> components;
    skr::span<NetComponentId> deleted;
};
GENERATED_BLOB_BUILDER(MPEntityDeltaView)

sreflect_struct("guid": "B2A257E7-E153-485E-A4B5-5D0B2EC65E42")  
sattr("blob" : true)
sattr("debug" : true)
sattr("serialize_config" : "uint16_t entityCount")
MPComponentDeltaView {
    NetComponentId         type;
    sattr("serialize_config" : "SpanSerdeConfig{entityCount}, IntegerPackConfig<NetEntityId>{0, entityCount}")
    skr::span<NetEntityId> entities;
    sattr("no-text" : true)
    skr::span<uint8_t>     data;
};
GENERATED_BLOB_BUILDER(MPComponentDeltaView)

sreflect_struct("guid": "0E7D9309-13EF-4EB8-9E8E-2DDE8D8F7BA0") 
sattr("blob" : true)
sattr("debug" : true)
MPWorldDeltaView {
    sugoi_entity_t                   maxEntity;
    sattr("serialize_config" : "record.maxEntity")
    skr::span<packed_entity_t>      entities;
    sattr("serialize_config" : "(uint16_t)record.entities.size()")
    skr::span<MPComponentDeltaView> components;
    sattr("serialize_config" : "SpanSerdeConfig{(uint32_t)record.entities.size()}")
    skr::span<MPEntityCreationView> created;
    sattr("serialize_config" : "SpanSerdeConfig{(uint32_t)record.entities.size()}")
    skr::span<MPEntityDeltaView>    changed;
    sattr("serialize_config" : "SpanSerdeConfig{(uint32_t)record.entities.size()}")
    skr::span<NetEntityId>          dead;
};
GENERATED_BLOB_BUILDER(MPWorldDeltaView)

sreflect_struct("guid": "54C41BFD-130C-4AF6-AD50-F43D5C6E8AE5")
sattr("serialize" : "bin")
MPWorldDelta {
    uint64_t         frame;
    skr_blob_arena_t arena;
    spush_attr("arena" : "arena")
    MPWorldDeltaView blob;
};

struct MPWorldDeltaBuildContext {
    uint32_t          connectionId;
    uint32_t          totalConnections;
    sugoi_type_index_t historyComponent;
};

using entity_map_t = skr::FlatHashMap<sugoi_entity_t, sugoi_entity_t>;

using component_delta_build_callback_t = skr::task::event_t (*)(sugoi_type_index_t type, sugoi_query_t* query, MPWorldDeltaBuildContext ctx, MPWorldDeltaViewBuilder& builder);
using component_delta_apply_callback_t = skr::task::event_t (*)(sugoi_type_index_t type, sugoi_query_t* query, const MPWorldDeltaView& delta, const entity_map_t& map);

struct IWorldDeltaBuilder {
    virtual ~IWorldDeltaBuilder()                                             = default;
    virtual void Initialize(sugoi_storage_t* storage)                          = 0;
    virtual void GenerateDelta(skr::Vector<MPWorldDeltaViewBuilder>& builder) = 0;
};

MP_SHARED_API IWorldDeltaBuilder* CreateWorldDeltaBuilder();
void                              RegisterComponentDeltaBuilder(sugoi_type_index_t component, component_delta_build_callback_t inCallback, sugoi_type_index_t historyComponent = sugoi::kInvalidTypeIndex);

struct IWorldDeltaApplier {
    using SpawnPrefab_t                                                                                          = skr::stl_function<sugoi_entity_t(sugoi_storage_t*, sugoi_entity_t entity, skr_guid_t prefab, sugoi_entity_type_t* type)>;
    using DestroyEntity_t                                                                                        = skr::stl_function<void(sugoi_storage_t*, sugoi_entity_t entity)>;
    virtual ~IWorldDeltaApplier()                                                                                = default;
    virtual void   Initialize(sugoi_storage_t* storage, SpawnPrefab_t spawnPrefab, DestroyEntity_t destroyPrefab) = 0;
    virtual void   ApplyDelta(const MPWorldDeltaView& delta, entity_map_t& map)                                  = 0;
    virtual double GetBandwidthOf(sugoi_type_index_t component)                                                   = 0;
};

MP_SHARED_API IWorldDeltaApplier* CreateWorldDeltaApplier();
void                              RegisterComponentDeltaApplier(sugoi_type_index_t component, component_delta_apply_callback_t inCallback);

struct BandwidthCounter {
    BandwidthCounter();
    skr::InlineRingBuffer<std::pair<double, double>, 30> dataRecord;
    SHiresTimer                                          timer;
    void                                                 AddRecord(double bytes);
    double                                               GetBytePerSecond();
};
