
#include "engine/globals.h"
#include "vendor_include.h"
//

#include "components/is_draggable.h"
#include "components/transform.h"
#include "entity.h"
#include "entity_helper.h"

//
#include "system/system.h"

int LOG_LEVEL = (int)LogLevel::INFO;

void make_entity(Entity &entity, vec2 pos, vec2 size) {
  entity.addComponent<Transform>().init(pos, size);
}

void make_card(Entity &entity, vec2 pos, vec2 size) {
  make_entity(entity, pos, size);
  entity.addComponent<IsDraggable>();
}

Entity &make_entity(EntityType etype, vec2 pos, vec2 size) {
  Entity &e = EntityHelper::createEntity();
  switch (etype) {
  case EntityType::Unknown:
  case EntityType::x:
  case EntityType::y:
  case EntityType::z:
    make_entity(e, pos, size);
    break;
  case EntityType::Card:
    make_card(e, pos, size);
    break;
  }
  return e;
}

using namespace raylib;

SystemManager system_manager;

void update() {
  float dt = raylib::GetFrameTime();
  auto &entities = EntityHelper::get_entities();
  system_manager.on_update(entities, dt);
}

void draw() {
  float dt = raylib::GetFrameTime();

  const auto &entities = EntityHelper::get_entities();
  BeginDrawing();
  ext::clear_background(RAYWHITE);
  ext::draw_fps(20, 20);
  DrawText(fmt::format("entities: {}", entities.size()).c_str(), 20, 50, 20,
           DARKGRAY);
  system_manager.on_render(entities, dt);
  EndDrawing();
}

int main(int, char **) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  // This goes above Init since that loves to spew errors
  SetTraceLogLevel(LOG_ERROR);

  InitWindow(screenWidth, screenHeight,
             "raylib [shapes] example - basic shapes drawing");
  SetTraceLogLevel(LOG_LEVEL);

  SetTargetFPS(240); // Set our game to run at 60 frames-per-second
                     //

  make_entity(EntityType::Card, {200, 20}, {200, 80});
  make_entity(EntityType::Card, {200, 200}, {200, 80});

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    update();
    draw();
  }

  CloseWindow();

  return 0;
}
