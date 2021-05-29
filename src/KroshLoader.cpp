//
// Created by xenon on 30.05.2021.
//

#include "KroshLoader.h"

namespace {

std::vector<KroshLoaderEntry> KroshEntryList() {
  return {
      {"body", "global_frame", {0.084746, 0.391249, 0.532962}},
      {"smile", "body", {0.058443, 0.185455, 0.363000}},
      {"nose", "body", {0.800000, 0.186098, 0.392874}},

      {"left_arm", "body", {0.084746, 0.391249, 0.532962}},
      {"left_ear", "body", {0.084746, 0.391249, 0.532962}},
      {"left_eyebrow", "body", {0.058443, 0.185455, 0.363000}},
      {"left_eye", "body", {1, 1, 1}},
      {"left_leg", "body", {0.084746, 0.391249, 0.532962}},
      {"left_pupil", "left_eye", {0., 0., 0.}},
      {"left_tooth", "smile", {1, 1, 1}},

      {"right_arm", "body", {0.084746, 0.391249, 0.532962}},
      {"right_ear", "body", {0.084746, 0.391249, 0.532962}},
      {"right_eyebrow", "body", {0.058443, 0.185455, 0.363000}},
      {"right_eye", "body", {1, 1, 1}},
      {"right_leg", "body", {0.084746, 0.391249, 0.532962}},
      {"right_pupil", "right_eye", {0., 0., 0.}},
      {"right_tooth", "smile", {1, 1, 1}},
  };
}

} // namespace

KroshLoader::KroshLoader(const std::string& dirname)
    : dirname_(dirname) {}

vcl::hierarchy_mesh_drawable KroshLoader::Load() const {
  vcl::hierarchy_mesh_drawable hierarchy;
  for (const auto& entry : KroshEntryList()) {
    vcl::mesh mesh = vcl::mesh_load_file_obj(dirname_ + entry.name + ".obj");
    mesh.color.fill(entry.color);

    vcl::mesh_drawable drawable = vcl::mesh_drawable(mesh);

    drawable.shading.phong.specular = 0.2;
    drawable.shading.phong.ambient  = 1.1;
    drawable.shading.phong.diffuse  = 0.5;

    hierarchy.add(drawable, entry.name, entry.parent);
  }
  return hierarchy;
}
