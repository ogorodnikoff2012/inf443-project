//
// Created by xenon on 30.05.2021.
//

// #ifndef INF443_KROSHLOADER_H
// #define INF443_KROSHLOADER_H

#pragma once

#include <array>
#include <string>

#include <vcl/containers/buffer_stack/special_types/special_types.hpp>
#include <vcl/display/drawable/hierarchy_mesh_drawable/hierarchy_mesh_drawable.hpp>

struct KroshLoaderEntry {
  const std::string name;
  const std::string parent;
  const vcl::vec3 color;
};

class KroshLoader {
 public:
  explicit KroshLoader(const std::string& dirname);
  vcl::hierarchy_mesh_drawable Load() const;

 private:
  const std::string dirname_;
};

// #endif //INF443_KROSHLOADER_H
