//
// Created by xenon on 28.05.2021.
//

#include "GameContext.h"
#include "Game.h"

#include <iostream>

void opengl_uniform(GLuint shader, Game::Scene const& current_scene) {
  opengl_uniform(shader, "projection", current_scene.projection);
  opengl_uniform(shader, "view", current_scene.camera.matrix_view());
  opengl_uniform(shader, "light", current_scene.light, false);
}

GameContext::GameContext()
    : last_update_(std::chrono::system_clock::now())
    , terrain_(16, 4, 4)
    , character_({0, 0, 0}, vcl::rotation(),
                 vcl::mesh_drawable(
                     vcl::mesh_primitive_cylinder(0.2, {0, 0, 0}, {1, 0, 0}))) {
  std::cout << "Create GameContext()" << std::endl;
  InitDefaultParams();
}

GameContext::~GameContext() {
  std::cout << "Destroy GameContext()" << std::endl;
}

void GameContext::InitDefaultParams() {
  auto& scene                     = Game::GetInstance()->GetScene();
  scene.camera.distance_to_center = 2.5f;
  scene.camera.look_at({4, 3, 4}, {0, 0, 2}, {0, 0, 1});
}

void GameContext::Render() {
  const auto& scene = Game::GetInstance()->GetScene();
  character_.DrawOn(scene);

  ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("X: %.2f\nY: %.2f\nZ: %.2f", character_.Position().x,
              character_.Position().y, character_.Position().z);
  ImGui::Text("===============\n"
              "Collected carrots: %d",
              character_.CountCollectedCarrots());
  ImGui::End();

  terrain_.Render(character_.Position().x, character_.Position().y, scene);
}

void GameContext::GoForward() { run_speed_ = 2; }
void GameContext::GoBackward() { run_speed_ = -2; }
void GameContext::StopGo() { run_speed_ = 0; }

void GameContext::TurnLeft() { turn_speed_ = 1; }

void GameContext::TurnRight() { turn_speed_ = -1; }

void GameContext::StopTurn() { turn_speed_ = 0; }

void GameContext::Update() {
  auto now                           = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = (now - last_update_);
  last_update_                       = now;

  double dt = diff.count();

  double turn_angle = turn_speed_ * dt;
  character_.Rotation() *= vcl::rotation({0, 0, 1}, turn_angle);

  vcl::vec3 step = character_.Rotation() * vcl::vec3{run_speed_ * dt, 0, 0};
  character_.Position() += step;

  character_.Position().z = terrain_.GetPhysicalHeight(character_.Position().x,
                                                       character_.Position().y);

  terrain_.InteractWith(character_);

  auto& scene = Game::GetInstance()->GetScene();
  character_.UpdateCamera(scene.camera, terrain_);
}

void GameContext::StartJumping() {}
void GameContext::StopJumping() {}
