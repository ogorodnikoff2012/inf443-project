//
// Created by xenon on 29.05.2021.
//

// #ifndef INF443_CHARACTER_H
// #define INF443_CHARACTER_H

#pragma once

#include "vcl/vcl.hpp"

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
    static const float kScale = 0.33;

    mesh_["body"].transform.translate = position_ + kCenterRebase * kScale;
    mesh_["body"].transform.rotate    = rotation_;
    mesh_["body"].transform.scale     = kScale;
    mesh_.update_local_to_global_coordinates();

    vcl::draw(mesh_, scene);
  }

  void OnCarrotCollect();
  int CountCollectedCarrots() const;

  void UpdateJumping(double dt, bool continue_jumping);

 private:
  vcl::vec3 position_;
  vcl::rotation rotation_;
  mutable vcl::hierarchy_mesh_drawable mesh_;
  bool view_from_back_{true};

  int collected_carrots_{0};
};

// #endif //INF443_CHARACTER_H
