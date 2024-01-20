
#pragma once

#include <map>
#include <set>
#include <thread>

#include "assert.h"
#include "vendor_include.h"

//
#include "components/transform.h"
#include "entity.h"

using Entities = std::vector<std::shared_ptr<Entity>>;
using RefEntities = std::vector<RefEntity>;

extern Entities client_entities_DO_NOT_USE;

extern std::set<int> permanant_ids;

struct EntityHelper {
  struct CreationOptions {
    bool is_permanent;
  };

  static Entities &get_entities();
  static RefEntities get_ref_entities();

  static Entity &createEntity();
  static Entity &createPermanentEntity();
  static Entity &createEntityWithOptions(const CreationOptions &options);

  static void markIDForCleanup(int e_id);
  static void removeEntity(int e_id);
  static void cleanup();
  static void delete_all_entities_NO_REALLY_I_MEAN_ALL();
  static void delete_all_entities(bool include_permanent = false);

  enum ForEachFlow {
    NormalFlow = 0,
    Continue = 1,
    Break = 2,
  };

  static void forEachEntity(std::function<ForEachFlow(Entity &)> cb);

  static std::vector<RefEntity>
  getFilteredEntitiesInRange(vec2 pos, float range,
                             const std::function<bool(const Entity &)> &filter);

  static std::vector<RefEntity> getEntitiesInRange(vec2 pos, float range);

  static std::vector<RefEntity> getEntitiesInPosition(vec2 pos) {
    return getEntitiesInRange(pos, 1);
  }

  // TODO exists as a conversion for things that need shared_ptr right now
  static std::shared_ptr<Entity> getEntityAsSharedPtr(OptEntity entity) {
    if (!entity)
      return {};
    for (std::shared_ptr<Entity> current_entity : get_entities()) {
      if (entity->id == current_entity->id)
        return current_entity;
    }
    return {};
  }

  static OptEntity getClosestMatchingFurniture(
      const Transform &transform, float range,
      const std::function<bool(const Entity &)> &filter);

  static OptEntity getEntityForID(int id);

  static OptEntity getClosestOfType(const Entity &entity,
                                    const EntityType &type,
                                    float range = 100.f);

  // TODO :BE: change other debugname filter guys to this

  static bool doesAnyExistWithType(const EntityType &type);

  static OptEntity
  getClosestMatchingEntity(vec2 pos, float range,
                           const std::function<bool(const Entity &)> &filter);

  template <typename T>
  static OptEntity getClosestWithComponent(const Entity &entity, float range) {
    const Transform &transform = entity.get<Transform>();
    return EntityHelper::getClosestMatchingEntity(
        transform.as2(), range,
        [](const Entity &entity) { return entity.has<T>(); });
  }

  static RefEntities getAllInRange(vec2 range_min, vec2 range_max);
  static RefEntities
  getAllInRangeFiltered(vec2 range_min, vec2 range_max,
                        const std::function<bool(const Entity &)> &filter);

  static OptEntity getOverlappingSolidEntityInRange(
      vec2 range_min, vec2 range_max,
      const std::function<bool(const Entity &)> & = {});

  static bool hasOverlappingSolidEntitiesInRange(vec2 range_min,
                                                 vec2 range_max);

  static OptEntity getOverlappingEntityIfExists(
      const Entity &entity, float range,
      const std::function<bool(const Entity &)> &filter = {});

private:
  template <typename T> static std::vector<RefEntity> getAllWithComponent() {
    std::vector<RefEntity> matching;
    for (const auto &e : get_entities()) {
      if (!e)
        continue;
      if (e->has<T>())
        matching.push_back(*e);
    }
    return matching;
  }

  template <typename T> static OptEntity getFirstWithComponent() {
    for (const auto &e : get_entities()) {
      if (!e)
        continue;
      if (e->has<T>())
        return *e;
    }
    return {};
  }

  static std::vector<RefEntity> getAllWithType(const EntityType &type);
};
