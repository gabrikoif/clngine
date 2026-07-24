#pragma once

#include "gameobj.hpp"

class Physics
{
public:
  Physics(float p_bounds, float p_g = 9.81f, float p_bound_restitution = 0.8f, float p_obj_to_obj_restitution = 0.8f);
  ~Physics();
  void update(std::vector<GameObj> &p_objs, float deltaTime = 0.016f);
  bool m_worldDataUpdated = false;
  float newBounds;

private:
  void apply(GameObj &curr, float dt);
  void handleBoundCollision(GameObj &curr);
  void handleObjsCollision(GameObj &obj1, GameObj &obj2);
  void updateWorldData();

  // World constants
  float gravity;
  float bound_restitution;
  float obj_to_obj_restitution;
  float bounds;
};
