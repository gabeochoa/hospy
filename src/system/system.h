

#include "../entity_helper.h"

struct System {
  void run_on(RefEntities entities, float dt);
  void run_on(const Entities &entities, float dt) const;

  void for_each(const Entities &entities, float dt,
                const std::function<void(const Entity &, float)> &cb) const {
    std::ranges::for_each(std::as_const(entities),
                          [cb, dt](std::shared_ptr<Entity> entity) {
                            if (!entity)
                              return;
                            cb(*entity, dt);
                          });
  }
};

namespace render {
inline void rect(const Entity &entity, float dt) {
  const Transform &transform = entity.get<Transform>();
  vec2 pos = transform.as2();
  vec2 size = transform.size;

  DrawRectangle(pos.x, pos.y, size.x, size.y, raylib::RED);
}

} // namespace render

struct RenderingSystem : System {
  void run_on(RefEntities, float) {}

  void run_on(const Entities &entities, float dt) const {
    for_each(entities, dt,
             [](const Entity &entity, float dt) { render::rect(entity, dt); });
  }
};
