//
// Created by xenon on 29.05.2021.
//

#include "Character.h"
#include "Terrain.h"

Character::Character(const vcl::vec3& position, const vcl::rotation& rotation,
                     const vcl::hierarchy_mesh_drawable& mesh)
    : position_(position)
    , rotation_(rotation)
    , mesh_(mesh) {
  SetupJumpAnimation();
  StopJumping();
}

vcl::vec3& Character::Position() { return position_; }
vcl::rotation& Character::Rotation() { return rotation_; }
vcl::hierarchy_mesh_drawable& Character::Mesh() { return mesh_; }

void Character::UpdateCamera(vcl::camera_around_center& camera,
                             Terrain& terrain) const {
  vcl::vec3 look_from =
      position_ + rotation_ * vcl::vec3{view_from_back_ ? -5 : 5, 0, 0};
  float z     = terrain.GetPhysicalHeight(look_from.x, look_from.y);
  look_from.z = std::max(std::max(z, look_from.z), 2.2f) + 2;

  camera.look_at(look_from, position_, {0, 0, 1});
}

void Character::OnCarrotCollect() { ++collected_carrots_; }
int Character::CountCollectedCarrots() const { return collected_carrots_; }
void Character::SwitchView() { view_from_back_ = !view_from_back_; }

void Character::StartJumping() { jump_animation_.StartLoop(); }

void Character::StopJumping() { jump_animation_.BreakLoop(); }

void Character::UpdateJumping(
    const std::chrono::time_point<std::chrono::system_clock>& now) {
  jump_animation_.Update(now);
}
void Character::SetupJumpAnimation() {
  jump_animation_.NewTimeline("arms")
      .AddKeyPoint(0, 0, Timeline::EInterpolationModeLinear)
      .AddKeyPoint(0.05, M_PI / 3, Timeline::EInterpolationModeLinear)
      .AddKeyPoint(0.15, -M_PI / 3, Timeline::EInterpolationModeLinear)
      .AddKeyPoint(0.95, -M_PI / 3, Timeline::EInterpolationModeLeft)
      .AddKeyPoint(1, 0, Timeline::EInterpolationModeLinear)
      .Reset();

  jump_animation_.NewTimeline("legs")
      .AddKeyPoint(0, 0, Timeline::EInterpolationModeLeft)
      .AddKeyPoint(0.1, 0, Timeline::EInterpolationModeLeft)
      .AddKeyPoint(0.15, M_PI / 4, Timeline::EInterpolationModeLinear)
      .AddKeyPoint(0.95, M_PI / 4, Timeline::EInterpolationModeLeft)
      .AddKeyPoint(1, 0, Timeline::EInterpolationModeLinear)
      .Reset();

  jump_animation_.NewTimeline("squat")
      .AddKeyPoint(0, 0, Timeline::EInterpolationModeLeft)
      .AddKeyPoint(0.05, 1, Timeline::EInterpolationModeLinear)
      .AddKeyPoint(0.1, 0, Timeline::EInterpolationModeLinear)
      .AddKeyPoint(1, 0, Timeline::EInterpolationModeLeft)
      .Reset();

  jump_animation_.NewTimeline("fly")
      .AddKeyPoint(0, 0, Timeline::EInterpolationModeLeft)
      .AddKeyPoint(0.1, 0, Timeline::EInterpolationModeLeft)
      .AddKeyPoint(0.55, 1, Timeline::EInterpolationModeOutQuad)
      .AddKeyPoint(1, 0, Timeline::EInterpolationModeInQuad)
      .Reset();

  jump_animation_.NewTimeline("ears")
      .AddKeyPoint(0.1, 0, Timeline::EInterpolationModeLeft)
      .AddKeyPoint(0.15, M_PI / 4, Timeline::EInterpolationModeLinear)
      .AddKeyPoint(0.55, M_PI / 4, Timeline::EInterpolationModeLeft)
      .AddKeyPoint(0.65, 0, Timeline::EInterpolationModeLinear)
      .AddKeyPoint(1, 0, Timeline::EInterpolationModeLeft)
      .AddKeyPoint(1.05, M_PI / 4, Timeline::EInterpolationModeLinear)
      .AddKeyPoint(1.1, 0, Timeline::EInterpolationModeLinear)
      .Reset();
}
