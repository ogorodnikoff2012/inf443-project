//
// Created by xenon on 29.05.2021.
//

#include "Character.h"
#include "Terrain.h"

Character::Character(const vcl::vec3& position, const vcl::rotation& rotation,
                     const vcl::mesh_drawable& mesh)
    : position_(position)
    , rotation_(rotation)
    , mesh_(mesh) {}

vcl::vec3& Character::Position() { return position_; }
vcl::rotation& Character::Rotation() { return rotation_; }
vcl::mesh_drawable& Character::Mesh() { return mesh_; }

void Character::UpdateCamera(vcl::camera_around_center& camera,
                             Terrain& terrain) const {
  vcl::vec3 look_from = position_ + rotation_ * vcl::vec3{-4, 0, 0};
  float z             = terrain.GetPhysicalHeight(look_from.x, look_from.y);
  look_from.z         = std::max(z, look_from.z) + 1;

  camera.look_at(look_from, position_, {0, 0, 1});
}

void Character::OnCarrotCollect() { ++collected_carrots_; }
int Character::CountCollectedCarrots() const { return collected_carrots_; }