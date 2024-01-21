

#include "../entity_helper.h"

#include "../components/is_draggable.h"
#include "../components/is_slot.h"
#include "../components/snaps_to_slot.h"

struct System {
  virtual void run_on(Entities &, float){};
  virtual void run_on(const Entities &, float) const {};

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

  virtual void debug_log_post() const {}
  virtual void debug_log_pre() const {}
  virtual void before_first() {}
};

struct DraggingSystem : System {
  const int EMPTY_ID = -1;
  const int FAKE_ID = -2;

  int active_id = EMPTY_ID;
  int hot_id = FAKE_ID;

  vec2 offset;
  bool mouse_down;

  void before_first() override { mouse_down = ext::is_mouse_down(); }

  inline void set_hot(int id) { hot_id = id; }
  inline void set_active(int id) { active_id = id; }
  inline bool is_hot(int id) { return hot_id == id; }
  inline bool is_active(int id) { return active_id == id; }
  inline bool is_active_or_hot(int id) { return is_hot(id) || is_active(id); }
  inline bool is_active_and_hot(int id) { return is_hot(id) && is_active(id); }

  void determine_active(Entity &entity) {
    const Transform &transform = entity.get<Transform>();
    bool inside = ext::is_mouse_inside(transform.rect());
    if (inside) {
      set_hot(entity.id);
      if (is_active(EMPTY_ID) && mouse_down) {
        set_active(entity.id);

        auto mouse_position = ext::get_mouse_position();
        offset = mouse_position - entity.get<Transform>().as2();
      }
    }
  }

  void move_if_dragging(Entity &entity) {
    if (is_active(entity.id)) {
      Transform &transform = entity.get<Transform>();

      auto mouse_position = ext::get_mouse_position();

      transform.update(
          {mouse_position.x - offset.x, mouse_position.y - offset.y});
    }
  }

  void snap_if_snappable() {
    auto maybe_e = EntityHelper::getEntityForID(active_id);
    if (!maybe_e)
      return;
    Entity &entity = maybe_e.asE();
    if (entity.is_missing<SnapsToSlot>())
      return;

    auto closest = EntityHelper::getClosestMatchingEntity(
        entity.get<Transform>().as2(), 1920.f, [this](const Entity &entity) {
          if (entity.is_missing<IsSlot>())
            return false;
          if (entity.get<IsSlot>().held_entity == active_id)
            return true;
          return entity.get<IsSlot>().is_empty();
        });
    if (!closest) {
      log_warn(" Could not find any empty slot to snap to");
      return;
    }

    SnapsToSlot &snaps = entity.get<SnapsToSlot>();

    // clear old parent
    auto old_parent = EntityHelper::getEntityForID(snaps.held_by);
    old_parent->get<IsSlot>().held_entity = -1;

    // write new parent
    closest->get<IsSlot>().held_entity = entity.id;
    snaps.held_by = closest->id;
    Transform &parent_transform = closest->get<Transform>();

    entity.get<Transform>().update({
        parent_transform.as2().x,
        parent_transform.as2().y,
    });
  }

  virtual void run_on(Entities &entities, float dt) override {
    set_hot(EMPTY_ID);

    for_each(entities, dt, [this](Entity &entity, float dt) {
      if (entity.is_missing<IsDraggable>())
        return;

      determine_active(entity);
      move_if_dragging(entity);
    });

    if (mouse_down) {
      if (is_active(EMPTY_ID)) {
        // this handles mouse held on empty space
        set_active(FAKE_ID);
        offset = {0, 0};
      }
    } else {
      snap_if_snappable();

      set_active(EMPTY_ID);
      offset = {0, 0};
    }
  }
};

namespace render {
inline void rect(const Entity &entity, float, raylib::Color color) {
  const Transform &transform = entity.get<Transform>();
  ext::draw_rectangle(transform.position, transform.size, color);
}

} // namespace render

struct RenderingSystem : System {
  void run_on(const Entities &entities, float dt) const {
    for_each(entities, dt, [](const Entity &entity, float dt) {
      switch (entity.type) {
      case EntityType::Unknown:
      case EntityType::x:
      case EntityType::y:
      case EntityType::z:
      case EntityType::Card:
        render::rect(entity, dt, raylib::RED);
        break;
      case EntityType::TraySlot:
        render::rect(entity, dt, raylib::BLUE);
        break;
      }
    });
  }
};

struct SystemManager {
  std::array<System *, 1> update_systems = {{
      new DraggingSystem(),
  }};

  std::array<System *, 1> render_systems = {{
      new RenderingSystem(),
  }};

  SystemManager() {}

  void on_update(Entities &entities, float dt) {
    for (auto &system : update_systems) {
      system->debug_log_pre();
      system->before_first();
      system->run_on(entities, dt);
      system->debug_log_post();
    }
  }

  void on_render(const Entities &entities, float dt) {
    for (auto &system : render_systems) {
      system->debug_log_pre();
      system->before_first();
      system->run_on(entities, dt);
      system->debug_log_post();
    }
  }
};
