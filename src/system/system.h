

#include "../entity_helper.h"

#include "../components/is_draggable.h"

struct System {
  void run_on(RefEntities entities, float dt);
  void run_on(const Entities &entities, float dt) const;

  void for_each(Entities &entities, float dt,
                const std::function<void(Entity &, float)> &cb) {
    std::ranges::for_each(entities, [cb, dt](std::shared_ptr<Entity> entity) {
      if (!entity)
        return;
      cb(*entity, dt);
    });
  }

  void for_each(const Entities &entities, float dt,
                const std::function<void(const Entity &, float)> &cb) const {
    std::ranges::for_each(std::as_const(entities),
                          [cb, dt](std::shared_ptr<Entity> entity) {
                            if (!entity)
                              return;
                            cb(*entity, dt);
                          });
  }

  void before_first() {}
};

struct DraggingSystem : System {
  int active_entity = -1;
  vec2 offset;

  bool mouse_down;

  void before_first() { mouse_down = ext::is_mouse_down(); }

  [[nodiscard]] bool has_active() const { return active_entity != -1; }
  [[nodiscard]] bool doesnt_have_active() const { return !has_active(); }
  [[nodiscard]] bool is_active(int i) const { return active_entity == i; }
  [[nodiscard]] bool is_not_active(int i) const { return !is_active(i); }

  void unmark_if_no_longer_dragging(Entity &entity) {
    // nothing to unmark
    if (doesnt_have_active())
      return;
    // We are dragging so dont run this function
    if (mouse_down)
      return;

    IsDraggable &isdrag = entity.get<IsDraggable>();
    if (!isdrag.is_selected)
      return;

    // keep selected as long as mouse is still inside
    isdrag.is_selected = ext::is_mouse_inside(entity.get<Transform>().rect());
    active_entity = isdrag.is_selected ? entity.id : -1;
  }

  void mark_if_dragging(Entity &entity) {
    // Cant have two active drags
    if (has_active())
      return;
    // not dragging
    if (!mouse_down)
      return;

    active_entity = entity.id;
    entity.get<IsDraggable>().is_selected = true;

    auto mouse_position = ext::get_mouse_position();
    offset = mouse_position - entity.get<Transform>().as2();
  }

  void move_if_dragging(Entity &entity) {
    if (doesnt_have_active())
      return;
    // not us
    if (is_not_active(entity.id))
      return;

    Transform &transform = entity.get<Transform>();

    auto mouse_position = ext::get_mouse_position();

    transform.update(vec3{mouse_position.x - offset.x, transform.position.y,
                          mouse_position.y - offset.y});
  }

  void run_on(Entities entities, float dt) {
    for_each(entities, dt, [this](Entity &entity, float dt) {
      if (entity.is_missing<IsDraggable>())
        return;
      // TODO add the debug system to system
      // log_info("active {} entity {}", active_entity, entity.id);

      unmark_if_no_longer_dragging(entity);
      mark_if_dragging(entity);
      move_if_dragging(entity);
    });
  }

  void run_on(const Entities &, float) const {}
};

namespace render {
inline void rect(const Entity &entity, float dt) {
  const Transform &transform = entity.get<Transform>();
  vec2 pos = transform.as2();
  vec2 size = transform.size;

  ext::draw_rectangle(pos, size, raylib::RED);
}

} // namespace render

struct RenderingSystem : System {
  void run_on(RefEntities, float) {}

  void run_on(const Entities &entities, float dt) const {
    for_each(entities, dt,
             [](const Entity &entity, float dt) { render::rect(entity, dt); });
  }
};
