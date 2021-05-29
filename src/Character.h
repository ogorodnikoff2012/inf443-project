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
            const vcl::mesh_drawable& mesh);

  vcl::vec3& Position();
  vcl::rotation& Rotation();
  vcl::mesh_drawable& Mesh();

  void UpdateCamera(vcl::camera_around_center& camera, Terrain& terrain) const;

  template <class SCENE>
  void DrawOn(const SCENE& scene) const {
    mesh_.transform.translate = position_;
    mesh_.transform.rotate    = rotation_;

    vcl::draw(mesh_, scene);
  }

  void OnCarrotCollect();
  int CountCollectedCarrots() const;

 private:
  vcl::vec3 position_;
  vcl::rotation rotation_;
  mutable vcl::mesh_drawable mesh_;

  int collected_carrots_{0};
};

// #endif //INF443_CHARACTER_H
