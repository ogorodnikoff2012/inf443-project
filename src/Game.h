//
// Created by xenon on 28.05.2021.
//

// #ifndef INF443_GAME_H
// #define INF443_GAME_H

#pragma once

#include "GameState.h"

#include "vcl/vcl.hpp"

#include <memory>

class Game {
 public:
  struct Scene {
    vcl::camera_around_center camera;
    vcl::mat4 projection;
    vcl::mat4 projection_inverse;
    vcl::vec3 light;
  };

  Game(int width, int height);
  ~Game();

  static Game* GetInstance();

  void run();

  void PushState(EGameState state);
  void PopState();
  EGameState GetState() const;
  const std::shared_ptr<SceneContext>& GetContext() const;
  Scene& GetScene();

 private:
  struct StateAndContext {
    EGameState state;
    std::shared_ptr<SceneContext> context;
  };

  static Game* instance;

  GLFWwindow* window_;
  std::vector<StateAndContext> state_stack_{};
  Scene scene_;

  void RenderScene();
  void RenderMainScene();
  void RenderGameScene();
  void RenderPauseScene();
  void RenderScoreScene();

  static StateCallbacks GetStateCallbacks(EGameState state);
  void UpdateCallbacks(const StateCallbacks& callbacks);

  void InitVCL();
};

// #endif //INF443_GAME_H
