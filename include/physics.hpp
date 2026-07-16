#pragma once

#include "gameobj.hpp"

class Physics
{
public:
  Physics(float p_bounds, float p_g = 9.81f, float p_bound_restitution = 0.8f);
  ~Physics();
  void update(std::vector<GameObj>& p_objs, float deltaTime = 0.016f);

private:
  void apply(GameObj& curr, float dt);
  void handleBoundCollision(GameObj& curr);
  void Physics::handleObjsCollision(GameObj& obj1, GameObj& obj2);

  // World constants
  float gravity;
  float bound_restitution;
  float bounds;
};