//
// Created by xenon on 28.05.2021.
//

#ifndef INF443_GAMESTATE_H
#define INF443_GAMESTATE_H

#include "SceneContext.h"

#include "vcl/vcl.hpp"

enum EGameState {
  EGameStateMain,
  EGameStateGame,
  EGameStatePause,
  EGameStateScores
};

struct StateCallbacks {
  using MouseMoveCallback = void (*)(GLFWwindow*, double, double);
  using KeyCallback       = void (*)(GLFWwindow*, int, int, int, int);
  using ContextBuilder    = SceneContext* (*)();

  MouseMoveCallback OnMouseMove;
  KeyCallback OnKey;
  ContextBuilder BuildContext;
};

#endif // INF443_GAMESTATE_H
