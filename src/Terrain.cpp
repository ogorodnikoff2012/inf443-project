//
// Created by xenon on 29.05.2021.
//

#include "Terrain.h"
#include "Models.h"
#include <vcl/shape/noise/noise.hpp>

float TerrainChunk::X() const { return x_; }
float TerrainChunk::Y() const { return y_; }
float TerrainChunk::Width() const { return width_; }
float TerrainChunk::Height() const { return height_; }

TerrainChunk::TerrainChunk(float x, float y, float width, float height,
                           TerrainSharedResources* shared)
    : x_(x)
    , y_(y)
    , width_(width)
    , height_(height)
    , shared_(shared) {
  BuildMesh();
  BuildWater();

  std::uniform_real_distribution<float> distr(0., 1.);
  object_x_         = x_ + width_ * (distr(shared_->rnd) * 0.8 + 0.1);
  object_y_         = y_ + height_ * (distr(shared_->rnd) * 0.8 + 0.1);
  float object_z    = Z(object_x_, object_y_);
  float rand_num = distr(shared_->rnd);
  if (object_z > 2.2 && rand_num < 0.1) {
    object_type_ = EObjectMushroom;
  } else if (object_z > 2.2 && (rand_num >= 0.1 && rand_num < 0.6)) {
    object_type_ = EObjectTree;
  } else if (object_z > 2.2 && (rand_num >= 0.6 && rand_num < 0.85)) {
    object_type_ = EObjectCarrot;
  } else {
    object_type_ = EObjectNone;
  }

  // mesh_.shading.color = {0.6f, 0.85f, 0.5f}; // Make the grass more green :)
  //  mesh_.shading.phong.specular = 0.0f; // non-specular terrain material
  // mesh_.texture =
  //      opengl_texture_to_gpu(texture, GL_MIRRORED_REPEAT,
  //      GL_MIRRORED_REPEAT);
}

float TerrainChunk::DistanceSqr(float x, float y) const {
  float proj_x = std::min(x_ + width_, std::max(x_, x));
  float proj_y = std::min(y_ + height_, std::max(y_, y));
  float dx     = x - proj_x;
  float dy     = y - proj_y;
  return dx * dx + dy * dy;
}

void TerrainChunk::BuildMesh() {
  vcl::mesh mesh;

  // Number of samples of the terrain is N x N
  constexpr unsigned int N = 100;

  mesh.position.resize(N * N);
  mesh.uv.resize(N * N);
  mesh.color.resize(N * N);

  // Fill terrain geometry
  for (unsigned int ku = 0; ku < N; ++ku) {
    for (unsigned int kv = 0; kv < N; ++kv) {
      // Compute local parametric coordinates (u,v) \in [0,1]
      const float x = x_ + ku / (N - 1.0f) * width_;
      const float y = y_ + kv / (N - 1.0f) * height_;

      // Compute the local surface function
      vcl::vec3 const p{x, y, Z(x, y)};

      // Store vertex coordinates
      mesh.position[kv + N * ku] = p;
      mesh.uv[kv + N * ku]       = {p.x, p.y};
      if (p.z < 2.5) {
        mesh.color[kv + N * ku] = {0.95f, 0.8f, 0.75f};
      } else {
        mesh.color[kv + N * ku] = {0.6f, 0.85f, 0.5f};
      }
    }
  }

  // Generate triangle organization
  //  Parametric surface with uniform grid sampling: generate 2 triangles for
  //  each grid cell
  for (size_t ku = 0; ku < N - 1; ++ku) {
    for (size_t kv = 0; kv < N - 1; ++kv) {
      const unsigned int idx = kv + N * ku; // current vertex offset

      const vcl::uint3 triangle_1 = {idx, idx + 1 + N, idx + 1};
      const vcl::uint3 triangle_2 = {idx, idx + N, idx + 1 + N};

      mesh.connectivity.push_back(triangle_1);
      mesh.connectivity.push_back(triangle_2);
    }
  }

  mesh.fill_empty_field(); // need to call this function to fill the other
  // buffer with default values (normal, color, etc)

  mesh_ = vcl::mesh_drawable(mesh);
}
float TerrainChunk::Z(float x, float y) const {
  float z = PhysicalZ(x, y);

  float u = (x + 10) / 20;
  float v = (y + 10) / 20;

  // Local roughness
  z += 0.125 * (vcl::noise_perlin({u, v}, 6, 0.6, 2.2) - 0.5);

  return z;
}

float TerrainChunk::PhysicalZ(float x, float y) const {
  float u = (x + 10) / 20;
  float v = (y + 10) / 20;

  float z = 0;

  // Global terrain profile
  z += 5 * vcl::noise_perlin({0.5 * u, 0.5 * v}, 2, 0.5, 2);

  return z;
}
void TerrainChunk::InteractWith(Character& character) {
  if (object_type_ != EObjectCarrot) {
    return;
  }
  float x = character.Position().x;
  float y = character.Position().y;

  float dx = object_x_ - x;
  float dy = object_y_ - y;

  if (dx * dx + dy * dy < 0.2) {
    object_type_ = EObjectNone;
    character.OnCarrotCollect();
  }
}
void TerrainChunk::BuildWater() {
  vcl::mesh water;
  water.position.resize(4);
  water.color.resize(4);

  water.position[0] = {x_, y_, 2.2};
  water.position[1] = {x_ + width_, y_, 2.2};
  water.position[2] = {x_ + width_, y_ + height_, 2.2};
  water.position[3] = {x_, y_ + height_, 2.2};

  water.color.fill({0, 0, 0.5});

  water.connectivity.push_back({0, 1, 2});
  water.connectivity.push_back({0, 2, 3});

  water.fill_empty_field();

  water_               = vcl::mesh_drawable(water);
  water_.shading.alpha = 0.5;
}

size_t ChunkIndex::operator()(const ChunkIndex& index) const noexcept {
  static constexpr size_t kBigPrime = 100'000'000'003;
  return index.x * kBigPrime + index.y;
}

bool ChunkIndex::operator==(const ChunkIndex& other) const noexcept {
  return x == other.x && y == other.y;
}

float Terrain::GetHeight(float x, float y) {
  auto index = FindCurrentChunkIndex(x, y);
  return GetChunk(index)->Z(x, y);
}

float Terrain::GetPhysicalHeight(float x, float y) {
  auto index = FindCurrentChunkIndex(x, y);
  return GetChunk(index)->PhysicalZ(x, y);
}

Terrain::Terrain(float view_radius, float chunk_width, float chunk_height)
    : view_radius_sqr_(view_radius * view_radius)
    , chunk_width_(chunk_width)
    , chunk_height_(chunk_height)
    , shared_{vcl::image_load_png("assets/texture_grass.png"),
              BuildCarrot(), BuildTree(), BuildMushroom(),
              std::default_random_engine()} {}

TerrainChunk* Terrain::GetChunk(const ChunkIndex& index) {
  auto it = chunks_.find(index);
  if (it == chunks_.end()) {
    std::unique_ptr<TerrainChunk> chunk = std::make_unique<TerrainChunk>(
        index.x * chunk_width_, index.y * chunk_height_, chunk_width_,
        chunk_height_, &shared_);

    TerrainChunk* result = chunk.get();

    chunks_[index] = std::move(chunk);
    return result;
  }
  return it->second.get();
}

ChunkIndex Terrain::FindCurrentChunkIndex(float x, float y) const {
  return ChunkIndex{static_cast<int64_t>(std::floor(x / chunk_width_)),
                    static_cast<int64_t>(std::floor(y / chunk_height_))};
}
void Terrain::InteractWith(Character& character) {
  float x = character.Position().x;
  float y = character.Position().y;

  auto index = FindCurrentChunkIndex(x, y);
  GetChunk(index)->InteractWith(character);
}
