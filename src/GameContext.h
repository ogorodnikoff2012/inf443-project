//
// Created by xenon on 28.05.2021.
//

// #ifndef INF443_GAMECONTEXT_H
// #define INF443_GAMECONTEXT_H

#pragma once

#include "SceneContext.h"

#include "Character.h"
#include "Terrain.h"
#include "vcl/vcl.hpp"

#include <chrono>

class GameContext : public SceneContext {
 public:
  GameContext();
  ~GameContext() override;

  void Update();
  void Render();

  void GoForward();
  void GoBackward();
  void StopGo();

  void TurnLeft();
  void TurnRight();
  void StopTurn();

  void StartJumping();
  void StopJumping();

 private:
  vcl::camera_around_center
      camera_; // A camera looking at, and rotating around, a
  // specific center position
  vcl::vec3 light_; // Position of the light in the scene

  float turn_speed_{0};
  float run_speed_{0};

  std::chrono::time_point<std::chrono::system_clock> last_update_;

  Terrain terrain_;
  Character character_;

  void InitDefaultParams();
};

// #endif //INF443_GAMECONTEXT_H
