
#include "engine/globals.h"
#include "vendor_include.h"
//

#include "components/transform.h"
#include "entity.h"
#include "entity_helper.h"

//
#include "system/system.h"

int LOG_LEVEL = (int)LogLevel::INFO;

void make_entity(Entity &entity, vec3 pos, vec2 size) {
  entity.addComponent<Transform>().init(pos, size);
}

Entity &get_new_entity(vec2 pos, vec2 size) {
  Entity &e = EntityHelper::createEntity();
  make_entity(e, vec::to3(pos), size);
  return e;
}

using namespace raylib;

RenderingSystem rendering_system;

void update() {}

void draw() {
  const auto &entities = EntityHelper::get_entities();
  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawText(fmt::format("entities: {}", entities.size()).c_str(), 20, 20, 20,
           DARKGRAY);
  rendering_system.run_on(entities, 0);
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

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
                    //

  Entity &hi = get_new_entity({200, 20}, {200, 80});

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    update();
    draw();
  }

  CloseWindow();

  return 0;
}
