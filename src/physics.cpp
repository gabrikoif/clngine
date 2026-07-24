#define GLM_ENABLE_EXPERIMENTAL
#include "physics.hpp"
#include <glm/gtx/norm.hpp>

Physics::Physics(float p_bounds, float p_g, float p_bound_restitution, float p_obj_to_obj_restitution)
    : bounds(p_bounds), gravity(p_g), bound_restitution(p_bound_restitution), obj_to_obj_restitution(p_obj_to_obj_restitution) { newBounds = bounds; };

Physics::~Physics() {};

void Physics::update(std::vector<GameObj> &p_objs, float deltaTime)
{
  if (m_worldDataUpdated)
  {
    bounds = newBounds;
  }
  for (auto &obj : p_objs)
  {
    apply(obj, deltaTime);
  }

  for (size_t i = 0; i < p_objs.size(); ++i)
  {
    for (size_t j = i + 1; j < p_objs.size(); ++j)
    {
      handleObjsCollision(p_objs[i], p_objs[j]);
    }
  }
}

void Physics::apply(GameObj &curr, float dt)
{
  curr.acl = glm::vec3(0.0f);
  const glm::vec3 g = glm::vec3(0.0f, -gravity, 0.0f);
  const float epsilon = 0.1f;
  bool onGround = (curr.pos.y - curr.radius <= 0.0f);
  bool atRest = (glm::dot(curr.vel, curr.vel) <= (epsilon * epsilon));

  // Stop moving if at rest
  if (atRest)
  {
    curr.vel = glm::vec3(0.0f);
  }
  // Apply gravity only if obj isn't resting on the ground.
  if (!(onGround && atRest))
  {
    curr.acl += g;
  }
  // Apply Euler's

  curr.vel += curr.acl * dt;
  curr.pos += curr.vel * dt;

  handleBoundCollision(curr);
}

void Physics::handleBoundCollision(GameObj &curr)
{
  const float r = curr.radius;
  bool collision = false;

  if (curr.pos.y - r <= 0.0f)
  {
    curr.pos.y = r;
    curr.vel.y = -curr.vel.y;
    collision = true;
  }
  else if (curr.pos.y + r >= bounds)
  {
    curr.pos.y = bounds - r;
    curr.vel.y = -curr.vel.y;
    collision = true;
  }

  if (curr.pos.x - r <= -bounds)
  {
    curr.pos.x = -bounds + r;
    curr.vel.x = -curr.vel.x;
    collision = true;
  }
  else if (curr.pos.x + r >= bounds)
  {
    curr.pos.x = bounds - r;
    curr.vel.x = -curr.vel.x;
    collision = true;
  }

  if (curr.pos.z - r <= -bounds)
  {
    curr.pos.z = -bounds + r;
    curr.vel.z = -curr.vel.z;
    collision = true;
  }
  else if (curr.pos.z + r >= bounds)
  {
    curr.pos.z = bounds - r;
    curr.vel.z = -curr.vel.z;
    collision = true;
  }

  if (collision)
  {
    curr.vel *= bound_restitution;
  }
}

void Physics::handleObjsCollision(GameObj &objA, GameObj &objB)
{
  // 1. Check for collision using squared distance (perf friendly!)
  glm::vec3 normal = objB.pos - objA.pos;
  float distSq = glm::length2(normal);

  float minDistance = objA.radius + objB.radius;

  // If distance squared is greater than combined radii squared, no collision
  if (distSq >= minDistance * minDistance)
  {
    return;
  }

  // Calculate actual distance now that we know they overlap
  float distance = glm::sqrt(distSq);

  // Normalize our collision axis
  if (distance > 0.0f)
  {
    normal /= distance; // equivalent to glm::normalize(normal)
  }
  else
  {
    // Edge case: Objects are perfectly on top of each other
    normal = glm::vec3(0.0f, 1.0f, 0.0f);
    distance = 0.0f;
  }

  // 2. Positional Correction (Stop them from sinking/sticking together)
  float overlap = minDistance - distance;

  // Distribute the correction based on mass ratio (lighter objects push back
  // more)
  float totalMass = objA.mass + objB.mass;
  float ratioA = objB.mass / totalMass; // How much A moves
  float ratioB = objA.mass / totalMass; // How much B moves

  // Move them apart along the collision normal
  objA.pos -= normal * (overlap * ratioA);
  objB.pos += normal * (overlap * ratioB);

  // 3. Elastic Impulse Calculation (The velocity bounce)

  // Relative velocity
  glm::vec3 relativeVel = objB.vel - objA.vel;

  // Calculate relative velocity along the normal
  float velAlongNormal = glm::dot(relativeVel, normal);

  // If they are already moving apart, don't apply another bounce
  if (velAlongNormal > 0.0f)
  {
    return;
  }

  // Coefficient of restitution (1.0 = perfectly elastic)
  const float restitution = obj_to_obj_restitution;

  // Calculate impulse scalar (J)
  float impulseScalar = -(1.0f + restitution) * velAlongNormal;
  impulseScalar /= (1.0f / objA.mass) + (1.0f / objB.mass);

  // Apply the impulse to each object's velocity
  glm::vec3 impulse = impulseScalar * normal;
  objA.vel -= (1.0f / objA.mass) * impulse;
  objB.vel += (1.0f / objB.mass) * impulse;
}