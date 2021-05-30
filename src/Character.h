//
// Created by xenon on 29.05.2021.
//

// #ifndef INF443_CHARACTER_H
// #define INF443_CHARACTER_H

#pragma once

#include "Animation.h"
#include "vcl/vcl.hpp"

#include <chrono>

class Terrain;

class Character {
 public:
  Character(const vcl::vec3& position, const vcl::rotation& rotation,
            const vcl::hierarchy_mesh_drawable& mesh);

  vcl::vec3& Position();
  vcl::rotation& Rotation();
  vcl::hierarchy_mesh_drawable& Mesh();

  void UpdateCamera(vcl::camera_around_center& camera, Terrain& terrain) const;
  void SwitchView();

  template <class SCENE>
  void DrawOn(const SCENE& scene) const {
    static const vcl::vec3 kCenterRebase{0, 0, 1.5};
    static const vcl::vec3 kRightLegRotationPoint{-0.023229, -0.327475,
                                                  -0.875475};
    static const vcl::vec3 kLeftLegRotationPoint{-0.023229, 0.327475,
                                                 -0.875475};
    static const vcl::vec3 kRightArmRotationPoint{0.034254, -0.871169,
                                                  -0.157259};
    static const vcl::vec3 kLeftArmRotationPoint{0.034254, 0.871169, -0.157259};
    static const vcl::vec3 kRightEarRotationPoint{0.001453, -0.215198,
                                                  0.897595};
    static const vcl::vec3 kLeftEarRotationPoint{0.001453, 0.215198, 0.897595};
    static const float kScale = 0.33;

    mesh_["body"].transform.translate = position_ + kCenterRebase * kScale;
    mesh_["body"].transform.rotate    = rotation_;
    mesh_["body"].transform.scale     = kScale;

    double leg_angle       = jump_animation_["legs"].Value();
    double squat_amplitude = jump_animation_["squat"].Value() * 0.1;
    double fly             = jump_animation_["fly"].Value();

    mesh_["body"].transform.translate.z += fly - squat_amplitude;

    mesh_["right_leg"].transform =
        vcl::affine_rts(vcl::rotation(),
                        vcl::vec3{0, 0, squat_amplitude - 0.1 * fly}, 1) *
        vcl::affine_rts(vcl::rotation_around_center(
            vcl::rotation({0, 1, 0}, leg_angle), kRightLegRotationPoint));
    mesh_["left_leg"].transform =
        vcl::affine_rts(vcl::rotation(),
                        vcl::vec3{0, 0, squat_amplitude - 0.1 * fly}, 1) *
        vcl::affine_rts(vcl::rotation_around_center(
            vcl::rotation({0, 1, 0}, leg_angle), kLeftLegRotationPoint));

    double arm_angle             = -jump_animation_["arms"].Value();
    mesh_["right_arm"].transform = vcl::affine_rts(vcl::rotation_around_center(
        vcl::rotation({0, 1, 0}, arm_angle), kRightArmRotationPoint));
    mesh_["left_arm"].transform  = vcl::affine_rts(vcl::rotation_around_center(
        vcl::rotation({0, 1, 0}, arm_angle), kLeftArmRotationPoint));

    double ear_angle             = jump_animation_["ears"].Value();
    mesh_["right_ear"].transform = vcl::affine_rts(vcl::rotation_around_center(
        vcl::rotation({0, 1, 0}, ear_angle), kRightEarRotationPoint));
    mesh_["left_ear"].transform  = vcl::affine_rts(vcl::rotation_around_center(
        vcl::rotation({0, 1, 0}, ear_angle), kLeftEarRotationPoint));

    mesh_.update_local_to_global_coordinates();

    vcl::draw(mesh_, scene);
  }

  void OnCarrotCollect();
  int CountCollectedCarrots() const;

  void StartJumping();
  void StopJumping();
  void
  UpdateJumping(const std::chrono::time_point<std::chrono::system_clock>& now);

 private:
  vcl::vec3 position_;
  vcl::rotation rotation_;
  mutable vcl::hierarchy_mesh_drawable mesh_;
  bool view_from_back_{true};

  Animation jump_animation_;

  int collected_carrots_{0};

  void SetupJumpAnimation();
};

// #endif //INF443_CHARACTER_H
