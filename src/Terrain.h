//
// Created by xenon on 29.05.2021.
//

// #ifndef INF443_TERRAIN_H
// #define INF443_TERRAIN_H

#pragma once

#include <memory>
#include <random>
#include <unordered_map>

#include "Character.h"
#include <vcl/display/drawable/mesh_drawable/mesh_drawable.hpp>

struct TerrainSharedResources {
  const vcl::image_raw im_grass;
  vcl::mesh_drawable carrot;
  vcl::hierarchy_mesh_drawable tree;
  vcl::hierarchy_mesh_drawable mushroom;
  vcl::mesh_drawable water;
  double wave_phase;
  std::default_random_engine rnd;
};

class TerrainChunk {
 public:
  TerrainChunk(float x, float y, float width, float height,
               TerrainSharedResources* shared);

  float X() const;
  float Y() const;
  float Width() const;
  float Height() const;

  float Z(float x, float y) const;
  float PhysicalZ(float x, float y) const;

  template <class SCENE>
  void Render(const SCENE& scene) const {
    vcl::draw(mesh_, scene);

    if (object_type_ == EObjectCarrot) {
      shared_->carrot.transform.translate = {object_x_, object_y_,
                                             Z(object_x_, object_y_)};
      vcl::draw(shared_->carrot, scene);
    }
    if (object_type_ == EObjectTree) {
      shared_->tree["trunk"].transform.translate = {object_x_, object_y_,
                                                    Z(object_x_, object_y_)};
      shared_->tree.update_local_to_global_coordinates();
      vcl::draw(shared_->tree, scene);
    }
    if (object_type_ == EObjectMushroom) {
      shared_->mushroom["trunk"].transform.translate = {object_x_, object_y_,
                                                    Z(object_x_, object_y_)};
      shared_->mushroom.update_local_to_global_coordinates();
      vcl::draw(shared_->mushroom, scene);
    }
  }

  template <class SCENE>
  void RenderWater(const SCENE& scene) const;
  float DistanceSqr(float x, float y) const;

  void InteractWith(Character& character);

 private:
  float x_;
  float y_;
  float width_;
  float height_;

  float object_x_;
  float object_y_;
  enum EObjectType { EObjectCarrot, EObjectTree, EObjectMushroom, EObjectNone };
  EObjectType object_type_;

  vcl::mesh_drawable mesh_;
  TerrainSharedResources* shared_;

  void BuildMesh();
};

struct ChunkIndex {
  int64_t x{0};
  int64_t y{0};

  size_t operator()(const ChunkIndex& index) const noexcept;
  bool operator==(const ChunkIndex& other) const noexcept;
};

class Terrain {
 public:
  static const vcl::vec3 kWaveVector;

  Terrain(float view_radius, float chunk_width, float chunk_height);

  template <class SCENE>
  void Render(float x, float y, const SCENE& scene) {
    auto index = FindCurrentChunkIndex(x, y);

    ScanByX(x, y, index, 1, scene, true,
            [&](TerrainChunk* chunk) { chunk->Render(scene); });
    ScanByX(x, y, index, -1, scene, false,
            [&](TerrainChunk* chunk) { chunk->Render(scene); });
    ScanByX(x, y, index, 1, scene, true,
            [&](TerrainChunk* chunk) { chunk->RenderWater(scene); });
    ScanByX(x, y, index, -1, scene, false,
            [&](TerrainChunk* chunk) { chunk->RenderWater(scene); });
  }

  template <class SCENE, class Callback>
  void ScanByX(float x, float y, ChunkIndex index, int delta,
               const SCENE& scene, bool include_first, Callback callback) {
    if (!include_first) {
      index.x += delta;
    }

    while (true) {
      TerrainChunk* chunk = GetChunk(index);
      if (chunk->DistanceSqr(x, y) >= view_radius_sqr_) {
        break;
      }

      ScanByY(x, y, index, 1, scene, true, callback);
      ScanByY(x, y, index, -1, scene, false, callback);

      index.x += delta;
    }
  }

  template <class SCENE, class Callback>
  void ScanByY(float x, float y, ChunkIndex index, int delta,
               const SCENE& scene, bool include_first, Callback callback) {
    if (!include_first) {
      index.y += delta;
    }

    while (true) {
      TerrainChunk* chunk = GetChunk(index);
      if (chunk->DistanceSqr(x, y) >= view_radius_sqr_) {
        break;
      }

      callback(chunk);

      index.y += delta;
    }
  }

  float GetHeight(float x, float y);
  float GetPhysicalHeight(float x, float y);

  void InteractWith(Character& character);

  void UpdateAnimation(
      const std::chrono::time_point<std::chrono::system_clock>& now);

 private:
  // const float view_radius_;
  const float view_radius_sqr_;
  const float chunk_width_;
  const float chunk_height_;

  TerrainSharedResources shared_;

  Animation waves_animation_;

  std::unordered_map<ChunkIndex, std::unique_ptr<TerrainChunk>, ChunkIndex>
      chunks_{};

  ChunkIndex FindCurrentChunkIndex(float x, float y) const;
  TerrainChunk* GetChunk(const ChunkIndex& index);
  vcl::mesh_drawable BuildWater();
};

template <class SCENE>
void TerrainChunk::RenderWater(const SCENE& scene) const {
  shared_->water.transform.translate =
      Terrain::kWaveVector * shared_->wave_phase * 2 * M_PI;
  shared_->water.transform.translate.x += x_;
  shared_->water.transform.translate.y += y_;
  vcl::draw(shared_->water, scene);
}

// #endif //INF443_TERRAIN_H
