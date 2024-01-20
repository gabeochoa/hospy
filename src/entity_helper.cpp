#include "entity_helper.h"

#include "components/transform.h"
#include "entity_query.h"

#include <set>

Entities entities_DO_NOT_USE;

std::set<int> permanant_ids;
std::map<vec2, bool> cache_is_walkable;

///////////////////////////////////
///

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

Entities &EntityHelper::get_entities() { return entities_DO_NOT_USE; }

RefEntities EntityHelper::get_ref_entities() {
  RefEntities matching;
  for (const auto &e : EntityHelper::get_entities()) {
    if (!e)
      continue;
    matching.push_back(*e);
  }
  return matching;
}

struct CreationOptions {
  bool is_permanent;
};

Entity &EntityHelper::createEntity() {
  return createEntityWithOptions({.is_permanent = false});
}

Entity &EntityHelper::createPermanentEntity() {
  return createEntityWithOptions({.is_permanent = true});
}

Entity &EntityHelper::createEntityWithOptions(const CreationOptions &options) {
  std::shared_ptr<Entity> e(new Entity());
  get_entities().push_back(e);
  // log_info("created a new entity {}", e->id);

  if (options.is_permanent) {
    permanant_ids.insert(e->id);
  }

  return *e;
}

void EntityHelper::markIDForCleanup(int e_id) {
  auto &entities = get_entities();
  auto it = entities.begin();
  while (it != get_entities().end()) {
    if ((*it)->id == e_id) {
      (*it)->cleanup = true;
      break;
    }
    it++;
  }
}

void EntityHelper::removeEntity(int e_id) {
  auto &entities = get_entities();

  auto newend = std::remove_if(
      entities.begin(), entities.end(),
      [e_id](const auto &entity) { return !entity || entity->id == e_id; });

  entities.erase(newend, entities.end());
}

void EntityHelper::cleanup() {
  // Cleanup entities marked cleanup
  Entities &entities = get_entities();

  auto newend =
      std::remove_if(entities.begin(), entities.end(), [](const auto &entity) {
        return !entity || entity->cleanup;
      });

  entities.erase(newend, entities.end());
}

void EntityHelper::delete_all_entities_NO_REALLY_I_MEAN_ALL() {
  Entities &entities = get_entities();
  // just clear the whole thing
  entities.clear();
}

void EntityHelper::delete_all_entities(bool include_permanent) {
  if (include_permanent) {
    delete_all_entities_NO_REALLY_I_MEAN_ALL();
    return;
  }

  // Only delete non perms
  Entities &entities = get_entities();

  auto newend =
      std::remove_if(entities.begin(), entities.end(), [](const auto &entity) {
        return !permanant_ids.contains(entity->id);
      });

  entities.erase(newend, entities.end());
}

enum ForEachFlow {
  NormalFlow = 0,
  Continue = 1,
  Break = 2,
};

void EntityHelper::forEachEntity(std::function<ForEachFlow(Entity &)> cb) {
  for (const auto &e : get_entities()) {
    if (!e)
      continue;
    auto fef = cb(*e);
    if (fef == 1)
      continue;
    if (fef == 2)
      break;
  }
}

OptEntity EntityHelper::getEntityForID(int id) {
  if (id == -1)
    return {};

  for (const auto &e : get_entities()) {
    if (!e)
      continue;
    if (e->id == id)
      return *e;
  }
  return {};
}

OptEntity EntityHelper::getClosestOfType(const Entity &entity,
                                         const EntityType &_type, float range) {
  const Transform &transform = entity.get<Transform>();
  return EntityHelper::getClosestMatchingEntity(
      transform.as2(), range, [](const Entity &) {
        return true; /*check_type(entity, type);*/
      });
}

// TODO :BE: change other debugname filter guys to this
std::vector<RefEntity> EntityHelper::getAllWithType(const EntityType &type) {
  return EntityQuery().whereType(type).gen();
}

bool EntityHelper::doesAnyExistWithType(const EntityType &type) {
  return EntityQuery().whereType(type).has_values();
}

std::vector<RefEntity> EntityHelper::getFilteredEntitiesInRange(
    vec2 pos, float range, const std::function<bool(const Entity &)> &filter) {
  return EntityQuery().whereLambda(filter).whereInRange(pos, range).gen();
}

std::vector<RefEntity> EntityHelper::getEntitiesInRange(vec2 pos, float range) {
  return EntityQuery().whereInRange(pos, range).gen();
}

OptEntity EntityHelper::getClosestMatchingEntity(
    vec2 pos, float range, const std::function<bool(const Entity &)> &filter) {
  // TODO We dont currently support order by
  //      and even if we did it would be two full loops
  /*
  EntityQuery()
  .whereLambda(filter)
  .whereInRange(pos, range)
  .orderBy(Distance)
  .gen_first()
  ;
  */

  float best_distance = range;
  OptEntity best_so_far = {};
  for (auto &e : get_entities()) {
    if (!e)
      continue;
    if (!filter(*e))
      continue;
    float d = vec::distance(pos, e->get<Transform>().as2());
    if (d > range)
      continue;
    if (d < best_distance) {
      best_so_far = *e;
      best_distance = d;
    }
  }
  return best_so_far;
}

bool EntityHelper::hasOverlappingSolidEntitiesInRange(vec2 range_min,
                                                      vec2 range_max) {
  OptEntity e =
      EntityHelper::getOverlappingSolidEntityInRange(range_min, range_max);
  return e.valid();
}

RefEntities EntityHelper::getAllInRangeFiltered(
    vec2 range_min, vec2 range_max,
    const std::function<bool(const Entity &)> &filter) {
  return EntityQuery()
      .whereInside(range_min, range_max)
      .whereLambda(filter)
      .gen();
}

RefEntities EntityHelper::getAllInRange(vec2 range_min, vec2 range_max) {
  return EntityQuery().whereInside(range_min, range_max).gen();
}

// TODO :EQ_CPP: We cant expose this function direction because
// EntityQuery cant be including in entity helper du eot circular
// need to move into a cpp
EntityQuery &getOverlappingSolidEntityInRangeQuery(
    vec2 range_min, vec2 range_max,
    const std::function<bool(const Entity &)> &filter) {
  return EntityQuery() //
                       //// TODO
      // .whereHasComponent<IsSolid>()      //
      .whereInside(range_min, range_max) //
      .whereLambdaExistsAndTrue(filter)  //
      .whereLambda([&](const Entity &entity) -> bool {
        return EntityQuery()       //
            .whereNotID(entity.id) //
                                   // TODO
            // .whereHasComponent<IsSolid>()      //
            .whereInside(range_min, range_max) //
            .wherePositionMatches(entity)      //
            .first()                           //
            .has_values();
      });
}

OptEntity EntityHelper::getOverlappingSolidEntityInRange(
    vec2 range_min, vec2 range_max,
    const std::function<bool(const Entity &)> &filter) {
  return getOverlappingSolidEntityInRangeQuery(range_min, range_max, filter)
      .gen_first();
}

OptEntity EntityHelper::getOverlappingEntityIfExists(
    const Entity &entity, float range,
    const std::function<bool(const Entity &)> &filter) {
  const vec2 position = entity.get<Transform>().as2();
  return EntityQuery()                  //
      .whereNotID(entity.id)            //
      .whereLambdaExistsAndTrue(filter) //
                                        //// TODO
      // .whereHasComponent<IsSolid>()     //
      .whereInRange(position, range) //
      .wherePositionMatches(entity)  //
      .gen_first();
}

#pragma clang diagnostic pop
