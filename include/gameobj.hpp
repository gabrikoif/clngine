#pragma once

#include <glm/glm.hpp>
#include <random>

struct GameObj
{
  glm::vec3 pos;
  glm::vec3 vel;
  glm::vec3 acl;

  float radius;
  float mass;

  GameObj(float p_mass, float p_radius) : mass(p_mass), radius(p_radius)
  {
    pos = glm::vec3(0.0f);
    acl = glm::vec3(0.0f);
  }
};