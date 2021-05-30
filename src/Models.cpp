//
// Created by Вероника Шилова on 30.05.2021.
//

#include "Models.h"

vcl::mesh_drawable BuildCarrot() {
  vcl::mesh body =
      vcl::mesh_primitive_cone(0.1f, 0.3f, {0, 0, 0.1f}, {0, 0, -1}, true);
  body.color.fill({0.95f, 0.55f, 0.15f});
  vcl::mesh green_part =
      vcl::mesh_primitive_cone(0.05f, 0.1f, {0, 0, 0.2f}, {0, 0, -1}, true);
  green_part.color.fill({0, 0.6f, 0});

  body.push_back(green_part);
  return vcl::mesh_drawable(body);
}

vcl::hierarchy_mesh_drawable BuildTree() {
  vcl::mesh trunk =
      vcl::mesh_primitive_cylinder(0.1f, {0, 0, 0.5f}, {0, 0, -1});
  vcl::mesh_drawable trunk_drawable(trunk);
  trunk_drawable.texture = opengl_texture_to_gpu(vcl::image_load_png("assets/trunk.png"),
                                                 GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
  trunk_drawable.shading.phong.specular = 0.0f;

  vcl::mesh bottom_layer =
      vcl::mesh_primitive_cone(0.5f, 0.5f, {0, 0, 0.45f}, {0, 0, 1}, true);
  vcl::mesh middle_layer =
      vcl::mesh_primitive_cone(0.5f, 0.5f, {0, 0, 0.8f}, {0, 0, 1}, true);
  vcl::mesh top_layer =
      vcl::mesh_primitive_cone(0.5f, 0.5f, {0, 0, 1.15f}, {0, 0, 1}, true);

  bottom_layer.push_back(middle_layer);
  bottom_layer.push_back(top_layer);
  vcl::mesh_drawable green_part(bottom_layer);
  green_part.texture = opengl_texture_to_gpu(vcl::image_load_png("assets/tree_texture.png"),
                                             GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
  vcl::hierarchy_mesh_drawable hierarchy;
  hierarchy.add(trunk_drawable, "trunk");
  hierarchy.add(green_part, "green_part", "trunk");
  hierarchy["trunk"].transform.scale = 2;
  return hierarchy;
}

vcl::hierarchy_mesh_drawable BuildMushroom() {
  vcl::mesh trunk =
      vcl::mesh_primitive_cylinder(0.05f, {0, 0, 0.15f}, {0, 0, -1});
  trunk.color.fill({1, 1, 1});
  vcl::mesh_drawable trunk_drawable(trunk);
  trunk_drawable.shading.phong.specular = 0.0f;

  vcl::mesh top =
      vcl::mesh_primitive_cone(0.15, 0.1, {0, 0, 0.1f}, {0, 0, 1}, true);
  vcl::mesh_drawable top_drawable(top);
  top_drawable.texture = opengl_texture_to_gpu(vcl::image_load_png("assets/mushroom_texture.png"),
                                             GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
  vcl::hierarchy_mesh_drawable hierarchy;
  hierarchy.add(trunk_drawable, "trunk");
  hierarchy.add(top_drawable, "top", "trunk");
  return hierarchy;
}

